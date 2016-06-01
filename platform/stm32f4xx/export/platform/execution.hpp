//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//!
#ifndef THE_CORE_EXECUTION_HPP
#define THE_CORE_EXECUTION_HPP

#include <stm32f4xx.h>

namespace ecl
{

//! \brief Waits in loop for a given amount of milliseconds.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \param[in] ms Milliseconds to wait
void spin_wait(uint32_t ms)
{
    auto start = DWT->CYCCNT;
    auto to_wait = ms * (SystemCoreClock / 1000L);

    // Handles wraparound as well.
    do { } while (DWT->CYCCNT - start < to_wait);
}

// TODO #72: add add suspend() and resume() routines

}

#endif //THE_CORE_EXECUTION_HPP
