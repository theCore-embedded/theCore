//! \file
//! \brief STM32 ADC driver.
//! \details Some app notes can be found at:
//! http://goo.gl/9rTqUT (AN3116 application note, STM32’s ADC modes and their applications)
#ifndef PLATFORM_ADC_HPP_
#define PLATFORM_ADC_HPP_

#include <stm32_device.hpp>

#include <array>
#include <ecl/utils.hpp>

namespace ecl
{

//! The external event used to trigger the start of conversion of a group.
enum class adc_trigger
{
    t1_cc1      = ADC_ExternalTrigConv_T1_CC1,
    t1_cc2      = ADC_ExternalTrigConv_T1_CC2,
    t1_cc3      = ADC_ExternalTrigConv_T1_CC3,
    t2_cc2      = ADC_ExternalTrigConv_T2_CC2,
    t2_cc3      = ADC_ExternalTrigConv_T2_CC3,
    t2_cc4      = ADC_ExternalTrigConv_T2_CC4,
    t2_trgo     = ADC_ExternalTrigConv_T2_TRGO,
    t3_cc1      = ADC_ExternalTrigConv_T3_CC1,
    t3_trgo     = ADC_ExternalTrigConv_T3_TRGO,
    t4_cc4      = ADC_ExternalTrigConv_T4_CC4,
    t5_cc1      = ADC_ExternalTrigConv_T5_CC1,
    t5_cc2      = ADC_ExternalTrigConv_T5_CC2,
    t5_cc3      = ADC_ExternalTrigConv_T5_CC3,
    t8_cc1      = ADC_ExternalTrigConv_T8_CC1,
    t8_trgo     = ADC_ExternalTrigConv_T8_TRGO,
    ext_it11    = ADC_ExternalTrigConv_Ext_IT11,
};

//! The external trigger edge.
enum class adc_trigger_edge
{
    none        = ADC_ExternalTrigConvEdge_None,
    rising      = ADC_ExternalTrigConvEdge_Rising,
    falling     = ADC_ExternalTrigConvEdge_Falling,
    both        = ADC_ExternalTrigConvEdge_RisingFalling,
};

//! ADC possible channels.
//! \details Be careful, since not all channels are connected to every ADC.
enum class adc_channel
{
    ch0     = ADC_Channel_0,
    ch1     = ADC_Channel_1,
    ch2     = ADC_Channel_2,
    ch3     = ADC_Channel_3,
    ch4     = ADC_Channel_4,
    ch5     = ADC_Channel_5,
    ch6     = ADC_Channel_6,
    ch7     = ADC_Channel_7,
    ch8     = ADC_Channel_8,
    ch9     = ADC_Channel_9,
    ch10    = ADC_Channel_10,
    ch11    = ADC_Channel_11,
    ch12    = ADC_Channel_12,
    ch13    = ADC_Channel_13,
    ch14    = ADC_Channel_14,
    ch15    = ADC_Channel_15,
    ch16    = ADC_Channel_16,
    ch17    = ADC_Channel_17,
    ch18    = ADC_Channel_18,
};

//! Channels grouped together along with trigger.
//! \tparam trig  Trigger source that will start channel conversion.
//! \tparam edge  Edge of the signal that acts like a trigger.
//! \tparam chans List of channels that grouped together.
template<adc_trigger trig, adc_trigger_edge edge, adc_channel... chans>
struct channel_group
{
    static constexpr auto trigger       = trig;
    static constexpr auto trigger_edge  = edge;

    template<template<adc_channel... chs>class Extractor>
    using extractor = Extractor<chans...>;
};

//! Extracts information of the given channel group.
//! \tparam chs List of channels to work on.
template<adc_channel... chs>
struct extractor
{
    //! Returns amount of conversions required to convert given channels.
    static constexpr uint8_t conv_num()
    {
        return sizeof ... (chs);
    }

    //! Returns scan mode for given channels.
    static constexpr FunctionalState scan_mode()
    {
        return conv_num() == 1 ? DISABLE : ENABLE;
    }

    //! Initialize channels.
    //! \param[in] spl_adc ADC MMIO structure used by SPL.
    static void init_channels(ADC_TypeDef *spl_adc)
    {
        // Channel's rank in the sequencer.
        uint8_t rank = 0;

        (void)std::initializer_list<int> {
            (ADC_RegularChannelConfig(spl_adc, extract_value(chs),
            ++rank, ADC_SampleTime_144Cycles), 0) ...
        };
    }
};

//! ADC peripheral class.
//! \tparam dev ADC device to work with.
template<adc_dev dev>
class adc
{
public:
    //! Type of signle sample. Enough to hold sample values of given ADC.
    using sample_type = uint16_t;

    //! Buffer required to hold all samples from given channel group.
    //! \todo replace it with array_span, array_view or simliar.
    //! \tparam Group Group of channels that will be converted and result will be
    //! stored in this buffer.
    template<typename Group>
    using sample_array = std::array<sample_type, Group::template extractor<extractor>::conv_num()>;

    //! Initializes given ADC.
    static void init();

    //! Configures ADC to work with given channels.
    template<typename Group>
    static void enable_channels(sample_array<Group> &out_samples);

