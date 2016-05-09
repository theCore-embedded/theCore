//! \file
//! \brief External interrupt manager for stm32f4xx platform.
//! \todo Detailed description.
//! \todo Mention about collision on the GPIO EXTI lines, when 2+ buttons
//! generates interrupt on the same line.
#ifndef PLATFORM_EXTI_MANAGER_HPP_
#define PLATFORM_EXTI_MANAGER_HPP_

#include <ecl/list.hpp>
#include <common/pin.hpp>

#include "irq_manager.hpp"

#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

namespace ecl
{

//! External interrupt manager class.
//! \details Allows users to handle and manage external interrupts.
//! \note For now, only GPIO external interrupts are usable.
class exti_manager
{
public:
    //! Useful alias.
    using callback = void(*)(void*);

    //! External interrupt handler.
    //! \details An object of this class is linked into the intrusive list of
    //! EXTI handlers. Client must retain the object and make sure that it
    //! will not be destroyed without deregistering.
    class handler
    {
        friend class exti_manager;
    public:
        ecl::list_node  m_node;       //!< List node of the EXTI handlers list.

        //! Constructs empty handler
        handler();

        //! Destroys handler and unsubscribe it from EXTI events.
        ~handler();

        //! Sets user-defined context.
        void set_ctx(void *ctx);

        //! Sets user-defined callback.
        void set_cb(callback cb);

    private:
        //! Calls this handler.
        void operator ()();

        void            *m_ctx;       //!< User-defined context.
        callback        m_cb;         //!< User-defined callback.
        uint32_t        m_exti_line;  //!< EXTI line that handled by this handler.
    };

public:
    //! EXTI trigger.
    enum class trigger
    {
        rising,         //!< Trigger on rising edge.
        falling,        //!< Trigger on falling edge.
        both,           //!< Trigger on both edges.
    };

    //! Construction isn't allowed
    exti_manager()  = delete;
    //! Destruction isn't allowed
    ~exti_manager() = delete;

    //! Initializes EXTI manager.
    //! \note Requires that IRQ manager will be initialized first.
    //! \sa irq_manager
    static void init();

    //! Subscribes to event on the given GPIO.
    //! \note It is safe to call this method if handler was already subscribed
    //! to an event. In that case, this method will have no effect.
    //! \pre Initialized EXTI manager.
    //! \param[in] h Handler of GPIO event.
    //! \param[in] t Trigger which will produce event.
    template< typename Gpio >
    static void subscribe(handler &h, trigger t);

    //! Unsubscribes handler from any event.
    //! \note It is safe to call this method, even if handler was not
    //! subscribed to an event previously.
    //! \pre Initialized EXTI manager.
    //! \param[in] h Handler, may be previously subscribed.
    static void unsubscribe(handler &h);

    //! Clears pending EXTI request.
    //! \param[in] h Previously subscribed handler.
    static void clear(handler &h);

    //! Masks (disables) pending EXTI request.
    //! \param[in] h Previously subscribed handler.
    static void mask(handler &h);

    //! Unmasks (enables) pending EXTI request.
    //! \param[in] h Previously subscribed handler.
    static void unmask(handler &h);

private:
    //! Type of a grouped EXTI handlers container.
    struct handlers
    {
        list< handler, &handler::m_node > lst; //!< List of handlers.
        ecl::irq_num irqn;                     //!< IRQ request for this EXTI line.
    };

    //! Maps interrupts to EXTI lines.
    struct mapping
    {
        //! Direct event handlers count.
        //! \details Direct are from EXTI0 to EXTI4 - one to one relationship
        //! between EXTI and IRQ
        static constexpr auto direct_cnt   = 5;

        //! Grouped event handlers.
        //! \details Two groups of EXTI:
        //! EXTI5  - EXTI9  resides on EXTI9_5_IRQn,
        //! EXTI10 - EXTI15 resides on EXTI15_10_IRQn,
        static constexpr auto grouped_cnt  = 2;

        //! Direct EXTI lines.
        handler  *direct[direct_cnt]       = {};
        //! Grouped IRQ lines.
        handlers grouped[grouped_cnt]      = {};
    };

    //! Gets EXTI line associated with GPIO.
    template< class Gpio >
    static constexpr auto gpio_to_exti();

    //! Gets direct array index associated with GPIO.
    static constexpr auto exti_to_idx(uint32_t exti);

    //! EXTI IRQ handler, one for all IRQs.
    static void irq_handler();

    //! IRQ to EXTI handlers mapping.
    static mapping m_irq_to_exti;

    //! Handles IRQs from direct EXTI.
    //! \tparam Idx  Index inside direct handlers array.
    //! \tparam Irqn IRQ number.
    template< size_t Idx, irq_num Irqn >
    static void direct_isr()
    {
        auto *h = m_irq_to_exti.direct[Idx];

        if (!h) {
            // TODO Abort: IRQ must not be raised if direct EXTI handler isn't present.
            for(;;);
        }

        mask(*h);
        EXTI_ClearFlag(h->m_exti_line);
        h->operator ()();

        irq_manager::clear(Irqn);
        irq_manager::unmask(Irqn);
    }

    //! Handles IRQs from grouped EXTI.
    //! \tparam idx Index inside grouped handlers array.
    template< size_t Idx, irq_num Irqn >
    static void group_isr()
    {
        auto &handlers = m_irq_to_exti.grouped[Idx];
        for (auto &h : handlers.lst) {
            auto exti = h.m_exti_line;
            // TODO: check EXTI and execute appropriate handler
            if ((EXTI->IMR | exti) && EXTI_GetITStatus(exti) == SET) {
                mask(h);
                EXTI_ClearFlag(h.m_exti_line);
                h();
            }
        }

        irq_manager::clear(Irqn);
        irq_manager::unmask(Irqn);
    }

