/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DEFAULT_CONSOLE_DRIVER_HPP_
#define DEFAULT_CONSOLE_DRIVER_HPP_

#include <aux/platform_defines.hpp>

#if THECORE_CONFIG_USE_CONSOLE

#include <platform/console.hpp>
#include <dev/bus.hpp>
#include <dev/console_pipe.hpp>

namespace ecl
{

// Required aliases
using console_bus = ecl::generic_bus<platform_console>;
using console_driver = ecl::console_pipe<console_bus>;

} // namespace ecl

#else // THECORE_CONFIG_USE_CONSOLE

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

} // namespace ecl

#endif // THECORE_CONFIG_USE_CONSOLE

#endif // DEFAULT_CONSOLE_DRIVER_HPP_
