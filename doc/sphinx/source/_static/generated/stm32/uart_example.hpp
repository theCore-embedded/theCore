/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of u(s)art driver configuration
#ifndef STM32XX_UART_CFGS_HPP_
#define STM32XX_UART_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/usart_bus.hpp>


namespace ecl
{

// UART configuration ---------------------------------------------------------


/* Console UART */

template<>
struct usart_cfg<usart_device::dev3>
{
    static auto constexpr baudrate  = 115200;
    static auto constexpr word_len  = USART_WordLength_8b;
    static auto constexpr stop_bit  = USART_StopBits_1;
    static auto constexpr parity    = USART_Parity_No;
    static auto constexpr mode      = USART_Mode_Rx | USART_Mode_Tx;
    static auto constexpr hw_flow   = USART_HardwareFlowControl_None;
};

using USART3_cfg = usart_cfg<usart_device::dev3>;
using USART3_driver = usart_bus<usart_device::dev3>;
using console_uart = USART3_driver;

/* Just another driver */

template<>
struct usart_cfg<usart_device::dev1>
{
    static auto constexpr baudrate  = 9600;
    static auto constexpr word_len  = USART_WordLength_8b;
    static auto constexpr stop_bit  = USART_StopBits_1;
    static auto constexpr parity    = USART_Parity_No;
    static auto constexpr mode      = USART_Mode_Rx | USART_Mode_Tx;
    static auto constexpr hw_flow   = USART_HardwareFlowControl_None;
};

using UART1_cfg = usart_cfg<usart_device::dev1>;
using UART1_driver = usart_bus<usart_device::dev1>;
using my_uart = UART1_driver;

// Console configuration -------------------------------------------------------


// Bypass console configuration ------------------------------------------------

static inline void bypass_putc(char c) { (void)c; }

} // namespace ecl

#endif // STM32XX_UART_CFGS_HPP_
