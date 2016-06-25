//! \file
//! \brief Platform console driver of the stm32l1xx platform

#ifndef STM32L1XX_CONSOLE_DRIVER_HPP_
#define STM32L1XX_CONSOLE_DRIVER_HPP_

#include <stm32l1xx_usart.h>

namespace ecl
{

void bypass_putc(char c);

}

#endif // STM32L1XX_CONSOLE_DRIVER_HPP_
