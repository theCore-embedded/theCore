//! \file
//! \brief Header provides template of ADC driver configuration
#ifndef STM32XX_ADC_CFGS_HPP_
#define STM32XX_ADC_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/adc.hpp>

namespace ecl
{


// ADC configuration -----------------------------------------------------------

/* Example ADC device */


template<>
struct adc_cfg<adc_dev::dev1>
{
    static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::dma;

    using dma = adc_dma;
};

using ADC1_driver = adc<adc_dev::dev1>;
using example_adc = ADC1_driver;

// ADC channels configuration --------------------------------------------------

/* PA1 pin and the temperature channel */


using test_channels = channel_group_sw_trigger<adc_channel::ch1, adc_channel::ch16>;


} // namespace ecl

#endif // STM32XX_ADC_CFGS_HPP_
