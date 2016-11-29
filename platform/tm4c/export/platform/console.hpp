//! \file
//! \brief TM4C bypass console driver
#ifndef TM4C_CONSOLE_DRIVER_HPP_
#define TM4C_CONSOLE_DRIVER_HPP_

#ifndef CORE_CONFIG_USE_CONSOLE

namespace ecl
{

//! Does nothing if console disabled.
static inline void bypass_putc(char c)
{
    (void) c;
}

} // namespace ecl

#else // CORE_CONFIG_USE_CONSOLE

#include "aux/uart_bus.hpp"

namespace ecl
{

//! Bypasses console drivers and puts data directly to the UART.
//! \details Required to print debug of the failed asserts including one that
//! executed during ISR.
void bypass_putc(char c);

//! Console device descriptor.
#define ECL_MACRO_CONSOLE_CONCAT(prefix, num) (prefix ## num)
#define ECL_MACRO_CONSOLE_EVAL(prefix, num) ECL_MACRO_CONSOLE_CONCAT(prefix, num)
#define ECL_CONSOLE_DEVICE ECL_MACRO_CONSOLE_EVAL(uart_device::dev, CORE_CONFIG_CONSOLE_DEVICE_NUM)

//! Console platform driver instance.
using platform_console = uart_bus<ECL_CONSOLE_DEVICE>;

} // namespace ecl

#endif // CORE_CONFIG_USE_CONSOLE


#endif // TM4C_CONSOLE_DRIVER_STUB_HPP_
