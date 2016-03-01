#ifndef DEV_BUS_BUS_HPP_
#define DEV_BUS_BUS_HPP_

#include <ecl/err.hpp>
#include <ecl/thread/mutex.hpp>
#include <functional>

namespace ecl
{

//!
//! \brief Generic bus interface.
//! The generic bus is useful adapter, that allows to:
//! \li Encapsulate locking policy when multithreded environment is used.
//! \li Hide differences between full-duplex and half-duplex busses.
//! \li Define and simplify platform-level bus interface
//! \tparam Bus Plaform-level bus driver (I2C, SPI, etc.)
//!
template< class Bus >
class generic_bus
{
public:
    //!
    //! \brief Events that are passed via handler.
    //! \sa handler
    //!
    enum class event
    {
        tx_half_complete  = Bus::event::tx_half_complete,
        rx_half_complete  = Bus::event::rx_half_complete,
        tx_complete       = Bus::event::tx_complete,
        rx_complete       = Bus::event::rx_complete,
        tx_err            = Bus::event::tx_err,
        rx_err            = Bus::event::rx_err,
    };

    //!
    //! \brief Event handler type.
    //! User can provide a function object in order to handle events from a bus.
    //! \sa xfer()
    //!
    using handler_fn = std::function< void(event type) >;

    //!
    //! \brief Constructs a bus.
    //!
    generic_bus();

    //!
    //! \brief Destructs a bus.
    //!
    ~generic_bus();

    //!
    //! \brief Inits a bus.
    //! Lazy initialization. Inits an underlaying platform bus.
    //! \return Status of operation.
    //!
    err init();

    //!
    //! \brief Locks a bus.
    //! Any further operations can be executed after call to this function.
    //! If previous async xfer is in progress then current thread will be blocked
    //! until its finish.
    //!
    //! \pre    Bus is inited successfully and unlocked.
    //! \post   Bus is locked.
    //! \sa     unlock()
    //!
    void lock();

    //!
    //! \brief Unlocks a bus.
    //! Any operations beside lock() is not permitted after this method finishes.
    //!
    //! \par Side effects:
    //! \li In block mode all buffers provided with set_buffers() will be discarded
    //! \li In async mode if opration still ongoing, buffers will be discarded
    //!     after operation will finish.
    //!
    //! \pre    Bus is locked.
    //! \post   Bus is unlocked.
    //! \sa     set_buffers()
    //!
    void unlock();

    //!
    //! \brief Sets RX and TX buffers and their sizes.
    //! If only TX or RX transaction required, then only one buffer must
    //! be passed. All effects from calls to set_buffers() will be discarded.
    //!
    //! \par Side effects:
    //! \li Bus will remember all buffers, until unlock() or set_buffers()
    //!     will be called.
    //!
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
    err set_buffers(const uint8_t *tx, uint8_t *rx, size_t size);

    //!
    //! \brief Sets TX buffer size and fills it with given byte.
    //! This will instruct a platform bus to send a byte given number of times.
    //! It is implementation defined how bis is chunks in which data is sent.
    //! If possible, platform bus will just send single-byte buffer via DMA.
    //!
    //! In half-duplex case RX will not be performed. If platform bus is in
    //! full duplex mode then RX will be exeucted but RX data will be ignored.
    //!
    //! \par Side effects:
    //! \li Bus will remember a buffer, until unlock() or set_buffers()
    //!     will be called.
    //!
    //! \pre       Bus is locked.
    //! \post      Bus is ready to execute xfer.
    //! \param[in] size         Size of filled buffer.
    //! \param[in] fill_byte    Byte which will be sent in TX stream.
    //! \retval    err::ok      Buffer successfully set and filled.
    //! \retval    err::busy    Device is still executing async xfer.
    //!
    err set_buffers(size_t size, uint8_t fill_byte = 0xff);

    //!
    //! \brief Performs xfer in blocking mode using buffers set previously.
    //! \pre       Bus is locked and buffers are set.
    //! \post      Bus remains in the same state.
    //! \retval    err::ok      Data is sent successfully.
    //! \retval    err::busy    Device is still executing async xfer.
    //! \retval    err          Any other error that can occur in platform bus
    //!
    err xfer();

    //!
    //! \brief xfer
    //! \pre
    //! \post
    //! \return
    //!
    err xfer(const handler_fn &handler);

private:

    //!
    //! \brief Event handler dedicated to the platform bus.
    //! \param[in] obj  This object.
    //! \param[in] type Type of event occured.
    //!
    static void bus_handler(generic_bus< Bus > *obj, typename Bus::event type);

    Bus         m_bus;      //!< Platform bus object.
    ecl::mutex  m_lock;     //!< Lock to protect a platform bus.
    bool        m_async;    //!< Flag holding an operation mode: sync or async
};


//------------------------------------------------------------------------------

template< class Bus >
generic_bus< Bus >::generic_bus()
    :m_bus{}
    ,m_lock{}
    ,m_async{false}
{

}

template< class Bus >
generic_bus< Bus >::~generic_bus()
{

}

template< class Bus >
err generic_bus< Bus >::init()
{
    auto fn = [this](typename Bus::event type) {
        this->bus_handler(this, type);
    };

    m_bus.set_handler(fn);
    return m_bus.init();
}

template< class Bus >
void generic_bus< Bus >::lock()
{
    m_lock.lock();

    if (m_async) {
        // TODO: implement a wait
    }
}

template< class Bus >
void generic_bus< Bus >::unlock()
{
    // Buffers should be reset in the bus_handler()
    if (!m_async) {
        m_bus.reset_buffers();
    }

    m_lock.unlock();
}

template< class Bus >
ecl::err generic_bus< Bus >::set_buffers(const uint8_t *tx, uint8_t *rx, size_t size)
{
    if (!tx && !rx) {
        return err::inval;
    }

    // TODO: check async flag, if set - error

    m_bus.reset_buffers();
    m_bus.set_tx(tx, size);
    m_bus.set_rx(rx, size);
    return err::ok;
}

template< class Bus >
ecl::err generic_bus< Bus >::set_buffers(size_t size, uint8_t fill_byte)
{
    // TODO: check async flag, if set - error

    m_bus.reset_buffers();
    m_bus.set_tx(size, fill_byte);
    return err::ok;
}

template< class Bus >
ecl::err generic_bus< Bus >::xfer()
{

}

//------------------------------------------------------------------------------

template< class Bus >
void generic_bus< Bus >::bus_handler(generic_bus< Bus > *obj, typename Bus::event type)
{
    (void) obj;
    (void) type;
}

}

#endif
