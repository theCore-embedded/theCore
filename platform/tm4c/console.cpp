//! \file
//! \brief TM4C bypass console implementation.

#include "platform/console.hpp"

#include <type_traits>

namespace ecl
{

void bypass_console_init()
{
    constexpr auto uart_dev = static_cast<std::underlying_type_t<uart_device>>(ECL_CONSOLE_DEVICE);

    SysCtlPeripheralEnable(bypass_console::pick_sysctl());

    UARTConfigSetExpClk(uart_dev, SysCtlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);
}

void bypass_putc(char c)
{
    constexpr auto uart_dev = static_cast<std::underlying_type_t<uart_device>>(ECL_CONSOLE_DEVICE);
    UARTCharPut(uart_dev, c);
}

} // namespace ecl
