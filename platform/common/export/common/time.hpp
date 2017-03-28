//! \file
//! \brief Time utilities of theCore

#ifndef LIB_TYPES_TIME_HPP_
#define LIB_TYPES_TIME_HPP_

#include <chrono>
#include <platform/execution.hpp>

namespace ecl
{

//! Gets current time in milliseconds.
//! \todo: substitute it with platform's RTC. See #213
static inline std::chrono::milliseconds get_ms_time()
{
    auto spd = ecl::clk_spd() / 1000; // In kHz
    auto clk = ecl::clk();

    return std::chrono::milliseconds(clk / spd);
}

} // namespace ecl

#endif // LIB_TYPES_TIME_HPP_
