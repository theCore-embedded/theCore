/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C GPIO configuration template

#ifndef TM4C_PLATFORM_GPIO_CFG_
#define TM4C_PLATFORM_GPIO_CFG_

#include <platform/gpio_device.hpp>

/*[[[cog
import cog
import json

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['menu-platform']['menu-tm4c']

]]]*/
//[[[end]]]

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_templates Auto-generated code, Python COG and CMake templates.
//! @{

// GPIO drivers ----------------------------------------------------------------

/*[[[cog

# Extracts pin directions
def extract_dir(pincfg):
    dirs = {
        'input':   'in',
        'output':  'out',
        'af':      'hw'    # Alternate Function called Hard Ware in TI terms
    }

    if 'config-direction' in pincfg:
        return dirs[pincfg['config-direction']]
    else:
        return dirs['output']

pincfg_ids = []

try:
    pincfg_ids = cfg['menu-pinconfig']['table-pins']
except:
    pass

for pin_id in pincfg_ids:
    pincfg = cfg['menu-pinconfig']['menu-' + pin_id]
    dir = extract_dir(pincfg)
    port = pin_id[1:-1].lower()
    num = int(pin_id[-1:])

    # Generate GPIO drivers for every pin, even in HW mode. Just in case.
    cog.outl('using %s_driver = gpio<gpio_hw::port::%s, gpio_hw::num::pin%d>;'
        % (pin_id, port, num))

    if 'config-comment' in pincfg:
        cog.outl('{}{}{}'.format('/', '* ' + pincfg['config-comment'] + ' *', '/'))

    if 'config-gpio-alias' in pincfg:
        alias = pincfg['config-gpio-alias']
        cog.outl('using %s = %s_driver;' % (alias, pin_id))

]]]*/
//[[[end]]]

//! @}

//! @}

//! @}

} // namespace ecl

// \brief Initializes GPIO using auto-generated code.
extern "C" void gpio_init_generated();

#endif // TM4C_PLATFORM_GPIO_CFG_
