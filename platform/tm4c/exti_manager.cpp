/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief External interrupt manager implementation.

#include "platform/exti_manager.hpp"

#include <common/irq.hpp>

#include <utility>

namespace ecl
{

safe_storage<exti_manager::handlers_list> exti_manager::m_storage;

void exti_manager::init()
{
    // Prepare user handlers list.
    m_storage.init();

    // List of GPIO interrupts and their handlers
    static constexpr int port_irqs[] = {
        gpio_hw::get_irqn(gpio_hw::port::a),
        gpio_hw::get_irqn(gpio_hw::port::b),
        gpio_hw::get_irqn(gpio_hw::port::c),
        gpio_hw::get_irqn(gpio_hw::port::d),
        gpio_hw::get_irqn(gpio_hw::port::e),
        gpio_hw::get_irqn(gpio_hw::port::f),
    };

    // Configure IRQ controller.
    for (const auto &port_irq : port_irqs) {
        irq::unmask(port_irq);
    }
}

void exti_manager::unsubscribe(handler &h)
{
    irq::disable();

    // Way to determine if handler was already subscribed.
    ecl_assert(h.m_node.linked());

    h.m_node.unlink();

    auto port = extract_value(h.m_port);
    auto intflag = gpio_hw::get_pin_intflag(h.m_pin);

    // Clear events, if any.
    GPIOIntClear(port, intflag);

    // Disable GPIO interrupts for given pin.
    GPIOIntDisable(port, intflag);
}

void exti_manager::mask(handler &h)
{
    irq::disable();

    // Way to determine if handler was already subscribed.
    ecl_assert(h.m_node.linked());

    auto port = extract_value(h.m_port);
    auto intflag = gpio_hw::get_pin_intflag(h.m_pin);

    GPIOIntDisable(port, intflag);

    irq::enable();
}

void exti_manager::unmask(handler &h)
{
    irq::disable();

    // Way to determine if handler was already subscribed.
    ecl_assert(h.m_node.linked());

    auto port = extract_value(h.m_port);
    auto intflag = gpio_hw::get_pin_intflag(h.m_pin);

    GPIOIntEnable(port, intflag);

    irq::enable();
}

//------------------------------------------------------------------------------

exti_manager::handlers_list &exti_manager::get_handlers()
{
    return m_storage.get();
}

void exti_manager::irq_handler(gpio_hw::port port, uint32_t hw_pins)
{
    auto hw_port = extract_value(port);

    for (auto &h : get_handlers()) {
        auto hw_pin = extract_value(h.m_pin);

        // Check who provoked interrupt
        if (h.m_port == port && (hw_pin | hw_pins)) {
            h();
        }
    }

    // Confirm event handling.
    GPIOIntClear(hw_port, hw_pins);
}


} // namespace ecl

