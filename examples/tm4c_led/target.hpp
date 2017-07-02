// Header contains target-specific interfaces.

#ifndef EXAMPLE_LED_TARGET_HPP_
#define EXAMPLE_LED_TARGET_HPP_

#include <aux/uart_bus.hpp>
#include <platform/gpio_device.hpp>


// Initializes pins
void gpio_init();

using led_red = ecl::gpio<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin1>;
using led_blue = ecl::gpio<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin2>;
using led_green = ecl::gpio<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin3>;

#endif // EXAMPLE_LED_TARGET_HPP_
