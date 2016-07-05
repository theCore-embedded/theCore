#include "platform/irq_manager.hpp"
#include "platform/irq.hpp"
#include "misc.h"

#include <new>


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

    irq::enable();
}

err irq_manager::subscribe(irq_num irqn, const std::function< void() > &handler)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    auto handlers = extract_handlers();

    irq::disable();

    // Magic here.
    // Logical priority of *any* user interrupt that use
    // FreeRTOS API must not be greather than
    // configMAX_SYSCALL_INTERRUPT_PRIORITY
    NVIC_SetPriority(irqn, CONFIG_MAX_ISR_PRIORITY);
    handlers[irqn] = handler;
    irq::enable();
    return err::ok;
}

err irq_manager::unsubscribe(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    auto handlers = extract_handlers();
    irq::disable();
    handlers[irqn] = default_handler;
    irq::enable();
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
    irq::unmask(irqn);
    return err::ok;
}

bool irq_manager::in_isr()
{
    return irq::in_isr();
}

void irq_manager::disable()
{
    irq::disable();
}

void irq_manager::enable()
{
    irq::enable();
}

err irq_manager::clear(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return err::inval;
    }

    // TODO: error check
    clear(irqn);
    return err::ok;
}

//------------------------------------------------------------------------------

void irq_manager::isr()
{
    volatile int irqn = irq::get_current_irqn();
    auto handlers     = extract_handlers();

    // TODO: Is it needed?
    irq::mask(static_cast< irq_num >(irqn));
    handlers[irqn]();
}

void irq_manager::default_handler()
{
    irq::disable();
    for(;;);
}

}
