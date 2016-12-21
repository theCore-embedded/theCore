//! \file
//! \brief Suite ADC definitions

#ifndef STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_
#define STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_

#include <aux/adc.hpp>

namespace ecl
{

using test_adc = ecl::adc<ecl::adc_dev::dev1>;

// Test channel - PA1 pin.
using test_channel = ecl::channel_group<ecl::adc_trigger::t1_cc1,
                ecl::adc_trigger_edge::none,
                ecl::adc_channel::ch1>;

} // namespace ecl

#endif // STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_
