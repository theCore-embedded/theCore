/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_irq IRQ control
//! @{

//! \file
//! \brief TM4C IRQ interface.
//!
#ifndef PLATFORM_IRQ_HPP_
#define PLATFORM_IRQ_HPP_

#include <platform/execution.hpp>

#include <ecl/err.hpp>

#include <functional>
#include <type_traits>

#include <interrupt.h>

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
    IntDisable(irqn);
}

//! Unmasks or enables the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void unmask(irq_num irqn)
{
    IntEnable(irqn);
}

//! Gets current IRQ number.
//! \warning Results are unspecified if called not within IRQ context.
//! \return Current IRQ number
static inline irq_num get_current_irqn()
{
    volatile int irqn;
    asm volatile (
    "mrs %0, ipsr" : "=r" (irqn)
    );

    // To be compatible with TivaWare library, IRQ number must be the same as exception number.
    // TODO: add reference links
    return irqn;
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
    IntMasterDisable();
}

//! Enables interrupts globally.
static inline void enable()
{
    IntMasterEnable();
}

//! Clears pending interrupt of the given IRQ.
//! \param[in] irqn Valid IRQ number.
//!
static inline void clear(irq_num irqn)
{
    IntPendClear(irqn);
}

} // namespace irq

} // namespace ecl

#endif // PLATFORM_IRQ_HPP_

//! @}

//! @}

//! @}
