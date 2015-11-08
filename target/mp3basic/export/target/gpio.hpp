#ifndef TARGET_GPIO_HPP
#define TARGET_GPIO_HPP

#include <platform/gpio_device.hpp>

// TODO: do I need better and more target-independent names?

// Bunch of leds present on stm32discovery board
using LED_Red      = GPIO< pin_port::port_d, pin_number::pin_14 >;
using LED_Green    = GPIO< pin_port::port_d, pin_number::pin_12 >;
using LED_Blue     = GPIO< pin_port::port_d, pin_number::pin_15 >;
using LED_Orange   = GPIO< pin_port::port_d, pin_number::pin_13 >;

// LCD pins
using PCD8544_Reset = GPIO< pin_port::port_c, pin_number::pin_1 >;
using PCD8544_Mode  = GPIO< pin_port::port_c, pin_number::pin_2 >;
using PCD8544_CS    = GPIO< pin_port::port_b, pin_number::pin_12 >;

// SDSPI CS
using SDSPI_CS      = GPIO< pin_port::port_c, pin_number::pin_8 >;

#endif //TARGET_GPIO_HPP 
