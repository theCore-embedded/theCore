/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
