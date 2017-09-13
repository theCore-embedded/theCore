//! \file
//! \brief STM32 ADC driver.
//! \details Some app notes can be found at:
//! http://goo.gl/9rTqUT (AN3116 application note, STM32’s ADC modes and their applications)
#ifndef PLATFORM_ADC_HPP_
#define PLATFORM_ADC_HPP_

#include "platform/execution.hpp"

#include <stm32_device.hpp>

#include <ecl/utils.hpp>
#include <ecl/err.hpp>
#include <common/adc.hpp>
#include <common/irq.hpp>

#include <array>

namespace ecl
{

//! ADC management mode.
//! \details Samples readings can occur either by software after end-of-conversion
//! IRQ is generated or with help of DMA.
enum class adc_mgmt_mode
{
    dma,    //! ADC managed using DMA.
    irq     //! ADC managed using IRQ.
};

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
};

//! The external trigger edge in case of EXTI line used as a trigger.
//! \details For F4 family EXTI 11 line can be configured as EXTI trigger
//! for regular channel group or EXTI 15 for injected group.
enum class adc_trigger_edge
{
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

//------------------------------------------------------------------------------

//! Channels grouped together along with an external trigger.
//! \tparam trig  Trigger source that will start channel conversion.
//! \tparam chans List of channels that grouped together.
template<adc_trigger trig, adc_channel... chans>
struct channel_group
{
    static constexpr auto trigger       = trig;
    // Does not matter really.
    static constexpr auto trigger_edge  = adc_trigger_edge::rising;

    template<template<adc_channel... chs> class Extractor>
    using extractor = Extractor<chans...>;
};

//! Channels grouped together, conversion triggered by software.
//! \tparam chans List of channels that grouped together.
template<adc_channel... chans>
struct channel_group_sw_trigger
{
    //! Helper type.
    enum { no_trigger = ADC_ExternalTrigConvEdge_None };

    // Does not matter really.
    static constexpr auto trigger       = adc_trigger::t1_cc1;
    static constexpr auto trigger_edge  = no_trigger;

    template<template<adc_channel... chs> class Extractor>
    using extractor = Extractor<chans...>;
};

//! Channels grouped together, conversion triggered by EXTI line.
//! \tparam edge Edge which is used to trigger conversion.
//! \tparam chans List of channels that grouped together.
template<adc_trigger_edge edge, adc_channel... chans>
struct channel_group_exti_trigger
{
     //! Helper type.
    enum { exti_trigger = ADC_ExternalTrigConv_Ext_IT11 };

    static constexpr auto trigger_edge   = edge;
    static constexpr auto trigger        = exti_trigger;

    template<template<adc_channel... chs>class Extractor>
    using extractor = Extractor<chans...>;
};

//------------------------------------------------------------------------------

//! ADC configuration prototype.
//! \details Specific ADC device is pre-configured by providing template
//! specialization of the adc_cfg class. Contents of the adc_cfg specialization
//! depends on ADC management mode used. Two options possible:
//!  - DMA management mode.
//!  - IRQ management mode.
//!
//! \par DMA mode
//! Following fields must be present:
//!  - Management mode constexpr field `mgtm_mode`, set to `adc_mgmt_mode::dma`
//!  - DMA descriptor `dma` for particular ADC. Refer to RM to find appropriate
//!    DMA configuration for given ADC.
//!
//! \code{.cpp}
//! namespace ecl
//! {
//!
//! // Provides configuration of the adc_dev::dev1 (ADC1).
//! template<>
//! struct adc_cfg<adc_dev::dev1>
//! {
//!     // Signifies DMA management mode.
//!     static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::dma;
//!
//!     // DMA2, Stream 0, Channel 0 is dedicated for ADC1
//!     using dma = dma_wrap<dma_stream::dma2_0, dma_channel::ch0>;
//! };
//!
//! } // namespace ecl
//! \endcode
//!
//! \par IRQ mode
//! Following fields must be present:
//!  - Management mode constexpr field `mgtm_mode`, set to `adc_mgmt_mode::irq`
//!
//! // Provides configuration of the adc_dev::dev1 (ADC1).
//! template<>
//! struct adc_cfg<adc_dev::dev1>
//! {
//!     // Signifies IRQ management mode.
//!     static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::irq;
//! };
//!
//! \warning To avoid potential problems with multiple configurations for single
//! ADC, **make sure that full specialization is placed in the
//! header included (directly or indirectly) by all dependent modules.**.
//! Thus, redefinition of the config class for given ADC will result in
//! compilation errors. *Good practice is to place all ADC configuration
//! class in the single target-related header.*
template<adc_dev dev>
struct adc_cfg
{
    // Always assert
    static_assert(std::is_integral<decltype(dev)>::value,
                  "The instance of this generic class should never be "
                          "instantiated. Please write your own template specialization "
                          "of this class. See documentation.");
};


//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

//! ADC event handler. Event code signifies result of a conversion.
using adc_evh = std::function<void(adc_event)>;

//! ADC interrupts helper.
//! \details Accross all 3 ADC there is only 1 interrupt channel avaliable.
//! ADC interrupt helper dispatches incoming interrupt and calls appropriate handlers.
//! \warning Intended to be used only by \ref adc class.
//! Acts like a singleton.
class adc_irq_dispatcher
{
public:
    //! Gets dispatcher instance.
    static inline adc_irq_dispatcher& get_instance();

