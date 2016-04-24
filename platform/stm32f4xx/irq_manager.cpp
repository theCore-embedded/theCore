#include <platform/irq_manager.hpp>
#include <new>

IRQ_manager::handler_storage IRQ_manager::m_storage[IRQ_manager::irqs];


void IRQ_manager::init()
{
    // Initialize storage
    for (auto &h : m_storage) {
        new (&h) handler_type{default_handler};
    }

    __enable_irq();
}

int IRQ_manager::subscribe(IRQn_t irqn, const std::function< void() > &handler)
{
    // TODO: error check
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

int IRQ_manager::unsubscribe(IRQn_t irqn)
{
    auto handlers = extract_handlers();
    __disable_irq();
    handlers[irqn] = default_handler;
    __enable_irq();
    return 0;
}

int IRQ_manager::mask(IRQn_t irqn)
{
    // TODO: error check
    NVIC_DisableIRQ(irqn);
    return 0;
}

int IRQ_manager::unmask(IRQn_t irqn)
{
    // TODO: error check
    NVIC_EnableIRQ(irqn);
    return 0;
}

int IRQ_manager::clear(IRQn_t irqn)
{
    // TODO: error check
    NVIC_ClearPendingIRQ(static_cast< IRQn_t > (irqn));
    return 0;
}

//------------------------------------------------------------------------------

void IRQ_manager::isr()
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
    mask(static_cast< IRQn_t >(irqn));
    handlers[irqn]();
}

void IRQ_manager::default_handler()
{
    __disable_irq();
    for(;;);
}
