#ifndef PLATFORM_IRQ_MANAGER_HPP
#define PLATFORM_IRQ_MANAGER_HPP

#include <stm32f4xx.h>
#include <core_cm4.h>

#include <functional>

using IRQn_t = IRQn_Type;

// Manages irqs
// TODO: singleton obviously
class IRQ_manager
{
public:
    // TODO: setup VTOR?
    IRQ_manager() = delete;
    ~IRQ_manager() = delete;

    static void init()
    {
        for (auto &h : m_handlers) {
            h = default_handler;
        }

        __enable_irq();
    }

    static int subscribe(IRQn_t IRQn, const std::function< void() > &handler)
    {
        // TODO: error check

        __disable_irq();

        // Magic here.
        // Logical priority of *any* user interrupt that use
        // FreeRTOS API must not be greather than
        // configMAX_SYSCALL_INTERRUPT_PRIORITY
        NVIC_SetPriority(IRQn, CONFIG_MAX_ISR_PRIORITY);
        m_handlers[IRQn] = handler;
        __enable_irq();
        return 0;
    }

    static int unsubscribe(IRQn_t IRQn)
    {
        __disable_irq();
        m_handlers[IRQn] = default_handler;
        __enable_irq();
        return 0;
    }

    static int mask(IRQn_t IRQn)
    {
        NVIC_DisableIRQ(IRQn);
        // TODO
        return 0;
    }

    static int unmask(IRQn_t IRQn)
    {
        NVIC_EnableIRQ(IRQn);
        // TODO
        return 0;
    }

    static int clear(IRQn_t IRQn)
    {
        NVIC_ClearPendingIRQ(static_cast< IRQn_t > (IRQn));
        return 0;
    }

private:
    // Prevent optimizing out an ISR routine
    __attribute__ ((used)) static void ISR()
    {
        volatile int IRQn;

        asm volatile (
                    "mrs %0, ipsr" : "=r" (IRQn)
                    );

        // IPSR holds exception numbers starting from 0
        // Valid IRQ number starts from -15
        // See Cortex-M4 processors documentation
        // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
        IRQn -= 16;

        // TODO: Is it needed?
        mask(static_cast< IRQn_t >(IRQn));
        m_handlers[IRQn]();
    }

    static void default_handler()
    {
        __disable_irq();
        for(;;);
    }

    // Registered IRQ handlers
    // TODO: magic numbers
    static std::function< void() > m_handlers[82];
};



#endif
