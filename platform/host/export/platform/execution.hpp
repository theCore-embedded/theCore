//! \file
//! \brief Module aggregates routines that are control execution flow on the host platform.
//!
#ifndef THE_CORE_HOST_PLATFORM_EXECUTION_HPP_
#define THE_CORE_HOST_PLATFORM_EXECUTION_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

namespace ecl
{

//! \brief Aborts execution of currently running code. Never return.
static inline void abort()
{
    abort();
}

//! \brief Gets core clock speed.
//! \return Current clock speed. Unreliable on host platform.
static inline uint64_t clk_spd()
{
    return 1000; // Pretend that there is a 1 kHz clock.
}

//! \brief Gets current clock count.
//! \return Current clock count.
static inline uint64_t clk()
{
    // Host platform is unreliable in terms of clock speed, so just return
    // seconds since epoch here. See also ecl::clk_spd()
    return time(NULL) * 1000;
}

} // namespace ecl

#endif // THE_CORE_HOST_PLATFORM_EXECUTION_HPP_
