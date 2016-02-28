#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include "dev/bus.hpp"
#include "mocks/platform_bus.hpp"

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
    ecl::err ret = ecl::err::tobig;
    mock("platform_bus")
            .expectOneCall("init")
            .andReturnValue(static_cast< int >(ret));

    test_bus->init();

    mock().checkExpectations();
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
