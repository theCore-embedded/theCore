/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief List of GPIOs exported by target, used by the test case.
#ifndef STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_
#define STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_

#include <platform/gpio_device.hpp>
#include <aux/generated.hpp>

namespace ecl
{

//! List of test GPIOs used by the test case.
template<typename ...Gs>
struct gpio_list
{
    //! Test executor interface.
    template<template<typename ...> class E>
    using executor = E<Gs...>;
};

// Device LEDs and push button are defined in target_defs.json

//! Exported list of GPIOs.
using test_gpios = gpio_list<red_led, blue_led, green_led, orange_led>;


} // namespace ecl

#endif // STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_
