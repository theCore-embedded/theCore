/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//! \details Common implementation accross multiple platforms.

#ifndef PLATFORM_COMMON_EXECUTION_HPP_
#define PLATFORM_COMMON_EXECUTION_HPP_

#include <platform/execution.hpp>
#include <ecl/assert.h>
#include <chrono>

namespace ecl
{

//! Waits conditionally not less than given amount of milliseconds.
//! \tparam Predicate Predicate type.
//! \param[in] ms Milliseconds to wait. Can be 0.
//! \param[in] pred Predicate to check.
//! \todo Use RTC instead of relying on clock. See #213 also.
//! \details Stops waiting if predicate returns true. Guarantees that predicate
//! will be called at least once.
//! \note If WFE and systimer are present and used, MCU will go to sleep,
//! waiting for events.
//! External events or interrupts will wake up MCU, allowing predicate
//! to be executed. This also means that at minimum, MCU will wake up on every
//! timer interrupt.
//! In such manner, granularity of predicate checks depends on events rate.
//! \retval false Timeout reached and predicate is still false.
//! \retval true Predicate returned true before timeout was hit.
template<class Predicate>
static inline bool wait_for(uint32_t ms, Predicate pred)
{
    // Small optimization - do not start timers if there is zero wait time.
    if (!ms) {
        return pred();
    }

#ifdef THECORE_OWNS_SYSTMR
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
#else
    // Amount of millisecond to spin in one step.
    // Predicate will be checked once per quant.
    // No rational reasoning behind this value.
    constexpr auto delay_quant = 25;

    while (ms) {
        if (pred()) { // Make sure that predicate is called at least once.
            return true;
        } else {
            spin_wait(delay_quant);
            ms = ms > delay_quant ? ms - delay_quant : 0;
        }
    }
#endif // THECORE_OWNS_SYSTMR

    return false;
}

//! \brief Waits in loop for a given amount of milliseconds.
//! \param[in] ms Milliseconds to wait
//! \todo Use RTC instead of relying on clock. See #213 also.
static inline void wait_for(uint32_t ms)
{
    wait_for(ms, []{ return false; /* Wait till timeout */} );
}

//! \brief Waits in loop for given amount of time, expressed using
//! std::chrono library.
//! \param[in] wait_duration Time to wait.
//! \todo Use RTC instead of relying on clock. See #213 also.
template<class Rep, class Period>
static inline void wait_for(const std::chrono::duration<Rep, Period>& wait_duration)
{
    wait_for(std::chrono::milliseconds(wait_duration).count(),
        []{ return false; /* Wait till timeout */} );
}

} // namespace ecl

#endif // PLATFORM_COMMON_EXECUTION_HPP_