    //! Checks if direct EXTI line is already used or not.
    //! \tparam Gpio   GPIO for which EXTI line will be configured.
    //! \return true if line is already used, false otherwise.
    template< typename Gpio >
    static std::enable_if_t< gpio_to_exti< Gpio >() < EXTI_Line5, bool >
    exti_used()
    {
        // In direct EXTI, presence of a handler under the corresponding index
        // means that this line is already used.

        constexpr auto exti = gpio_to_exti< Gpio >();
        constexpr auto idx = exti_to_idx(exti);

        return m_irq_to_exti.direct[idx] != nullptr;
    }

    //! Checks if EXTI line is already used or not.
    //! \tparam Gpio GPIO that feeds required EXTI line.
    //! \return true if line is already used, false otherwise.
    template< typename Gpio >
    static std::enable_if_t< gpio_to_exti< Gpio >() >= EXTI_Line5, bool >
    exti_used()
    {
        // In grouped EXTIs a handler with the same exti line reside somewhere
        // in the list.

        constexpr auto exti = gpio_to_exti< Gpio >();
        constexpr auto idx = exti_to_idx(exti);

        for (auto &h : m_irq_to_exti.grouped[idx].lst) {
            if (h.m_exti_line == exti) {
                return true;
            }
        }

        return false;
    }

    //! Platform-level EXTI configuration routine.
    //! \tparam     Gpio   GPIO for which EXTI line will be configured.
    //! \param[in]  t      Event/IRQ trigger.
    template< typename Gpio >
    static void configure_line(trigger t)
    {
        constexpr auto pinsource
            = static_cast< std::underlying_type< pin::number >::type >(Gpio::pin);
        constexpr auto portsource
            = static_cast< std::underlying_type< pin::port >::type >(Gpio::port);
        constexpr auto exti = gpio_to_exti< Gpio >();

        EXTI_InitTypeDef    exti_init;
        EXTITrigger_TypeDef exti_trigger;

        // Only signle GPIO can act as a source for EXTI line. See RM.

        SYSCFG_EXTILineConfig(portsource, pinsource);

        switch (t) {
            case trigger::falling:
                exti_trigger =  EXTI_Trigger_Falling;
                break;
            case trigger::rising:
                exti_trigger =  EXTI_Trigger_Rising;
                break;
            case trigger::both:
                exti_trigger =  EXTI_Trigger_Rising_Falling;
                break;
        }

        exti_init.EXTI_Line     = exti;
        exti_init.EXTI_Mode     = EXTI_Mode_Interrupt;
        exti_init.EXTI_Trigger  = exti_trigger;
        exti_init.EXTI_LineCmd  = ENABLE;

        EXTI_Init(&exti_init);
    }

    // TODO: rename it to 'tie_handler' to reflect state changes inside handler
    //! Saves direct EXTI handler object.
    //! \tparam     Gpio   GPIO for which EXTI object will be saved.
    //! \param[in]  h      EXTI handler.
    template< typename Gpio >
    static std::enable_if_t< gpio_to_exti< Gpio >() < EXTI_Line5, bool >
    save_handler(handler &h)
    {
        constexpr auto exti = gpio_to_exti< Gpio >();
        constexpr auto idx = exti_to_idx(exti);

        h.m_exti_line = exti;
        m_irq_to_exti.direct[idx] = &h;
    }

    //! Saves grouped EXTI handler object.
    //! \tparam     Gpio   GPIO for which EXTI object will be saved.
    //! \param[in]  h      EXTI handler.
    template< typename Gpio >
    static std::enable_if_t< gpio_to_exti< Gpio >() >= EXTI_Line5, bool >
    save_handler(handler &h)
    {
        constexpr auto exti = gpio_to_exti< Gpio >();
        constexpr auto idx = exti_to_idx(exti);

        h.m_exti_line = exti;
        m_irq_to_exti.grouped[idx].lst.push_back(h);
    }

};

//------------------------------------------------------------------------------

template< class Gpio >
void exti_manager::subscribe(handler &h, trigger t)
{
    __disable_irq();

    if (exti_used< Gpio >()) {
        for(;;); // TODO: abort
    }

    save_handler< Gpio >(h);

    // Do not let EXTI fire unless user explicitely ask for it.
    mask(h);
    configure_line< Gpio >(t);

    __enable_irq();
}

//------------------------------------------------------------------------------

template< class Gpio >
constexpr auto exti_manager::gpio_to_exti()
{
    // Hackish, but fast.
    // TODO: rich comment about it.
    constexpr auto shift =
            static_cast< typename std::underlying_type< pin::number >::type >
            (Gpio::pin);

    return 1 << shift;
}

constexpr auto exti_manager::exti_to_idx(uint32_t exti)
{
    switch (exti) {
    // Direct EXTI index.
    case EXTI_Line0:
        return 0;
    case EXTI_Line1:
        return 1;
    case EXTI_Line2:
        return 2;
    case EXTI_Line3:
        return 3;
    case EXTI_Line4:
        return 4;

    // Grouped EXTI index
    case EXTI_Line5:
    case EXTI_Line6:
    case EXTI_Line7:
    case EXTI_Line8:
    case EXTI_Line9:
       return 0;
    case EXTI_Line10:
    case EXTI_Line11:
    case EXTI_Line12:
    case EXTI_Line13:
    case EXTI_Line14:
    case EXTI_Line15:
       return 1;

    default:
        // TODO: Out of domain
        return 0;
    }
}

} // namespace ecl

#endif // PLATFORM_EXTI_MANAGER_HPP_
