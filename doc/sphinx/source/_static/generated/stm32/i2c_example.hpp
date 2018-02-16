/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of I2C driver configuration.
//! \ingroup stm32_templates
#ifndef STM32XX_I2C_CFGS_HPP_
#define STM32XX_I2C_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/i2c_bus.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

// I2C configuration -----------------------------------------------------------

/* Just another I2C bus */

using I2C1_driver_cfg = i2c_config<
    i2c_device::bus1,
    i2c_mode::IRQ,
    1000,
    I2C_Mode_I2C,
    I2C_DutyCycle_2,
    51,
    I2C_Ack_Enable,
    I2C_AcknowledgedAddress_7bit
>;

using I2C1_driver = i2c_bus<I2C1_driver_cfg>;
using i2c1 = I2C1_driver;

} // namespace ecl

//! @}

//! @}

//! @}

#endif // STM32XX_I2C_CFGS_HPP_
