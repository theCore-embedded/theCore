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
cfg = cfg['menu-platform']['menu-tm4c']

pin_ids = []

try:
    pin_ids = cfg['menu-pinconfig']['table-pins']
except:
    pass

def extract_dir(pincfg):
    dirs = {
        'input':   'in',
        'output':  'out',
        'af':      'hw'
    }

    if 'config-direction' in pincfg:
        return dirs[pincfg['config-direction']]
    else:
        return dirs['output']


def extract_pintype(pingcfg):
    pintypes = {
        'standard':     'push_pull',
        'pull':         'pull',
        'push':         'push',
        'open drain':   'open_drain',
        'analog':       'analog',
        'wake high':    'wake_high',
        'wake low':     'wake_low'
    }

    if 'config-type' in pincfg:
        return pintypes[pincfg['config-type']]
    else:
        return pintypes['standard']

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

    if 'config-strength' in pincfg:
        return strengths[pincfg['config-strength']]
    else:
        return strengths['2mA']

def extract_hw(pin, hw):
    hw = hw.upper()
    return 'GPIO_' + pin + '_' + hw

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

    # Set to preserve an order
    ports = set([])

    for pin_id in pin_ids:
        port = pin_id[1:-1].lower()
        ports |= set([ port ])

    for port in ports:
        cog.outl('ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::%s>();' % port)

    ]]]*/
    //[[[end]]]

    /*[[[cog
    # Configure individual pins.

    for pin_id in pin_ids:
        pincfg = {}

        # Missing configuration?
        try:
            pincfg = cfg['menu-pinconfig']['menu-' + pin_id]
        except:
            continue

        dir = extract_dir(pincfg)
        pin_type = extract_pintype(pincfg)
        strength = extract_strength(pincfg)

        cog.outl('')

        if 'config-comment' in pincfg:
            cog.outl('/' + '* ' + pincfg['config-comment'] + ' *' + '/')

        port = pin_id[1:-1].lower()
        num = int(pin_id[-1:])
        cog.outl(pincfg_template % (port, num, dir, pin_type, strength))

        # Extract alternate function (if any)
        if dir == 'hw':
            hw = extract_hw(pin_id, pincfg['config-afsel'])
            cog.outl('GPIOPinConfigure(%s);' % hw)

    ]]]*/
    //[[[end]]]
}
