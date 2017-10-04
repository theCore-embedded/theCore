/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_THREAD_FREERTOS_OS_SIGNALS_
#define LIB_THREAD_FREERTOS_OS_SIGNALS_

#include "utils.hpp"

#include <ecl/err.hpp>

namespace ecl
{

namespace os
{

// TODO: little more explanation about that signals act like task-bound
// binary semaphores.
namespace signal
{

//!
//! \brief Sends a signal to a given thread.
//! It is possible to send a signal even from ISR.
//! Any signal that was sent previously to the same thread
//! will be discarded.
//! \param[in] handle Represents a target thread.
//! \return Status of operation.
//!
ecl::err send(const thread_handle &handle);

//!
//! \brief Clears pending signal for current thread.
//! Can't be called from ISR context.
//!
void clear();

//!
//! \brief Waits for signal for current thread.
//! Can't be called from ISR context.
//!
void wait();

//!
//! \brief Tries to wait a signal.
//! Can't be called from ISR context.
//! \retval err::again  No signal avaliable. Retry again.
//! \retval err::ok     Signal consumed.
//!
ecl::err try_wait();


} // namespace signal

} // namespace os

} // namespace ecl

#endif // LIB_THREAD_FREERTOS_OS_SIGNALS_
