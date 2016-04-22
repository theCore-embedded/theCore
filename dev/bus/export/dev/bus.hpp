#ifndef DEV_BUS_BUS_HPP_
#define DEV_BUS_BUS_HPP_

//!
//! \file
//! \brief      Generic bus interface module.
//! \copyfirght
//! \todo       Description and examples.


#include <ecl/err.hpp>
#include <ecl/thread/mutex.hpp>
#include <ecl/thread/semaphore.hpp>
#include <ecl/assert.h>

#include <platform/common/bus.hpp>

#include <atomic>

namespace ecl
{

//! \brief Generic bus interface.
//!
//! The generic bus is useful adapter, that allows to:
//! \li Encapsulate locking policy when multithreded environment is used.
//! \li Hide differences between full-duplex and half-duplex busses.
//! \li Define and simplify platform-level bus interface
//! \tparam PBus Plaform-level bus driver (I2C, SPI, etc.)
//!
template< class PBus >
class generic_bus
{
public:
    //! Costruction is not allowed.
    generic_bus() = delete;
    ~generic_bus() = delete;

    //! \brief Inits a bus.
    //!
    //! Lazy initialization. Inits an underlaying platform bus.
    //! \todo introduce init counter.
    //! \return Status of operation.
    //!
    static err init();

    //! \brief De-inits a bus.
    //! \pre Bus is initied.
    //! \todo use init counter to measure how many users are inited this bus.
    //! \return Status of operation.
    //!
    static err deinit();

    //! \brief Locks a bus.
    //!
    //! Any further operations can be executed after call to this function.
    //! If previous async xfer is in progress then current thread will be blocked
    //! until its finish.
    //! \pre    Bus is inited successfully.
    //! \post   Bus is locked.
    //! \sa     unlock()
    //!
    static void lock();

    //! \brief Unlocks a bus.
    //!
    //! Any operations beside lock() is not permitted after this method finishes.
    //! \par Side effects:
    //! \li In block mode all buffers provided with set_buffers() will be discarded
    //! \li In async mode if opration still ongoing, buffers will be discarded
    //!     after operation will finish.
    //!
    //! \pre    Bus is locked.
    //! \post   Bus is unlocked.
    //! \sa     set_buffers()
    //!
    static void unlock();

    //! \brief Sets RX and TX buffers and their sizes.
    //!
    //! If only TX or RX transaction required, then only one buffer must
    //! be passed. All effects from calls to set_buffers() will be discarded.
    //! \par Side effects:
    //! \li Bus will remember all buffers, until unlock() or set_buffers()
    //!     will be called.
    //! \pre       Bus is locked.
    //! \post      Bus is ready to execute xfer.
    //! \param[in] tx   Data to transmit.
    //!                 If this param is null, then rx must be set.
    //! \param[in] rx   Buffer to receive a data. Optional.
    //!                 If this param is null, then tx must be set.
    //! \param[in] size Size of buffers. Zero is valid size.
    //! \retval    err::ok      Buffers successfully set.
    //! \retval    err::inval   Both buffers are null.
    //! \retval    err::busy    Device is still executing async xfer.
    //!
    static err set_buffers(const uint8_t *tx, uint8_t *rx, size_t size);

    //! \brief Sets TX buffer size and fills it with given byte.
    //!
    //! This will instruct a platform bus to send a byte given number of times.
    //! It is implementation defined how bis is chunks in which data is sent.
    //! If possible, platform bus will just send single-byte buffer via DMA.
    //!
    //! In half-duplex case RX will not be performed. If platform bus is in
    //! full duplex mode then RX will be exeucted but RX data will be ignored.
    //! \par Side effects:
    //! \li Bus will remember a buffer, until unlock() or set_buffers()
    //!     will be called.
    //! \pre       Bus is locked.
    //! \post      Bus is ready to execute xfer.
    //! \param[in] size         Size of filled buffer.
    //! \param[in] fill_byte    Byte which will be sent in TX stream.
    //! \retval    err::ok      Buffer successfully set and filled.
    //! \retval    err::busy    Device is still executing async xfer.
    //!
    static err set_buffers(size_t size, uint8_t fill_byte = 0xff);

    //! \brief Performs xfer in blocking mode using buffers set previously.
    //!
    //! If underlying bus works in half-duplex mode then first tx transaction
    //! will occur and then rx.
    //! \warning Method uses a semaphore to wait for a bus event (most likely
    //! an IRQ event). It means that in bare-metal environment,
    //! without RTOS it is implemented as simple spin-lock. Likely that
    //! such behaviour is unwanted. In order to control event handling,
    //! consider using xfer(const handler_fn &handler) overload.
    //! \pre        Bus is locked and buffers are set.
    //! \post       Bus remains in the same state.
    //! \param[out] sent        Optional. Bytes sent during this xfer.
    //! \param[out] received    Optional. Bytes received during this xfer.
    //! \retval     err::ok     Data is sent successfully.
    //! \retval     err::busy   Device is still executing async xfer.
    //! \retval     err::io     Transaction started but failed.
    //! \retval     err         Any other error that can occur in platform bus
    //!
    static err xfer(size_t *sent = nullptr, size_t *received = nullptr);

