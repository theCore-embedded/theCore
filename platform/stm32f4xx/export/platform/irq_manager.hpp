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

// TODO: wrap whole module into the namespace
// TODO: convert it to the snake_case

using IRQn_t = IRQn_Type;

// Manages irqs
// TODO: singleton obviously
class IRQ_manager
{
public:
    using handler_type = std::function< void() >;

    // TODO: setup VTOR?
    IRQ_manager() = delete;
    ~IRQ_manager() = delete;

    //! Initializes IRQ manager and setups default handlers for every IRQ.
    static void init();

    //!
    //! \brief Subscribes to given IRQ.
    //! \param[in] irqn     Valid IRQ number.
    //! \param[in] handler  New IRQ handler for given IRQ.
    //! \retval 0 Success.
    //!
    static int subscribe(IRQn_t irqn, const handler_type &handler);

    //!
    //! \brief Unsubscribes from given IRQ.
    //! \detail Default handler will be used for given IRQ if this call succeed.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int unsubscribe(IRQn_t irqn);

    //!
    //! \brief Masks or disables given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int mask(IRQn_t irqn);

    //!
    //! \brief Unmasks or enables given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int unmask(IRQn_t irqn);

    //!
    //! \brief Clears pending interrupt of given IRQ.
    //! \param[in] irqn Valid IRQ number.
    //! \retval 0 Success.
    //!
    static int clear(IRQn_t irqn);

private:
    //! Total IRQ count.
    static constexpr auto irqs = 82;
    using handler_storage =
    std::aligned_storage< sizeof(handler_type), alignof(handler_type) >;

    // Prevent optimizing out an ISR routine
    __attribute__ ((used)) void isr();

    //! Default iRQ handler. Terminates execution if called.
    static void default_handler();

    //! Storage for registered IRQ handlers.
    static handler_storage m_storage[irqs];
    // Useful alias to the handler storage.
    // static handler_type (&m_handlers)[irqs];


    static constexpr auto extract_handlers()
    {
        return reinterpret_cast< handler_type* >(&m_storage);
    }
};


#endif
