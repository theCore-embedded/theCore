//! \file
//! \brief Serial interface for platform drivers with listen mode support.
//! \todo Find better place (and probably name?) for current header and class.
#ifndef DEV_BUS_SERIAL_HPP_
#define DEV_BUS_SERIAL_HPP_

#include <ecl/err.hpp>
#include <ecl/assert.h>
#include <ecl/utils.hpp>
#include <common/bus.hpp>
#include <ecl/thread/semaphore.hpp>

#include <atomic>

namespace ecl
{

//! Serial driver interface.
//! \details The serial allows to abstract async,
//! interrupt-driven nature of platform-level drivers and
//! provide synchronus, buffered data management interface.
//! \tparam PBus Exclusively owned platform bus.
//! \tparam buf_size Size of internal rx and tx buffers.
template<class PBus, size_t buf_size = 128>
class serial
{
public:
    static constexpr auto buffer_size = buf_size;
    using platform_handle = PBus;

    serial() = delete;
    serial(const serial &other) = delete;
    serial(serial &&other) = delete;
    ~serial() = delete;

    //! Initialize serial driver and underlying platform bus.
    //! \pre Driver is not initialized.
    //! \return Operation status.
    static err init();

    //! Deinitialize serial driver and underlying platform bus.
    //! \pre Driver is initialized.
    //! \return Operation status.
    static err deinit();

    //! Sets or clears non-block mode of operation.
    //! \pre Driver is initialized.
    //! \details Setting nonblock mode will prevent recv and send functions
    //! from blocking. Special error code will be returned to notify user about
    //! possible blocking.
    //! \param[in] state 'true' to enable non-blocking mode.
    //! \retval err::ok If state set without error.
    static err nonblock(bool state = true);

    //! Obtains a byte from serial device.
    //! \pre Driver is initialized.
    //! \details If there is no data to return call with block in blocking mode,
    //! and return immediately in non-blocking mode.
    //! Otherwise data will be stored in the given argument and
    //! method will return.
    //! \param[out] byte Place for incoming byte. Will remain
    //!                  unchanged if error occurs.
    //! \return Operation status.
    //! \retval err::wouldblock Non-blocking mode is set and read would block
    //!                         otherwise.
    static err recv_byte(uint8_t &byte);

    //! Gets buffer from serial device.
    //! \pre Driver is initialized.
    //! \details Call will block if there is no data to return in blocking mode.
    //! In non-blocking mode special code is returned. Otherwise, buffer will be
    //! populated with avaliable data and method will return.
    //! \param[out]     buf Buffer to fill with data.
    //! \param[in,out]  sz  Size of buffer. Will be updated with
    //!                     amount of bytes written to the buffer.
    //!                     In case of error, size will be updated
    //!                     with bytes successfully stored in the buffer
    //!                     before error occurred.
    //! \return Operation status.
    //! \retval err::wouldblock Non-blocking mode is set and read would block
    //!                         otherwise.
    static err recv_buf(uint8_t *buf, size_t &sz);

    //! Sends byte to a serial device.
    //! \pre Driver is initialized.
    //! \details It may not block if internal buffering is applied.
    //! It may also block if platform device is not yet ready to
    //! send data. In non-blocking mode special code is returned instead of
    //! blocking.
    //! \param[in] byte Byte to send.
    //! \return Operation status.
    //! \retval err::wouldblock Non-blocking mode is set and send would block
    //!                         otherwise.
    static err send_byte(uint8_t byte);

    //! Sends buffer to a serial stream.
    //! \pre Driver is initialized.
    //! \details It may not block if internal buffering is applied.
    //! It may also block if platform device is not yet ready to
    //! send data. In non-blocking mode special code is returned instead of
    //! blocking.
    //! \param[in]      buf Buffer to send.
    //! \param[in,out]  sz  Size of buffer. Will be updated with
    //!                     amount of bytes sent In case of error,
    //!                     size will be updated with amount of bytes
    //!                     successfully sent before error occur.
    //! \return Operation status.
    //! \retval err:again The buffer was not consumed entirely.
    //! \retval err::wouldblock Non-blocking mode is set and send would block
    //!                         otherwise.
    static err send_buf(const uint8_t *buf, size_t &sz);

private:
    //! Bus event handler
    static void bus_handler(bus_channel ch, bus_event type, size_t total);

