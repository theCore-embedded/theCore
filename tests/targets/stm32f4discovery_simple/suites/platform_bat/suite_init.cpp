#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

void suite_board_init()
{
    // Init Port D clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef init_struct;
    GPIO_StructInit(&init_struct);

    // LED GPIO configuration

    init_struct.GPIO_Mode   = GPIO_Mode_OUT;
    init_struct.GPIO_OType  = GPIO_OType_PP;
    init_struct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    init_struct.GPIO_Pin    = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

    // Init Port D LED GPIOs
    GPIO_Init(GPIOD, &init_struct);

    // Console

    init_struct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    init_struct.GPIO_Mode = GPIO_Mode_AF;

    // Init Port D UART

    GPIO_Init(GPIOD, &init_struct);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    // Init Port A clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_StructInit(&init_struct);

    // Push button GPIO configuration

    init_struct.GPIO_Mode   = GPIO_Mode_IN;
    init_struct.GPIO_OType  = GPIO_OType_PP;
    init_struct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    init_struct.GPIO_Pin    = GPIO_Pin_0;

    GPIO_StructInit(&init_struct);
}
