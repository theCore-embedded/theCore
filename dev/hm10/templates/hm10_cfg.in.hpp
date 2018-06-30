/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief HTU21D configuration code template

#ifndef DEV_HM10_CFG_HPP_
#define DEV_HM10_CFG_HPP_

#include <dev/serial.hpp>
#include <dev/hm10.hpp>

namespace ecl
{

/*[[[cog

import cog
import json
import common
import dev

cfg = json.load(open(JSON_CFG))

hm10_ids = []
try:
    hm10_ids = cfg['menu-dev']['menu-hm10']['table-hm10-dev']
except:
    pass

hm10_template = '''
using %s = ecl::hm10_sync<%s>;
'''

for hm10_id in hm10_ids:
    hm10_cfg = cfg['menu-dev']['menu-hm10']['menu-' + hm10_id]

    uart = common.resolve_uart_driver(cfg, hm10_cfg['config-uart'])
    cog.outl(hm10_template % (hm10_id, uart))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // DEV_HM10_CFG_HPP_