    //! Set if serial is initialized.
    static bool m_is_inited;

    //! Set if serial is in non-blocking mode.
    static bool m_nonblock;

    //! Helper struct, represents xfer chunk (part of the buffer) for RX
    //! communications.
    //! \details When accessing buffers and metadata, it is convinient to
    //! think about two contexes - user (performed in regular thread) and
    //! xfer (performed in the bus_handler(), interrupt).
    //! We are pretty safe if both contexes does not write into the same object..
    struct chunk
    {
        // Convenient aliases.
        using bsem = safe_storage<binary_semaphore>;
        using asize_t = std::atomic_size_t;

        //! Buffer size. Each chunk is a half of a requested buffer size.
        static constexpr auto xfer_size = buffer_size / 2;
        //! Helper function to return size.
        static constexpr auto size() { return xfer_size; }

        //! Buffer itself. Written by platform (xfer context). Read by user.
        uint8_t   data[xfer_size];

        //! Start marker, written from user context, read by xfer and user contexes.
        //! Changes when user reads data from the buffer.
        asize_t   start {0};

        //! End marker, written by xfer, read by xfer and user contexes.
        //! Changes when xfer writes data to the buffer.
        asize_t   end {0};

        //! Pending xfer flag. Set when xfer is failed to start
        //! because there is still some data. User _must_ read that data,
        //! before starting new xfer.
        bool      xfer_pend = false;

        //! Raised by xfer when empty buffer is filled with a new data.
        bsem      data_flag = {};

        //! Initializes chunk
        chunk() { data_flag.init(); }

        //! Resters state of the chunk. Called by user when buffer is
        //! finally depleted.
        void restore() { start = end = 0; }

        //! Sets pending xfer flag. Called by xfer, if current buffer still
        //! read by the user.
        void set_pend() { xfer_pend = true; }

        //! Checks pending xfer flag. Called by user after moving to the next buffer.
        //! Thus, user can start new xfer with current chunk.
        bool pend() { return xfer_pend; }

        //! Checks if buffer is ready to start xfer. Called by xfer before
        //! starting new xfer. Returns true, if restore() call was made earlier
        //! and no other changes were done.
        bool fresh() { return !start && !end; }

        //! Starts new xfer. Called by user or xfer (depending on xfer_pend
        //! flag, when new xfer must be started.
        //! Pre: previous xfer must be fully completed and buffer is fresh.
        ecl::err start_xfer()
        {
            PBus::set_rx(data, size());

            auto rc = PBus::enable_listen_mode();
            if (is_error(rc)) {
                PBus::set_rx(nullptr, 0);
                return rc;
            }

            rc = PBus::do_rx();
            if (is_error(rc)) {
                PBus::set_rx(nullptr, 0);
                return rc;
            }

            // Convinient move - pending xfer satisfied.
            xfer_pend = false;
            return rc;
        }

        //! Checks how much data inside the buffer. Called by user.
        size_t fill() { ecl_assert(end >= start); return end - start; }

        //! Checks if there is new data to read.
        //! Called by user and xfer.
        bool no_data() { return !fill(); }

        //! Checks if no more data can be read _and_ written to the buffer.
        //! Called by user.
        bool depleted() { return start == size(); }

        //! Checks if no more data can be written to the buffer.
        //! Callled by xfer.
        bool no_space() { return end == size(); }

        //! Waits for new data inside buffer.
        //! _Must_ be called by user and _must_ be called if no_data() returned
        //! true before.
        bool wait_data() { data_flag.get().wait(); return true; } // TODO

        //! Signals new data arrival. _Must_ be called by xfer when empty
        //! buffer receives some data.
        void signal_data() { data_flag.get().signal(); }

        //! Increments write data counters when new data arrives.
        //! Called by xfer.
        void new_bytes(size_t cnt) { end += cnt; ecl_assert(end <= size()); }

