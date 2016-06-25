#include <platform/irq_manager.hpp>
#include <new>

#include "misc.h"

//! Encapsultaes entry point of the IRQ service routine
struct irq_manager_entry_point : public ecl::irq_manager
{
    //! Bypasses encapsulation, but hides it from a user of the IRQ manager itself
    static void isr()
    {
        ecl::irq_manager::isr();
    }
};

//! Handles IRQ and visible to startup code
extern "C" __attribute__ ((used))
void core_isr()
{
    irq_manager_entry_point::isr();
}


//------------------------------------------------------------------------------

namespace ecl
{

irq_manager::handler_storage irq_manager::m_storage[CONFIG_IRQ_COUNT];


void irq_manager::init()
{
    // Initialize storage
    for (auto &h : m_storage) {
        new (&h) handler_type{default_handler};
    }

    __enable_irq();
}

err irq_manager::subscribe(irq_num irqn, const std::function< void() > &handler)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    auto handlers = extract_handlers();

    __disable_irq();

    // Magic here.
    // Logical priority of *any* user interrupt that use
    // FreeRTOS API must not be greather than
    // configMAX_SYSCALL_INTERRUPT_PRIORITY
    NVIC_SetPriority(irqn, CONFIG_MAX_ISR_PRIORITY);
    handlers[irqn] = handler;
    __enable_irq();
    return err::ok;
}

err irq_manager::unsubscribe(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    auto handlers = extract_handlers();
    __disable_irq();
    handlers[irqn] = default_handler;
    __enable_irq();
    return err::ok;
}

err irq_manager::mask(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    NVIC_DisableIRQ(irqn);
    return err::ok;
}

err irq_manager::unmask(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    // TODO: error check
    NVIC_EnableIRQ(irqn);
    return err::ok;
}

bool irq_manager::in_isr()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

void irq_manager::disable()
{
    __disable_irq();
}

void irq_manager::enable()
{
    __enable_irq();
}

err irq_manager::clear(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    // TODO: error check
    NVIC_ClearPendingIRQ(static_cast< irq_num > (irqn));
    return err::ok;
}

//------------------------------------------------------------------------------

void irq_manager::isr()
{
    volatile int irqn;
    auto handlers = extract_handlers();

    asm volatile (
                "mrs %0, ipsr" : "=r" (irqn)
                );

    // IPSR holds exception numbers starting from 0
    // Valid IRQ number starts from -15
    // See Cortex-M4 processors documentation
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
    irqn -= 16;

    // TODO: Is it needed?
    mask(static_cast< irq_num >(irqn));
    handlers[irqn]();
}

void irq_manager::default_handler()
{
    __disable_irq();
    for(;;);
}

}
