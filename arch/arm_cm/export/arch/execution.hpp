//! \file
//! \brief Various routines for CM* platform, altering execution flow.

#ifndef CORE_ARCH_EXECUTION_HPP_
#define CORE_ARCH_EXECUTION_HPP_

#ifdef arm_cm4
#include <core_cm4.h>
#elif defined arm_cm3
#include <core_cm3.h>
#endif

namespace ecl
{

//! \brief Gets core clock speed.
//! \return Current clock speed.
static inline uint32_t clk_spd()
{
    return SystemCoreClock;
}

//! \brief Gets current clock count.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \return Current clock count.
static inline uint32_t clk()
{
    return DWT->CYCCNT;
}

//! \brief Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void abort()
{
    __disable_irq();
    for(;;);
}

//! Waits for interrupts.
//! \details Processor will stop executing until any interrupt will occur.
//! See also: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0014q/CJAJGICJ.html
static inline void wfi()
{
    __WFI();
}

} // namespace ecl


#endif // CORE_ARCH_EXECUTION_HPP_
