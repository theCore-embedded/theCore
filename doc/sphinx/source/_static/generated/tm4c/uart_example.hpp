/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C UART definitions template

#ifndef TM4C_UART_DEFINES_
#define TM4C_UART_DEFINES_

#include "aux/platform_defines.hpp"
#include "aux/uart.hpp"



namespace ecl
{

// UART configuration ---------------------------------------------------------


/* UART-over-USB console output */

static constexpr uart_channel UART0_channel = uart_channel::ch0;
using UART0_driver = uart<UART0_channel>;

using test_uart = UART0_driver;

/* Example UART */

static constexpr uart_channel UART1_channel = uart_channel::ch1;
using UART1_driver = uart<UART1_channel>;

using another_uart = UART1_driver;

/* Console configuration */

static constexpr auto console_channel = UART1_channel;
using platform_console = UART1_driver;


} // namespace ecl

#endif // TM4C_UART_DEFINES_
