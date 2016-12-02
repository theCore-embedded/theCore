//! \file
//! \brief Suite initialization of the TI TIVAC (TM4C) platform BAT suite.

#include <aux/pin_cfg.hpp>
#include <pin_map.h>

void suite_board_init()
{
    // GPIO LEDS

    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::f>();

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f,
        ecl::gpio_hw::num::pin1,
        ecl::gpio_hw::num::pin2,
        ecl::gpio_hw::num::pin3>();

    // GPIO push button
    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin4>(ecl::gpio_hw::dir::in);

    // Console
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::a>();

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::a,
        ecl::gpio_hw::num::pin0,
        ecl::gpio_hw::num::pin1>(ecl::gpio_hw::dir::hw);

    // Console AF

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
}
