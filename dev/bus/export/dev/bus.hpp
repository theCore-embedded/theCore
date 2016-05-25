#ifndef DEV_BUS_BUS_HPP_
#define DEV_BUS_BUS_HPP_

//!
//! \file
//! \brief      Generic bus interface module.
//! \copyright
//! \todo       Description and examples.


#include <ecl/err.hpp>
#include <ecl/thread/mutex.hpp>
#include <ecl/thread/semaphore.hpp>
#include <ecl/assert.h>

#include <platform/common/bus.hpp>

#include <atomic>

namespace ecl
{

//! Generic bus interface.
//! \details The generic bus is useful adapter, that allows to:
//! - Encapsulate locking policy when multithreded environment is used.
//! - Hide differences between full-duplex and half-duplex busses.
//! - Define and simplify platform-level bus interface
//! \tparam PBus Platform-level bus driver (I2C, SPI, etc.)
//!
//! This class uses one of methods to prevent “static initialization order
//! fiasco” to handle initialization of the static members.
//! See https://isocpp.org/wiki/faq/ctors
//!
template< class PBus >
class generic_bus
{
public:
    //! Costruction is not allowed.
    generic_bus() = delete;
    ~generic_bus() = delete;

    //! Inits a bus.
    //! \details Lazy initialization. Inits an underlying platform bus.
    //! \todo introduce init counter.
    //! \return Status of operation.
    //!
    static err init();

    //! De-inits a bus.
    //! \pre Bus is initied.
    //! \todo use init counter to measure how many users are inited this bus.
    //! \return Status of operation.
    //!
    static err deinit();

    //! Locks a bus.
    //! \details Any further operations can be executed after call to this function.
    //! If previous async xfer is in progress then current thread will be blocked
    //! until its finish.
    //! \pre    Bus is inited successfully.
    //! \post   Bus is locked.
    //! \sa     unlock()
    //!
    static void lock();

    //! Unlocks a bus.
    //! \details Any operations beside lock() is not permitted after this method finishes.
    //! \par Side effects:
    //! \li In block mode all buffers provided with set_buffers() will be discarded
    //! \li In async mode if operation still ongoing, buffers will be discarded
    //!     after operation will finish.
    //!
    //! \pre    Bus is locked.
    //! \post   Bus is unlocked.
    //! \sa     set_buffers()
    //!
    static void unlock();

    //! Sets RX and TX buffers and their sizes.
    //! \details If only TX or RX transaction required, then only one buffer must
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

    //! Sets TX buffer size and fills it with given byte.
    //! \details This will instruct a platform bus to send a byte given number
    //! of times. It is implementation defined how platform-level bus handles
    //! incoming data, e.g. it can be splitted into chunks when sending to
    //! the destination, if platform bus protocol allows such thing.
    //! \details In half-duplex case RX will not be performed. If platform bus
    //! is in full duplex mode then RX will be executed but RX data will be
    //! ignored.
    //! \par Side effects
    //! - Bus will remember a buffer, until unlock() or set_buffers() will
    //!   be called.
    //! \pre       Bus is locked.
    //! \post      Bus is ready to execute xfer.
    //! \param[in] size         Size of filled buffer.
    //! \param[in] fill_byte    Byte which will be sent in TX stream.
    //! \retval    err::ok      Buffer successfully set and filled.
    //! \retval    err::busy    Device is still executing async xfer.
    //!
    static err set_buffers(size_t size, uint8_t fill_byte = 0xff);

    //! Performs xfer in blocking mode using buffers set previously.
    //! \details If underlying bus works in half-duplex mode then first
    //! tx transaction will occur and then rx. Any deffered xfer will be
    //! canceled in result of this call and simple, blocking xfer will proceed.
    //! \warning Method uses a semaphore to wait for a bus event (most likely
    //! an IRQ event). It means that in bare-metal environment,
    //! without RTOS it is implemented as simple spin-lock. Likely that
    //! such behavior is unwanted. In order to control event handling,
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

