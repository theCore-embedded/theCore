/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef KE02_PLATFORM_CONSOLE_HPP_
#define KE02_PLATFORM_CONSOLE_HPP_

extern "C" {
#include <io.h>
}

namespace ecl
{

//! Bypasses console drivers and puts data directly to the UART
static inline void bypass_putc(char c)
{
    out_char(c);
}

} // namespace ecl


#endif // KE02_PLATFORM_CONSOLE_HPP_