    //! Subscribes for ADC events and registers given handler handler.
    //! \tparam adc ADC device for which event handler should be registered.
    //! \param[in] h ADC event handler.
    template<adc_dev dev>
    inline void subscribe(const adc_evh &h);

    //! Unsubscribes from ADC events and deregisters event handler.
    //! \tparam adc ADC device for which events should be disabled.
    template<adc_dev dev>
    inline void unsubscribe();

private:
    //! Default event handler.
    static inline void default_evh(adc_event);

    //! ADC IRQ handler.
    //! \param[in] irqn IRQ that served by this handler.
    inline void irqh();

    //! Constructs ADC interrupt dispatcher with default interrupt handlers.
    inline adc_irq_dispatcher();

    //! ADC event handlers. One per each ADC.
    adc_evh m_evhs[adcs_count()];
};

//------------------------------------------------------------------------------

adc_irq_dispatcher &adc_irq_dispatcher::get_instance()
{
    static adc_irq_dispatcher instance;
    return instance;
}

template<adc_dev dev>
void adc_irq_dispatcher::subscribe(const adc_evh &h)
{
    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);
    constexpr auto idx = get_adc_idx(dev);

    // Avoid possible context switch.
    irq::disable();

    ADC_ITConfig(spl_adc, ADC_IT_EOC, ENABLE);
    m_evhs[idx] = h;

    irq::enable();
}

template<adc_dev dev>
void adc_irq_dispatcher::unsubscribe()
{
    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);
    constexpr auto idx = get_adc_idx(dev);

    // Avoid possible context switch.
    irq::disable();

    ADC_ITConfig(spl_adc, ADC_IT_EOC, DISABLE);
    m_evhs[idx] = default_evh;

    irq::enable();
}

//------------------------------------------------------------------------------

adc_irq_dispatcher::adc_irq_dispatcher()
    :m_evhs{}
{
    // Initialize event handlers.
    for (auto &evh : m_evhs) {
        evh = default_evh;
    }

    // Register IRQ handler.
    irq::subscribe(adc_irqn, [this]{ this->irqh(); });

    // Enable ADC interrupt.
    irq::unmask(adc_irqn);
}

void adc_irq_dispatcher::irqh()
{
    for (size_t i = 0; i < adcs_count(); ++i) {
        // TODO: Use adc::pick_spl_adc() somehow
        auto spl_adc = reinterpret_cast<ADC_TypeDef*>(adcs[i]);

        if (ADC_GetITStatus(spl_adc, ADC_IT_EOC) == SET) {
            m_evhs[i](adc_event::eoc);

            ADC_ClearFlag(spl_adc, ADC_FLAG_EOC);
            ADC_ClearITPendingBit(spl_adc, ADC_IT_EOC);
        }
    }

    irq::clear(adc_irqn);
    irq::unmask(adc_irqn);
}

void adc_irq_dispatcher::default_evh(adc_event)
{
    // Event not handled - abort execution.
    ecl::abort();
}

//------------------------------------------------------------------------------


// Forward-declared ADC class.
template<adc_dev dev>
class adc;

//! Management configurator class.
//! \details Helps to configure ADC parameters with respect to the management
//! mode used.
//! \warning Intended to be used only by \ref adc class.
template<adc_dev dev, adc_mgmt_mode mode = adc_cfg<dev>::mgtm_mode>
class mgmt_configurator
{ };

//! Configurator for ADC IRQ mode.
template<adc_dev dev>
class mgmt_configurator<dev, adc_mgmt_mode::irq>
{
public:
    //! Initializes ADC to work in the IRQ mode.
    static void init();

    //! Enables IRQ mode for given channels.
    template<typename Group>
    static void enable_channels(
            typename adc<dev>::template sample_array<Group> &out_samples);

    //! Performs IRQ configuration for single-mode async conversion.
    static void single(const adc_evh &evh);

private:
    //! Handles IRQ from ADC interrupt dispatcher.
    static void irq_handler(adc_event evt);

    //! User handler of ADC events.
    static safe_storage<adc_evh> m_user_evh;

    //! Pointer to a single output sample.
    //! \details IRQ mode currently capable of converting 1 sample at a time.
    static typename adc<dev>::sample_type *m_out;
};

