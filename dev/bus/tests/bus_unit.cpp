/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dev/bus.hpp"
#include "mocks/platform_bus.hpp"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

// Our resident
using bus_t = ecl::generic_bus<platform_mock>;

TEST_GROUP(bus)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().disable();
        bus_t::deinit();
        mock().enable();

        mock().clear();
    }
};

TEST(bus, init)
{
    ecl::err expected_ret = ecl::err::tobig;
    mock("platform_bus")
            .expectOneCall("init")
            .andReturnValue(static_cast< int >(expected_ret));
    mock("platform_bus").expectOneCall("set_handler");

    auto ret = bus_t::init();

    // Retval must be the same as produced by platform counterpart
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();
}

TEST(bus, lock_unlock)
{
    // TODO: split this test into few with double-lock and double-unlock cases.
    // This will require mocking of ecl_assert and placing 'throw' there.
    // When assertion hits an exception will be propagated and we can
    // detect it here

    // TODO: async mode not yet covered

    mock().disable();
    bus_t::init();
    mock().enable();

    mock("mutex").expectOneCall("lock");
    mock("platform_bus").expectOneCall("reset_buffers");
    mock("mutex").expectOneCall("unlock");

    bus_t::lock();
    bus_t::unlock();

    mock().checkExpectations();
}

TEST(bus, continuation_mode_xfer)
{
    mock().disable();
    bus_t::init();
    bus_t::lock();
    mock().enable();

    auto   expected_event    = ecl::bus_event::ht;
    size_t expected_total    = 100500;
    auto   expected_channel  = ecl::bus_channel::tx;
    bool   call_xfer         = true;

    auto handler = [&](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        mock("handler").actualCall("call");
        CHECK_TRUE(expected_event    == e);
        CHECK_TRUE(expected_total    == total);
        CHECK_TRUE(expected_channel  == ch);

        if (call_xfer && ch == ecl::bus_channel::meta && e == ecl::bus_event::tc) {
            bus_t::trigger_xfer();
            // Test logic is to trigger only one consequent xfer.
            call_xfer = false;
        }
    };

    ecl::err expected_ret = ecl::err::ok;

    auto ret = bus_t::xfer(handler, ecl::async_type::deferred);
    CHECK_EQUAL(expected_ret, ret);

    // First xfer, triggered from current execution context
    mock("platform_bus")
    .expectOneCall("do_xfer")
    .andReturnValue(static_cast< int >(expected_ret));

    ret = bus_t::trigger_xfer();
    CHECK_EQUAL(expected_ret, ret);

    // Bus now busy transferring data
    ret = bus_t::xfer(handler);
    CHECK_EQUAL(ecl::err::busy, ret);

    // Consequent xfer will be started as soon as TC event will be delivered
    mock("platform_bus")
    .expectOneCall("do_xfer")
    .andReturnValue(static_cast< int >(expected_ret));

    // 1st event - tx, ht
    // 2nd event - meta, tc
    // 3rd event - tx, ht
    // 4th event - meta, tc - last for this test
    mock("handler").expectNCalls(4, "call");

    // Now, trigger the xfer event and see what happens
    platform_mock::invoke(expected_channel, expected_event, expected_total);

    expected_event    = ecl::bus_event::tc;
    expected_total    = 0;
    expected_channel  = ecl::bus_channel::meta;

    // Provide TC event.
    platform_mock::invoke(expected_channel, expected_event, expected_total);

    // Request for unlock. Bus must remain locked, since continuation xfer
    // is requested.

    mock().disable();
    bus_t::unlock();
    mock().enable();

    // Trigger new event as a result of xfer continuation.

    expected_event    = ecl::bus_event::ht;
    expected_total    = 100500;
    expected_channel  = ecl::bus_channel::tx;

    platform_mock::invoke(expected_channel, expected_event, expected_total);

    // TC event not yet shipped for continuation mode. Bus must remain in busy
    // state, but unlocked state. Deliver TC event in order to complete xfer
    // chains.

    expected_event    = ecl::bus_event::tc;
    expected_total    = 0;
    expected_channel  = ecl::bus_channel::meta;

    // As a result of cleanup
    mock("platform_bus").expectOneCall("reset_buffers");
    platform_mock::invoke(expected_channel, expected_event, expected_total);

    mock().checkExpectations();
}

