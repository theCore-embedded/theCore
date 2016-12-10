#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP(demo_group);

TEST_SETUP(demo_group) { }

TEST_TEAR_DOWN(demo_group) { }

TEST(demo_group, test1)
{
    TEST_ASSERT_TRUE(0 == 1);
}
