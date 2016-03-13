//!
//! \file
//! \brief \todo
//! \copyright
//!

#ifndef PLATFORM_COMMON_BUS_
#define PLATFORM_COMMON_BUS_

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

}

}

#endif // PLATFORM_COMMON_BUS_
