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

/*[[[cog
import cog
import json
import os

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

serials = []

if 'serial' in cfg:
    serials = cfg['serial']
    cog.msg("Serials enabled: " + str(serials))

for serial in serials:
    cog.outl("using serial_device{} = uart_bus<uart_device::serial{}>;"
        .format(serial, serial))
]]]*/

//[[[end]]]

// Console definitions (if enabled)

/*[[[cog
if 'console' in cfg and cfg['console']:
    cog.msg('Enabling console on serial device 0')
    cog.outl('using platform_console = serial_device0;')

]]]*/
//[[[end]]]

//! Initializes bypass console.
static inline void bypass_init()
{
    /*[[[cog
    if 'console' in cfg and cfg['console']:
        cog.outl('Serial.begin();')
    ]]]*/
    //[[[end]]]
}

//! Puts character to bypass console. Does nothing if console is disabled.
static inline void bypass_putc(char c)
{
    /*[[[cog
    if 'console' in cfg and cfg['console']:
        cog.outl('Serial.write(c);')
    else:
        cog.outl('(void)c;')
    ]]]*/
    //[[[end]]]
}

} // namespace ecl

#endif // PLATFORM_SERIAL_CFGS_HPP_
