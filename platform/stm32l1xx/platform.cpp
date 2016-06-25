#include "platform_console.hpp"
#include "platform/execution.h"

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

#if CONFIG_BYPASS_CONSOLE_ENABLED
    bypass_console_init();
#endif // CONFIG_BYPASS_CONSOLE_ENABLED
}

