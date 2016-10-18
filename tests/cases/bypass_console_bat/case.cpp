#include <platform/execution.h>
#include <platform/console.hpp>

#include <unity.h>
#include <unity_fixture.h>


TEST_GROUP(bypass_console_bat);

TEST_SETUP(bypass_console_bat)
{
}

TEST_TEAR_DOWN(bypass_console_bat)
{
}

TEST(bypass_console_bat, print_ascii)
{
    ecl::bypass_putc('\r');
    ecl::bypass_putc('\n');

    for (char c = 0x20; c < 0x7f; ++c) {
        ecl::bypass_putc(c);
        if ((c - 0x20 & 0xf) == 0xf) {
            ecl::bypass_putc('\r');
            ecl::bypass_putc('\n');
        }
    }
}

