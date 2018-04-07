/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Module aggregates routines that are control execution flow on the host platform.
//!
#ifndef THE_CORE_HOST_PLATFORM_EXECUTION_HPP_
#define THE_CORE_HOST_PLATFORM_EXECUTION_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <thread>
#include <chrono>

namespace ecl
{

//! \brief Aborts execution of currently running code. Never return.
static inline void abort()
{
    ::abort();
}

//! \brief Performs a dummy busy wait for specified amount of milliseconds.
//! \param ms number of milliseconds to wait.
//!
//! This function is useful for a short delays.
//!
//! \return None.
static inline void spin_wait(unsigned ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace ecl

#endif // THE_CORE_HOST_PLATFORM_EXECUTION_HPP_
