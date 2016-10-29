#include <unity.h>
#include <unity_fixture.h>

#include "test_uart.hpp"

static bool uart_inited;

TEST_GROUP(uart_bat);

TEST_SETUP(uart_bat)
{
    if (!uart_inited) {
        auto rc = ecl::test_uart::init();
        TEST_ASSERT_TRUE(rc == ecl::err::ok);
    }

    uart_inited = true;
}

TEST_TEAR_DOWN(uart_bat)
{
}

TEST(uart_bat, set_tx)
{
}

