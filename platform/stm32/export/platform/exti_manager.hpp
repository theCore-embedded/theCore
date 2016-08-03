//! \file
//! \brief External interrupt manager for STM32 platform.

#ifndef STM32_EXTI_MANAGER_HPP_
#define STM32_EXTI_MANAGER_HPP_

#include <ecl/list.hpp>
#include <common/irq.hpp>
#include <stm32_device.hpp>

#include "gpio_device.hpp"

namespace ecl
{

//! External interrupt manager class.
//! \details Allows users to handle and manage external interrupts.
//! \note For now, only GPIO external interrupts are usable.
class exti_manager
{
public:
    //! Useful alias.
    using callback = void (*)(void *);

    //! External interrupt handler.
    //! \details An object of this class is linked into the intrusive list of
    //! EXTI handlers. Client must retain the object and make sure that it
    //! will not be destroyed without deregistering.
    class handler
    {
        friend class exti_manager;

    public:
        ecl::list_node m_node;       //!< List node of the EXTI handlers list.

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
        void operator()();

        void     *m_ctx;       //!< User-defined context.
        callback m_cb;         //!< User-defined callback.
        uint32_t m_exti_line;  //!< EXTI line that handled by this handler.
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
    exti_manager() = delete;
    //! Destruction isn't allowed
    ~exti_manager() = delete;

    //! Initializes EXTI manager.
    //! \note Requires that IRQ manager will be initialized first.
    //! \sa irq_manager
    static void init();

    //! Subscribes to event on the given GPIO.
    //! \details User should retain handler object in the valid state in order
    //! to receive any events associated with given EXTI. Upon deletion of the
    //! handler object unsubscription will be executed by itself.
    //! \pre Initialized EXTI manager.
    //! \pre Unused handler.
    //! \warn If handler was already subscribed to EXTI events then
    //! behaviour is undefined.
    //! \sa unsubscribe()
    //! \param[out] h Unsubscribed handler of a GPIO event.
    //! \param[in]  t Trigger which will produce event.
    template<typename Gpio>
    static void subscribe(handler &h, trigger t);

    //! Unsubscribes handler from any event.
    //! \pre Initialized EXTI manager.
    //! \param[in] h Handler, must be previously subscribed.
    static void unsubscribe(handler &h);

    //! Masks (disables) pending EXTI request.
    //! \param[in] h Previously subscribed handler.
    static void mask(handler &h);

    //! Unmasks (enables) pending EXTI request.
    //! \param[in] h Previously subscribed handler.
    static void unmask(handler &h);

private:
    //! Type of a grouped EXTI handlers container.
    using handlers = list<handler, &handler::m_node>;

    //! Maps interrupts to EXTI lines.
    struct mapping
    {
// Family must expose direct EXTI count.
#ifndef CONFIG_ECL_EXTI_DIRECT_COUNT
#error "STM32 direct EXTI count is not defined."
#endif

// Family must expose grouped EXTI count.
#ifndef CONFIG_ECL_EXTI_GROUPED_COUNT
#error "STM32 grouped EXTI count is not defined."
#endif
        //! Direct EXTI lines count.
        //! \details Direct are such that have 1-to-1 mapping between the EXTI line
        //! and the IRQ line.
        static constexpr auto direct_cnt = CONFIG_ECL_EXTI_DIRECT_COUNT;

        //! Grouped event handlers.
        //! \details Two groups of EXTI:
        //! EXTI5  - EXTI9  resides on EXTI9_5_IRQn,
        //! EXTI10 - EXTI15 resides on EXTI15_10_IRQn,
        static constexpr auto grouped_cnt = CONFIG_ECL_EXTI_GROUPED_COUNT;

// Not all STM32 families have direct EXTI lines.
#if CONFIG_ECL_EXTI_DIRECT_COUNT > 0
        //! Direct EXTI lines.
        handler  *direct[direct_cnt]  = {};
#endif

// Not all platforms have grouped EXTI lines.
#if CONFIG_ECL_EXTI_GROUPED_COUNT > 0
        //! Grouped IRQ lines.
        handlers grouped[grouped_cnt] = {};
#endif
    };

    //! Gets EXTI line associated with GPIO.
    template<class Gpio>
    static constexpr auto extract_exti();

    //! SFINAE helper for determining EXTI type of the given GPIO
    template<typename Gpio>
    using is_direct_exti = std::enable_if_t<direct_exti<exti_manager::extract_exti<Gpio>()>(), bool>;

    //! SFINAE helper for determining EXTI type of the given GPIO
    template<typename Gpio>
    using is_grouped_exti = std::enable_if_t<grouped_exti<exti_manager::extract_exti<Gpio>()>(), bool>;

    //! Handles IRQs from direct EXTI.
    //! \param[in] idx  Index inside direct handlers array.
    //! \param[in] irqn IRQ number.
    static void direct_isr(size_t idx, irq_num irqn);

    //! Handles IRQs from grouped EXTI.
    //! \param idx Index inside grouped handlers array.
    //! \param[in] irqn IRQ number.
    static void group_isr(size_t idx, irq_num irqn);

