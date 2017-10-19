/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FC-28 Soil Humidity/Moisture Detection Sensor Module.
//! \details No official documentation is provided whatsoever.
//! But some information can be found at:
//! - http://www.fecegypt.com/uploads/dataSheet/1480854383_water%20and%20soil.pdf
//! - http://arduinosensors.com/index.php/soil-moisture-sensor-interface-with-arduino-uno/
//!
#ifndef DEV_SENSOR_FC28_HPP_
#define DEV_SENSOR_FC28_HPP_

#include <ecl/assert.h>
#include <ecl/err.hpp>
#include <common/gpio.hpp>

namespace ecl
{

namespace sensor
{

namespace fc28_defs
{

//! Moisture value resolution. I.e. amount of bits dedicated for holding
//! the moisture. See also \ref max_moisture and \ref min_moisture
static constexpr auto resolution = 8;

//! Type that represents a moisture.
//! \details Higher and lower bound are defined by \ref max_moisture and
//! \ref min_moisture . In between amount of water contained in soil is
//! propotional to the value itself. Well, at least it is naively assumed.
//! No calibration is provided with sensor.
using moisture_type = int;

//! Maximum value of soil moisture.
//! \details Upper bound defined when sensor is fully immersed in water.
//! I.e. sampled volume contains 100% water.
static constexpr moisture_type max_moisture = (1 << resolution) - 1;

//! Minimum value of soild moisture.
//! \details Lower bound defined
//! I.e. sampled volume contains 0% water.
static constexpr moisture_type min_moisture = 0;

} // namespace fc28_defs

//! FC-28 Soil Humidity/Moisture Sensor class.
//! \tparam PowerPin Power GPIO connected to the power input of FC-28.
//! \details According to what can be found in Web, power pin must be capable
//! to supply around as 20-30 ma at 5V.
//! In practice, 3V is enough too.
//! If sensor is connected directly to the power output pin on the board and
//! there is no way to control it (usually it is always "on") then
//! \ref ecl::empty_gpio can be used in a place of PowerPin.
//! \par ADC control
//! ADC must be controlled outside of this sensor. This is required to avoid
//! any race conditions when multiple sensors access the same ADC.
//! To convert ADC sample to the moisture value, use \ref get_moisture() routine.
template<typename PowerPin = ecl::empty_gpio>
class fc28
{
public:
    //! Power-on the sensor.
    static void power_on();

    //! Power-off the sensor.
    static void power_off();

    //! Gets moisture value based on previously converted ADC sample.
    //! \tparam St Sample type.
    //! \tparam Rt Resolution type.
    //! \param[in]  sample      ADC sample.
    //! \param[in]  resolution  ADC resolution, in bits.
    //! \param[out] moisture    Resulting moisture. Valid value can be read only
    //! if operation is successfull.
    //! \return Status of operation. err::ok indicates successfull operation.
    template<typename St, typename Rt>
    static err get_moisture(St sample, Rt resolution, fc28_defs::moisture_type &moisture);
};

//------------------------------------------------------------------------------

template<typename PowerPin>
void fc28<PowerPin>::power_on()
{
    PowerPin::set();
}

template<typename PowerPin>
void fc28<PowerPin>::power_off()
{
    PowerPin::reset();
}

template<typename PowerPin>
template<typename St, typename Rt>
err fc28<PowerPin>::get_moisture(St sample, Rt resolution, fc28_defs::moisture_type &moisture)
{
    using namespace ecl::sensor::fc28_defs;

    // Must be greather than 0.
    ecl_assert(resolution > 0);

    // Must fall within a range.
    ecl_assert(sample >= 0 && sample < (1 << resolution));

    // ADC resolution that used to test and calibrate this sensor during development.
    constexpr auto calibr_resolution = 10;

    // Maximum sensor value obtained from ADC during development.
    // Maximum value corresponds to the minumum water content in the soil.
    // Value is obtained from ADC with calibr_resolution bits of resolution.
    // In fact it equals to the maximum value produced by ADC.
    constexpr auto calibr_max_value = 0x3ff;

    // Minimum sensor value obtained from ADC during development.
    // Minimum value corresponds to the maximum water content in the soil.
    // Value is obtained from ADC with calibr_resolution bits of resolution.
    constexpr auto calibr_min_value = 0xff;

    // Range of sensor values obtained during calibration.
    constexpr auto calibr_range = calibr_max_value - calibr_min_value;

    // Range of output moisture values.
    constexpr auto out_range = max_moisture - min_moisture;

    // Map real ADC sample to a calibr_resolution space.
    auto mapped_sample = sample * (1 << calibr_resolution) / (1 << resolution);

    // Scale sample so it will fit within output range [0, 2 ^ moisture_resolution).
    // TODO: return error if mapped_sample < calibr_min_value ?
    mapped_sample = mapped_sample < calibr_min_value ? 0 : mapped_sample - calibr_min_value;
    mapped_sample = out_range * mapped_sample / calibr_range;

    // Finally, convert sample to a moisture.
    moisture = max_moisture - mapped_sample;

    return err::ok;
}

} // namespace sensor

} // namespace ecl

#endif // DEV_SENSOR_FC28_HPP_
