//! \file
//! \brief Various routines for particle electron platform, altering execution flow.

#ifndef PLATFORM_EXECUTION_HPP_
#define PLATFORM_EXECUTION_HPP_

#include <stdint.h>
#include <application.h>
#include "platform/defines.hpp"

namespace ecl
{

//! Temporary define that notifies system that delay routine is present
//! Will stay here before #247 will be implemented
#define THECORE_PLATFORM_STUB_DEFINE_SPIN_WAIT

//! Waits not less than given amount of milliseconds.
static inline void spin_wait(unsigned ms)
{
    ::delay(ms);
}

//! \brief Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void abort()
{
    // TODO
    for(;;);
}

namespace systmr
{

 //! Enables timer, start counting
void enable();

//! Disables timer, stop counting
void disable();

//! Gets speed in which timer generate events.
//! \details _Each event generates interrupt_, but not necessarily timer interrupt
//! is passed to the user. In other words, each event will wake-up processor.
//! Useful in conjunction with WFI.
//! \retval System timer events generation speed in Hz.
uint32_t speed();

//! Gets amount of events occurred so far.
//! \details Events counter may not reset after disable() call.
//! \retval Amount of events occured so far
uint32_t events();

} // namespace systmr

} // namespace ecl

#endif // PLATFORM_EXECUTION_HPP_
