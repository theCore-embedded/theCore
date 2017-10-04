/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Provides debug stubs required by TivaWare library.

// Required by TivaWare
#include <cstdint>
#include "debug.h"

#include <ecl/assert.h>

extern "C"
void __error__(char *pcFilename, uint32_t ui32Line)
{
    ecl_assert_failed("", "TivaWare check failed.", pcFilename, "", ui32Line);
}
