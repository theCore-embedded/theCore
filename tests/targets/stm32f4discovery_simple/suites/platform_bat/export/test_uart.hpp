//! \file
//! \brief Test UART for gpio_case test
#ifndef STM32F4_PLATFORM_BAT_TEST_UART_HPP_
#define STM32F4_PLATFORM_BAT_TEST_UART_HPP_

#include <aux/usart_bus.hpp>

namespace ecl
{

using test_uart = usart_bus<usart_device::dev3>;

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

}

#endif // STM32F4_PLATFORM_BAT_TEST_UART_HPP_
