#ifndef KE02_PLATFORM_IRQ_HPP_
#define KE02_PLATFORM_IRQ_HPP_


// Temporary measure, while we are throwing away redundant IRQ manager
#define THECORE_NO_IRQ_MANAGER 1

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
    (void)irqn;
}

//! Unmasks or enables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void unmask(irq_num irqn)
{
    (void)irqn;
}

//! Gets current IRQ number.
//! \warn Results are unspecified if called not within IRQ context.
//! \return Current IRQ number
static inline irq_num get_current_irqn()
{
    return 0;
}

//! Checks if a processor is in handler mode of execution at this time.
//! \retval true Processor is in handler mode. I.e. servicing IRQ or exception.
//! \retval false Processor is in thread mode.
static inline bool in_isr()
{
    return false;
}

//! Disables interrupts globally.
static inline void disable()
{
}

//! Enables interrupts globally.
static inline void enable()
{
}

//! Clears pending interrupt of the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void clear(irq_num irqn)
{
    (void)irqn;
}

} // namespace irq

} // namespace ecl


#endif //
