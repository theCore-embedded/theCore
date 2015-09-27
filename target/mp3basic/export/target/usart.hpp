#ifndef TARGET_USART_HPP_
#define TARGET_USART_HPP_

#include <platform/usart_device.hpp>
#include <cstdint>

using ConsoleDriver = UsartHandler< UsartDevices::DEV_3,
									UsartMode::IRQ,
									UsartState::RXP >;

#endif
