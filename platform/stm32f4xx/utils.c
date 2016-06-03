//! \file
//! \brief Platform-level utilities

#include <platform/execution.h>

/* Required for STM32 Peripherial library */
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
