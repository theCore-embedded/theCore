#include <iostream>
#include "dev/serial.hpp"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include "mocks/platform_bus.hpp"
#include "ecl/thread/semaphore.hpp"

#include <thread>
#include <numeric>

using serial_t = ecl::serial<platform_mock>;

TEST_GROUP(serial)
{
    void setup()
    {
        mock().disable();
        serial_t::init();
        CHECK_EQUAL(serial_t::buffer_size / 2, platform_mock::m_rx_size);
        // TODO: add check for tx when send_* functions
        // will be implemented for serial class.
        mock().enable();
    }

    void teardown()
    {
        platform_mock::m_ignore_buffer_setters = false;
        mock().disable();
        serial_t::deinit();
        mock().enable();
        mock().clear();
    }

};

TEST(serial, recv_byte_block_unblock)
{
    // Internal buffer is empty so the recv_byte() should block
    // until the platform invokes bus_handler
    ecl::binary_semaphore recv_status;
    std::atomic_bool recv_returned;
    uint8_t expected_byte = 0x42;
    std::thread t([&] {
        recv_status.wait(); //Wait for the recv_byte() call
        CHECK_EQUAL(false, recv_returned);
        platform_mock::m_rx[0] = expected_byte;

        // Give some time for recv_byte to block waiting for the data
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, 1); // Unblock
    });
    auto expected = ecl::err::ok;
    uint8_t byte;
    recv_status.signal();
    auto actual = serial_t::recv_byte(byte);
    recv_returned = true;
    t.join();
    CHECK_EQUAL(expected, actual);
    CHECK_EQUAL(expected_byte, byte);
}

TEST(serial, multiple_recv_no_block)
{
    // Fill internal rx buffer and read less bytes in multiple calls

    mock("platform_bus")
        .expectOneCall("do_rx")
        .andReturnValue(static_cast<int>(ecl::err::ok));

    // Ignore set_rx.
    mock("platform_bus").ignoreOtherCalls();

    // Exploiting the fact that serial operates over double buffer and will
    // not switch to the next buffer until it will be fully filled.
    uint8_t rx_buf[serial_t::buffer_size / 2];

    // Prepare buffer.
    for (size_t i = 0; i < sizeof(rx_buf); i++) {
        rx_buf[i] = static_cast<uint8_t>(i);
    }

    // Safely copy buffer
    platform_mock::copy_to_rx(rx_buf, sizeof(rx_buf));

    for (size_t i = 0; i < sizeof(rx_buf); i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i + 1);
    }

    auto expected_ret = ecl::err::ok;
    uint8_t buf[32];
    size_t total_actual_read = 0;
    size_t total_counted_read = 0;
    //size_t remain_read = sizeof(rx_buf);
    size_t last = 0;

    while (total_counted_read < sizeof(rx_buf)) {
        size_t ret_sz = sizeof(buf);
        auto actual_ret = serial_t::recv_buf(buf, ret_sz);
        CHECK_EQUAL(expected_ret, actual_ret);
        CHECK(ret_sz <= sizeof(buf));

        for (size_t j = 0; j < ret_sz; j++) {
            // Verify that the retrieved data is correct
            CHECK_EQUAL(static_cast<uint8_t>(last + j), buf[j]);
        }

        last += ret_sz;

        total_counted_read += sizeof(buf);
        total_actual_read += ret_sz;
    }

    CHECK_EQUAL(sizeof(rx_buf), total_actual_read);


    mock().checkExpectations();
}

TEST(serial, multiple_recv_block_and_unblock)
{
    // Fill internal rx buffer and read all bytes in multiple calls
    mock("platform_bus")
        .expectOneCall("do_rx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    // Ignore set_rx.
    mock("platform_bus").ignoreOtherCalls();

    for (size_t i = 0; i < serial_t::buffer_size / 2; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i + 1);
    }

    auto expected_ret = ecl::err::ok;
    uint8_t buf[32];
    size_t buf_size = sizeof(buf);

    for (size_t i = 0; i < serial_t::buffer_size / 2 / buf_size; i++) {
        auto actual_ret = serial_t::recv_buf(buf, buf_size);
        CHECK_EQUAL(expected_ret, actual_ret);
        CHECK_EQUAL(sizeof(buf), buf_size);
        for (size_t j = 0; j < buf_size; j++) {
            // Verify that the retrieved data is correct
            CHECK_EQUAL(static_cast<uint8_t>(i*buf_size + j), buf[j]);
        }
    }

    // The next read will block since no data is present in the buffer
    ecl::binary_semaphore recv_status;
    std::atomic_bool recv_returned;
    const uint8_t expected_byte = 0x42;
    std::thread t([&] {
        recv_status.wait();
        bool ret = recv_returned;
        CHECK_EQUAL(false, ret);
        platform_mock::m_rx[0] = expected_byte;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, 1);
    });

    recv_status.signal();
    auto actual_ret = serial_t::recv_buf(buf, buf_size);
    recv_returned = true;
    t.join();
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(expected_byte, buf[0]);
    CHECK_EQUAL(1, buf_size);
    mock().checkExpectations();
}

TEST(serial, recv_bigger_than_available)
{
    for (size_t i = 0; i < serial_t::buffer_size / 3; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i + 1);
    }

    // Ask for more data than it is present in buffer
    uint8_t buf[serial_t::buffer_size] = {};
    size_t buf_size = sizeof(buf);

    auto actual_ret = serial_t::recv_buf(buf, buf_size);
    CHECK_EQUAL(ecl::err::ok, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size / 3, buf_size);

    for (size_t i = 0; i < buf_size / 3; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i), buf[i]);
    }

    mock().checkExpectations();
}

