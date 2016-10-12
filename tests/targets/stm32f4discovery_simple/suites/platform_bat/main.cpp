//! \file
//! \brief
//! \todo Autogenerate this file

#include <platform/console.hpp>

#include <unity.h>
#include <unity_fixture.h>

#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>


extern "C"
void board_init()
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
}


TEST_GROUP_RUNNER(gpio_bat)
{
    RUN_TEST_CASE(gpio_bat, set_reset);
    RUN_TEST_CASE(gpio_bat, toggle);
}

static void suite_runner()
{
    RUN_TEST_GROUP(gpio_bat);
}

int main()
{
    UNITY_BEGIN();
    suite_runner();
    UNITY_END();

    // Suite completed...
    for (;;);
}
