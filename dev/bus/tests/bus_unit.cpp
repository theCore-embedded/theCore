#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include "dev/bus.hpp"
#include "mocks/platform_bus.hpp"

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

TEST_GROUP(bus)
{
    ecl::generic_bus< platform_mock > *test_bus;

    void setup()
    {
        test_bus = new ecl::generic_bus< platform_mock >;
    }

    void teardown()
    {
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

    ecl::generic_bus< platform_mock > *test_bus;

    void setup()
    {
        test_bus = new ecl::generic_bus< platform_mock >;

        mock().disable();
        test_bus->lock();
        mock().enable();
    }

    void teardown()
    {
        mock().disable();
        test_bus->unlock();
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

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
