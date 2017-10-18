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
cfg = cfg['platform']

]]]*/
//[[[end]]]

namespace ecl
{

// GPIO drivers ----------------------------------------------------------------

/*[[[cog

gpio_aliases = []

if 'gpio_alias' in cfg:
    gpio_aliases = cfg['gpio_alias']

for gpio_alias in gpio_aliases:
    if 'comment' in gpio_alias:
        cog.outl('/' + '* ' + gpio_alias['comment'] + ' *' + '/')

    alias = gpio_alias['alias']
    port = gpio_alias['id'][1:-1].lower()
    pin = int(gpio_alias['id'][-1:])
    cog.outl('using %s = gpio<gpio_hw::port::%s, gpio_hw::num::pin%d>;'
        % (alias, port, pin))

]]]*/
//[[[end]]]

} // namespace ecl

// \brief Initializes GPIO using auto-generated code.
extern "C" void gpio_init_generated();

#endif // TM4C_PLATFORM_GPIO_CFG_
