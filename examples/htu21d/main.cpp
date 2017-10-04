/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief HTU21D temperature and humidity sensor usage example.
//! \details Example shows how to use HTU21D sensor to get temperature
//! and humidity data.

#include "target.hpp"

#include <ecl/iostream.hpp>
#include <ecl/thread/utils.hpp>

extern "C"
void board_init()
{
    gpio_init_generated();
}

struct measurement {
    explicit measurement(int x) : x(x) { }

    int x;
};

template<typename IO_device>
ecl::ostream<IO_device> &operator<<(ecl::ostream<IO_device> &o, measurement m)
{
    return o << (m.x / 1000) << "."
             << (m.x % 1000 / 100)
             << (m.x % 100 / 10)
             << (m.x % 10);
}

int main()
{
    ecl::cout << "Starting HTU21D sensor..." << ecl::endl;

    // Initialize HTU21D sensor
    ecl::htu21d::init();
    ecl::htu21d::soft_reset();

    ecl::cout << "Reset done" << ecl::endl;

    while (true) {
        int temperature = 0;
        ecl::htu21d::get_temperature(temperature);

        ecl::cout << "Temperature: " << measurement(temperature) << " Celsius   ";

        int humidity = 0;
        ecl::htu21d::get_humidity(humidity);

        ecl::cout << "Humidity: " << measurement(humidity) << "%" << ecl::endl;

        // Sleep for a second.
        ecl::this_thread::sleep_for(1000);
    }

    return 0;
}