    //! Performs single conversion of the previously configured channels
    //! and waits till the end of the conversion. Results will be saved to a
    //! buffer previously passed with enable_channels()
    //! \pre Channels configured with use_channels() call.
    static void single();

private:
    //! Picks peripheral clock asociated with given ADC.
    static constexpr auto pick_periph();

    //! Picks ADC MMIO pointer for given ADC.
    static auto           pick_spl_adc();

    //! ADC mode. See ADC_ContinuousConvMode.
    enum class current_mode
    {
        single,     //! Perform single conversion of the channel group.
        continuous, //! Continue to convert channels after previous conversion round finished.
    };

    //! Current ADC mode.
    static current_mode m_mode;

    //! Output samples buffer. Size is guaranteed by channels_enable() call.
    static sample_type  *m_samples;

    //! Amount of conversions that required to fully convert all channels.
    //! Equals to a size of a samples buffer.
    static size_t m_convs;
};

template<adc_dev dev>
typename adc<dev>::current_mode adc<dev>::m_mode;

template<adc_dev dev>
typename adc<dev>::sample_type * adc<dev>::m_samples;

template<adc_dev dev>
size_t adc<dev>::m_convs;

template<adc_dev dev>
constexpr auto adc<dev>::pick_periph()
{
    switch (dev) {
    case adc_dev::dev1:
        return RCC_APB2Periph_ADC1;
    case adc_dev::dev2:
        return RCC_APB2Periph_ADC2;
    }
}

template<adc_dev dev>
auto adc<dev>::pick_spl_adc()
{
    return reinterpret_cast<ADC_TypeDef*>(dev);
}

//------------------------------------------------------------------------------

template<adc_dev dev>
void adc<dev>::init()
{
    constexpr auto periph = pick_periph();
    RCC_APB2PeriphClockCmd(periph, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE);
}

template<adc_dev dev>
template<typename Group>
void adc<dev>::enable_channels(sample_array<Group> &out_samples)
{
    auto spl_adc = pick_spl_adc();

    ADC_InitTypeDef         init_struct;

    // Parameters that are not dependent on channels.

    init_struct.ADC_Resolution              = ADC_Resolution_10b;
    init_struct.ADC_ContinuousConvMode      = DISABLE;
    init_struct.ADC_ExternalTrigConvEdge    = extract_value(Group::trigger_edge);
    init_struct.ADC_ExternalTrigConv        = extract_value(Group::trigger);
    init_struct.ADC_DataAlign               = ADC_DataAlign_Right;

    // Parameters that are dependent on the channels count.

    // TODO: Use Group::template extractor<extractor>::scan_mode() here
    // if DMA is used.
    init_struct.ADC_ScanConvMode     = ENABLE;
    init_struct.ADC_NbrOfConversion  = Group::template extractor<extractor>::conv_num();

    ADC_Init(spl_adc, &init_struct);

    // Init channels one by one.
    Group::template extractor<extractor>::init_channels(spl_adc);

    m_mode      = current_mode::single;
    m_samples   = out_samples.data();
    m_convs     = out_samples.size();

    ADC_ContinuousModeCmd(spl_adc, DISABLE);
    ADC_EOCOnEachRegularChannelCmd(spl_adc, ENABLE);
}

template<adc_dev dev>
void adc<dev>::single()
{
    auto spl_adc = pick_spl_adc();

    // Restore single mode. TODO: read register instead?
    if (m_mode != current_mode::single) {
        ADC_ContinuousModeCmd(spl_adc, DISABLE);
        ADC_EOCOnEachRegularChannelCmd(spl_adc, ENABLE);
        m_mode = current_mode::single;
    }

    // Save and clear external trigger state. TODO: do it in cross-family fashion.
    auto trigger = spl_adc->CR2 & 0x30000000;
    spl_adc->CR2 &= ~(0x30000000);

    ADC_Cmd(spl_adc, ENABLE);

    for (size_t i = 0; i < m_convs; ++i) {
        // Trigger software conversion.
        ADC_SoftwareStartConv(spl_adc);

        // Wait for each channel to be converted.
        while (ADC_GetFlagStatus(spl_adc, ADC_FLAG_EOC) == RESET) ;

        // Get result.
        auto sample = ADC_GetConversionValue(spl_adc);
        m_samples[i] = sample;

        // Clear overrun flag set as a result of conversion.
        // According to RM:
        // If the conversions are slow enough, the conversion sequence can be handled by the
        // software. In this case the EOCS bit must be set in the ADC_CR2 register for the EOC status
        // bit to be set at the end of each conversion, and not only at the end of the sequence.
        //
        // When EOCS = 1, overrun detection is automatically enabled. Thus, each time a conversion is
        // complete, EOC is set and the ADC_DR register can be read. The overrun management is
        // the same as when the DMA is used.
        //
        // To recover the ADC from OVR state when the EOCS is set, follow the steps below:
        //  1. Clear the ADC OVR bit in ADC_SR register
        //  2. Trigger the ADC to start the conversion.
        //
        // Note that in case if overrun is really happen, samples will be invalid.
        // This is a temporary solution before something more reliable (like DMA mode)
        // will be implemented.
        ADC_ClearFlag(spl_adc, ADC_FLAG_OVR);
    }

    // Restore external trigger.
    spl_adc->CR2 |= trigger;

    ADC_Cmd(spl_adc, DISABLE);
}


} // namespace ecl


#endif // PLATFORM_ADC_HPP_
