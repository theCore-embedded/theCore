//! \file
//! \brief Module aggregates routines that are control execution flow of the MCU.
//!
#ifndef THE_CORE_EXECUTION_H_
#define THE_CORE_EXECUTION_H_

#include <stdint.h>
#include <stm32_device.h>

//! \brief Gets core clock speed.
//! \return Current clock speed.
static inline uint32_t get_clk_spd()
{
    return SystemCoreClock;
}

//! \brief Gets current clock count.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \return Current clock count.
static inline uint32_t get_clk()
{
    return DWT->CYCCNT;
}

// TODO #72: add suspend() and resume() routines
// TODO #213 add RTC support

//! \brief Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void ecl_abort()
{
    __disable_irq();
    for(;;);
}


#endif // THE_CORE_EXECUTION_H_