    //! Performs xfer in async mode using buffers set previously.
    //! \details If underlying bus works in half-duplex mode then first
    //! tx transaction will occur and then rx. When xfer will be done, given
    //! handler will be invoked with a type of an event.
    //! \details It is also possible to postpone xfer, i.e. "defer" it
    //! and then trigger at will using xfer_trigger() method. See xfer_trigger()
    //! for more defails. Any previous defered xfer will be canceled in result
    //! of this call.
    //! \warning Event handler most likely will be executed in ISR context.
    //! Pay attention to it. Do not block inside of it or do anything else that
    //! can break ISR or impose high interrupt latency.
    //! \note Bytes transferred are reported via user-supplied handler
    //! per each channel (tx, rx) separately.
    //! \note Errors that occur after transaction has started are reported via
    //! user-supplied handler.
    //! \sa PBus::event
    //! \pre       Bus is locked and buffers are set.
    //! \post      Bus remains in the same state.
    //! \param[in] handler   User-supplied event handler.
    //! \param[in] type      Type of xfer. Possible values are:
    //!                      - async_type::immediate:
    //!                      xfer will be started as soon as possible.
    //!                      - async_type::deferred:
    //!                      xfer will be postponed until xfer_trigger() call
    //! \retval    err::ok   Data is sent successfully.
    //! \retval    err::busy Device is still executing async xfer.
    //! \retval    err       Any other error that can occur in platform bus.
    //!
    static err xfer(const bus_handler &handler, async_type type = async_type::immediate);

    //! Triggers continued or deffered xfer in async mode.
    //! \details Method is capable of triggering two types of xfers:
    //!  - Continuated:
    //!    triggers a new xfer with the same buffers and handlers that were used
    //!    during last xfer.
    //!  - Deffered:
    //!    triggers the scheduled xfer, such that was defer using xfer() call.
    //! \details This method is a kind of a important optimization trick. It is
    //! possible to call it from IRQ context. It uses buffers and handlers
    //! from previous xfers, so there is no overhead for unneeded assignments.
    //! \pre       Bus is locked, buffers are set and async xfer is either
    //!            defered or executed.
    //! \post      Xfer is ongoing.
    //!
    static err trigger_xfer();

    //! Returns a reference to underlying platform bus.
    //! \details   This allows to perform specific for underlying bus operations.
    //! \retval    PBus&    Reference to underlying platform bus.
    //!
    static PBus& platform_handle();

private:
    //! Convinient alias.
    using atomic_flag   = std::atomic_flag;

    //! Event handler dedicated to the platform bus.
    //! \param[in] ch     Channel (rx or tx) where the event occurred.
    //! \param[in] type   Type of event occurred.
    //! \param[in] total  Total amount of bytes transferred within given channel.
    //!
    static void platform_handler(bus_channel ch, bus_event type, size_t total);

    //! Checks if bus is busy transferring data at this moment or not.
    //! \retval true Bus is busy.
    //!
    static bool bus_is_busy();

    //! Performs cleanup required after unlocking and delivering an event.
    static void cleanup();

    //! Lock proxy to protect a platform bus.
    static mutex& mut();

    //! Semaphore proxy to notify about end of the xfer.
    static binary_semaphore& sem();

    //! User-supplied handler proxy, used in async mode.
    static bus_handler& cb();

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

    static size_t       m_received; //!< Bytes received during last blocking xfer.
    static size_t       m_sent;     //!< Bytes sent during last blocking xfer.
    static atomic_flag  m_cleaned;  //!< Cleanup is performed after xfer and unlock are done.
    static uint8_t      m_state;    //!< State flags.
};

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

    platform_handle().set_handler(platform_handler);

    auto rc = platform_handle().init();

    if (is_ok(rc)) {
        m_state |= bus_inited;
    }

    // Call these methods here to guarantee that
    // all static objects were allocated before first use
    mut();
    cb();
    sem();

    return rc;
}

template< class PBus >
err generic_bus< PBus >::deinit()
{
    if (!(m_state & bus_inited)) {
        return err::perm;
    }

    platform_handle().reset_handler();
    cleanup();
    m_state = 0;

    return err::ok;
}

