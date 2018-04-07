/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/unity_helpers.hpp>

#include <test_gpio.hpp>

#include <platform/exti_manager.hpp>

// Headers are included in the last place to avoid some
// overrides of stdlib functions (like calloc/malloc/etc.)
#include <unity.h>
#include <unity_fixture.h>

// Holds info about expectancy of handler call.
static volatile bool                call_is_expected    = false;

// True if test_exti_hanlder() call occur. Must be set to false before new call.
static volatile bool                call_occur          = true;

// Expected ctx during test_exti_hanlder() call.
static void                         *expected_ctx       = nullptr;

// Handler object.
static ecl::exti_manager::handler   test_handler        = {};

// Handles EXTI events during test
static void test_exti_hanlder(void *ctx)
{
    TEST_ASSERT_TRUE(call_is_expected);
    TEST_ASSERT_TRUE(ctx == expected_ctx);
    TEST_ASSERT_FALSE(call_occur);

    call_occur = true;
}

TEST_GROUP(exti_basic);

TEST_SETUP(exti_basic)
{
    expected_ctx        = nullptr;
    call_is_expected    = false;
    call_occur          = false;

    test_handler.set_cb(test_exti_hanlder);
    test_handler.set_ctx(nullptr);

    ecl::exti_manager::subscribe<ecl::push_button>(test_handler,
        ecl::exti_manager::trigger::rising);
}

TEST_TEAR_DOWN(exti_basic)
{
    ecl::exti_manager::unsubscribe(test_handler);
}

TEST(exti_basic, subscribe_unsubscribe)
{
    // Both subscribe and unsubscribe take place in the setup and teardown
}

TEST(exti_basic, mask_unmask)
{
    ecl::exti_manager::mask(test_handler);
    ecl::exti_manager::unmask(test_handler);
}

TEST(exti_basic, handle_presses)
{
    int test_ctx            = -1;
    int calls_expected      = 3;
    int calls_occur         = 0;

    call_is_expected = true;
    expected_ctx = &test_ctx;

    test_handler.set_ctx(expected_ctx);

    UnityPrintWithEOL("Press button 3 times...");

    while (calls_occur < calls_expected) {
        ecl::exti_manager::unmask(test_handler);
        while (!call_occur) { } // Wait for a call

        UnityPrintWithEOL("Button press detected");

        call_occur = false;

        calls_occur++;
    }
}
