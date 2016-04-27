#ifndef COMMON_I2C_HPP_
#define COMMON_I2C_HPP_

#include <cstdint>

namespace ecl
{
// Represents distinct peripheral devices
enum class i2c_device {
    bus_1,
    bus_2,
    bus_3
};

}

#endif // COMMON_I2C_HPP_