        //! Copies data from buffer to user, increments read counters.
        //! Called by user.
        size_t copy(uint8_t *dst, size_t sz)
        {
            // This method operates ISR-owned `end` marker.
            // End marker is read atomically as a part of fill() call.
            // Serial buffer opeartions designed to prevent xfer from
            // overwriting unread user data and user markers.

            auto occupied = fill();

            auto to_copy = std::min(sz, occupied);
            ecl_assert(to_copy); // There must be some data.

            std::copy(data + start, data + start + to_copy, dst);
            start += to_copy;
            ecl_assert(start <= end);
            return to_copy;
        }
    };

    //! Helper aggregate with two chunks organized together.
    //! \details Chunk can be assigned to user, to xfer, or to both user and xfer.
    //! When chunk is assigned to user (regardless of chunk assignment for xfer),
    //! it means that user can read from that chunk.
    //! When chunk is assigned to xfer, it means that xfer either ongoing
    //! in the xfer chunk, or about to happen.
    struct serial_chunks
    {
        chunk chunks[2] = {};   //!< Chunks themselves
        int user_idx = 0;       //!< Current chunk, owned by user.
        int xfer_idx = 0;       //!< Current chunk, owned by xfer.

        //! Returns buffer for user to operate on.
        chunk &user() { return chunks[user_idx]; }
        //! Returns buffer for xfer to operate on.
        chunk &xfer() { return chunks[xfer_idx]; }

        //! Returns next user buffer.
        chunk &user_next() { user_idx ^= 1; return user(); }
        //! Returns next xfer buffer.
        chunk &xfer_next() { xfer_idx ^= 1; return xfer(); }
    };

