/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief CS43L22 configuration code template

#ifndef DEV_CS43L22_CFG_HPP_
#define DEV_CS43L22_CFG_HPP_

#include <dev/bus.hpp>
#include <dev/cs43l22.hpp>

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

cs43l22_alias = '''
using %s = cs43l22<bus_%s, bus_%s, %s>;
'''

cfg = json.load(open(JSON_CFG))

cs43l22_ids = []
try:
    cs43l22_ids = cfg['menu-dev']['menu-cs43l22']['table-cs43l22-dev']
except:
    pass


for cs43l22_id in cs43l22_ids:
    cs43l22_cfg = cfg['menu-dev']['menu-cs43l22']['menu-' + cs43l22_id]

    rst_gpio = common.resolve_gpio_driver(cfg, cs43l22_cfg['config-rst'])
    i2s_driver = common.resolve_i2s_driver(cfg, cs43l22_cfg['config-i2s'])
    i2c_driver = common.resolve_i2c_driver(cfg, cs43l22_cfg['config-i2c'])
    cog.outl(bus_typedef % (i2s_driver, i2s_driver))
    cog.outl(bus_typedef % (i2c_driver, i2c_driver))
    cog.outl(cs43l22_alias % (cs43l22_id, i2c_driver, i2s_driver, rst_gpio))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_CS43L22_CFG_HPP_
