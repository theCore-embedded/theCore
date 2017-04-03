#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

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
        .expectOneCall("do_rx")
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
    mock("platform_bus")
        .expectOneCall("do_rx")
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
    mock().checkExpectations();
}

TEST(serial, try_start_xfer)
{
    // Fill internal rx buffer and verify that xfer starts in the bus_handler
    for (size_t i = 0; i < serial_t::buffer_size-1; i++) {
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    mock("platform_bus")
        .expectOneCall("do_rx")
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
    auto actual_ret = serial_t::send_buf(send_buf, send_size);
    CHECK_EQUAL(expected_ret, actual_ret);
    CHECK_EQUAL(serial_t::buffer_size, send_size);
    mock().checkExpectations();
}

TEST(serial, send_buff_larger)
{
    // We send buffer larger than serial_t::buffer_size
    // send_buf() should copy at most serial_t::buffer_size bytes and
    // return err:again.
    // Next, we send rest of buffer
    // while consuming bytes with platform_mock in another thread
    uint8_t buf[serial_t::buffer_size+32];
    size_t buf_size = sizeof(buf);

    for (size_t i = 0; i < buf_size; i++) {
        buf[i] = static_cast<uint8_t>(i);
    }

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
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
    ret = serial_t::send_buf(buf + buf_size, new_size);
    CHECK_EQUAL(ecl::err::ok, ret);

    t.join();
}

TEST(serial, send_by_parts)
{
    uint8_t buf[serial_t::buffer_size/32];
    size_t buf_size = sizeof(buf);
    auto expected_ret = ecl::err::ok;

    // Initialize buffer to some distinct values.
    for (size_t i = 0; i < buf_size; i++) {
        buf[i] = static_cast<uint8_t>(i);
    }

    // Fill the buffer, but leafe some space in order to prevent start of tx xfer.
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
    uint8_t buffer[serial_t::buffer_size];
    size_t buffer_size = sizeof(buffer);

    mock("platform_bus")
        .expectOneCall("do_rx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    for (size_t i = 0; i < buffer_size; i++) {
        platform_mock::m_rx[i] = static_cast<uint8_t>(i);
        platform_mock::invoke(ecl::bus_channel::rx, ecl::bus_event::tc, i+1);
    }

    auto ret = serial_t::recv_buf(buffer, buffer_size);
    CHECK_EQUAL(ecl::err::ok, ret);

    mock("platform_bus")
        .expectOneCall("do_tx")
        .andReturnValue(static_cast<int>(ecl::err::ok));
    ret = serial_t::send_buf(buffer, buffer_size);
    CHECK_EQUAL(ecl::err::ok, ret);
    CHECK_EQUAL(buffer_size, sizeof(buffer));
    for (size_t i = 0; i < buffer_size; i++) {
        CHECK_EQUAL(static_cast<uint8_t>(i), buffer[i]);
    }

    mock().checkExpectations();
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
