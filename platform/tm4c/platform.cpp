/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cstdint>
#include <sysctl.h>

#include <common/irq.hpp>

#include "platform/exti_manager.hpp"

#ifdef CORE_CONFIG_USE_BYPASS_CONSOLE
namespace ecl
{
extern void bypass_console_init();
} // namespace ecl
#endif // CORE_CONFIG_USE_BYPASS_CONSOLE

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

    // IRQ must be ready before anything else will start work
    ecl::irq::init_storage();

    // EXTI manager must be ready after IRQ, but before user code start working with it
    ecl::exti_manager::init();

#ifdef CORE_CONFIG_USE_BYPASS_CONSOLE
    ecl::bypass_console_init();
#endif
}


namespace ecl
{

#if USE_SYSTMR

namespace systmr
{

static volatile uint32_t event_cnt;

uint32_t events()
{
    return event_cnt;
}

} // namespace systmr

#endif // USE_SYSTMR

} // namespace ecl

#if USE_SYSTMR

// User can override systimer handler to receive events on its side.
extern "C" void systmr_handler(void) __attribute__((weak));

extern "C" void systmr_handler(void)
{
}

extern "C" void SysTick_Handler(void)
{
    ecl::systmr::event_cnt++;
    systmr_handler();
}

#endif // USE_SYSTMR
