/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief KE02 bypass console driver
//! \ingroup ke02_console
#ifndef KE02_PLATFORM_CONSOLE_HPP_
#define KE02_PLATFORM_CONSOLE_HPP_

// All console definitions are generated in the USART config header
#include <aux/uart_cfg.hpp>

#include <common.h> // Test if we need this or not.
#include <uart.h>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup ke02 Freescale FRDM-KE02Z platform
//! @{

//! \defgroup ke02_console Console
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

#endif // KE02_PLATFORM_CONSOLE_HPP_
