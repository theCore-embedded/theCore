//! \file
//! \brief Memmap for stm32f4 device
//! \todo Split memmap by peripherals.

#ifndef STM32F4_MEMMAP_HPP_
#define STM32F4_MEMMAP_HPP_

#include <stm32f4xx.h>

namespace ecl
{

//! Present ADC devices in the MCU
enum class adc_dev
{
    dev1 = ADC1_BASE,
    dev2 = ADC2_BASE,
    dev3 = ADC3_BASE,
};

} // namespace ecl


#endif // STM32F4_MEMMAP_HPP_
