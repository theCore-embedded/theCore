//! \file
//! \brief Platform console implementation

#include <stm32l1xx_rcc.h>
#include <stm32l1xx_usart.h>

namespace ecl
{

void bypass_console_init()
{
    // Debug platform console

    USART_InitTypeDef usart_init;
    USART_StructInit(&usart_init);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    usart_init.USART_BaudRate               = 115200;
    usart_init.USART_WordLength             = USART_WordLength_8b;
    usart_init.USART_StopBits               = USART_StopBits_1;
    usart_init.USART_Parity                 = USART_Parity_No;
    usart_init.USART_Mode                   = USART_Mode_Rx | USART_Mode_Tx;
    usart_init.USART_HardwareFlowControl    = USART_HardwareFlowControl_None;

    USART_Init(USART3, &usart_init);
    USART_Cmd(USART3, ENABLE);

}

void bypass_putc(char c)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, c);
}

}
