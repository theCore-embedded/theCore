#include <platform/irq_manager.hpp>
#include <new>

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

int irq_manager::subscribe(irq_num irqn, const std::function< void() > &handler)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return -1;
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
    return 0;
}

int irq_manager::unsubscribe(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return -1;
    }

    auto handlers = extract_handlers();
    __disable_irq();
    handlers[irqn] = default_handler;
    __enable_irq();
    return 0;
}

int irq_manager::mask(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return -1;
    }

    NVIC_DisableIRQ(irqn);
    return 0;
}

int irq_manager::unmask(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return -1;
    }

    // TODO: error check
    NVIC_EnableIRQ(irqn);
    return 0;
}

int irq_manager::clear(irq_num irqn)
{
    // TODO: Use platform assert when it will be ready
    if (irqn < 0) {
        return -1;
    }

    // TODO: error check
    NVIC_ClearPendingIRQ(static_cast< irq_num > (irqn));
    return 0;
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
