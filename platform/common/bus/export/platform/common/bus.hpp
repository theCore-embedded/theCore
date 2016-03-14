//!
//! \file
//! \brief \todo
//! \copyright
//!

#ifndef PLATFORM_COMMON_BUS_
#define PLATFORM_COMMON_BUS_

#include <ecl/err.hpp>

#include <functional>

namespace platform
{

namespace common
{

//!
//! \brief Channles of a bus.
//!
enum class bus_channel
{
    rx,         //!< Receive channel.
    tx,         //!< Transmit channel.
    meta,       //!< Meta-channel.
};

//!
//! \brief Various events.
//!
enum class bus_event
{
    ht,         //!< Half transfer event.
    tc,         //!< Transfer complete event.
    err,        //!< Error event.
};

//!
//! \brief Event handler type.
//! User can provide a function object in order to handle events from a bus.
//! \param[in] ch    Channel where event occurred.
//! \param[in] type  Type of the event.
//! \param[in] total Bytes transferred trough given channel
//!                  during current xfer.
//! \sa generic_bus::xfer()
//!
using bus_handler = std::function< void(bus_channel ch, bus_event type, size_t total) >;

//!
//! \brief The dummy bus class.
//! Provided just as an API example of platform-level bus.
//!
class dummy_platform_bus
{
public:
    //!
    //! \brief Lazy initialization.
    //! \return Status of opeartion.
    //!
    ecl::err init()
    { return ecl::err::nosys; }

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    void set_rx(uint8_t *rx, size_t size)
    { (void) rx; (void) size; }

    //!
    //! \brief Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    //!
    void set_tx(size_t size, uint8_t fill_byte = 0xff)
    { (void) size; (void) fill_byte; }

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    void set_tx(const uint8_t *tx, size_t size)
    { (void) tx; (void) size; }

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    void set_handler(const bus_handler &handler)
    { (void) handler; }

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    void reset_buffers()
    { return; }

    //!
    //! \brief Resets previously set handler.
    //!
    void reset_handler()
    { return; }

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    ecl::err do_xfer()
    { return ecl::err::nosys; }
};

}

}

#endif // PLATFORM_COMMON_BUS_
