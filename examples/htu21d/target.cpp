// Source file contains target-specific implementations.

#include "target.hpp"

void gpio_init()
{
    // pinmux:

    // PD5 - USART2 TX
    // PD6 - USART2 RX

    // PB6 - HTU21D I2C
    // PB9 - HTU21D I2C

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef init;
    GPIO_StructInit(&init);

    // Console

    init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOD, &init);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    // HTU21D I2C

    init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
    init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    init.GPIO_OType = GPIO_OType_OD;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOB, &init);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
}
