#include <ecl/thread/thread.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

TEST_GROUP(exposed_thread)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(exposed_thread, default_thread_is_not_joinable)
{
    ecl::exposed_thread tr;

    CHECK_EQUAL(ecl::err::perm, tr.join());
}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