template<adc_dev dev>
safe_storage<adc_evh> mgmt_configurator<dev, adc_mgmt_mode::irq>::m_user_evh;

template<adc_dev dev>
typename adc<dev>::sample_type *mgmt_configurator<dev, adc_mgmt_mode::irq>::m_out;

//------------------------------------------------------------------------------

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::irq>::init()
{
    // TODO: assert if not inited.

    // Init user handler storage.
    m_user_evh.init();

    // Register own ADC IRQ handler.
    adc_irq_dispatcher::get_instance().subscribe<dev>(irq_handler);
}

template<adc_dev dev>
template<typename Group>
void mgmt_configurator<dev, adc_mgmt_mode::irq>::enable_channels(
        typename adc<dev>::template sample_array<Group> &out_samples)
{
    // TODO: assert that already inited.

    static_assert(out_samples.size() == 1,
                  "Cannot sample more than one channel in IRQ mode. Use DMA instead.");

    m_out = &out_samples[0];
}

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::irq>::single(const adc_evh &evh)
{
    // TODO: assert that already inited.

    // Save user handler.
    m_user_evh.get() = evh;
}

//------------------------------------------------------------------------------

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::irq>::irq_handler(adc_event evt)
{
    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);

    if (evt == adc_event::eoc) {
        // Read the value.
        *m_out = ADC_GetConversionValue(spl_adc);

        // Shutdown ADC - not needed anymore.
        ADC_Cmd(spl_adc, DISABLE);

        // IRQs will be enabled.
    }

    // Inform user.
    m_user_evh.get()(evt);
}

//------------------------------------------------------------------------------

//! Configurator for ADC DMA mode.
//! \warning Intended to be used only by \ref adc class.
template<adc_dev dev>
class mgmt_configurator<dev, adc_mgmt_mode::dma>
{
public:
    //! Initializes ADC to work in the DMA mode.
    static void init();

    //! Enables DMA mode for given channels.
    template<typename Group>
    static void enable_channels(
            typename adc<dev>::template sample_array<Group> &out_samples);

    //! Performs DMA configuration for single-mode async conversion.
    static void single(const adc_evh &evh);
private:
    //! Handles DMA IRQ.
    static void irq_handler();

    //! User handler of ADC events.
    static safe_storage<adc_evh> m_user_evh;
};

template<adc_dev dev>
safe_storage<adc_evh> mgmt_configurator<dev, adc_mgmt_mode::dma>::m_user_evh;

//------------------------------------------------------------------------------

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::dma>::init()
{
    // TODO: assert if not inited.

    m_user_evh.init();

    constexpr auto dma_irqn = adc_cfg<dev>::dma::get_irqn();

    adc_cfg<dev>::dma::init();
    // TODO: add DMA error flags & handling.
    adc_cfg<dev>::dma::template enable_events_irq<true, false, false>();

    // Subscribe for DMA events.
    irq::subscribe(dma_irqn, irq_handler);
    irq::unmask(dma_irqn);
}

template<adc_dev dev>
template<typename Group>
void mgmt_configurator<dev, adc_mgmt_mode::dma>::enable_channels(
        typename adc<dev>::template sample_array<Group> &out_samples)
{
    // TODO: assert that already inited.

    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);

    // Prepare DMA
    adc_cfg<dev>::dma::template periph_to_mem<dma_data_sz::hword>(
                reinterpret_cast<volatile uint16_t*>(&spl_adc->DR),
                reinterpret_cast<uint8_t*>(out_samples.data()),
                out_samples.size() * sizeof(typename adc<dev>::sample_type));
}

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::dma>::single(const adc_evh &evh)
{
    // TODO: assert that already inited.

    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);

    m_user_evh.get() = evh;

    // Good to go. ADC will be enabled/triggered in the ADC class.
    adc_cfg<dev>::dma::enable();

    // Enable DMA requests from ADC periphery.
    ADC_DMACmd(spl_adc, ENABLE);
}

//------------------------------------------------------------------------------

template<adc_dev dev>
void mgmt_configurator<dev, adc_mgmt_mode::dma>::irq_handler()
{
    // TODO: add DMA error flags & handling.

    constexpr auto dma_irqn = adc_cfg<dev>::dma::get_irqn();

    // TODO: Use adc::pick_spl_adc() somehow
    auto spl_adc = reinterpret_cast<ADC_TypeDef*>(dev);

    if (adc_cfg<dev>::dma::ht()) {
        // Half of channels are converted
        m_user_evh.get()(adc_event::hc);
        adc_cfg<dev>::dma::clear_ht();
    }

    if (adc_cfg<dev>::dma::tc()) {
        m_user_evh.get()(adc_event::eoc);

        // Stop ADC
        ADC_DMACmd(spl_adc, DISABLE);
        ADC_Cmd(spl_adc, DISABLE);

        // Stop DMA
        adc_cfg<dev>::dma::clear_tc();
        adc_cfg<dev>::dma::disable();

    }

    irq::clear(dma_irqn);
    irq::unmask(dma_irqn);
}

