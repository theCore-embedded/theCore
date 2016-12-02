#include <unity.h>
#include <unity_fixture.h>

#include "test_uart.hpp"

#include <atomic>

static bool uart_inited;
static volatile std::atomic_bool transfer_complete;
static volatile std::atomic_bool tx_complete;
static volatile std::atomic_bool rx_complete;
static size_t expected_rx;
static size_t expected_tx;

static void test_handler(ecl::bus_channel ch, ecl::bus_event type, size_t total)
{
    if (!tx_complete) {
        TEST_ASSERT_TRUE(ch == ecl::bus_channel::tx);
        TEST_ASSERT_TRUE(type == ecl::bus_event::tc); // TODO: handle ht as well
        TEST_ASSERT_EQUAL_UINT(expected_tx, total);
        tx_complete = true;
    } else if (!rx_complete) {
        TEST_ASSERT_TRUE(ch == ecl::bus_channel::rx);
        TEST_ASSERT_TRUE(type == ecl::bus_event::tc); // TODO: handle ht as well
        TEST_ASSERT_EQUAL_UINT(expected_rx, total);
        rx_complete = true;
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
    rx_complete = false;
    transfer_complete = false;
    expected_tx = 0;
    expected_rx = 0;

    ecl::test_uart::reset_buffers();
    ecl::test_uart::reset_handler();
}

TEST_TEAR_DOWN(uart_bat)
{
}

TEST(uart_bat, transmit_buf)
{
    const uint8_t buf[] = "This is UART BAT test line\r\n";
    expected_tx = sizeof(buf);
    rx_complete = true; // Doesn't care about RX

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
// Some of platform drivers use fill mode differently. To let tests pass,
// this mode must be disabled.
#ifndef UART_TEST_NO_FILL_MODE
    uint8_t byte = 'C';
    expected_tx = 32; // Try to transfer 32 bytes
    rx_complete = true; // Doesn't care about RX

    ecl::test_uart::set_tx(expected_tx, byte);
    ecl::test_uart::set_handler(test_handler);

    ecl::test_uart::do_xfer();
    wait_transfer_complete();

    // Repeat with another byte
    byte = 'E';
    transfer_complete = false;
    tx_complete = false;
    expected_tx = 64;

    ecl::test_uart::set_tx(expected_tx, byte);
    ecl::test_uart::do_xfer();
    wait_transfer_complete();
#endif
}

TEST(uart_bat, receive_buf)
{
// Some of platform drivers use RX feedback differently. To let tests pass,
// this mode must be disabled.
#ifndef UART_TEST_NO_RX_MODE
    tx_complete = true; // Doesn't case about TX
    constexpr size_t body = 10; // Well, tester must type 10 characters
    constexpr size_t offt = 10;
    constexpr size_t tail = 10;

    uint8_t buf[offt + body + tail] = {};

    expected_rx = body;
    const uint8_t expected_buf[] = "qwertyuiop";
    uint8_t *rx_buf = buf + offt; // To check underflow

    ecl::test_uart::set_rx(rx_buf, expected_rx);
    ecl::test_uart::set_handler(test_handler);

    ecl::test_uart::do_xfer();
    wait_transfer_complete();

    TEST_ASSERT_EQUAL_STRING_LEN(expected_buf, rx_buf, expected_rx);
    TEST_ASSERT_EQUAL_STRING_LEN("\0\0\0\0\0\0\0\0\0\0", buf, offt);
    TEST_ASSERT_EQUAL_STRING_LEN("\0\0\0\0\0\0\0\0\0\0", rx_buf + offt, tail);
#endif
}
