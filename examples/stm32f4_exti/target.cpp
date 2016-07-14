// Source file contains target-specific implementations.

#include "target.hpp"

void gpio_init()
{
    // pinmux:
    // PA0 - user button
    // PD8 - USART3 TX
    // PD9 - USART3 RX

    GPIO_InitTypeDef init;
    GPIO_StructInit(&init);

    // Button

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    init.GPIO_Pin = GPIO_Pin_0;
    init.GPIO_Mode = GPIO_Mode_IN;

    GPIO_Init(GPIOA, &init);

    // Console

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOD, &init);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
}
