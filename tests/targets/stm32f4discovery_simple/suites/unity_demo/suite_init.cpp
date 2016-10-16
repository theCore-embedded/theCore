#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>

void suite_board_init()
{
    // Only console is required for this suite

    GPIO_InitTypeDef init;
    GPIO_StructInit(&init);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    init.GPIO_Mode = GPIO_Mode_AF;

    GPIO_Init(GPIOD, &init);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
}
