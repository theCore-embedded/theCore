#include <dev/sensor/fc28.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <iostream>

//------------------------------------------------------------------------------
// Mocks

// Mocking power pin for FC-28
struct mock_power_pin
{
    static void set()
    {
        mock("power_pin").actualCall("set");
    }

    static void reset()
    {
        mock("power_pin").actualCall("reset");
    }
};

//------------------------------------------------------------------------------
// Tests

using test_fc28 = ecl::sensor::fc28<mock_power_pin>;

TEST_GROUP(fc28)
{
    void setup()
    {

    }

    void teardown()
    {
        mock().clear();
    }
};

static void test_get_sample_with_delta(auto expected, auto delta, auto sample, auto resolution)
{
    using namespace ecl::sensor::fc28_defs;
    moisture_type moisture;

    CHECK_TRUE(ecl::err::ok == test_fc28::get_moisture(sample, resolution, moisture));

    std::cout << "Expected: " << expected << " actual: " << moisture
              << " delta: " << delta << std::endl;

    CHECK_TRUE(moisture <= expected + delta && moisture >= expected - delta);
}

TEST(fc28, power_up_down)
{
    mock("power_pin").expectOneCall("set");

    test_fc28::power_on();

    mock().checkExpectations();

    mock("power_pin").expectOneCall("reset");

    test_fc28::power_off();

    mock().checkExpectations();
}

TEST(fc28, get_moisture_min_max)
{
    using namespace ecl::sensor::fc28_defs;

    moisture_type moisture;

    // 10 bits resolution
    test_get_sample_with_delta(min_moisture, 0, 1023, 10);
    test_get_sample_with_delta(max_moisture, 0, 0, 10);

    // 8 bits resolution
    test_get_sample_with_delta(min_moisture, 1, 255, 8);
    test_get_sample_with_delta(max_moisture, 1, 0, 8);

    // 12 bit resolution
    test_get_sample_with_delta(min_moisture, 0, 4095, 12);
    test_get_sample_with_delta(max_moisture, 0, 0, 12);
}

TEST(fc28, get_moisture_in_between_10bits)
{
    using namespace ecl::sensor::fc28_defs;

    moisture_type moisture;

    // Maximum 10-bit ADC value that can be obtained from sensor is 0x3ff,
    // minimum value - 0xff.
    auto sensor_10bit_min = 0xff;
    auto sensor_10bit_max = 0x3ff;
    auto sensor_10bit_range = sensor_10bit_max - sensor_10bit_min;
    auto sensor_10bit_half_range = sensor_10bit_range / 2;

    test_get_sample_with_delta(max_moisture / 2, 1, sensor_10bit_min + sensor_10bit_half_range, 10);
}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
