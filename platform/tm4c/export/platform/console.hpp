//! \file
//! \brief TM4C bypass console driver
#ifndef TM4C_CONSOLE_DRIVER_HPP_
#define TM4C_CONSOLE_DRIVER_HPP_

namespace ecl
{

#ifndef CONFIG_BYPASS_CONSOLE_ENABLED

//! Does nothing if, console disabled.
static inline void bypass_putc(char c)
{
    (void) c;
}

#else // CONFIG_BYPASS_CONSOLE_ENABLED

//! Bypasses console drivers and puts data directly to the UART.
//! \details Required to print debug of the failed asserts including one that
//! executed during ISR.
void bypass_putc(char c);

#endif // CONFIG_BYPASS_CONSOLE_ENABLED

} // namespace ecl


#endif // TM4C_CONSOLE_DRIVER_STUB_HPP_