//------------------------------------------------------------------------------

//! ADC peripheral class.
//! \details Provides API to work with ADC on stm32 platform.
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

    //! Configures ADC to work with given channels, in IRQ mode.
    //! \param[in,out] out_samples Samples array that will be populated with
    //! target samples. Should not be destroyed until conversion of given
    //! channels will complete.
    //! \note ADC configured in IRQ mode cannot handle conversion of more than
    //! 1 channel. Use DMA mode. See \ref ecl::adc_cfg to get hints about how
    //! to configure ADC in DMA mode.
    template<typename Group>
    static void enable_channels(sample_array<Group> &out_samples);

    //! Performs single conversion of the previously configured channels
    //! and waits till the end of the conversion.
    //! \details Results will be saved to a buffer previously passed with
    //! enable_channels(). If external trigger is selected then conversion will
    //! start after trigger will hit. Otherwise, conversion will be immediately
    //! triggered by software.
    //! \pre Channels configured with use_channels() call.
    //! \return Result of operation.
    //! \retval ecl::err::ok Channels are converted and data is stored in the buffer.
    //! \todo Implement support for external triggers.
    static err single();

    //! Performs single conversion of the previously configured channels asynchronously.
    //! \details After end of conversion event handler will be called.
    //! In case of successfull conversion results will be saved to a buffer
    //! previously passed with enable_channels(). If external trigger is
    //! selected then conversion will start after trigger will hit.
    //! Otherwise, conversion will be immediately triggered by software.
    //! \pre Channels configured with use_channels() call.
    //! \return Result of operation.
    //! \retval ecl::err::ok Channels conversion is pending.
    //! \todo Implement support for external triggers.
    static err single(const adc_evh &evh);

private:
    //! Picks peripheral clock asociated with given ADC.
    static constexpr auto pick_periph();

    //! Picks ADC MMIO pointer for given ADC.
    static auto           pick_spl_adc();

    //! ADC mode. See ADC_ContinuousConvMode.
    enum class conversion_mode
    {
        single,     //! Perform single conversion of the channel group.
        continuous, //! Continue to convert channels after previous conversion round finished.
    };

    //! Current ADC conversion mode.
    static conversion_mode m_conv_mode;
};

template<adc_dev dev>
typename adc<dev>::conversion_mode adc<dev>::m_conv_mode;

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
    // TODO: assert if not inited

    constexpr auto periph = pick_periph();
    RCC_APB2PeriphClockCmd(periph, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE);

    mgmt_configurator<dev>::init();
}

template<adc_dev dev>
template<typename Group>
void adc<dev>::enable_channels(sample_array<Group> &out_samples)
{
    // TODO: assert if inited

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
    init_struct.ADC_ScanConvMode     = Group::template extractor<extractor>::scan_mode();
    init_struct.ADC_NbrOfConversion  = Group::template extractor<extractor>::conv_num();

    ADC_Init(spl_adc, &init_struct);

    // Init channels one by one.
    Group::template extractor<extractor>::init_channels(spl_adc);

    m_conv_mode = conversion_mode::single;
    ADC_ContinuousModeCmd(spl_adc, DISABLE);

    mgmt_configurator<dev>::template enable_channels<Group>(out_samples);
}

template<adc_dev dev>
err adc<dev>::single()
{
    // TODO: assert if inited

    // Busy-wait end of conversion.
    volatile int spin = 0;
    auto evh = [&spin](adc_event) { spin = 1; };
    single(evh);

    // TODO: wait for interrupt here.

    while(!spin);

    return err::ok;
}

template<adc_dev dev>
err adc<dev>::single(const adc_evh &evh)
{
    // TODO: assert if inited

    auto spl_adc = pick_spl_adc();

    // Restore single mode. TODO: read register instead?
    if (m_conv_mode != conversion_mode::single) {
        ADC_ContinuousModeCmd(spl_adc, DISABLE);
        m_conv_mode = conversion_mode::single;
    }

    mgmt_configurator<dev>::single(evh);

    // Prepare ADC. Conversion will start as soon as trigger fires in case
    // if trigger was selected. Otherwise, software will trigger the conversion
    // (see below).
    ADC_Cmd(spl_adc, ENABLE);

    // Check if software trigger is needed.
    // TODO: do it in cross-family fashion
    if (!(spl_adc->CR2 & 0x30000000)) {
        ADC_SoftwareStartConv(spl_adc);
    }

    // From now, conversion is either ongoing or pending. mgmt_configurator class
    // will decide whether to disable ADC and clear some flags or not.

    return err::ok;
}


} // namespace ecl


#endif // PLATFORM_ADC_HPP_
