//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//!
#ifndef THE_CORE_EXECUTION_H_
#define THE_CORE_EXECUTION_H_

#include <cstdint>


//! \brief Waits in loop for a given amount of milliseconds.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \param[in] ms Milliseconds to wait
static inline void ecl_spin_wait(uint32_t ms)
{
    // TODO: implement this routine
    (void) ms;
    for(;;);
}

// TODO #72: add add suspend() and resume() routines

//! \brief Aborts execution of currently running code. Never return.
static inline void ecl_abort()
{
    for(;;);
}


#endif // THE_CORE_EXECUTION_H_
