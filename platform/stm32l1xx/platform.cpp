#include "platform/execution.h"
#include "common/irq.hpp"

#include <stm32l1xx_rcc.h>
#include <stm32l1xx_usart.h>

#if CONFIG_BYPASS_CONSOLE_ENABLED
extern void bypass_console_init();
#endif // CONFIG_BYPASS_CONSOLE_ENABLED


/* Required for STM32 Peripherial library */
extern "C"
void assert_param(int exp)
{
#ifdef NDEBUG
    (void)exp;
#else
    if (!exp) {
        ecl_abort();
    }
#endif
}

extern "C" void platform_init()
{
    // IRQ must be ready before anything else will start work
    ecl::irq::init_storage();

    // Update clocks so it is visible to the rest of the system
    SystemCoreClockUpdate();

    // Initialize DWT, used by platform to count ticks.
    // See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#if CONFIG_BYPASS_CONSOLE_ENABLED
    bypass_console_init();
#endif // CONFIG_BYPASS_CONSOLE_ENABLED
}

