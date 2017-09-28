// Header contains target-specific interfaces.

#ifndef EXAMPLE_EXTI_TARGET_HPP_
#define EXAMPLE_EXTI_TARGET_HPP_

#include <aux/generated.hpp>

#include <dev/cs43l22.hpp>
#include <dev/bus.hpp>

namespace ecl
{

// I2S3 - I2S for cs43l22 codec is defined in stm32f4_cs43l22.json
// I2C1 - I2C for cs43l22 codec is defined in stm32f4_cs43l22.json
// Reset GPIO is also defined in stm32f4_cs43l22.json

// Codec definition
using cs43l22_instance = cs43l22<generic_bus<i2c1>, generic_bus<i2s3>, rst_gpio>;

} // namespace ecl

#endif // EXAMPLE_EXTI_UART_TARGET_HPP_