template< class PBus >
void generic_bus< PBus >::lock()
{
    // If bus is not initialized then pre-conditions are violated.
    ecl_assert(m_state & bus_inited);

    mut().lock();

    m_state |= bus_locked;

    // Bus may be busy at this moment, wait until it finished most
    // recent transaction.
    if (m_state & async_mode) {
        sem().wait();
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

    if (m_state & async_mode) {
        if (m_state & xfer_served) {
            // Cleanup routine is a critical section and both platform_handler()
            // and unlock() routine can try to access it concurrently.
            // Wrapping this part with mutexes must be avoided
            // because platform_handler() will likely be executed in ISR context.
            // Using atomics is most convenient solution.
            if (!m_cleaned.test_and_set()) {
                cleanup();
            }
        }
    } else {
        // Do cleanup in block mode - bus is guaranteed to finish all transaction
        // prior to unlock() call
        cleanup();
    }

    mut().unlock();
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

    platform_handle().reset_buffers();
    platform_handle().set_tx(tx, size);
    platform_handle().set_rx(rx, size);

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

    platform_handle().reset_buffers();
    platform_handle().set_tx(size, fill_byte);
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
    sem().try_wait();

    // Reset transfer counters
    m_received = m_sent = 0;

    auto rc = platform_handle().do_xfer();

    if (is_ok(rc)) {
        sem().wait();

        // Errors can occur after transaction start, check this
        if (m_state & xfer_error) {
            rc = err::io;
        }

        // Return amount of bytes written and/or read.
        // Even if error occurred, some data was possibly transferred.

        if (received) {
            *received = m_received;
        }
        if (sent) {
            *sent = m_sent;
        }

    } else {
        // Deem that xfer virtually occurs in blocking mode and thus
        // momentarily served in case of error.
        m_state |= xfer_served;
    }

    return rc;
}

template< class PBus >
ecl::err generic_bus< PBus >::xfer(const bus_handler &handler, async_type type)
{
    // If bus is not locked then pre-conditions are violated
    // and it is clearly a sign of a bug
    ecl_assert(m_state & bus_locked);

    if (bus_is_busy()) {
        return err::busy;
    }

    cb() = handler;

    if (type == async_type::deferred) {
        // Xfer will be executed when upon user's will.
        return err::ok;
    }

    return trigger_xfer();
}

template< class PBus >
ecl::err generic_bus< PBus >::trigger_xfer()
{
    ecl_assert(m_state & bus_locked);
    ecl_assert(!bus_is_busy()); // Violating of pre-conditions

    // Async mode xfer is provided
    m_state |= async_mode;

    m_cleaned.clear();

    auto rc = platform_handle().do_xfer();

    if (is_error(rc)) {
        // Deem that xfer virtually occurs in blocking mode and thus
        // momentarily served in case of error.
        // No need to signal semaphore here, since any pending client
        // (i.e. other thread) is blocked waiting on mutex and do not yet
        // discovered xfer mode. See lock() for details.
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
    // Transfer complete across all channels
    bool last_event = (ch == bus_channel::meta && type == bus_event::tc);

    if (type == bus_event::err) {
        m_state |= xfer_error;
    }

    if (last_event) {
        // Spurious events are not allowed
        // TODO #27: platform assert here is required since this routine
        // is likely will be executed in IRQ context
        ecl_assert(!(m_state & xfer_served));

        m_state |= xfer_served;
    }

    if (m_state & async_mode) {
        cb()(ch, type, total);

        // User is granted to continue xfer when handling the last bus event,
        // thus xfer_served flag will be cleared. It is called 'continuous xfer'.
        // During continuous xfers any client waiting for a bus lock will remain
        // blocked until current bus client will finish all its xfers.
        // TODO: assert that user start new xfer _only_ during last event handling.
        // TODO #27: platform assert here is required since this routine
        // is likely will be executed in IRQ context
        last_event = (m_state | xfer_served);

        if (last_event && !(m_state & bus_locked)) {
            // Bus is unlocked, it is time to check if bus cleaned.

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
        // In blocking mode a bus is responsible for bytes counting.
        if (ch == bus_channel::tx) {
            m_sent = total;
        } else if (ch == bus_channel::rx) {
            m_received = total;
        }

        // Don't do anything for 'meta' channel
    }

    if (last_event) {
        // Inform rest of the bus about event handling
        sem().signal();
    }
}

template< class PBus >
bool generic_bus< PBus >::bus_is_busy()
{
    // Asynchronous operation still in progress.
    return (m_state & async_mode) && !(m_state & xfer_served);
}

template< class PBus >
void generic_bus< PBus >::cleanup()
{
    platform_handle().reset_buffers();
    cb() = bus_handler{};
    // When bus will be locked again, no need to wait for events.
    m_state &= ~(async_mode);
}

template< class PBus >
mutex& generic_bus< PBus >::mut()
{
    static mutex m;
    return m;
}

template< class PBus >
binary_semaphore& generic_bus< PBus >::sem()
{
    static binary_semaphore s;
    return s;
}

template< class PBus >
bus_handler& generic_bus< PBus >::cb()
{
    static bus_handler bh;
    return bh;
}

template< class PBus >
PBus& generic_bus< PBus >::platform_handle()
{
    static PBus pb;
    return pb;
}
}

#endif
