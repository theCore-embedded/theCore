//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//! \details Common implementation accross multiple platforms.

#ifndef PLATFORM_COMMON_EXECUTION_HPP_
#define PLATFORM_COMMON_EXECUTION_HPP_

#include <platform/execution.hpp>

namespace ecl
{

//! \brief Waits in loop for a given amount of milliseconds.
//! \param[in] ms Milliseconds to wait
//! \todo Use RTC instead of relying on clock. See #213 also.
static inline void spin_wait(uint32_t ms)
{
    uint32_t start = ecl::clk();
    uint32_t to_wait = ms * (ecl::clk_spd() / 1000L);

    // Handles wraparound as well.
    do { } while (ecl::clk() - start < to_wait);
}

} // namespace ecl

#endif // PLATFORM_COMMON_EXECUTION_HPP_
