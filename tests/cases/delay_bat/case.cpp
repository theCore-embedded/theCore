//! \file
//! \brief Delay test case

#include <common/execution.hpp>

// Headers are included in the last place to avoid some
// overrides of stdlib functions (like calloc/malloc/etc.)
#include <unity.h>
#include <unity_fixture.h>
#include <ecl/unity_helpers.hpp>

TEST_GROUP(delay_bat);

TEST_SETUP(delay_bat)
{
}

TEST_TEAR_DOWN(delay_bat)
{
}

TEST(delay_bat, platform_spin_wait)
{
    UnityPrintWithEOL("Delay 100 ms");
    ecl::spin_wait(100);

    UnityPrintWithEOL("Delay 2500 ms");
    ecl::spin_wait(2500);

    UnityPrintWithEOL("Delay 500 ms");
    ecl::spin_wait(500);
}

TEST(delay_bat, common_wait_for)
{
    UnityPrintWithEOL("Wait for 100 ms");
    ecl::wait_for(100);

    UnityPrintWithEOL("Wait for 2500 ms");
    ecl::wait_for(2500);

    bool rc;

    UnityPrintWithEOL("Wait for 2500 ms with false predicate");
    rc = ecl::wait_for(2500, []{ return false; });
    TEST_ASSERT_FALSE(rc);
}