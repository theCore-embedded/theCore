/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Mock IRQ header for FreeRTOS semaphore implementation.

#ifndef FREERTOS_COMMON_IRQ_HPP_
#define FREERTOS_COMMON_IRQ_HPP_

namespace ecl
{

namespace irq
{

bool in_isr()
{
    return false;
}

} // namespace irq

} // namespace ecl

#endif // FREERTOS_COMMON_IRQ_HPP_
