/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Stubs implementation for Unity functions.

#include <platform/console.hpp>

extern "C" void unity_putc(char c)
{
    if (c == '\n') {
        ecl::bypass_putc('\r');
    }

    ecl::bypass_putc(c);
}
