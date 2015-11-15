#ifndef TARGET_USART_HPP_
#define TARGET_USART_HPP_

#include <platform/usart_device.hpp>

using console_driver =
USART_dev< USART_device::dev_3,
USART_mode::poll,
USART_state::RXP >;


#endif
