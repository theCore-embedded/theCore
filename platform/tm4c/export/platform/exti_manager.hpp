/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief External interrupt manager for TM4C platform.
#ifndef TM4C_EXTI_MANAGER_HPP_
#define TM4C_EXTI_MANAGER_HPP_

#include <ecl/list.hpp>
#include <ecl/utils.hpp>
#include <ecl/assert.h>

#include <common/irq.hpp>

#include "platform/gpio_device.hpp"

namespace ecl
{

//! External interrupt manager class.
//! \note Only GPIO interrupts are supported.
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
        ecl::list_node m_node = {};       //!< List node of the EXTI handlers list.

        //! Constructs empty handler
        handler() = default;

        //! Constructs handler with user data.
        inline handler(callback cb, void *ctx) :m_ctx{ctx}, m_cb{cb} { }

        //! Destroys handler and unsubscribe it from EXTI events.
        inline ~handler() { exti_manager::unsubscribe(*this); }

        //! Sets user-defined context.
        inline void set_ctx(void *ctx) { m_ctx = ctx; }

        //! Sets user-defined callback.
        inline void set_cb(callback cb) { m_cb = cb; }

        handler &operator=(handler&) = default;
        handler(const handler&) = default;

    private:
        //! Calls this handler.
        inline void operator()() { ecl_assert(m_cb); m_cb(m_ctx); }

        void            *m_ctx        = nullptr;    //!< User-defined context.
        callback        m_cb          = {};         //!< User-defined callback.
        gpio_hw::port   m_port        = {};         //!< GPIO port for this handler.
        gpio_hw::num    m_pin         = {};         //!< GPIO pin for this handler.
    };

public:
    //! EXTI trigger.
    using trigger = gpio_hw::int_source;

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
    //! To start receving events, they must be additionally unmasked via
    //! unmask() call.
    //! \pre Initialized EXTI manager.
    //! \pre Unused handler.
    //! \warning If handler was already subscribed to EXTI events then
    //! behaviour is undefined.
    //! \post Handler subscribed for events, but events are masked.
    //! \sa unsubscribe()
    //! \sa unmask()
    //! \param[out] h Unsubscribed handler of a GPIO event.
    //! \param[in]  t Trigger which will produce event.
    template<typename Gpio>
    static void subscribe(handler &h, trigger t);

    //! Unsubscribes handler from any event.
    //! \pre Initialized EXTI manager.
    //! \pre Previously subscribed handler.
    //! \post Unsubscribed handler.
    //! \details It is possible to use the same handler for subscribe() call
    //! if it was successfully unsubscribed().
    //! \param[in] h Handler, must be previously subscribed.
    static void unsubscribe(handler &h);

    //! Masks (disables) pending EXTI request.
    //! \pre Subscribed handler.
    //! \pre Initialized EXTI manager.
    //! \param[in] h Previously subscribed handler.
    static void mask(handler &h);

    //! Unmasks (enables) pending EXTI request.
    //! \pre Subscribed handler.
    //! \pre Initialized EXTI manager.
    //! \param[in] h Previously subscribed handler.
    static void unmask(handler &h);

private:
    //! EXTI interrupt handlers list type.
    using handlers_list = list<handler, &handler::m_node>;

    //! EXTI interrupt handlers list storage.
    static safe_storage<handlers_list> m_storage;

    //! Gets handler list storage.
    static handlers_list &get_handlers();

    //! Handles event from given port.
    //! \tparam Port Port where interrupt has occur.
    template<gpio_hw::port Port>
    static void irq_port_handler();

    //! Handles GPIO IRQ provoked by pins in the given port.
    //! \param[in] port    GPIO port where interrupt occur.
    //! \param[in] hw_pins Bitstring representing what pins provoked interrupt.
    static void irq_handler(gpio_hw::port port, uint32_t hw_pins);
};

//------------------------------------------------------------------------------

template<typename Gpio>
void exti_manager::subscribe(exti_manager::handler &h, exti_manager::trigger t)
{
    // Way to determine if handler was not subscribed.
    ecl_assert(!h.m_node.linked());

    constexpr auto port     = extract_value(Gpio::port);
    constexpr auto pin      = extract_value(Gpio::pin);
    constexpr auto intflag  = gpio_hw::get_pin_intflag(Gpio::pin);
    auto inttype            = extract_value(t);

    // TODO: improve performance by reducing critical section.
    irq::disable();

    // Configure trigger
    GPIOIntTypeSet(port, pin, inttype);

    // Clear events, if any.
    GPIOIntClear(port, intflag);

    // Do not enable interrupts until unmask() call.
    GPIOIntDisable(port, intflag);

    // Save handler's data.
    h.m_port = Gpio::port;
    h.m_pin = Gpio::pin;

    // Store handler in the list.
    get_handlers().push_back(h);

    irq::enable();
}

} // namespace ecl

#endif // TM4C_EXTI_MANAGER_HPP_
