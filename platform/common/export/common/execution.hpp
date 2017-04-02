//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//! \details Common implementation accross multiple platforms.

#ifndef PLATFORM_COMMON_EXECUTION_HPP_
#define PLATFORM_COMMON_EXECUTION_HPP_

#include <platform/execution.hpp>
#include <ecl/assert.h>

namespace ecl
{

//! Waits conditionally not less than given amount of milliseconds.
//! \tparam Predicate Predicate type.
//! \param[in] ms Milliseconds to wait.
//! \param[in] pred Predicate to check.
//! \todo Use RTC instead of relying on clock. See #213 also.
//! \details Stops waiting if predicate returns true. Guarantees that predicate
//! will be called at least once.
//! \note If WFE is present and used, MCU will go to sleep waiting for events.
//! Events will wake up MCU, allowing predicate to be executed.
//! In such manner, granurality of predicate checks depends on events rate.
//! \retval false Timeout reached and predicate is still false.
//! \retval true Predicate returned true before timeout was hit.
template<class Predicate>
static inline bool wait_for(uint32_t ms, Predicate pred)
{
    ecl_assert(ms);

#ifdef USE_SYSTMR
    // Current events count
    auto start = ecl::systmr::events();

    // Timer speed
    auto spd = ecl::systmr::speed();

    // TODO: handle possible wraparounds using 64 bit arithmetic?

    // Time to wait in timer events
    auto to_wait = spd * ms / 1000;

    // TODO: handle wraparounds using 64 bit arithmetic?
    ecl_assert(spd * ms / spd == ms);

    // Calculations may produce 0, if amount of milliseconds is less than
    // timer period.
    if (!to_wait) {
        to_wait = 1;
    }

    ecl::systmr::enable();

    while (1) {
        if (pred()) { // Make sure that predicate is called at least once.
            return true;
        } else if (ecl::systmr::events() - start >= to_wait) { // Handles wraparound.
            break;
        }

#ifdef USE_WFI_WFE
        ecl::wfe();
#endif // USE_WFI_WFE
    }

    ecl::systmr::disable();
#else // No system timer
    uint32_t start = ecl::clk();
    uint32_t to_wait = ms * (ecl::clk_spd() / 1000L);

    while (1) {
        if (pred()) { // Make sure that predicate is called at least once.
            return true;
        } else if (ecl::clk() - start >= to_wait) { // Handles wraparound.
            break;
        }

#ifdef USE_WFI_WFE
        ecl::wfe();
#endif // USE_WFI_WFE
    }
#endif // USE_SYSTMR

    return false;
}

//! \brief Waits in loop for a given amount of milliseconds.
//! \param[in] ms Milliseconds to wait
//! \todo Use RTC instead of relying on clock. See #213 also.
static inline void wait_for(uint32_t ms)
{
    wait_for(ms, []{ return false; /* Wait till timeout */} );
}

} // namespace ecl

#endif // PLATFORM_COMMON_EXECUTION_HPP_