    //! \brief Performs xfer in async mode using buffers set previously.
    //!
    //! If underlying bus works in half-duplex mode then first tx transaction
    //! will occur and then rx.
    //! When xfer will be done, given handler will be invoked with a
    //! type of an event.
    //! \warning Event handler most likely will be executed in ISR context.
    //! Pay attention to it. Do not block inside of it or do anything else that
    //! can break ISR or impose high interrupt latency.
    //! \note Errors that occur after transaction has started are reported via
    //! user-supplied handler. \sa PBus::event
    //! \note Bytes transferred are reported via user-supplied handler
    //! per each channel (tx, rx) separately.
    //! \pre       Bus is locked and buffers are set.
    //! \post      Bus remains in the same state.
    //! \param[in] handler      User-supplied event handler.
    //! \retval    err::ok      Data is sent successfully.
    //! \retval    err::busy    Device is still executing async xfer.
    //! \retval    err          Any other error that can occur in platform bus.
    //!
    static err xfer(const bus_handler &handler);

private:
    using semaphore     = ecl::binary_semaphore;
    using mutex         = ecl::mutex;
    using atomic_flag   = std::atomic_flag;

    //! \brief Event handler dedicated to the platform bus.
    //! \param[in] ch     Channel (rx or tx) where the event occurred.
    //! \param[in] type   Type of event occurred.
    //! \param[in] total  Total amount of bytes transferred within given channel.
    //!
    static void platform_handler(bus_channel ch, bus_event type, size_t total);

    //! \brief Checks if bus is busy transferring data at this moment or not.
    //! \retval true Bus is busy.
    //!
    static bool bus_is_busy();

    //! \brief Performs cleanup required after unlocking and delivering an event.
    static void cleanup();

    // State flags.
    //! Bus init status: set - bus initialized, reset - bus not yet initialized
    static constexpr uint8_t bus_inited     = 0x1;
    //! Operation mode of a bus: set - async mode, reset - block mode
    static constexpr uint8_t async_mode     = 0x2;
    //! Bus lock state: set - locked, reset - unlocked
    static constexpr uint8_t bus_locked     = 0x4;
    //! Xfer event status: set - all events from xfer are served,
    //! reset - not all are served
    static constexpr uint8_t xfer_served    = 0x8;
    //! Xfer error status: set - error(s) occurred during transfer,
    //! reset - no error occurred.
    static constexpr uint8_t xfer_error     = 0x10;

    static PBus         m_bus;      //!< Platform bus object.
    static mutex        m_lock;     //!< Lock to protect a platform bus.
    static semaphore    m_complete; //!< Semaphore to notify about end of xfer.
    static bus_handler  m_handler;  //!< User-supplied handler, used in async mode
    static size_t       m_received; //!< Bytes received during last blocking xfer.
    static size_t       m_sent;     //!< Bytes sent during last blocking xfer.
    static atomic_flag  m_cleaned;  //!< Cleanup is performed after xfer and unlock are done.
    static uint8_t      m_state;    //!< State flags.
};

template< class PBus > PBus                     generic_bus< PBus >::m_bus{};
template< class PBus > ecl::mutex               generic_bus< PBus >::m_lock{};
template< class PBus > ecl::binary_semaphore    generic_bus< PBus >::m_complete{};
template< class PBus > bus_handler              generic_bus< PBus >::m_handler{};
template< class PBus > size_t                   generic_bus< PBus >::m_received{};
template< class PBus > size_t                   generic_bus< PBus >::m_sent{};
template< class PBus > std::atomic_flag         generic_bus< PBus >::m_cleaned{};
template< class PBus > uint8_t                  generic_bus< PBus >::m_state{};

//------------------------------------------------------------------------------

template< class PBus >
err generic_bus< PBus >::init()
{
    if (m_state & bus_inited) {
        return err::ok;
    }

    m_bus.set_handler(platform_handler);

    auto rc = m_bus.init();

    if (is_ok(rc)) {
        m_state |= bus_inited;
    }

    return rc;
}

template< class PBus >
err generic_bus< PBus >::deinit()
{
    if (!(m_state & bus_inited)) {
        return err::perm;
    }

    m_bus.reset_handler();
    cleanup();
    m_state = 0;

    return err::ok;
}

template< class PBus >
void generic_bus< PBus >::lock()
{
    // If bus is not initialized then pre-conditions are violated.
    ecl_assert(m_state & bus_inited);

    m_lock.lock();

    m_state |= bus_locked;

    // Bus may be busy at this moment, wait until it finished most
    // recent transaction.
    if (m_state & async_mode) {
        m_complete.wait();
    }
}

template< class PBus >
void generic_bus< PBus >::unlock()
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    // Notify handler routine that unlock is called and it is possible to
    // do cleanup.
    // An assert above becomes unreliable, if unlock flag is set so early,
    // however main purpose of this flag is to notify handler about unlocking,
    // rather than an error check.
    m_state &= ~(bus_locked);

