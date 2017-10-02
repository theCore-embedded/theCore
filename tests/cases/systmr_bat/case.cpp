/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <platform/execution.hpp>

#include <unity.h>
#include <unity_fixture.h>

#include <ecl/unity_helpers.hpp>

static volatile uint32_t test_cnt;

extern "C" void systmr_handler(void)
{
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

    ecl::systmr::enable();

    // Dumb delay
    while (!test_cnt);

    ecl::systmr::disable();

    TEST_ASSERT_EQUAL(test_cnt, ecl::systmr::events() - events_now);
}

TEST(systmr, five_seconds)
{
    // Should delay by 5 seconds
    UnityPrintWithEOL("Test 5 seconds delay using systimer");

    auto expected_cnt = ecl::systmr::speed() * 5;

    ecl::systmr::enable();

    // Delay until deadline will be reached.
    while (expected_cnt > test_cnt);

    ecl::systmr::disable();

    UnityPrintWithEOL("5 seconds delay completed");
}
