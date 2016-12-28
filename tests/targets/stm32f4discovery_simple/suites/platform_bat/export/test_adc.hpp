//! \file
//! \brief Suite ADC definitions

#ifndef STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_
#define STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_

#include <aux/adc.hpp>

namespace ecl
{

// Test ADC device.
using test_adc = adc<ecl::adc_dev::dev1>;

// ADC configuration.
template<>
struct adc_cfg<adc_dev::dev1>
{
    static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::irq;
};

// Test channel - PA1 pin and temperature channel.
using test_channel = channel_group_sw_trigger<
                adc_channel::ch1 /*, ecl::adc_channel::ch16 */>;

} // namespace ecl

#endif // STM32DISC_PLATFORM_BAT_TEST_ADC_HPP_
