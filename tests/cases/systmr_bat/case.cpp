#include <unity.h>
#include <unity_fixture.h>

#include <ecl/unity_helpers.hpp>

#include <platform/execution.hpp>

static volatile uint32_t test_cnt;
static volatile bool tmr_enabled;

extern "C" void systmr_handler(void)
{
    // Should not be called if timer is disabled
    TEST_ASSERT_TRUE(tmr_enabled);
    test_cnt++;
}

TEST_GROUP(systmr);

TEST_SETUP(systmr)
{
    test_cnt = 0;
}

TEST_TEAR_DOWN(systmr) { }

TEST(systmr, on_off)
{
    // Should not crash (at least)

    // Current event count may not be equal to 0 if timer was running
    // previously.
    auto events_now = ecl::systmr::events();

    tmr_enabled = true;
    ecl::systmr::enable();

    // Dumb delay
    while (!test_cnt);

    ecl::systmr::disable();
    tmr_enabled = false;

    TEST_ASSERT_EQUAL(test_cnt, ecl::systmr::events() - events_now);
}

TEST(systmr, five_seconds)
{
    // Should delay by 5 seconds
    UnityPrintWithEOL("Test 5 seconds delay using systimer");

    auto expected_cnt = ecl::systmr::speed() * 5;

    tmr_enabled = true;
    ecl::systmr::enable();

    // Delay until deadline will be reached.
    while (expected_cnt > test_cnt);

    ecl::systmr::disable();

    tmr_enabled = false;
    UnityPrintWithEOL("5 seconds delay completed");
}
