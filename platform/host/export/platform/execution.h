//! \file
//! \brief Module aggregates routines that are control execution flow on the host platform.
//!
#ifndef THE_CORE_HOST_PLATFORM_EXECUTION_H_
#define THE_CORE_HOST_PLATFORM_EXECUTION_H_

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

//! \brief Aborts execution of currently running code. Never return.
static inline void ecl_abort()
{
    abort();
}

//! \brief Gets core clock speed.
//! \return Current clock speed. Unreliable on host platform.
static inline uint32_t get_clk_spd()
{
    return 1; // Pretend that there is a 1 Hz clock.
}

//! \brief Gets current clock count.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \return Current clock count.
static inline uint32_t get_clk()
{
    // Host platform is unreliable in terms of clock speed, so just return
    // seconds since epoch here. See also get_clk_spd()
    return (uint32_t) time(NULL);
}

#endif // THE_CORE_HOST_PLATFORM_EXECUTION_H_
