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


template<>
struct i2c_bus_cfg<i2c_device::wire0>
{
    static constexpr auto speed         = 20000;
    static constexpr auto stretch_clk   = false;
};

using i2c_dev = i2c_bus<i2c_device::wire0>;

} // namespace ecl

#endif // PLATFORM_I2C_CFGS_HPP_
