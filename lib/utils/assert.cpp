#include "assert.h"

#include <ecl/iostream.hpp>

extern "C"
void ecl_assert_failed(const char *assertion,
                       const char *message,
                       const char *file,
                       const char *func,
                       unsigned int line)
{
    ecl::cout << file << ':' << line << ": ";

    if (func)
        ecl::cout << func << ": ";

    ecl::cout << "assertion failed: " << assertion << ecl::endl;

    if (message)
        ecl::cout << "-> " << message << ecl::endl;

    for(;;);
}
