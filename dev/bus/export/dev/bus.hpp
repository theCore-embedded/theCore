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
    using handler = std::function< void(void *ctx, event type) >;

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
    //! \pre    Bus is in unlocked state.
    //! \post   Bus is locked.
    //! \sa     unlock()
    //!
    void lock();

    //!
    //! \brief Unlocks a bus.
    //! Any operations beside unlock() is not permitted after this method finishes.
    //! Buffers that were provided by a user will be no longer valid.
    //! This means that if one will lock this bus afterwards then it should call
    //! set_buffers() again.
    //! \pre    Bus is locked.
    //! \post   Bus is unlocked.
    //! \sa     set_buffers()
    //!
    void unlock();

    //!
    //! \brief set_buffers
    //! \pre
    //! \post
    //! \param[in] tx
    //! \param[in] rx
    //! \param[in] size
    //! \return
    //!
    err set_buffers(const uint8_t *tx, uint8_t *rx, size_t size);

    //!
    //! \brief set_buffers
    //! \pre
    //! \post
    //! \param[in] count
    //! \param[in] fill_byte
    //! \return
    //!
    err set_buffers(size_t count, uint8_t fill_byte = 0xff);

    //!
    //! \brief xfer
    //! \pre
    //! \post
    //! \return
    //!
    err xfer();

    //!
    //! \brief xfer
    //! \pre
    //! \post
    //! \return
    //!
    err xfer(const handler &event_handler);

private:
    //!
    //! \brief Event handler dedicated to the platform bus.
    //! \param[in] ctx  Custom context.
    //! \param[in] type Type of event occured.
    //!
    static void bus_handler(void *ctx, typename Bus::event type);

    Bus         m_bus;      //!< Platform bus object.
};


//------------------------------------------------------------------------------

template< class Bus >
generic_bus< Bus >::generic_bus()
    :m_bus{}
{

}

template< class Bus >
generic_bus< Bus >::~generic_bus()
{

}

template< class Bus >
err generic_bus< Bus >::init()
{
    return m_bus.init();
}

template< class Bus >
void generic_bus< Bus >::lock()
{

}

template< class Bus >
void generic_bus< Bus >::unlock()
{

}




}

#endif
