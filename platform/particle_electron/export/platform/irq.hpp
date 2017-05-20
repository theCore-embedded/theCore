//! \file
//! \brief Electron IRQ interface.
//!
#ifndef PLATFORM_IRQ_HPP_
#define PLATFORM_IRQ_HPP_

#include <spark_wiring_interrupts.h>

namespace ecl
{

using irq_num = int;

namespace irq
{

//! Masks or disables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void mask(irq_num irqn)
{
    // TODO
}

//! Unmasks or enables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void unmask(irq_num irqn)
{
    // TODO
}

//! Gets current IRQ number.
//! \warn Results are unspecified if called not within IRQ context.
//! \return Current IRQ number
static inline irq_num get_current_irqn()
{
    // TODO
    return -1;
}

//! Checks if a processor is in handler mode of execution at this time.
//! \retval true Processor is in handler mode. I.e. servicing IRQ or exception.
//! \retval false Processor is in thread mode.
static inline bool in_isr()
{
    // TODO
    return false;
}

//! Disables interrupts globally.
static inline void disable()
{
    noInterrupts();
}

//! Enables interrupts globally.
static inline void enable()
{
    interrupts();
}

//! Clears pending interrupt of the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void clear(irq_num irqn)
{
    // TODO
}

} // namespace irq

} // namespace ecl

#endif // PLATFORM_IRQ_HPP_
