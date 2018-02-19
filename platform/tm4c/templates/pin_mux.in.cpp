/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Pin multiplexer and configurator for TM4C platform.

#include <aux/platform_defines.hpp>
#include <aux/pin_cfg.hpp>
#include <pin_map.h>

/*[[[cog
import cog
import json
import re

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['platform']

pincfgs = []
if 'pinmux' in cfg:
    pincfgs = cfg['pinmux']

def extract_dir(pincfg):
    dirs = {
        'in':   'in',
        'out':  'out',
        'hw':   'hw'
    }

    if 'dir' in pincfg:
        return dirs[pincfg['dir']]
    else:
        return dirs['out']


def extract_pintype(pingcfg):
    pintypes = {
        'std':          'push_pull',
        'pull':         'pull',
        'push':         'push',
        'od':           'open_drain',
        'analog':       'analog',
        'wake_high':    'wake_high',
        'wake_low':     'wake_low'
    }

    if 'pin_type' in pincfg:
        return pintypes[pincfg['pin_type']]
    else:
        return pintypes['std']

def extract_strength(pincfg):
    strengths = {
        '2mA':          'i2ma',
        '4mA':          'i4ma',
        '6mA':          'i6ma',
        '8mA':          'i8ma',
        '8mA_sc':       'i8ma_sc',
        '10mA':         'i10ma',
        '12mA':         'i12ma'
    }

    if 'strength' in pincfg:
        return strengths[pincfg['strength']]
    else:
        return strengths['2mA']

def extract_hw(pin, hw):
    periph_map = {
        'UART':     'U',
        'SPI':      'SSI',
    }

    m = re.search('(\w+)(\d)_(\w+)', hw)
    periph_id = periph_map[m.group(1)]
    periph_num = m.group(2)
    pin_purpose = m.group(3)

    return 'GPIO_' + pin + '_' + periph_id + str(periph_num) + pin_purpose

]]]*/
//[[[end]]]

extern "C" void gpio_init_generated()
{
    /*[[[cog

    pincfg_template = \
    '''
    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::%s, ecl::gpio_hw::num::pin%d>(
            ecl::gpio_hw::dir::%s,
            ecl::gpio_hw::pad_type::%s,
            ecl::gpio_hw::strength::%s
        );
    '''

    # Collect all ports required to enable periphery via sysctl.
    # Should be done before any pin is configured to avoid usage fault.

    ports = set([])

    for pincfg in pincfgs:
        for pin in pincfg['ids']:
            port = pin[1:-1].lower()
            ports |= set([ port ])

    for port in ports:
        cog.outl('ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::%s>();' % port)

    ]]]*/
    //[[[end]]]

    /*[[[cog
    # Configure individual pins.

    for pincfg in pincfgs:
        dir = extract_dir(pincfg)
        pin_type = extract_pintype(pincfg)
        strength = extract_strength(pincfg)

        cog.outl('')

        if 'comment' in pincfg:
            cog.outl('/' + '* ' + pincfg['comment'] + ' *' + '/')

        for pin in pincfg['ids']:
            port = pin[1:-1].lower()
            num = int(pin[-1:])
            cog.outl(pincfg_template % (port, num, dir, pin_type, strength))

        # Extract alternate function (if any)
        if dir == 'hw':
            for pin_key, pin_hw in pincfg['hws'].items():
                hw = extract_hw(pin_key, pin_hw)
                cog.outl('GPIOPinConfigure(%s);' % hw)

    ]]]*/
    //[[[end]]]
}
