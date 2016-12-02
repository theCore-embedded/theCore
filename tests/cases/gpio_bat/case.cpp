#include <unity.h>
#include <unity_fixture.h>

#include <ecl/utils.hpp>

#include <platform/execution.h>

#include "test_gpio.hpp"

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
        ecl_spin_wait(1000);

        Gpio::reset();
        ecl_spin_wait(1000);
    }

    template<typename Gpio>
    static void toggle()
    {
        Gpio::toggle();
        ecl_spin_wait(1000);
        TEST_ASSERT_TRUE(Gpio::get());

        Gpio::toggle();
        ecl_spin_wait(1000);
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
