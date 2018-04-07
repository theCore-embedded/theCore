/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C bypass console driver
//! \ingroup tm4c_console
#ifndef TM4C_CONSOLE_DRIVER_HPP_
#define TM4C_CONSOLE_DRIVER_HPP_

// All console definitions are generated in the USART config header
#include <aux/uart_cfg.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_console Console
//! @{


#if !THECORE_CONFIG_USE_CONSOLE

//! Does nothing if console is disabled.
static inline void bypass_putc(char c)
{
    (void) c;
}

#else // THECORE_CONFIG_USE_CONSOLE

//! Bypasses console drivers and puts data directly to the UART.
//! \details Required to print debug of the failed asserts including one that
//! executed during ISR.
void bypass_putc(char c);

#endif // THECORE_CONFIG_USE_CONSOLE

//! @}

//! @}

//! @}

} // namespace ecl

#endif // TM4C_CONSOLE_DRIVER_STUB_HPP_