    // Do cleanup in block mode - bus is guaranteed to finish all transaction
    // prior to unlock() call
    if (m_state & async_mode) {
        if (m_state & xfer_served) {

            // Cleanup routine is a critical section and both platform_handler()
            // and unlock() routine can try to access it concurently.
            // Wrapping this part with mutexes must be avoided
            // because platform_handler() will likely be executed in ISR context.
            // Using atomics is most convinient solution.
            if (!m_cleaned.test_and_set()) {
                cleanup();
            }
        }
    } else {
        cleanup();
    }

    m_lock.unlock();
}

template< class PBus >
ecl::err generic_bus< PBus >::set_buffers(const uint8_t *tx, uint8_t *rx, size_t size)
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    if (!tx && !rx) {
        return err::inval;
    }

    if (bus_is_busy()) {
        return err::again;
    }

    m_bus.reset_buffers();
    m_bus.set_tx(tx, size);
    m_bus.set_rx(rx, size);

    return err::ok;
}

template< class PBus >
ecl::err generic_bus< PBus >::set_buffers(size_t size, uint8_t fill_byte)
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    if (bus_is_busy()) {
        return err::again;
    }

    m_bus.reset_buffers();
    m_bus.set_tx(size, fill_byte);
    return err::ok;
}

template< class PBus >
ecl::err generic_bus< PBus >::xfer(size_t *sent, size_t *received)
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    if (bus_is_busy()) {
        return err::busy;
    }

    // Blocking mode xfer is provided.
    m_state &= ~(async_mode);

    // Events of this particular xfer is not yet served.
    m_state &= ~(xfer_served);

    // Clear previous errors
    m_state &= ~(xfer_error);

    // Reset binary semaphore counter
    m_complete.try_wait();

    // Reset transfer counters
    m_received = m_sent = 0;

    auto rc = m_bus.do_xfer();

    if (!is_error(rc)) {
        m_complete.wait();

        // Errors can occur after transaction start, check this
        if (m_state & xfer_error) {
            rc = err::io;
        }

        // Return amount of bytes written and/or read.
        // Even if error occurred, some data was possibly transffered.

        if (received) {
            *received = m_received;
        }
        if (sent) {
            *sent = m_sent;
        }

    } else {
        // Deem that xfer virtually occurs in blocking mode and thus
        // momentally served in case of error.
        m_state |= xfer_served;
    }

    return rc;
}

template< class PBus >
ecl::err generic_bus< PBus >::xfer(const bus_handler &handler)
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    // Clears IRQ flag as well.
    if (bus_is_busy()) {
        return err::busy;
    }

    // Async mode xfer is provided
    m_state |= async_mode;
    m_handler = handler;

    auto rc = m_bus.do_xfer();

    if (is_error(rc)) {
        // Deem that xfer virtually occurs in blocking mode and thus
        // momentally served in case of error.
        m_state |= xfer_served;
        m_state &= ~(async_mode);
    } else {
        // Events of this particular xfer is not yet served.
        m_state &= ~(xfer_served);
    }

    return rc;
}

//------------------------------------------------------------------------------

template< class PBus >
void generic_bus< PBus >::platform_handler(bus_channel ch, bus_event type, size_t total)
{
    // Transfer complete accross all channels
    bool last_event = (ch == bus_channel::meta && type == bus_event::tc);

    if (type == bus_event::err) {
        m_state |= xfer_error;
    }

    if (last_event) {
        // Spurious events are not allowed
        ecl_assert(!(m_state & xfer_served));

        m_state |= xfer_served;
    }

    if (m_state & async_mode) {
        m_handler(ch, type, total);

        // Bus unlocked, it is time to check if bus cleaned.
        if (last_event && !(m_state & bus_locked)) {

            // It is possible that bus_handler() is executed in thread context
            // rather in ISR context. This means that context switch can occur
            // right after unlock flag is set inside unlock() but before
            // cleanup will occur. Atomic test_and_set will protect important
            // call with lock-free critical section.
            // In case if handler is executed in ISR context, this check
            // is almost meaningless, besides that setting a flag is required
            // to inform rest of the system that cleanup already done.
            if (!m_cleaned.test_and_set()) {
                cleanup();
            }
        }
    } else {
        // In blocking mode a bus is responcible for bytes counting
        if (ch == bus_channel::tx) {
            m_sent = total;
        } else if (ch == bus_channel::rx) {
            m_received = total;
        }

        // Don't do anything for 'meta' channel
    }

    if (last_event) {
        // Inform rest of the bus about event handling
        m_complete.signal();
    }
}

template< class PBus >
bool generic_bus< PBus >::bus_is_busy()
{
    // Asynchronius operation still in progress.
    return (m_state & async_mode);
}

template< class Bus >
void generic_bus< Bus >::cleanup()
{
    m_bus.reset_buffers();
    m_handler = bus_handler{};
    // When bus will be locked agian, no need to wait for events.
    m_state &= ~(async_mode);
}

}

#endif
