//! \file
//! \brief Common definitions for ADC module.
#ifndef PLATFORM_COMMON_ADC_
#define PLATFORM_COMMON_ADC_

namespace ecl
{

//! ADC events that can occur during coversion.
enum class adc_event
{
    hc,     //!< Half conversion event. Generated when half of channels are converted.
    eoc,    //!< End of conversion event. Generated when all channels are converted.
    ovr,    //!< Overrun event. Generated when data is lost during conversion.
};

} // namespace ecl

#endif // PLATFORM_COMMON_ADC_
