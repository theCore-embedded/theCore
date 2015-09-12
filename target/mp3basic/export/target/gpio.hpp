#ifndef TARGET_GPIO_HPP
#define TARGET_GPIO_HPP

#include <platform/gpio_device.hpp>

// Bunch of leds present on stm32discovery board
// TODO: do I need better and more target-independent names?
using LED_Red      = GPIO< PinPort::PORT_D, PinNum::PIN_14 >;
using LED_Green    = GPIO< PinPort::PORT_D, PinNum::PIN_12 >;
using LED_Blue     = GPIO< PinPort::PORT_D, PinNum::PIN_15 >;
using LED_Orange   = GPIO< PinPort::PORT_D, PinNum::PIN_13 >;

#endif //TARGET_GPIO_HPP 
