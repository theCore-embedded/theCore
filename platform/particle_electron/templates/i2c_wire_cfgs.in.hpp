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

i2c_cfg_template = '''
template<>
struct i2c_bus_cfg<i2c_device::wire0>
{
    static constexpr auto speed         = %d; // 10 kHz
    static constexpr auto stretch_clk   = %s;
};
'''

if 'I2C_ENABLED' in globals():
    cfgs = I2C_CFG.split(' ')
    speed = int(cfgs[0])
    clk_stretch = bool(cfgs[1])

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
