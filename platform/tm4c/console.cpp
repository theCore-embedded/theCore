//! \file
//! \brief TM4C bypass console implementation.

#include "platform/gpio_device.hpp"
#include <uart.h>

namespace ecl
{

void bypass_console_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);
}

void bypass_putc(char c)
{
    UARTCharPut(UART0_BASE, c);
}

} // namespace ecl
