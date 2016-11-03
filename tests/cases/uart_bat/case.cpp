#include <unity.h>
#include <unity_fixture.h>

#include "test_uart.hpp"

#include <atomic>

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

TEST(uart_bat, transmit_buf)
{
    volatile std::atomic_bool finished;
    finished = false;

    auto test_handler = [&finished](ecl::bus_channel ch, ecl::bus_event type, size_t total) {
        (void) ch;
        (void) type;
        (void) total;

        if (ch == ecl::bus_channel::meta && type == ecl::bus_event::tc) {
            finished = true;
        }
    };

    const uint8_t buf[] = "This is TM4C UART BAT test first line\r\n";

    ecl::test_uart::set_tx(buf, sizeof(buf));
    ecl::test_uart::set_handler(test_handler);

    ecl::test_uart::do_xfer();

    while (!finished) { };
}

