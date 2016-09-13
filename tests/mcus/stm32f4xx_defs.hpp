//! \file
//! \brief stm32f4xx exported definitions and peripherals, required by tests

#ifndef TEST_STM32F4XX_DEFS_HPP
#define TEST_STM32F4XX_DEFS_HPP

#include <aux/spi_bus.hpp>
#include <aux/i2c_bus.hpp>
#include <platform/gpio_device.hpp>

namespace ecl
{

namespace test
{

// Available SPIs

using spi1 = spi_i2s_bus<spi_device::bus1>;
using spi_i2s2 = spi_i2s_bus<spi_device::bus2>;
using spi_i2s3 = spi_i2s_bus<spi_device::bus3>;

// Available GPIOs

using gpio_pa0 = gpio<gpio_port::a, gpio_num::pin0>;


} // namespace test

} // namespace ecl


#endif // TEST_STM32F4XX_DEFS_HPP
