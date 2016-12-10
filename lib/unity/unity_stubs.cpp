//! \file
//! \brief Stubs implementation for Unity functions.

#include <platform/console.hpp>

extern "C" void unity_putc(char c)
{
    if (c == '\n') {
        ecl::bypass_putc('\r');
    }

    ecl::bypass_putc(c);
}
