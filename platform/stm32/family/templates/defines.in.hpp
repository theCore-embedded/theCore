/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


//! \file
//! \brief Family-related defintions that should be exported.
//! \ingroup stm32_templates
#ifndef STM32_FAMILY_DEFINES_
#define STM32_FAMILY_DEFINES_

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

//! Used by theCore
#define stm32${STM32_FAMILY}
//! Used by SPL
#define ${ST_DEVICE}
//! Used by SPI driver.
#define CONFIG_SPI_COUNT ${TARGET_MCU_SPI_COUNT}
//! Used by EXTI driver.
#define CONFIG_ECL_EXTI_DIRECT_COUNT ${TARGET_MCU_EXTI_DIRECT_COUNT}
//! Used by EXTI driver.
#define CONFIG_ECL_EXTI_GROUPED_COUNT ${TARGET_MCU_EXTI_GROUPED_COUNT}
//! Used by ADC driver.
#define CONFIG_ECL_ADC_COUNT ${TARGET_MCU_ADC_COUNT}
//! Used by SPL.
#define HSE_VALUE ${CONFIG_PLATFORM_HSE_VAL}
//! Used by the IRQ manager.
#define CONFIG_IRQ_COUNT ${TARGET_MCU_IRQ_COUNT}

//! @}

//! @}

//! @}

#endif // STM32_FAMILY_DEFINES_

