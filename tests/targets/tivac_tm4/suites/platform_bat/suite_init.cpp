#include <aux/pin_cfg.hpp>

void suite_board_init()
{
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::f>();

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f,
        ecl::gpio_hw::num::pin1,
        ecl::gpio_hw::num::pin2,
        ecl::gpio_hw::num::pin3>();
}
