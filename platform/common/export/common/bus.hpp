/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Platform bus common interface.
#ifndef PLATFORM_COMMON_BUS_
#define PLATFORM_COMMON_BUS_

#include <ecl/err.hpp>

#include <functional>

namespace ecl
{

//! Channels of a bus.
enum class bus_channel
{
    rx,         //!< Receive channel.
    tx,         //!< Transmit channel.
    meta,       //!< Meta-channel.
};

//! Various events.
enum class bus_event
{
    ht,         //!< Half transfer event.
    tc,         //!< Transfer complete event.
    err,        //!< Error event.
};

//! Defines async xfer types.
enum class async_type
{
    deferred,   //!< Xfer going to be deferred until user asks.
    immediate,  //!< Xfer will be executed as soon as possible.
};

//! Event handler type.
//! \details User can provide a function object in order to handle events
//! from a bus.
//! \param[in] ch    Channel where event occurred.
//! \param[in] type  Type of the event.
//! \param[in] total Bytes transferred trough given channel
//!                  during current xfer.
//! \sa generic_bus::xfer()
//!
using bus_handler = std::function<void(bus_channel ch, bus_event type, size_t total)>;

} // namespace ecl


#endif // PLATFORM_COMMON_BUS_
