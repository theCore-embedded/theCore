#include <ecl/iostream.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

TEST_GROUP(console_integrity)
{
};

TEST(console_integrity, print)
{
    ecl::cout << "Printing some numbers ";
    for (int i = 0; i < 10; ++i) {
        ecl::cout << i << ' ';
    }

    ecl::cout << ecl::endl;
    ecl::cout << "End printing numbers" << ecl::endl;
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
