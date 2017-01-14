//! \file
//! \brief Execution control for TM4C platform.
#ifndef TM4C_EXECUTION_H_
#define TM4C_EXECUTION_H_

#include <stdbool.h>
#include <stdint.h>
#include <sysctl.h>

//! \brief Waits in loop for a given amount of milliseconds.
//! \param[in] ms Milliseconds to wait
static inline void ecl_spin_wait(uint32_t ms)
{
    // TODO: use more precise method to get current clock.
    // See documentation for SysCtlClockGet() for more info.
    uint32_t clock = SysCtlClockGet();
    uint32_t wait_cycles = ms * (clock / 1000);
    SysCtlDelay(wait_cycles);
}

__attribute__((noreturn))
static inline void ecl_abort()
{
    for(;;);
}

#endif  // TM4C_EXECUTION_H_
