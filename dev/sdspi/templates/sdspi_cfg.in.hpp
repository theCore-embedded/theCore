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

bus_typedef = '''
using bus_sdspi_%s = generic_bus<%s>;
'''

sdspi_typedef = '''
using sdspi_%s = sdspi<bus_sdspi_%s, %s>;
'''

sdspi_alias = '''
using %s = sdspi_%s;
'''

cfg = json.load(open(JSON_CFG))
sdspi_cfg = []

if 'device' in cfg and 'sdspi' in cfg['device']:
    sdspi_cfg = cfg['device']['sdspi']

for sdspi in sdspi_cfg:
    cs_gpio = common.resolve_gpio_driver(cfg, sdspi['cs_gpio'])
    spi_drv = common.resolve_spi_driver(cfg, sdspi['spi'])
    #alias = sdspi_cfg['alias']
    cog.outl(bus_typedef % (spi_drv, spi_drv))
    cog.outl(sdspi_typedef % (spi_drv, spi_drv, cs_gpio))
]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_SDSPI_CFG_HPP_