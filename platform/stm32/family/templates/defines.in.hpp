//! \file
//! \brief Family-related defintions that should be exported.
#ifndef STM32_FAMILY_DEFINES_
#define STM32_FAMILY_DEFINES_

// Used by theCore
#define stm32${STM32_FAMILY}
// Used by SPL
#define ${ST_DEVICE}
// Used by SPI driver.
#define CONFIG_SPI_COUNT ${TARGET_MCU_SPI_COUNT}
// Used by EXTI driver.
#define CONFIG_ECL_EXTI_DIRECT_COUNT ${TARGET_MCU_EXTI_DIRECT_COUNT}
// Used by EXTI driver.
#define CONFIG_ECL_EXTI_GROUPED_COUNT ${TARGET_MCU_EXTI_GROUPED_COUNT}
// Used by ADC driver.
#define CONFIG_ECL_ADC_COUNT ${TARGET_MCU_ADC_COUNT}
// Used by SPL.
#define HSE_VALUE ${CONFIG_PLATFORM_HSE_VAL}
// Used by the IRQ manager.
#define CONFIG_IRQ_COUNT ${TARGET_MCU_IRQ_COUNT}

#endif // STM32_FAMILY_DEFINES_