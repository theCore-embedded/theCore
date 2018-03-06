/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief SDSPI configuration code template

#ifndef DEV_SDSPI_CFG_HPP_
#define DEV_SDSPI_CFG_HPP_

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
sdspi_cfg = []

if 'device' in cfg and 'sdspi' in cfg['device']:
    sdspi_cfg = cfg['device']['sdspi']

for sdspi in sdspi_cfg:
    sdspi_name = dev.resolve_sdspi_driver(cfg, sdspi['spi'], sdspi['cs_gpio'])
    cs_gpio = common.resolve_gpio_driver(cfg, sdspi['cs_gpio'])
    spi_drv = common.resolve_spi_driver(cfg, sdspi['spi'])
    cog.outl(bus_typedef % (spi_drv, spi_drv))
    cog.outl(sdspi_typedef % (sdspi_name, spi_drv, cs_gpio))
    cog.outl(sdspi_alias % (sdspi['id'], sdspi_name))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_SDSPI_CFG_HPP_