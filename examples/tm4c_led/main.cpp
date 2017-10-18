/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "target.hpp"

#include <ecl/iostream.hpp>
#include <common/execution.hpp>

extern "C"
void board_init()
{
    gpio_init_generated();
}

int main()
{
    ecl::cout << "Starting TM4C LED demo..." << ecl::endl;

    while (1) {
        ecl::led_red::toggle();
        ecl::wait_for(100);
        ecl::led_green::toggle();
        ecl::wait_for(100);
        ecl::led_blue::toggle();
        ecl::wait_for(100);
    }
}