TEST(serial, try_start_xfer)
{
    // Fill internal rx buffer and verify that xfer starts in the bus_handler
    for (size_t i = 0; i < serial_t::buffer_size / 2 - 1; i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    // During xfer, due to double-buffering, new buffer will be used for xfer
    mock("platform_bus")
        .expectOneCall("do_rx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    // Ignore set_rx.
    mock("platform_bus").ignoreOtherCalls();

    platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, serial_t::buffer_size / 2);
    mock().checkExpectations();
}

TEST(serial, send_byte_basic)
{
    // This test implies that the internal tx buffer is empty
    // and no xfer is in progress currently
    // It also covers basic send_buf test.
    uint8_t byte = 0x42;
    auto expected_ret = ecl::err::ok;

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    // Ignore set_tx.
    mock("platform_bus").ignoreOtherCalls();

    auto actual_ret = serial_t::send_byte(byte);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(byte, platform_mock::m_tx[0]);
    mock().checkExpectations();
}

TEST(serial, send_full_buffer)
{
    // Send serial_t::buffer_size bytes and expect tx xfer to start.
    uint8_t send_buf[serial_t::buffer_size];
    size_t send_size = sizeof(send_buf);
    auto expected_ret = ecl::err::ok;

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    // Ignore set_tx.
    mock("platform_bus").ignoreOtherCalls();

    auto actual_ret = serial_t::send_buf(send_buf, send_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, send_size);
    mock().checkExpectations();
}

TEST(serial, send_buf_larger)
{
    // We send buffer larger than serial_t::buffer_size
    // send_buf() should copy at most serial_t::buffer_size bytes and
    // return err:again.
    // Next, we send rest of buffer
    // while consuming bytes with platform_mock in another thread
    uint8_t buf[serial_t::buffer_size + 32];
    size_t buf_size = sizeof(buf);

    for (size_t i = 0; i < buf_size; i++) {
        buf[i] = static_cast<uint8_t>(i);
    }

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    // Ignore set_tx.
    mock("platform_bus").ignoreOtherCalls();

    auto ret = serial_t::send_buf(buf, buf_size);
    CHECK_EQUAL(ecl::err::again, ret);
    CHECK_EQUAL(serial_t::buffer_size, buf_size);
    mock().checkExpectations();
    for (size_t i = 0; i < platform_mock::m_tx_size; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i), platform_mock::m_tx[i]);
    }

    // Signals when bytes are consumed by platform_mock
    ecl::binary_semaphore bytes_consumed;
    std::thread t([&bytes_consumed] {
        platform_mock::invoke(ecl::bus_channel::tx, ecl::bus_event::tc, serial_t::buffer_size);
        bytes_consumed.signal();
    });

    bytes_consumed.wait();
    size_t new_size = sizeof(buf) - buf_size;
    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    ret = serial_t::send_buf(buf + buf_size, new_size);
    CHECK_EQUAL(ecl::err::ok, ret);

    t.join();
}

// Test is ignored - logic was changed. Now every consequent xfer will block
// until previous one will be delivered..
IGNORE_TEST(serial, send_by_parts)
{
    uint8_t buf[serial_t::buffer_size/32];
    size_t buf_size = sizeof(buf);
    auto expected_ret = ecl::err::ok;

    // Initialize buffer to some distinct values.
    for (size_t i = 0; i < buf_size; i++) {
        buf[i] = static_cast<uint8_t>(i);
    }

    // Fill the buffer, but leave some space in order to prevent start of tx xfer.
    for (size_t i = 0; i < serial_t::buffer_size / buf_size - 1; i++) {
        auto actual_ret = serial_t::send_buf(buf, buf_size);
        CHECK_EQUAL(expected_ret, actual_ret);
    }

    // Check that all bytes have been placed at the correct positions.
    for (size_t i = 0; i < serial_t::buffer_size - buf_size; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i % buf_size), platform_mock::m_tx[i]);
    }

    // Send last chunk and expect tx xfer to be started.
    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    auto actual_ret = serial_t::send_buf(buf, buf_size);
    CHECK_EQUAL(actual_ret, expected_ret);
    CHECK_EQUAL(buf_size, sizeof(buf));

    mock().checkExpectations();
}

TEST(serial, send_recv_mixed)
{
    uint8_t buffer_rx[serial_t::buffer_size / 2];
    uint8_t buffer_tx[serial_t::buffer_size];
    size_t buffer_size_rx = sizeof(buffer_rx);
    size_t buffer_size_tx = sizeof(buffer_tx);

    // Ignore set_rx.
    mock("platform_bus").ignoreOtherCalls();

    mock("platform_bus")
        .expectOneCall("do_rx")
        .andReturnValue(static_cast<int>(ecl::err::ok));

    std::iota(buffer_rx, buffer_rx + buffer_size_rx, 0);
    std::iota(buffer_tx, buffer_tx + buffer_size_tx, 0);

    platform_mock::copy_to_rx(buffer_rx, buffer_size_rx);

    for (size_t i = 0; i < buffer_size_rx; i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    auto ret = serial_t::recv_buf(buffer_rx, buffer_size_rx);
    CHECK_EQUAL(ecl::err::ok, ret);

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));

    ret = serial_t::send_buf(buffer_tx, buffer_size_tx);
    CHECK_EQUAL(ecl::err::ok, ret);
    CHECK_EQUAL(sizeof(buffer_tx), buffer_size_tx);
    for (size_t i = 0; i < buffer_size_tx; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i), platform_mock::m_tx[i]);
    }

    mock().checkExpectations();
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
