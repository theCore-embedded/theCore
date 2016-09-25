#include <unity.h>
#include <unity_fixture.h>

extern "C"
void board_init()
{
}


TEST_GROUP(demo_group);

TEST_SETUP(demo_group) { }

TEST_TEAR_DOWN(demo_group) { }

TEST(demo_group, test1)
{
    TEST_ASSERT_TRUE(0 == 1);
}

TEST_GROUP_RUNNER(demo_group)
{
    RUN_TEST_CASE(demo_group, test1);
}

int main()
{
    RUN_TEST_GROUP(demo_group);
}
