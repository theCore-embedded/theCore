#include <unity.h>
#include <unity_fixture.h>

#include <ecl/unity_helpers.hpp>
#include <platform/execution.h>

#include <test_adc.hpp>

TEST_GROUP(adc_bat);

TEST_SETUP(adc_bat) {
}

TEST_TEAR_DOWN(adc_bat) {
}

TEST(adc_bat, single_run)
{
    using sample_container = ecl::test_adc::sample_array<ecl::test_channel>;

    sample_container arr;

    ecl::test_adc::init();

    ecl::test_adc::enable_channels<ecl::test_channel>(arr);

    UnityPrintWithEOL("Performing 10 conversions");

    for (int i = 0; i < 10; ++i) {
        ecl::test_adc::single();

        UnityPrint("ADC value: ---> ");
        UnityPrintNumberHex(arr[0], 4);
        UnityPrintWithEOL(" <---");

        ecl_spin_wait(1000);
    }
}
