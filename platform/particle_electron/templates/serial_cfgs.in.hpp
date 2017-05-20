//! \file Particle Serial drivers configuration template
#ifndef PLATFORM_SERIAL_CFGS_HPP_
#define PLATFORM_SERIAL_CFGS_HPP_

#include <aux/uart_bus.hpp>

namespace ecl
{

// Serial devices definitions

/*[[[cog
import cog

# No serial configuration is requested, aborting.
if 'SERIALS' in globals():
    serials = SERIALS.split(' ')
    cog.msg("Serials enabled: " + str(serials))

    for serial in serials:
        cog.outl("using serial_device{} = uart_bus<uart_device::serial{}>;"
            .format(serial, serial))
]]]*/

//[[[end]]]

// Console definitions (if enabled)

/*[[[cog
if 'CONSOLE' in globals():
    cog.msg('Enabling console on serial device ' + str(CONSOLE))
    cog.outl('using platform_console = serial_device{};'.format(CONSOLE))
]]]*/
//[[[end]]]

//! Initializes bypass console.
static inline void bypass_init()
{
    /*[[[cog
    if 'CONSOLE' in globals():
        num = int(CONSOLE)
        if num == 0:
            cog.outl('Serial.begin();')
        else:
            cog.outl('Serial{}.begin();'.format(num))
    ]]]*/
    //[[[end]]]
}

//! Puts character to bypass console. Does nothing if console is disabled.
static inline void bypass_putc(char c)
{
    /*[[[cog
    if 'CONSOLE' in globals():
        num = int(CONSOLE)
        if num == 0:
            cog.outl('Serial.write(c);')
        else:
            cog.outl('Serial{}.write(c);'.format(num))
    else:
        cog.outl('(void)c;')
    ]]]*/
    //[[[end]]]
}

} // namespace ecl

#endif // PLATFORM_SERIAL_CFGS_HPP_
