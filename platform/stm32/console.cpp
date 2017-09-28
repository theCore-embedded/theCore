//! \file
//! \brief Platform console implementation

#include <stm32_device.hpp>
#include "platform/console.hpp"

#if THECORE_CONFIG_USE_BYPASS_CONSOLE

namespace ecl
{

struct bypass_console
{
    static void init()
    {
        // TODO: Duplicates usart_bus::init(), but it is ok for this moment

        USART_InitTypeDef init_struct;
        auto rcc_periph = bypass_console_bus::pick_rcc();
        auto rcc_fn = bypass_console_bus::pick_rcc_fn();
        auto usart = bypass_console_bus::pick_usart();

        // Enable peripheral clock
        rcc_fn(rcc_periph, ENABLE);

        // Configure UART
        init_struct.USART_BaudRate              = bypass_config::baudrate;
        init_struct.USART_WordLength            = bypass_config::word_len;
        init_struct.USART_StopBits              = bypass_config::stop_bit;
        init_struct.USART_Parity                = bypass_config::parity;
        init_struct.USART_Mode                  = bypass_config::mode;
        init_struct.USART_HardwareFlowControl   = bypass_config::hw_flow;

        USART_Init(usart, &init_struct);
        USART_Cmd(usart, ENABLE);
    }

    static void putc(char c)
    {
        auto usart = bypass_console_bus::pick_usart();

        while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
        USART_SendData(usart, c);
    }
};

} // namespace ecl

void bypass_console_init()
{
    ecl::bypass_console::init();
}

void ecl::bypass_putc(char c)
{
    ecl::bypass_console::putc(c);
}

#endif // THECORE_CONFIG_USE_BYPASS_CONSOLE
