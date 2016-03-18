#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include "dev/bus.hpp"
#include "mocks/platform_bus.hpp"

// Our resident
using bus_t = ecl::generic_bus< platform_mock >;

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

TEST_GROUP(bus)
{
    bus_t *test_bus;

    void setup()
    {
        test_bus = new bus_t;
    }

    void teardown()
    {
        mock().disable();
        test_bus->deinit();
        mock().enable();

        delete test_bus;
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

    auto ret = test_bus->init();

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
    test_bus->init();
    mock().enable();

    mock("mutex").expectOneCall("lock");
    mock("platform_bus").expectOneCall("reset_buffers");
    mock("mutex").expectOneCall("unlock");

    test_bus->lock();
    test_bus->unlock();

    mock().checkExpectations();
}

// -----------------------------------------------------------------------------

TEST_GROUP(bus_is_ready)
{
    static constexpr size_t buf_size = 16;

    uint8_t tx_buf[buf_size];
    uint8_t rx_buf[buf_size];

    bus_t *test_bus;

    void setup()
    {
        test_bus = new bus_t;

        mock().disable();
        test_bus->init();
        test_bus->lock();
        mock().enable();
    }

    void teardown()
    {
        mock().disable();
        test_bus->unlock();
        test_bus->deinit();
        mock().enable();

        delete test_bus;
        mock().clear();
    }
};

TEST(bus_is_ready, set_buffers_invalid)
{
    auto rc = test_bus->set_buffers(nullptr, nullptr, 1);
    CHECK_EQUAL(ecl::err::inval, rc);
    mock().checkExpectations();
}


TEST(bus_is_ready, set_buffers)
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

    auto rc = test_bus->set_buffers(tx_buf, rx_buf, buf_size);
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

    auto rc = test_bus->set_buffers(tx_buf, rx_buf, zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();

}

TEST(bus_is_ready, consequent_calls_and_buffers_reset)
{
    constexpr size_t zero_size = 0;
    constexpr size_t non_zero_size = 8;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus").ignoreOtherCalls();

    auto rc = test_bus->set_buffers(tx_buf, rx_buf, zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();

    // When doing a second call, reset must be called again

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus").ignoreOtherCalls();

    rc = test_bus->set_buffers(tx_buf, rx_buf, non_zero_size);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(bus_is_ready, fill_tx)
{
    constexpr size_t fill_size = 32;
    constexpr uint8_t fill_byte = 0xae;

    mock("platform_bus").expectOneCall("reset_buffers");
    mock("platform_bus")
            .expectOneCall("set_tx")
            .withParameter("rx_size", fill_size)
            .withParameter("fill_byte", fill_byte);

    auto rc = test_bus->set_buffers(fill_size, fill_byte);
    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(bus_is_ready, xfer_error)
{
    ecl::err expected_ret = ecl::err::nomem;
    mock("platform_bus")
            .expectOneCall("do_xfer")
            .andReturnValue(static_cast< int >(expected_ret));

    auto ret = test_bus->xfer();

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

    auto ret = test_bus->xfer(handler);

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

    auto ret = test_bus->xfer(handler);

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



int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
