/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cstdint>
#include <sysctl.h>

#include "platform/exti_manager.hpp"

#ifdef THECORE_CONFIG_USE_CONSOLE
namespace ecl
{
extern void bypass_console_init();
} // namespace ecl
#endif // THECORE_CONFIG_USE_CONSOLE

// Required by ARM ARCH startup code
extern "C" void SystemInit()
{
    auto cfg = SYSCTL_USE_OSC | // System clock is the osc clock (not PLL)
               SYSCTL_OSC_INT | // Osc clock is internal oscillator (PIOSC)
               SYSCTL_MAIN_OSC_DIS  | // Disable main oscillator
               SYSCTL_SYSDIV_2; // Processor clock is OSC / 2

    SysCtlClockSet(cfg);

    // Initialize DWT, used by platform to count ticks.
    // See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

extern "C" void platform_init()
{
    // TODO: implement

    // EXTI manager must be ready after IRQ, but before user code start working with it
    ecl::exti_manager::init();

#ifdef THECORE_CONFIG_USE_CONSOLE
    ecl::bypass_console_init();
#endif
}

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
