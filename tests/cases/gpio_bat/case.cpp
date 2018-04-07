/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/utils.hpp>

#include <common/execution.hpp>

#include "test_gpio.hpp"

// Headers are included in the last place to avoid some
// overrides of stdlib functions (like calloc/malloc/etc.)
#include <unity.h>
#include <unity_fixture.h>

//! Test executor.
//! \tparam Ts List of GPIOs to execute test.
template<typename ...Ts>
struct gpio_tests
{
    static void do_setup()
    {
        [](...){ }((setup<Ts>(), 0)...);

    }

    static void do_teardown()
    {
        [](...){ }((teardown<Ts>(), 0)...);
    }

    static void do_set_reset()
    {
        [](...){ }((set_reset<Ts>(), 0)...);
    }

    static void do_toggle()
    {
        [](...){ }((toggle<Ts>(), 0)...);
    }


private:
    template<typename Gpio>
    static void set_reset()
    {
        Gpio::set();
        ecl::wait_for(1000);

        Gpio::reset();
        ecl::wait_for(1000);
    }

    template<typename Gpio>
    static void toggle()
    {
        Gpio::toggle();
        ecl::wait_for(1000);
        TEST_ASSERT_TRUE(Gpio::get());

        Gpio::toggle();
        ecl::wait_for(1000);
    }

    template<typename Gpio>
    static void setup()
    {
        Gpio::reset();
    }

    template<typename Gpio>
    static void teardown()
    {
        Gpio::reset();
    }
};


TEST_GROUP(gpio_bat);

TEST_SETUP(gpio_bat)
{
    ecl::test_gpios::executor<gpio_tests>::do_setup();
}

TEST_TEAR_DOWN(gpio_bat)
{
    ecl::test_gpios::executor<gpio_tests>::do_teardown();
}

TEST(gpio_bat, set_reset)
{
    ecl::test_gpios::executor<gpio_tests>::do_set_reset();
}

TEST(gpio_bat, toggle)
{
    ecl::test_gpios::executor<gpio_tests>::do_toggle();
}
