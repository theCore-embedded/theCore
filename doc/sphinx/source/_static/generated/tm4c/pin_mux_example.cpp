/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Pin multiplexer and configurator for TM4C platform.

#include <aux/platform_defines.hpp>
#include <aux/pin_cfg.hpp>
#include <pin_map.h>


extern "C" void gpio_init_generated()
{
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::a>();
    ecl::gpio_hw::enable_periph<ecl::gpio_hw::port::f>();

    /* LED pins */

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin1>(
            ecl::gpio_hw::dir::out,
            ecl::gpio_hw::pad_type::push_pull,
            ecl::gpio_hw::strength::i2ma
        );


    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin2>(
            ecl::gpio_hw::dir::out,
            ecl::gpio_hw::pad_type::push_pull,
            ecl::gpio_hw::strength::i2ma
        );


    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::f, ecl::gpio_hw::num::pin3>(
            ecl::gpio_hw::dir::out,
            ecl::gpio_hw::pad_type::push_pull,
            ecl::gpio_hw::strength::i2ma
        );

    /* UART0 console pins */

    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::a, ecl::gpio_hw::num::pin0>(
            ecl::gpio_hw::dir::hw,
            ecl::gpio_hw::pad_type::push_pull,
            ecl::gpio_hw::strength::i2ma
        );


    ecl::gpio_hw::apply_cfg<ecl::gpio_hw::port::a, ecl::gpio_hw::num::pin1>(
            ecl::gpio_hw::dir::hw,
            ecl::gpio_hw::pad_type::push_pull,
            ecl::gpio_hw::strength::i2ma
        );

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
}
