#include <unity.h>
#include <unity_fixture.h>

#include <ecl/iostream.hpp>

TEST_GROUP(streams_bat);

TEST_SETUP(streams_bat) { }

TEST_TEAR_DOWN(streams_bat) { }

TEST(streams_bat, cout)
{
    // Do not bother about anything except printing in console. Yet.

    ecl::cout << "Streams test case output" << ecl::endl;
}
