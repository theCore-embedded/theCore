// Source file contains target-specific implementations.

#include "target.hpp"

void gpio_init()
{
    // pinmux:

    // PD8 - USART3 TX
    // PD9 - USART3 RX

    // PD4 - CS43L22 RST

    // PC7  - CS43L22 I2S
    // PC10 - CS43L22 I2S
    // PC12 - CS43L22 I2S
    // PA4  - CS43L22 I2S

    // PB6 - CS43L22 I2C
    // PB9 - CS43L22 I2C

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
    | RCC_AHB1Periph_GPIOB
    | RCC_AHB1Periph_GPIOC
    | RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef init;
    GPIO_StructInit(&init);

    // Console

    init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOD, &init);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    // CS43L22 RST

    init.GPIO_Pin = GPIO_Pin_4;
    init.GPIO_Mode = GPIO_Mode_OUT;
    init.GPIO_PuPd = GPIO_PuPd_DOWN;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOD, &init);

    // CS43L22 I2S

    init.GPIO_Pin = GPIO_Pin_4;
    init.GPIO_Mode = GPIO_Mode_AF;
    init.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &init);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);

    init.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOC, &init);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

    // CS43L22 I2C

    init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
    init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    init.GPIO_OType = GPIO_OType_OD;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOB, &init);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
}
