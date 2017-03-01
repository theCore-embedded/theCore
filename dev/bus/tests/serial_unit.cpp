#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <iostream>
#include "dev/serial.hpp"
#include "mocks/platform_bus.hpp"
#include "ecl/thread/semaphore.hpp"

#include <thread>

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

using serial_t = ecl::serial<platform_mock>;

TEST_GROUP(serial)
{
    void setup()
    {
        mock().disable();
        serial_t::init();
        CHECK_EQUAL(serial_t::buffer_size, platform_mock::m_rx_size);
        // TODO: add check for tx when send_* functions
        // will be implemented for serial class.
        mock().enable();
    }

    void teardown()
    {
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
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, 1); // Unblock
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        CHECK_EQUAL(true, recv_returned);
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
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    for (size_t i = 0; i < serial_t::buffer_size; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    auto expected_ret = ecl::err::ok;
    uint8_t buf[32];
    size_t buf_size = sizeof(buf);

    for (size_t i = 0; i < serial_t::buffer_size/buf_size; i++) {
        auto actual_ret = serial_t::recv_buf(buf, buf_size);
        CHECK_EQUAL(expected_ret, actual_ret);
        CHECK_EQUAL(sizeof(buf), buf_size);
        for (size_t j = 0; j < buf_size; j++) {
            // Verify that the retrieved data is correct
            CHECK_EQUAL(static_cast<uint8_t>(i*buf_size + j), buf[j]);
        }
    }

    mock().checkExpectations();
}

TEST(serial, multiple_recv_block_and_unblock)
{
    // Fill internal rx buffer and read all bytes in multiple calls
    mock("platform_bus")
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    for (size_t i = 0; i < serial_t::buffer_size; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    auto expected_ret = ecl::err::ok;
    uint8_t buf[32];
    size_t buf_size = sizeof(buf);

    for (size_t i = 0; i < serial_t::buffer_size/buf_size; i++) {
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
        CHECK_EQUAL(false, recv_returned);
        platform_mock::m_rx[0] = expected_byte;
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        CHECK_EQUAL(true, recv_returned);
    });
    recv_status.signal();
    // This should block
    auto actual_ret = serial_t::recv_buf(buf, buf_size);
    recv_returned = true;
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(expected_byte, buf[0]);
    CHECK_EQUAL(1, buf_size);
    t.join();
    mock().checkExpectations();
}

TEST(serial, recv_bigger_than_available)
{
    mock("platform_bus")
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    for (size_t i = 0; i < serial_t::buffer_size; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    // Ask for more data than it is present in buffer
    uint8_t buf[serial_t::buffer_size+42] = {};
    size_t buf_size = sizeof(buf);
    auto expected_ret = ecl::err::ok;
    auto actual_ret = serial_t::recv_buf(buf, buf_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, buf_size);
    for (size_t i = 0; i < buf_size; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i), buf[i]);
    }
}

TEST(serial, try_start_xfer)
{
    // Fill internal rx buffer and verify that xfer starts in the bus_handler
    for (size_t i = 0; i < serial_t::buffer_size-1; i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    mock("platform_bus")
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, serial_t::buffer_size);
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
        .expectOneCall("cancel_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    mock("platform_bus")
        .expectOneCall("set_buffers")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    mock("platform_bus")
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    auto actual_ret = serial_t::send_byte(byte);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(byte, platform_mock::m_tx[0]);
    mock().checkExpectations();
}

TEST(serial, recv_buf_send_buf)
{
    // Send and receive buffer at the same time
    uint8_t recv_buf[serial_t::buffer_size];
    size_t recv_size = sizeof(recv_buf);
    uint8_t send_buf[serial_t::buffer_size];
    size_t send_size = sizeof(send_buf);
    auto expected_ret = ecl::err::ok;

    for (size_t i = 0; i < serial_t::buffer_size; i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    auto actual_ret = serial_t::recv_buf(recv_buf, recv_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, recv_size);

    mock("platform_bus")
        .expectOneCall("cancel_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    mock("platform_bus")
        .expectOneCall("set_buffers")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    mock("platform_bus")
        .expectOneCall("do_xfer")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    actual_ret = serial_t::send_buf(send_buf, send_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, send_size);
    mock().checkExpectations();
}

TEST(serial, send_buff_larger)
{
    uint8_t buf[serial_t::buffer_size+1];
    size_t buf_size = sizeof(buf);
    auto expected_ret = ecl::err::ok;
    auto actual_ret = serial_t::send_buf(buf, buf_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, buf_size);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
