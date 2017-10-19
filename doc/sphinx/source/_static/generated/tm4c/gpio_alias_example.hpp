/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C GPIO configuration template

#ifndef TM4C_PLATFORM_GPIO_CFG_
#define TM4C_PLATFORM_GPIO_CFG_

#include <platform/gpio_device.hpp>


namespace ecl
{

// GPIO drivers ----------------------------------------------------------------

/* Red LED */
using led_red = gpio<gpio_hw::port::f, gpio_hw::num::pin1>;
/* Blue LED */
using led_blue = gpio<gpio_hw::port::f, gpio_hw::num::pin2>;
/* Green LED */
using led_green = gpio<gpio_hw::port::f, gpio_hw::num::pin3>;

} // namespace ecl

// \brief Initializes GPIO using auto-generated code.
extern "C" void gpio_init_generated();

#endif // TM4C_PLATFORM_GPIO_CFG_
