/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef KE02_PLATFORM_CONSOLE_HPP_
#define KE02_PLATFORM_CONSOLE_HPP_

#include "uart.h"

namespace ecl
{

//! Bypasses console drivers and puts data directly to the UART
static inline void bypass_putc(char c)
{
    // TODO: use theCore UART driver instead of UART periphery directly

    UART_PutChar(TERM_PORT, c);
}

static inline void bypass_console_init()
{
    // TODO: use theCore UART driver instead of UART periphery directly

    UART_ConfigType sConfig;

    sConfig.u32SysClkHz = BUS_CLK_HZ;
    sConfig.u32Baudrate  = UART_PRINT_BITRATE;

    UART_Init(TERM_PORT, &sConfig);
}

} // namespace ecl


#endif // KE02_PLATFORM_CONSOLE_HPP_
