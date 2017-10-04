/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief STM32F4XX ADC wrappers.
#ifndef PLATFORM_STM32F4XX_ADC_WRAP_
#define PLATFORM_STM32F4XX_ADC_WRAP_

#include <stm32f4xx.h>
#include <utility>

namespace ecl
{

//! Present ADC devices in the MCU
enum class adc_dev
{
    dev1 = ADC1_BASE,
    dev2 = ADC2_BASE,
    dev3 = ADC3_BASE,
};

//! Gets amount of present ADC devices.
static constexpr auto adcs_count()
{
    // TODO: decide if this define is really needed.
    return CONFIG_ECL_ADC_COUNT;
}

//! ADC IRQ channel.
//! \details It is typical for stm32 devices to have only one IRQ for all ADCs.
//! If for some families it is not a case, additional IRQ must be added
//! and logic must be changed.
static constexpr auto adc_irqn = ADC_IRQn;

//! Maps ADC to index in the ADC array.
static constexpr auto get_adc_idx(adc_dev dev)
{
    switch (dev) {
    case adc_dev::dev1:
        return 0;
    case adc_dev::dev2:
        return 1;
    case adc_dev::dev3:
        return 2;
    }
}

//! Possible ADCs/
static constexpr adc_dev adcs[] = { adc_dev::dev1, adc_dev::dev2, adc_dev::dev3  };

} // namespace ecl

#endif // PLATFORM_STM32F4XX_ADC_WRAP_
