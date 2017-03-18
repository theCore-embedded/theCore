//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//! \details Common implementation accross multiple platforms.

#ifndef PLATFORM_COMMON_EXECUTION_H_
#define PLATFORM_COMMON_EXECUTION_H_

#include <platform/execution.h>

//! \brief Waits in loop for a given amount of milliseconds.
//! \param[in] ms Milliseconds to wait
//! \todo Use RTC instead of relying on clock. See #213 also.
static inline void ecl_spin_wait(uint32_t ms)
{
    uint32_t start = ecl_get_clk();
    uint32_t to_wait = ms * (ecl_get_clk_spd() / 1000L);

    // Handles wraparound as well.
    do { } while (ecl_get_clk() - start < to_wait);
}

#endif // PLATFORM_COMMON_EXECUTION_H_
