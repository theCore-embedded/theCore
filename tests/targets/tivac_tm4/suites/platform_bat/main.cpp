//! \file
//! \brief
//! \todo Autogenerate this file

#include <unity.h>
#include <unity_fixture.h>

#include <aux/pin_cfg.hpp>

extern "C"
void board_init()
{
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::f>();

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f,
        ecl::gpio_hw::num::pin1,
        ecl::gpio_hw::num::pin2,
        ecl::gpio_hw::num::pin3>();
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
    suite_runner();
    // Suite completed...
    for (;;);
}
