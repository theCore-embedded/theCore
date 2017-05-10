// Header contains target-specific interfaces.

#ifndef EXAMPLE_HTU21D_TARGET_HPP_
#define EXAMPLE_HTU21D_TARGET_HPP_

#include <aux/i2c_bus.hpp>

#include <dev/sensor/htu21d.hpp>
#include <dev/bus.hpp>

namespace ecl
{

// I2C1 - I2C for HTU21D sensor.

using i2c1_cfg = i2c_config<
    i2c_device::bus1,
    i2c_mode::IRQ,
    10000,
    I2C_Mode_I2C,
    I2C_DutyCycle_2,
    0x33,
    I2C_Ack_Enable,
    I2C_AcknowledgedAddress_7bit
>;

using i2c1 = generic_bus<i2c_bus<i2c1_cfg>>;

using htu21d = sensor::htu21d<i2c1>;

} // namespace ecl

// Initializes pins
void gpio_init();

#endif // EXAMPLE_HTU21D_TARGET_HPP_
