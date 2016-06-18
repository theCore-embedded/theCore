// Header contains target-specific interfaces.

#ifndef EXAMPLE_EXTI_TARGET_HPP_
#define EXAMPLE_EXTI_TARGET_HPP_

#include <platform/usart_bus.hpp>
#include <platform/gpio_device.hpp>
#include <stm32f4xx_gpio.h>


// Initializes pins
void gpio_init();

// User button
using usr_btn = ecl::gpio<ecl::gpio_port::a, ecl::gpio_num::pin0>;

#endif // EXAMPLE_EXTI_UART_TARGET_HPP_
