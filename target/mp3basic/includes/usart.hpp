#ifndef TARGET_USART_HPP_
#define TARGET_USART_HPP_

#include <usart_device.hpp>
#include <cstdint>

using ConsoleDriver = UsartHandler< (std::uintptr_t) USART3 >;

#endif
