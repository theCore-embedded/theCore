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

    // IRQ handler type
    using irq_fn = void (*)();

    // List of GPIO interrupts and their handlers
    static constexpr std::pair<irq_num, irq_fn> port_irq_list[] = {
        { gpio_hw::get_irqn(gpio_hw::port::a), &irq_port_handler<gpio_hw::port::a> },
        { gpio_hw::get_irqn(gpio_hw::port::b), &irq_port_handler<gpio_hw::port::b> },
        { gpio_hw::get_irqn(gpio_hw::port::c), &irq_port_handler<gpio_hw::port::c> },
        { gpio_hw::get_irqn(gpio_hw::port::d), &irq_port_handler<gpio_hw::port::d> },
        { gpio_hw::get_irqn(gpio_hw::port::e), &irq_port_handler<gpio_hw::port::e> },
        { gpio_hw::get_irqn(gpio_hw::port::f), &irq_port_handler<gpio_hw::port::f> },
    };

    // Configure IRQ controller.
    for (const auto &port_irq : port_irq_list) {
        irq::subscribe(port_irq.first, port_irq.second);
        irq::unmask(port_irq.first);
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

template<gpio_hw::port Port>
void exti_manager::irq_port_handler()
{
    constexpr auto port = extract_value(Port);
    constexpr auto irqn = gpio_hw::get_irqn(Port);

    // Get pins that provoked the interrupt.
    auto pins = GPIOIntStatus(port, true);

    // Mask GPIO lines to prevent spurious events
    GPIOIntDisable(port, pins);

    irq_handler(Port, pins);

    irq::clear(irqn);
    irq::unmask(irqn);
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

