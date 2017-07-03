//! \file
//! \brief Various routines for CM* architectures, altering execution flow.

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

#include <limits.h>

//! \brief Provides a small delay.
//! \param loop_count is the number of delay loop iterations to perform.
//!
//! This function provides a means of generating a delay by executing a simple
//! 3 instruction cycle loop a given number of times.  It is written in
//! assembly to keep the loop instruction count consistent across tool chains.
//!
//! \return None.
extern "C" void arch_delay(uint32_t loop_count);

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

//! Waits for events.
//! \details Processor will stop executing until any event will occur.
//! \note Event flag will be set regardless of execution state. To track
//! spurious wakeup, additional flags must be checked.
//! See also: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0014q/CJAJGICJ.html
static inline void wfe()
{
    __WFE();
}

#if USE_SYSTMR

namespace systmr
{

//! Represents the minimum and the maximun possible values for SYSTMR frequency.
//! This allows to guarantee the correct SYSTRM frequency regardless of SystemCoreClock.
static const uint32_t systmr_freq_min = 20;
static const uint32_t systmr_freq_max = 1000;

static_assert((THECORE_CONFIG_SYSTMR_FREQ > systmr_freq_min) &&
               (THECORE_CONFIG_SYSTMR_FREQ < systmr_freq_max),
               "The value of the THECORE_CONFIG_SYSTMR_FREQ frequency is out of the range.");

 //! Enables timer, start counting
static inline void enable()
{
    NVIC_EnableIRQ(SysTick_IRQn);

    uint32_t reload_val = (1000 / THECORE_CONFIG_SYSTMR_FREQ) * SystemCoreClock / 1000;

    SysTick_Config(reload_val);
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
    // with the frequency of THECORE_CONFIG_SYSTMR_FREQ.
    uint32_t reload_val = (1000 / THECORE_CONFIG_SYSTMR_FREQ) * SystemCoreClock / 1000;

    return SystemCoreClock / reload_val;
}

//! Gets amount of events occurred so far.
//! \details Events counter may not reset after disable() call.
//! \retval Amount of events occured so far
uint32_t events();

} // namespace systmr

#endif // USE_SYSTMR

//! \brief Performs a dummy busy wait for specified amount of milliseconds.
//! \param ms number of milliseconds to wait.
//!
//! This function is useful for a short delays.
//!
//! \return None.
static inline void arch_spin_wait(uint32_t ms)
{
    const short arch_delay_ticks = 3;

    // handle overflow
    uint64_t ticks_left = (ecl::clk_spd() / 1000L / arch_delay_ticks) * ms;
    while (UINT_MAX < ticks_left) {
        arch_delay(UINT_MAX);
        ticks_left -= UINT_MAX;
    }

    arch_delay(ticks_left);
}

} // namespace ecl


#endif // CORE_ARCH_EXECUTION_HPP_