    static safe_storage<serial_chunks>      m_chunks;           //! Chunks for RX.
    static safe_storage<binary_semaphore>   m_tx_rdy;           //!< Signalled if tx channel is ready
    static uint8_t                          m_tx_buf[buf_size]; //!< TX buffer.
};

template <class PBus, size_t buf_size>
bool serial<PBus, buf_size>::m_is_inited;

template <class PBus, size_t buf_size>
bool serial<PBus, buf_size>::m_nonblock;

template <class PBus, size_t buf_size>
safe_storage<typename serial<PBus, buf_size>::serial_chunks> serial<PBus, buf_size>::m_chunks;

template <class PBus, size_t buf_size>
safe_storage<binary_semaphore> serial<PBus, buf_size>::m_tx_rdy;

template <class PBus, size_t buf_size>
uint8_t serial<PBus, buf_size>::m_tx_buf[buf_size];

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::init()
{
    m_chunks.init();
    ecl_assert(!m_is_inited);

    auto result = PBus::init();
    if (is_error(result)) {
        return result;
    }
    PBus::set_handler(bus_handler);

    m_tx_rdy.get().signal();
    result = m_chunks.get().xfer().start_xfer();
    if (is_ok(result)) {
        m_is_inited = true;
    }
    return result;
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::deinit()
{
    ecl_assert(m_is_inited);
    PBus::cancel_xfer();
    PBus::reset_buffers();
    m_is_inited = false;
    m_tx_rdy.deinit();
    return err::ok;
}

template <class PBus, size_t buf_size>
void serial<PBus, buf_size>::bus_handler(bus_channel ch, bus_event type, size_t total)
{
    if (ch == bus_channel::rx) {
        if (type == bus_event::tc) {
            auto &xfer_buf = m_chunks.get().xfer();

            // TODO: correct assert to consume more than 1 byte at time.
            ecl_assert(xfer_buf.end + 1 == total);

            // Notify user if new data arrives
            if (xfer_buf.no_data()) {
                xfer_buf.signal_data();
            }

            xfer_buf.new_bytes(1);

            if (xfer_buf.no_space()) {
                // If there is no place to write, we must move to the next buffer.
                auto &new_xfer_buf = m_chunks.get().xfer_next();

                // If chunk is fresh it means that all data was succesfully
                // read by user and it is safe to start new xfer.
                if (new_xfer_buf.fresh()) {
                    new_xfer_buf.start_xfer();
                } else {
                    // User operates over a chunk. New xfer must not be started.
                    // Now, it is user responsibility to start it after all data
                    // will be read.
                    new_xfer_buf.set_pend();
                }
            }
        }
    } else if (ch == bus_channel::tx) {
        if (type == bus_event::tc) {
            m_tx_rdy.get().signal();
        } // else error - ignore. TC event _must_ be supplied
          // after possible error.
    }
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::nonblock(bool state)
{
    m_nonblock = state;
    return err::ok;
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::recv_byte(uint8_t &byte)
{
    size_t sz = 1;
    return recv_buf(&byte, sz);
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::recv_buf(uint8_t *buf, size_t &sz)
{
    ecl_assert(m_is_inited);
    err result = err::ok;

    auto &user_buf = m_chunks.get().user();

    // no_data() can return true, even if there is some data, due to
    // expected race with ISR. However, it does not lead to any data corruption,
    // only user should repeat reading again.
    if (m_nonblock && user_buf.no_data()) {
        sz = 0;
        return err::wouldblock;
    }

    // It is possible that semaphore is rised, even if there is no data.
    // For example, consider case:
    // - xfer places 1 byte into the buffer
    // - user does not block, reads 1 byte, 0 bytes left
    // - xfer see 0 byte data, writes 1 more again, rises semaphore
    // - user does not block, reads 1 byte, 0 bytes left
    // - user tries to read 1 byte again, but there are no data
    //   -> and semaphore is still rised <-
    // So checking semaphore once is not enough. That's why loop is here.
    // Also, no_data() can return true, even if there is some data, due to
    // expected race with ISR. However, it does not lead to any data corruption.
    // Semaphore guarantees reliability.
    while (user_buf.no_data()) {
        user_buf.wait_data(); // All ok: semaphore
    }

    // Read data. ISR running in the same time will not cause data overwrite
    // by design of this serial class.
    size_t read = user_buf.copy(buf, sz);
    sz = read;

    if (user_buf.depleted()) {  // All ok: user-owned variable
        // No data inside the buffer. As a consequence, no more place to write
        // a new data. Next buffer should be processed.

        // In this moment, ISR can occur and switch to current user buf.
        // It will see that buffer is not fresh, and sent pending flag.
        // This flag will force user to start new xfer below.

        // Restore modifies xfer-owned variable. However, since depleted()
        // returned 'true', that variable is no longer modified by xfer.
        // Restore isn't atomic, thus if ISR will be called during execution
        // of restore(), same logic will apply as described in previous comment.
        user_buf.restore();

        // ISR can occur here as well.
        // Since restore() call is completed, buffer will be in the fresh
        // state. xfer will be started from ISR context. Pending flag
        // will be unset.

        // New buffer will be used in the next call of recv_buf();
        m_chunks.get().user_next();

        if (user_buf.pend()) {
            // Start pending xfer, if ISR failed to do it.
            user_buf.start_xfer();
        }
    }

    return result;
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::send_byte(uint8_t byte)
{
    size_t sz = 1;
    return send_buf(&byte, sz);
}

template <class PBus, size_t buf_size>
err serial<PBus, buf_size>::send_buf(const uint8_t *buf, size_t &size)
{
    // Wait until TX will be ready
    if (m_nonblock) {
        if (!m_tx_rdy.get().try_wait()) {
            size = 0;
            return err::wouldblock;
        }
    } else {
        m_tx_rdy.get().wait();
    }

    auto to_copy = std::min(size, buf_size);
    std::copy(buf, buf + to_copy, m_tx_buf);
    PBus::set_tx(m_tx_buf, to_copy);
    auto rc = PBus::do_tx();

    if (is_error(rc)) {
        return rc;
    }

    // Only part of the buffer was consumed
    if (to_copy < size) {
        rc = err::again;
    }

    size = to_copy;

    return rc;
}

} // namespace ecl

#endif // DEV_BUS_SERIAL_HPP_
