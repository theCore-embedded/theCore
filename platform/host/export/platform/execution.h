//! \file
//! \brief Module aggregates routines that are control execution flow on the host platform.
//!
#ifndef THE_CORE_HOST_PLATFORM_EXECUTION_H_

#include <stdlib.h>

//! \brief Aborts execution of currently running code. Never return.
static inline void ecl_abort()
{
    abort();
}

#endif // THE_CORE_HOST_PLATFORM_EXECUTION_H_
