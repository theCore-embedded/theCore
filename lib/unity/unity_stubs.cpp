//! \file
//! \brief Stubs implementation for Unity functions.

#include <platform/console.hpp>

extern "C" void unity_putc(char c)
{
    ecl::bypass_putc(c);
}
