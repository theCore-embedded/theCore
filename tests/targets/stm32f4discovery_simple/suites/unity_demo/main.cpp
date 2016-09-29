//! \file
//! \brief
//! \todo Autogenerate this file

#include <unity.h>
#include <unity_fixture.h>

extern "C"
void board_init()
{

}

TEST_GROUP_RUNNER(demo_group)
{
    RUN_TEST_CASE(demo_group, test1);
}

static void suite_runner()
{
    RUN_TEST_GROUP(demo_group);
}

int main()
{
    suite_runner();
}
