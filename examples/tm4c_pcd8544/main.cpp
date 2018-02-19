/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "target.hpp"

#include <ecl/iostream.hpp>
#include <common/execution.hpp>
#include <dev/pcd8544.hpp>
#include <dev/bus.hpp>

#include <math.h>

extern "C"
void board_init()
{
    gpio_init_generated();
}

using bus = ecl::generic_bus<ecl::spi_3_driver>;
using pcd = ecl::pcd8544<bus, ecl::pcd_ss, ecl::pcd_mode, ecl::pcd_rst>;

int main()
{
    ecl::cout << "<<<<<<<<<<\n";
    pcd display;
    display.init();
    ecl::cout << "init\n";
    display.open();
    ecl::cout << "open\n";

    int set = 1;

    for (;;) {
        for (int i = 0; i < 84; ++i) {
            float f = 10 * sinf(i / 3.0) + 20;
            ecl::cout << (int)f << ecl::endl;
            ecl::point p{i, (int)f};

            if (set) {
                display.set_point(p);
            } else {
                display.clear_point(p);
            }

            display.flush();
            ecl::this_thread::sleep_for(100);
        }
        set ^= 1;
    }

    return -1;
}
