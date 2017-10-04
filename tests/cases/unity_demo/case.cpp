/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP(demo_group);

TEST_SETUP(demo_group) { }

TEST_TEAR_DOWN(demo_group) { }

TEST(demo_group, test1)
{
    TEST_ASSERT_TRUE(0 == 1);
}