// -----------------------------------------------------------------------------

TEST_GROUP(bus_is_ready)
{
    static constexpr size_t buf_size = 16;

    uint8_t tx_buf[buf_size];
    uint8_t rx_buf[buf_size];

    void setup()
    {
        mock().disable();
        bus_t::init();
        bus_t::lock();
        mock().enable();
    }

    void teardown()
    {
        mock().disable();
        bus_t::unlock();
        bus_t::deinit();
        mock().enable();

        mock().clear();
    }
};

TEST(bus_is_ready, set_buffers_invalid)
{
    auto rc = bus_t::set_buffers(nullptr, nullptr, 1);
    CHECK_EQUAL(ecl::err::inval, rc);
    mock().checkExpectations();
}


TEST(bus_is_ready, set_buffers)
{
    size_t tx_size = buf_size / 2;
    size_t rx_size = buf_size / 3;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus")
            .expectOneCall("set_tx")
            .withConstPointerParameter("tx_buf", tx_buf)
            .withParameter("size", tx_size);
    mock("platform_bus")
            .expectOneCall("set_rx")
            .withParameter("rx_buf", rx_buf)
            .withParameter("size", rx_size);

    auto rc = bus_t::set_buffers(tx_buf, rx_buf, tx_size, rx_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}


TEST(bus_is_ready, set_buffers_non_equal_size)
{
    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus")
            .expectOneCall("set_tx")
            .withConstPointerParameter("tx_buf", tx_buf)
            .withParameter("size", buf_size);
    mock("platform_bus")
            .expectOneCall("set_rx")
            .withParameter("rx_buf", rx_buf)
            .withParameter("size", buf_size);

    auto rc = bus_t::set_buffers(tx_buf, rx_buf, buf_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(bus_is_ready, size_is_zero)
{
    constexpr size_t zero_size = 0;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus")
            .expectOneCall("set_tx")
            .withConstPointerParameter("tx_buf", tx_buf)
            .withParameter("size", zero_size);
    mock("platform_bus")
            .expectOneCall("set_rx")
            .withParameter("rx_buf", rx_buf)
            .withParameter("size", zero_size);

    auto rc = bus_t::set_buffers(tx_buf, rx_buf, zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();

}

TEST(bus_is_ready, consequent_calls_and_buffers_reset)
{
    constexpr size_t zero_size = 0;
    constexpr size_t non_zero_size = 8;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus").ignoreOtherCalls();

    auto rc = bus_t::set_buffers(tx_buf, rx_buf, zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();

    // When doing a second call, reset must be called again

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus").ignoreOtherCalls();

    rc = bus_t::set_buffers(tx_buf, rx_buf, non_zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(bus_is_ready, fill_tx)
{
    constexpr size_t fill_size = 32;
    constexpr uint8_t fill_byte = 0xae;
    uint8_t *rx_expected = nullptr;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus")
            .expectOneCall("set_tx")
            .withParameter("tx_size", fill_size)
            .withParameter("fill_byte", fill_byte);
    mock("platform_bus")
            .expectOneCall("set_rx")
            .withParameter("rx_buf", rx_expected)
            .withParameter("size", 0);

    auto rc = bus_t::set_buffers(fill_size, fill_byte);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(bus_is_ready, xfer_error)
{
    ecl::err expected_ret = ecl::err::nomem;
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast< int >(expected_ret));

    auto ret = bus_t::xfer();

    // Retval must be the same as produced by platform counterpart
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();
}

TEST(bus_is_ready, async_xfer_error)
{
    // Call to this handler in case of error is a buggy behaviour.
    auto handler = [](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        (void) e;
        (void) ch;
        (void) total;
        mock("handler").actualCall("call");
    };

    ecl::err expected_ret = ecl::err::nomem;
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast< int >(expected_ret));

    auto ret = bus_t::xfer(handler);

    // Retval must be the same as produced by platform counterpart
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();
}

TEST(bus_is_ready, async_xfer_valid)
{
    auto   expected_event    = ecl::bus_event::ht;
    size_t expected_total    = 100500;
    auto   expected_channel  = ecl::bus_channel::tx;

    auto handler = [&](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        CHECK_TRUE(expected_event    == e);
        CHECK_TRUE(expected_total    == total);
        CHECK_TRUE(expected_channel  == ch);
        mock("handler").actualCall("call");
    };

    ecl::err expected_ret = ecl::err::ok;
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast< int >(expected_ret));

    auto ret = bus_t::xfer(handler);

    // Retval must be the same as produced by platform counterpart
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();

    // Now, trigger the event and see what happens

    mock("handler").expectNCalls(2, "call");

    platform_mock::invoke(expected_channel, expected_event, expected_total);

    expected_event    = ecl::bus_event::tc;
    expected_total    = 0;
    expected_channel  = ecl::bus_channel::meta;

    // Small cleanup. TC event is required.
    platform_mock::invoke(expected_channel, expected_event, expected_total);

    mock().checkExpectations();
}

TEST(bus_is_ready, async_deffered)
{
    auto   expected_event    = ecl::bus_event::ht;
    size_t expected_total    = 100500;
    auto   expected_channel  = ecl::bus_channel::tx;

    auto handler = [&](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        CHECK_TRUE(expected_event    == e);
        CHECK_TRUE(expected_total    == total);
        CHECK_TRUE(expected_channel  == ch);
        mock("handler").actualCall("call");
    };

    ecl::err expected_ret = ecl::err::ok;

    auto ret = bus_t::xfer(handler, ecl::async_type::deferred);
    CHECK_EQUAL(expected_ret, ret);

    // Platform bus xfer shouldn't be called.
    mock().checkExpectations();

    // Fire a xfer
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast< int >(expected_ret));

    ret = bus_t::trigger_xfer();
    CHECK_EQUAL(expected_ret, ret);

    // Bus now busy transferring data
    ret = bus_t::xfer(handler);
    CHECK_EQUAL(ecl::err::busy, ret);

    // Now, trigger the xfer event and see what happens

    mock("handler").expectNCalls(2, "call");

    platform_mock::invoke(expected_channel, expected_event, expected_total);

    expected_event    = ecl::bus_event::tc;
    expected_total    = 0;
    expected_channel  = ecl::bus_channel::meta;

    // Small cleanup. TC event is required.
    platform_mock::invoke(expected_channel, expected_event, expected_total);

    mock().checkExpectations();
}

TEST(bus_is_ready, cancel_async_xfer)
{
    ecl::err expected_ret;

    // Should not be called
    auto handler = [&](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        (void)ch;
        (void)e;
        (void)total;
        mock("handler").actualCall("call");
    };

    expected_ret = ecl::err::ok;

    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast<int>(expected_ret));

    auto ret = bus_t::xfer(handler);
    CHECK_EQUAL(expected_ret, ret);

    // Platform bus xfer shouldn't be called.
    mock().checkExpectations();

    // Cancel xfer. Return some unusual error code.
    expected_ret = ecl::err::acces;

    mock("platform_bus")
            .expectOneCall("cancel_xfer")
            .andReturnValue(static_cast<int>(expected_ret));

    ret = bus_t::cancel_xfer();
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();
}

TEST(bus_is_ready, cancel_deferred_xfer)
{
    ecl::err expected_ret;

    // Should not be called
    auto handler = [&](ecl::bus_channel ch, ecl::bus_event e, size_t total) {
        (void)ch;
        (void)e;
        (void)total;
        mock("handler").actualCall("call");
    };

    expected_ret = ecl::err::ok;

    auto ret = bus_t::xfer(handler, ecl::async_type::deferred);
    CHECK_EQUAL(ecl::err::ok, ret);

    // Platform bus xfer shouldn't be called.
    mock().checkExpectations();

    // Cancel xfer. Return some unusual error code.
    expected_ret = ecl::err::acces;

    mock("platform_bus")
            .expectOneCall("cancel_xfer")
            .andReturnValue(static_cast<int>(expected_ret));

    ret = bus_t::cancel_xfer();
    CHECK_EQUAL(expected_ret, ret);

    mock().checkExpectations();
}

TEST(bus_is_ready, xfer_timeout)
{
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast<int>(ecl::err::ok));

    mock("platform_bus")
            .expectOneCall("cancel_xfer")
            .andReturnValue(static_cast<int>(ecl::err::ok));

    auto ret = bus_t::xfer(nullptr, nullptr, std::chrono::milliseconds(100));

    // Retval must indicate timeout condition
    CHECK_EQUAL(ecl::err::timedout, ret);

    mock().checkExpectations();
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
