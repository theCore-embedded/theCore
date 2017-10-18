/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C bypass console implementation.

#include "platform/console.hpp"

#include <type_traits>

namespace ecl
{

void bypass_console_init()
{
    constexpr auto uart_dev =
        static_cast<std::underlying_type_t<uart_device>>(ecl::bypass_console_dev);

    SysCtlPeripheralEnable(bypass_console::pick_sysctl());

    UARTConfigSetExpClk(uart_dev, SysCtlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);
}

void bypass_putc(char c)
{
    constexpr auto uart_dev =
        static_cast<std::underlying_type_t<uart_device>>(ecl::bypass_console_dev);
    UARTCharPut(uart_dev, c);
}

} // namespace ecl
