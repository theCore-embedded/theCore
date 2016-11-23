//! \file
//! \brief stm32f4xx IRQ interface.
//!
#ifndef PLATFORM_IRQ_HPP_
#define PLATFORM_IRQ_HPP_

#include <stm32f4xx.h>
#include <ecl/err.hpp>

#include <functional>
#include <type_traits>

namespace ecl
{

using irq_num = IRQn_Type;

namespace irq
{

//! Masks or disables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void mask(irq_num irqn)
{
    NVIC_DisableIRQ(irqn);
}

//! Unmasks or enables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void unmask(irq_num irqn)
{
    NVIC_EnableIRQ(irqn);
}

//! Gets current IRQ number.
//! \warn Results are unspecified if called not within IRQ context.
//! \return Current IRQ number
static inline irq_num get_current_irqn()
{
    volatile int irqn;
    asm volatile (
    "mrs %0, ipsr" : "=r" (irqn)
    );

    // IPSR holds exception numbers starting from 0
    // Valid IRQ number starts from -15
    // See Cortex-M4 processors documentation
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
    return static_cast<irq_num>(irqn - 16);
}

//! Checks if a processor is in handler mode of execution at this time.
//! \retval true Processor is in handler mode. I.e. servicing IRQ or exception.
//! \retval false Processor is in thread mode.
static inline bool in_isr()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

//! Disables interrupts globally.
static inline void disable()
{
    __disable_irq();
}

//! Enables interrupts globally.
static inline void enable()
{
    __enable_irq();
}

//! Clears pending interrupt of the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void clear(irq_num irqn)
{
    NVIC_ClearPendingIRQ(irqn);
}

} // namespace irq

} // namespace ecl

#endif // PLATFORM_IRQ_HPP_
