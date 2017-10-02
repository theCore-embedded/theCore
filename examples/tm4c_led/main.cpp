/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "target.hpp"

#include <ecl/iostream.hpp>
#include <common/execution.hpp>

using ecl::cout;
using ecl::endl;

extern "C"
void board_init()
{
    gpio_init();
}

int main()
{
    ecl::cout << "Starting TM4C led demo..." << ecl::endl;

    while (1) {
        led_red::toggle();
        ecl::wait_for(100);
        led_green::toggle();
        ecl::wait_for(100);
        led_blue::toggle();
        ecl::wait_for(100);
    }
}
