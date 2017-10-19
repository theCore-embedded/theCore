/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Various routines for particle electron platform, altering execution flow.

#ifndef PLATFORM_EXECUTION_HPP_
#define PLATFORM_EXECUTION_HPP_

#include <stdint.h>
#include <application.h>
#include "aux/platform_defines.hpp"

namespace ecl
{

//! Waits not less than given amount of milliseconds.
static inline void spin_wait(unsigned ms)
{
    ::delay(ms);
}

//! Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void abort()
{
    // TODO: disable interrupts
    for(;;);
}

namespace systmr
{

 //! Enables timer, start counting
void enable();

//! Disables timer, stop counting
void disable();

//! Gets speed in which timer generate events.
//! \details _Each event generates interrupt_, but not necessarily timer interrupt
//! is passed to the user. In other words, each event will wake-up processor.
//! Useful in conjunction with WFI.
//! \retval System timer events generation speed in Hz.
uint32_t speed();

//! Gets amount of events occurred so far.
//! \details Events counter may not reset after disable() call.
//! \retval Amount of events occured so far
uint32_t events();

} // namespace systmr

} // namespace ecl

#endif // PLATFORM_EXECUTION_HPP_
