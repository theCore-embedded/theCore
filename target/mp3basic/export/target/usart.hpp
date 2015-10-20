#ifndef TARGET_USART_HPP_
#define TARGET_USART_HPP_

#include <platform/usart_device.hpp>
#include <cstdint>

using console_driver = USART_dev< USART_device::dev_3,
                                    USART_mode::poll,
                                    USART_state::RXP >;

// Convinient wrapper. For now.
static void write_string(const char *str)
{
    size_t sz = 0;
    while (*(str + sz++) != 0) {}

    console_driver::write((const uint8_t *)str, sz - 1);
}


#endif
