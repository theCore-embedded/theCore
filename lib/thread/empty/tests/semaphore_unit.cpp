#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <ecl/thread/semaphore.hpp>

/* Declare TestGroup with name CheatSheet */
TEST_GROUP(semaphore)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(semaphore, signal_wait_no_hang)
{
    // Should complete without a hang
    ecl::semaphore sem;
    sem.signal();
    sem.wait();
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
