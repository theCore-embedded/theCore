/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Particle Serial I2C driver configuration template
#ifndef PLATFORM_I2C_CFGS_HPP_
#define PLATFORM_I2C_CFGS_HPP_

#include <aux/i2c_bus.hpp>

namespace ecl
{

// I2C device defintions

/*[[[cog
import cog
import os
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

i2c_cfg_template = '''
template<>
struct i2c_bus_cfg<i2c_device::wire0>
{
    static constexpr auto speed         = %d; // 10 kHz
    static constexpr auto stretch_clk   = %s;
};
'''

if 'i2c' in cfg:
    i2c = cfg['i2c']
    speed = i2c['speed']

    if 'stretch_clk' in i2c:
        clk_stretch = i2c['stretch_clk']
    else:
        clk_stretch = False

    if clk_stretch:
        clk_stretch = 'true'
    else:
        clk_stretch = 'false'

    cog.msg('I2C speed: {} clock stretch: {}'.format(speed, clk_stretch))

    cog.outl(i2c_cfg_template % (speed, clk_stretch))
    cog.outl("using i2c_dev = i2c_bus<i2c_device::wire0>;")
]]]*/

//[[[end]]]

} // namespace ecl

#endif // PLATFORM_I2C_CFGS_HPP_
