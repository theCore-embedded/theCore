#include <unity.h>
#include <unity_fixture.h>

#include "test_uart.hpp"

#include <atomic>

static bool uart_inited;
static volatile std::atomic_bool transfer_complete;
static volatile std::atomic_bool tx_complete;
static size_t expected_total;

static void test_handler(ecl::bus_channel ch, ecl::bus_event type, size_t total) {
    if (!tx_complete) {
        TEST_ASSERT_TRUE(ch == ecl::bus_channel::tx);
        TEST_ASSERT_TRUE(type == ecl::bus_event::tc); // TODO: handle ht as well
        TEST_ASSERT_EQUAL_UINT(expected_total, total);
        tx_complete = true;
    } else if (!transfer_complete) {
        TEST_ASSERT_TRUE(ch == ecl::bus_channel::meta);
        TEST_ASSERT_TRUE(type == ecl::bus_event::tc);
        transfer_complete = true;
    } else {
        TEST_FAIL_MESSAGE("Spurious event detected");
    }
}

static void wait_transfer_complete()
{
    while (!transfer_complete) { };
}

TEST_GROUP(uart_bat);

TEST_SETUP(uart_bat)
{
    if (!uart_inited) {
        auto rc = ecl::test_uart::init();
        TEST_ASSERT_TRUE(rc == ecl::err::ok);
    }

    uart_inited = true;
    tx_complete = false;
    transfer_complete = false;
    expected_total = 0;
}

TEST_TEAR_DOWN(uart_bat)
{
}

TEST(uart_bat, transmit_buf)
{
    const uint8_t buf[] = "This is TM4C UART BAT test line\r\n";
    expected_total = sizeof(buf);

    ecl::test_uart::set_tx(buf, sizeof(buf));
    ecl::test_uart::set_handler(test_handler);

    ecl::test_uart::do_xfer();
    wait_transfer_complete();

    // Repeat with same params
    transfer_complete = false;
    tx_complete = false;
    ecl::test_uart::do_xfer();
    wait_transfer_complete();
}

TEST(uart_bat, transmit_buf_fill_mode)
{
    uint8_t byte = 'C';
    expected_total = 32; // Try to transfer 32 bytes

    ecl::test_uart::set_tx(expected_total, byte);
    ecl::test_uart::set_handler(test_handler);

    ecl::test_uart::do_xfer();
    wait_transfer_complete();

    // Repeat with another byte
    byte = 'E';
    transfer_complete = false;
    tx_complete = false;
    expected_total = 64;

    ecl::test_uart::set_tx(expected_total, byte);
    ecl::test_uart::do_xfer();
    wait_transfer_complete();
}
