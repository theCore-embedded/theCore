//! \file
//! \brief Common GPIO definitions for all platforms.
#ifndef PLATFORM_COMMON_GPIO_
#define PLATFORM_COMMON_GPIO_

namespace ecl
{

//! Empty GPIO that does nothing.
struct empty_gpio
{
    static void set() { }
    static void reset() { }
    static void toggle() { }
    static bool get() { return false; }
};

} // namespace ecl

#endif // PLATFORM_COMMON_GPIO_
