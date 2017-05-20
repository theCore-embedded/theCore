#ifndef DEFAULT_CONSOLE_DRIVER_HPP_
#define DEFAULT_CONSOLE_DRIVER_HPP_

#if THECORE_USE_CONSOLE

#include <platform/console.hpp>
#include <dev/bus.hpp>
#include <dev/bus_pipe.hpp>

namespace ecl
{

// Required aliases
using console_bus = ecl::generic_bus<platform_console>;
using console_driver = ecl::bus_pipe<console_bus>;

}

#else // THECORE_USE_CONSOLE

#include <cstdint>
#include <unistd.h>
#include <ecl/err.hpp>

namespace ecl
{

// Do nothing class. Must be used in case if console driver doesn't needed.
// Must have the same interface as platform's console driver.
class console_driver
{
public:
    ecl::err init() { return ecl::err::ok; }

    ssize_t write(const uint8_t *data, size_t count)
    {
        (void) data;
        (void) count;
        return 0;
    }

    ssize_t read(uint8_t *data, size_t count)
    {
        (void) data;
        (void) count;
        return 0;
    }
};

}

#endif // THECORE_USE_CONSOLE

#endif // DEFAULT_CONSOLE_DRIVER_HPP_
