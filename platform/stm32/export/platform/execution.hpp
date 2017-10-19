/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//!
#ifndef THE_CORE_EXECUTION_HPP_
#define THE_CORE_EXECUTION_HPP_

// Order matters
#include <stm32_device.hpp>
#include <arch/execution.hpp>

namespace ecl
{
//! \brief Performs a dummy busy wait for specified amount of milliseconds.
//! \param ms number of milliseconds to wait.
//!
//! This function is useful for a short delays.
//!
//! \return None.
static inline void spin_wait(uint32_t ms)
{
    ecl::arch_spin_wait(ms);
}

} //namespace ecl

// TODO #213 add RTC support

#endif // THE_CORE_EXECUTION_HPP_
