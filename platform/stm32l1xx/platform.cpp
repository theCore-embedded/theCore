#include "platform_console.hpp"
#include "platform/execution.h"

#include <stm32l1xx_rcc.h>
#include <stm32l1xx_usart.h>


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

namespace ecl
{
    extern void bypass_console_init();
}

extern "C" void platform_init()
{
    ecl::bypass_console_init();
}

