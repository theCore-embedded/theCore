/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Common bypas console implementation.

#ifndef PLATFORM_COMMON_CONSOLE_HPP_
#define PLATFORM_COMMON_CONSOLE_HPP_

#include <platform/console.hpp>

namespace ecl
{

//! Bypasses console drivers and puts string directly to the UART.
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
