/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup ke02 Freescale FRDM-KE02Z platform
//! @{

//! \defgroup ke02_console Console
//! @{

//! \file
//! \brief KE02 bypass console implementation.

#include "platform/console.hpp"

#include <type_traits>

#include <common.h> // Test if we need this or not.
#include <uart.h>

namespace ecl
{

void bypass_console_init()
{
    UART_ConfigType sConfig;

    sConfig.u32SysClkHz = BUS_CLK_HZ;
    sConfig.u32Baudrate  = UART_PRINT_BITRATE; // 115200

    UART_Init(platform_console::pick_sysctl(), &sConfig);
}

void bypass_putc(char c)
{
    UART_PutChar(platform_console::pick_sysctl(), c);
}

} // namespace ecl

//! @}

//! @}

//! @}
