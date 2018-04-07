/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "arch/execution.hpp"

#if THECORE_ENABLE_SYSTMR_API

namespace ecl
{

namespace systmr
{

static volatile uint32_t event_cnt;

uint32_t events()
{
    return event_cnt;
}

} // namespace systmr

} // namespace ecl

//------------------------------------------------------------------------------

#if !defined(THECORE_OWNS_SYSTMR) || !THECORE_OWNS_SYSTMR
// User can override systimer handler to receive events on its side.
extern "C" void systmr_handler(void) __attribute__((weak));

extern "C" void systmr_handler(void)
{
}

#endif // !defined(THECORE_OWNS_SYSTMR) || !THECORE_OWNS_SYSTMR

extern "C" void SysTick_Handler(void)
{
    ecl::systmr::event_cnt++;
#if !defined(THECORE_OWNS_SYSTMR) || !THECORE_OWNS_SYSTMR
    systmr_handler();
#endif // !defined(THECORE_OWNS_SYSTMR) || !THECORE_OWNS_SYSTMR
}


#endif // THECORE_ENABLE_SYSTMR_API
