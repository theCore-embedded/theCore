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
