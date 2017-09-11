//! \file
//! \brief Common bypas console implementation.

#ifndef PLATFORM_COMMON_CONSOLE_HPP_
#define PLATFORM_COMMON_CONSOLE_HPP_

#include <platform/console.hpp>

namespace ecl
{

static inline void bypass_puts(const char *str)
{
    char ch;
    while ((ch = *str++)) {
        if (ch == '\n') {
            bypass_putc('\r');
        }
        bypass_putc(ch);
    }
}

} // namespace ecl

#endif // PLATFORM_COMMON_CONSOLE_HPP_
