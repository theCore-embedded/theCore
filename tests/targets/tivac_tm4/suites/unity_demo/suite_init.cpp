#include <aux/pin_cfg.hpp>
#include <pin_map.h>

void suite_board_init()
{
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::a>();

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::a,
        ecl::gpio_hw::num::pin0,
        ecl::gpio_hw::num::pin1>(ecl::gpio_hw::dir::hw);

    // AF

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
}
