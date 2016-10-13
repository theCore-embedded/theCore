//! \file
//! \brief
//! \todo Autogenerate this file

#include <unity.h>
#include <unity_fixture.h>

#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>

extern "C"
void board_init()
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

TEST_GROUP_RUNNER(demo_group)
{
    RUN_TEST_CASE(demo_group, test1);
}

static void suite_runner()
{
    RUN_TEST_GROUP(demo_group);
}

int main()
{
    UNITY_BEGIN();
    suite_runner();
    UNITY_END();
    for (;;);
}
