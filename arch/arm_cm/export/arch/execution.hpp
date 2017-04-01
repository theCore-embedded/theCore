//! \file
//! \brief Various routines for CM* platform, altering execution flow.

#ifndef CORE_ARCH_EXECUTION_HPP_
#define CORE_ARCH_EXECUTION_HPP_

#ifndef PLATFORM_CMSIS_COMPLIANT
#error "Platform is not CMSIS compliant and does not provide any device definitions"
#else
#include <platform/cm_device.hpp>
#endif

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

#if USE_SYSTMR_SYSTICK

namespace systmr
{

 //! Enables timer, start counting
static inline void enable()
{
    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Config(SysTick_LOAD_RELOAD_Msk - 1);
}

//! Disables timer, stop counting
static inline void disable()
{
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
    NVIC_DisableIRQ(SysTick_IRQn);
}

//! Gets speed in which timer generate events.
//! \details _Each event generates interrupt_, but not necessarily timer interrupt
//! is passed to the user. In other words, each event will wake-up processor.
//! Useful in conjunction with WFI.
//! \retval System timer events generation speed in Hz.
static inline auto speed()
{
    // Systick is running from system core clock and generates events
    // once per SysTick_LOAD_RELOAD_Msk clock pulses.
    return SystemCoreClock / SysTick_LOAD_RELOAD_Msk;
}

//! Gets amount of events occurred so far.
//! \details Events counter may not reset after disable() call.
//! \retval Amount of events occured so far
uint32_t events();

} // namespace systmr

#endif // USE_SYSTMR_SYSTICK


} // namespace ecl


#endif // CORE_ARCH_EXECUTION_HPP_
