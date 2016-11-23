//! \file
//! \brief Platform console implementation

#include <stm32_device.hpp>
#include "platform/console.hpp"

namespace ecl
{

struct bypass_console
{
    static void init()
    {
        // TODO: Duplicates usart_bus::init(), but it is ok for this moment

        USART_InitTypeDef init_struct;
        constexpr auto rcc_periph = platform_console::pick_rcc();
        constexpr auto rcc_fn = platform_console::pick_rcc_fn();
        constexpr auto usart = platform_console::pick_usart();

        // Enable peripheral clock
        rcc_fn(rcc_periph, ENABLE);

        // Configure UART
        init_struct.USART_BaudRate = usart_cfg<ECL_CONSOLE_DEVICE>::baudrate;
        init_struct.USART_WordLength = usart_cfg<ECL_CONSOLE_DEVICE>::word_len;
        init_struct.USART_StopBits = usart_cfg<ECL_CONSOLE_DEVICE>::stop_bit;
        init_struct.USART_Parity = usart_cfg<ECL_CONSOLE_DEVICE>::parity;
        init_struct.USART_Mode = usart_cfg<ECL_CONSOLE_DEVICE>::mode;
        init_struct.USART_HardwareFlowControl
                = usart_cfg<ECL_CONSOLE_DEVICE>::hw_flow;

        USART_Init(usart, &init_struct);
        USART_Cmd(usart, ENABLE);
    }

    static void putc(char c)
    {
        constexpr auto usart = platform_console::pick_usart();

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


