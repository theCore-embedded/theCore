//! \file

#ifndef TM4C_CONSOLE_DRIVER_STUB_HPP_
#define TM4C_CONSOLE_DRIVER_STUB_HPP_

namespace ecl
{

//! Does nothing if, console disabled.
static inline void bypass_putc(char c)
{
    (void) c;
}

} // namespace ecl


#endif // TM4C_CONSOLE_DRIVER_STUB_HPP_
