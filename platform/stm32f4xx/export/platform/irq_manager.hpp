//!
//! \file
//! \brief stm32f4xx IRQ manager.
//!
#ifndef PLATFORM_IRQ_MANAGER_HPP
#define PLATFORM_IRQ_MANAGER_HPP

#include <stm32f4xx.h>
#include <ecl/err.hpp>

#include <functional>
#include <type_traits>

namespace ecl
{

using irq_num = IRQn_Type;

// Manages irqs
// TODO: singleton obviously
class irq_manager
{
public:
    using handler_type = std::function< void() >;

    // TODO: setup VTOR?
    irq_manager() = delete;
    ~irq_manager() = delete;

    //! Initializes IRQ manager and setups default handler for every IRQ.
    static void init();

    //!
    //! Subscribes to the given IRQ.
    //! \param[in] irqn     Valid IRQ number.
    //! \param[in] handler  New IRQ handler for given IRQ.
    //! \retval 0 Success.
    //!
    static err subscribe(irq_num irqn, const handler_type &handler);

    //!
    //! Unsubscribes from the given IRQ.
    //! \details Default handler will be used for given IRQ if this call succeed.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static err unsubscribe(irq_num irqn);

    //!
    //! Masks or disables the given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static err mask(irq_num irqn);

    //!
    //! Unmasks or enables the given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static err unmask(irq_num irqn);

    //! Checks if a processor is in handler mode of execution at this time.
    //! \retval true Processor is in handler mode. I.e. servicing IRQ or exception.
    //! \retval false Processor is in thread mode.
    static bool in_isr();

    //! Disables interrupts globally.
    static void disable();

    //! Enables interrupts globally.
    static void enable();

    //!
    //! Clears pending interrupt of the given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static err clear(irq_num irqn);

protected:
    //! Handles IRQ
    static void isr();

private:
    using handler_storage =
    std::aligned_storage< sizeof(handler_type), alignof(handler_type) >::type;

    //! Default iRQ handler. Terminates execution if called.
    static void default_handler();

    //! Storage for registered IRQ handlers.
    static handler_storage m_storage[CONFIG_IRQ_COUNT];

    static constexpr auto extract_handlers()
    {
        return reinterpret_cast< handler_type* >(&m_storage);
    }
};

}

#endif
