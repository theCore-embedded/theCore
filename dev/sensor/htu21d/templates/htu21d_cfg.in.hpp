/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief HTU21D configuration code template

#ifndef DEV_SENSOR_HTU21D_CFG_HPP_
#define DEV_SENSOR_HTU21D_CFG_HPP_

#include <dev/bus.hpp>
#include <dev/sensor/htu21d.hpp>

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

htu21d_alias = '''
using %s = sensor::htu21d<bus_%s>;
'''

cfg = json.load(open(JSON_CFG))

htu21d_ids = []
try:
    htu21d_ids = cfg['menu-dev']['menu-sensors']['menu-htu21d']['table-htu21d-dev']
except:
    pass

for htu21d_id in htu21d_ids:
    htu21d_cfg = cfg['menu-dev']['menu-sensors']['menu-htu21d']['menu-' + htu21d_id]

    i2c_driver = common.resolve_i2c_driver(cfg, htu21d_cfg['config-i2c'])
    cog.outl(bus_typedef % (i2c_driver, i2c_driver))
    cog.outl(htu21d_alias % (htu21d_id, i2c_driver))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_SENSOR_HTU21D_CFG_HPP_
