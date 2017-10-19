/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Particle Serial drivers configuration template
#ifndef PLATFORM_SERIAL_CFGS_HPP_
#define PLATFORM_SERIAL_CFGS_HPP_

#include <aux/uart_bus.hpp>

namespace ecl
{

// Serial devices definitions

using serial_device0 = uart_bus<uart_device::serial0>;
using serial_device2 = uart_bus<uart_device::serial2>;
using serial_device3 = uart_bus<uart_device::serial3>;

// Console definitions (if enabled)


//! Initializes bypass console.
static inline void bypass_init()
{
}

//! Puts character to bypass console. Does nothing if console is disabled.
static inline void bypass_putc(char c)
{
    (void)c;
}

} // namespace ecl

#endif // PLATFORM_SERIAL_CFGS_HPP_
