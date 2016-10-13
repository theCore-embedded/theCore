//! \file
//! \brief List of GPIOs exported by target, used by the test case.
#ifndef STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_
#define STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_

#include <platform/gpio_device.hpp>

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

// Device LEDs

using red_led = gpio<gpio_port::d, gpio_num::pin14>;
using blue_led = gpio<gpio_port::d, gpio_num::pin15>;
using green_led = gpio<gpio_port::d, gpio_num::pin12>;
using orange_leds = gpio<gpio_port::d, gpio_num::pin13>;

//! Exported list of GPIOs.
using test_gpios = gpio_list<red_led, blue_led, green_led, orange_leds>;

} // namespace ecl

#endif // STM32DISC_PLATFORM_BAT_TEST_GPIO_HPP_
