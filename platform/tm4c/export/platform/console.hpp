/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C bypass console driver
#ifndef TM4C_CONSOLE_DRIVER_HPP_
#define TM4C_CONSOLE_DRIVER_HPP_

// All console definitions are generated in the USART config header
#include <aux/uart_cfg.hpp>

namespace ecl
{

#if !CORE_CONFIG_USE_BYPASS_CONSOLE

//! Does nothing if console is disabled.
static inline void bypass_putc(char c)
{
    (void) c;
}

#else // CORE_CONFIG_USE_BYPASS_CONSOLE

//! Bypasses console drivers and puts data directly to the UART.
//! \details Required to print debug of the failed asserts including one that
//! executed during ISR.
void bypass_putc(char c);

// //! Console device descriptor.
// #define ECL_MACRO_CONSOLE_CONCAT(prefix, num) (prefix ## num)
// #define ECL_MACRO_CONSOLE_EVAL(prefix, num) ECL_MACRO_CONSOLE_CONCAT(prefix, num)
// #define ECL_CONSOLE_DEVICE ECL_MACRO_CONSOLE_EVAL(uart_device::dev, CORE_CONFIG_CONSOLE_DEVICE_NUM)

// //! The same console driver will be used for whole theCore.
// using platform_console = bypass_console;
// -using bypass_console = uart_bus<ECL_CONSOLE_DEVICE>;
#endif // CORE_CONFIG_USE_BYPASS_CONSOLE

} // namespace ecl

#endif // TM4C_CONSOLE_DRIVER_STUB_HPP_
