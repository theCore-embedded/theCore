/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Enlists all generated headers by stm32 platform. Further used
//! by theCore or the user.
#ifndef PLATFORM_GENERATED_HPP_
#define PLATFORM_GENERATED_HPP_

// DMA configuration is specific to each family, thus moved there.
#include "family_dma_cfg.hpp"

#include "aux/uart_cfg.hpp"
#include "aux/adc_cfg.hpp"
#include "aux/gpio_cfg.hpp"
#include "aux/i2c_cfg.hpp"
#include "aux/spi_i2s_cfg.hpp"

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

//! Init all GPIOs using generated code based on incoming JSON.
//! \details Defined in pin_mux.in.cpp
extern "C" void gpio_init_generated();

//! @}

//! @}

//! @}

#endif // PLATFORM_GENERATED_HPP_