    //! Checks if direct EXTI line is already used or not.
    //! \tparam Gpio   GPIO for which EXTI line will be configured.
    //! \return true if line is already used, false otherwise.
    template<typename Gpio>
    static is_direct_exti<Gpio> exti_used();

    //! Checks if grouped EXTI line is already used or not.
    //! \tparam Gpio GPIO that feeds required EXTI line.
    //! \return true if line is already used, false otherwise.
    template<typename Gpio>
    static is_grouped_exti<Gpio> exti_used();

    //! Platform-level EXTI configuration routine.
    //! \tparam     Gpio   GPIO for which EXTI line will be configured.
    //! \param[in]  t      Event/IRQ trigger.
    template<typename Gpio>
    static void configure_line(trigger t);

    // TODO: rename it to 'tie_handler' to reflect state changes inside a handler
    //! Saves direct EXTI handler object.
    //! \tparam     Gpio   GPIO for which EXTI object will be saved.
    //! \param[in]  h      EXTI handler.
    template<typename Gpio>
    static is_direct_exti<Gpio> save_handler(handler &h);

    //! Saves grouped EXTI handler object.
    //! \tparam     Gpio   GPIO for which EXTI object will be saved.
    //! \param[in]  h      EXTI handler.
    template<typename Gpio>
    static is_grouped_exti<Gpio> save_handler(handler &h);

    //! Type of POD storage that holds mapping structure.
    using mapping_storage =
    std::aligned_storage<sizeof(mapping), alignof(mapping)>::type;

    //! Storage holding IRQ to EXTI mapping.
    static mapping_storage m_storage;

    //! Obtains map from storage
    static constexpr auto map();
};

//------------------------------------------------------------------------------

template<class Gpio>
void exti_manager::subscribe(handler &h, trigger t)
{
    __disable_irq();

    if (exti_used<Gpio>()) {
        for (;;) {} // TODO: abort
    }

    save_handler<Gpio>(h);

    // Do not let EXTI fire unless user explicitely ask for it.
    mask(h);
    configure_line<Gpio>(t);

    __enable_irq();
}

//------------------------------------------------------------------------------

template<class Gpio>
constexpr auto exti_manager::extract_exti()
{
    // Hackish, but fast.
    // TODO: rich comment about it.

    constexpr auto pin = static_cast<typename std::underlying_type<gpio_num>::type>(Gpio::pin);
    constexpr auto spl_exti = 1 << pin;

    static_assert(IS_GET_EXTI_LINE(spl_exti), "EXTI line is invalid.");
    return spl_exti;
}

template<typename Gpio>
exti_manager::is_direct_exti<Gpio> exti_manager::exti_used()
{
    // In direct EXTI, presence of a handler under the corresponding index
    // means that this line is already used.

    constexpr auto exti = extract_exti<Gpio>();
    constexpr auto idx  = get_exti_idx<exti>();

    return map()->direct[idx] != nullptr;
}

template<typename Gpio>
exti_manager::is_grouped_exti<Gpio> exti_manager::exti_used()
{
    // In grouped EXTIs a handler with the same exti line reside somewhere
    // in the list.

    constexpr auto exti = extract_exti<Gpio>();
    constexpr auto idx  = get_exti_idx<exti>();

    for (auto &h : map()->grouped[idx]) {
        if (h.m_exti_line == exti) {
            return true;
        }
    }

    return false;
}

template<typename Gpio>
void exti_manager::configure_line(trigger t)
{
    constexpr auto pinsource  = static_cast< std::underlying_type<gpio_num>::type>(Gpio::pin);
    constexpr auto portsource = static_cast< std::underlying_type<gpio_port>::type>(Gpio::port);
    constexpr auto exti       = extract_exti<Gpio>();

    EXTI_InitTypeDef    exti_init;
    EXTITrigger_TypeDef exti_trigger;

    // Only single GPIO can act as a source for EXTI line. See RM.

    SYSCFG_EXTILineConfig(portsource, pinsource);

    switch (t) {
        case trigger::falling:
            exti_trigger = EXTI_Trigger_Falling;
            break;
        case trigger::rising:
            exti_trigger = EXTI_Trigger_Rising;
            break;
        case trigger::both:
            exti_trigger = EXTI_Trigger_Rising_Falling;
            break;
    }

    exti_init.EXTI_Line    = exti;
    exti_init.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = exti_trigger;
    exti_init.EXTI_LineCmd = ENABLE;

    EXTI_Init(&exti_init);
}

template<typename Gpio>
exti_manager::is_direct_exti<Gpio> exti_manager::save_handler(handler &h)
{
    constexpr auto exti = extract_exti<Gpio>();
    constexpr auto idx  = get_exti_idx<exti>();

    h.m_exti_line = exti;
    map()->direct[idx] = &h;
}

template<typename Gpio>
exti_manager::is_grouped_exti<Gpio> exti_manager::save_handler(handler &h)
{
    constexpr auto exti = extract_exti<Gpio>();
    constexpr auto idx  = get_exti_idx<exti>();

    h.m_exti_line = exti;
    map()->grouped[idx].push_back(h);
}

constexpr auto exti_manager::map()
{
    return reinterpret_cast<mapping *>(&m_storage);
}

} // namespace ecl

#endif // STM32_EXTI_MANAGER_HPP_
