/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/iostream.hpp>

// Headers are included in the last place to avoid some
// overrides of stdlib functions (like calloc/malloc/etc.)
#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP(streams_bat);

TEST_SETUP(streams_bat) { }

TEST_TEAR_DOWN(streams_bat) { }

TEST(streams_bat, cout)
{
    // Do not bother about anything except printing in console. Yet.

    ecl::cout << "Streams test case output" << ecl::endl;
}
