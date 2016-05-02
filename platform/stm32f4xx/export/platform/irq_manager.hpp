//!
//! \file
//! \brief stm32f4xx IRQ manager.
//!
#ifndef PLATFORM_IRQ_MANAGER_HPP
#define PLATFORM_IRQ_MANAGER_HPP

#include <stm32f4xx.h>
#include <core_cm4.h>

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

    //! Initializes IRQ manager and setups default handlers for every IRQ.
    static void init();

    //!
    //! \brief Subscribes to given IRQ.
    //! \param[in] irqn     Valid IRQ number.
    //! \param[in] handler  New IRQ handler for given IRQ.
    //! \retval 0 Success.
    //!
    static int subscribe(irq_num irqn, const handler_type &handler);

    //!
    //! \brief Unsubscribes from given IRQ.
    //! \detail Default handler will be used for given IRQ if this call succeed.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int unsubscribe(irq_num irqn);

    //!
    //! \brief Masks or disables given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int mask(irq_num irqn);

    //!
    //! \brief Unmasks or enables given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int unmask(irq_num irqn);

    //!
    //! \brief Clears pending interrupt of given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int clear(irq_num irqn);

private:
    using handler_storage =
    std::aligned_storage< sizeof(handler_type), alignof(handler_type) >::type;

    // Prevent optimizing out an ISR routine
    __attribute__ ((used)) void isr();

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
