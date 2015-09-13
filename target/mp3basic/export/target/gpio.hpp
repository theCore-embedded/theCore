#ifndef TARGET_GPIO_HPP
#define TARGET_GPIO_HPP

#include <platform/gpio_device.hpp>

// TODO: do I need better and more target-independent names?

// Bunch of leds present on stm32discovery board
using LED_Red      = GPIO< PinPort::PORT_D, PinNum::PIN_14 >;
using LED_Green    = GPIO< PinPort::PORT_D, PinNum::PIN_12 >;
using LED_Blue     = GPIO< PinPort::PORT_D, PinNum::PIN_15 >;
using LED_Orange   = GPIO< PinPort::PORT_D, PinNum::PIN_13 >;

// LCD pins
using PCD8544_Reset = GPIO< PinPort::PORT_C, PinNum::PIN_1 >;
using PCD8544_Mode  = GPIO< PinPort::PORT_C, PinNum::PIN_2 >;
using PCD8544_CS    = GPIO< PinPort::PORT_B, PinNum::PIN_12 >;

#endif //TARGET_GPIO_HPP 
