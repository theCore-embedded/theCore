#include <ecl/unity_helpers.hpp>
#include <common/execution.hpp>

#include <test_adc.hpp>

// Headers are included in the last place to avoid some
// overrides of stdlib functions (like calloc/malloc/etc.)
#include <unity.h>
#include <unity_fixture.h>

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

    UnityPrintWithEOL("Performing 5 channels conversions");

    for (int i = 0; i < 5; ++i) {
        ecl::test_adc::single();

        UnityPrintWithEOL("--------- ADC values ---------");

        for (size_t ch = 0; ch < arr.size(); ++ch) {
            UnityPrint("channel: ");
            UnityPrintNumberHex(ch, 2);
            UnityPrint(" value: ");
            UnityPrintNumberHex(arr[ch], 4);
            UNITY_PRINT_EOL();
        }

        ecl::wait_for(1000);
    }
}
