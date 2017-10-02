/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMMON_I2C_HPP_
#define COMMON_I2C_HPP_

#include <cstdint>

namespace ecl
{
// Represents distinct peripheral devices
enum class i2c_device
{
    bus1,
    bus2,
    bus3
};

}

#endif // COMMON_I2C_HPP_
