/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief SDSPI configuration code template

#ifndef DEV_SDSPI_CFG_HPP_
#define DEV_SDSPI_CFG_HPP_

#include <dev/bus.hpp>
#include <dev/sdspi.hpp>

namespace ecl
{

/*[[[cog

import cog
import json
import common
import dev

bus_typedef = '''
using bus_%s = generic_bus<%s>;
'''

sdspi_typedef = '''
using %s = sdspi<bus_%s, %s>;
'''

sdspi_alias = '''
using %s = %s;
'''

cfg = json.load(open(JSON_CFG))

sdspi_ids = []
sdspi_menu = {}

try:
    sdspi_menu = cfg['menu-dev']['menu-sdspi']
    sdspi_ids = sdspi_menu['table-sdspi-dev']
except:
    pass

for sdspi_id in sdspi_ids:
    sdspi_cfg = sdspi_menu['menu-' + sdspi_id]

    sdspi_name = dev.resolve_sdspi_driver(cfg, sdspi_cfg['config-spi'], sdspi_cfg['config-cs'])
    cs_gpio = common.resolve_gpio_driver(cfg, sdspi_cfg['config-cs'])
    spi_drv = common.resolve_spi_driver(cfg, sdspi_cfg['config-spi'])
    cog.outl(bus_typedef % (spi_drv, spi_drv))
    cog.outl(sdspi_typedef % (sdspi_name, spi_drv, cs_gpio))
    cog.outl(sdspi_alias % (sdspi_id, sdspi_name))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_SDSPI_CFG_HPP_
