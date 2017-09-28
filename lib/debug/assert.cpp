#include "ecl/assert.h"

#include <common/console.hpp>
#include <platform/execution.hpp>

using ecl::bypass_putc;
using ecl::bypass_puts;

// TODO: move it to the separate module working with integer to string conversions
static void print_line(unsigned int line)
{
    unsigned int mult = 1;
    unsigned int tmp = line;

    while(tmp /= 10) {
        mult *= 10;
    }

    for (unsigned int i = 1; i <= mult; i *= 10) {
        unsigned  int digit = line * i / mult;
        bypass_putc(digit + 48);
        line = line - digit * mult / i;
    }
}

extern "C"
void ecl_assert_failed(const char *assertion,
                       const char *message,
                       const char *file,
                       const char *func,
                       unsigned int line)
{
    bypass_puts(file);
    bypass_putc(':');

    print_line(line);

    bypass_puts(": ");

    if (func) {
        bypass_puts(func);
        bypass_puts(": ");
    }

    bypass_puts("assertion failed: ");
    bypass_puts(assertion);

    if (message) {
        bypass_puts("\r\n-> ");
        bypass_puts(message);
        bypass_puts("\r\n");
    }

    ecl::abort();
}
