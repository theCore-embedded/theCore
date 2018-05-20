/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief KE02 execution control header.
//! \ingroup ke02_execution

#ifndef KE02_PLATFORM_EXECUTION_H_
#define KE02_PLATFORM_EXECUTION_H_

#include <stdint.h>
#include <arch/execution.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup ke02 KE02 platform
//! @{

//! \defgroup ke02_execution Execution control module
//! @{

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

//! @}

//! @}

//! @}

} //namespace ecl

#endif // KE02_PLATFORM_EXECUTION_H_
