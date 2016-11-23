//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//!
#ifndef THE_CORE_EXECUTION_H
#define THE_CORE_EXECUTION_H

#include <stm32f4xx.h>
#include <core_cm4.h>

#ifdef __cplusplus
extern "C" {
#endif

//! \brief Waits in loop for a given amount of milliseconds.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \param[in] ms Milliseconds to wait
static inline void ecl_spin_wait(uint32_t ms)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t to_wait = ms * (SystemCoreClock / 1000L);

    // Handles wraparound as well.
    do {} while (DWT->CYCCNT - start < to_wait);
}

// TODO #72: add add suspend() and resume() routines

//! \brief Aborts execution of currently running code. Never return.
static inline void ecl_abort()
{
    __disable_irq();
    for (;;);
}

#ifdef __cplusplus
}
#endif

#endif //THE_CORE_EXECUTION_H
