/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Common IRQ implementation

#include "common/irq.hpp"

#if !defined THECORE_NO_IRQ_MANAGER || THECORE_NO_IRQ_MANAGER == 0


#include <ecl/assert.h>
#include <platform/irq.hpp>
#include <common/execution.hpp>

#include <new>

namespace ecl
{

//! Storage type for the IRQ handlers
using handler_storage =
std::aligned_storage_t<sizeof(irq::handler_type), alignof(irq::handler_type)>;

//! IRQ storage itself
static handler_storage storage[IRQ_COUNT];

//! Unhandled interrupt must cause the abort
static void default_handler()
{
    ecl::abort();
}

//! Effectively casts IRQ storage
static auto extract_handlers()
{
    return reinterpret_cast<irq::handler_type *>(&storage);
}

//------------------------------------------------------------------------------

//! Handles IRQ and visible to the startup code
extern "C" __attribute__ ((used))
void core_isr()
{
    volatile int irqn = irq::get_current_irqn();
    auto handlers = extract_handlers();

    // TODO: Is it needed?
    irq::mask(static_cast< irq_num >(irqn));
    handlers[irqn]();
}

//------------------------------------------------------------------------------

namespace irq
{

void init_storage()
{
    // Initialize storage
    for (auto &h : storage) {
        new(&h) irq::handler_type{default_handler};
    }

    irq::enable();
}

void subscribe(irq_num irqn, const handler_type &handler)
{
    ecl_assert(static_cast<int>(irqn) >= 0);
    ecl_assert(static_cast<int>(irqn) < IRQ_COUNT);

    auto handlers = extract_handlers();

    irq::disable();
    handlers[irqn] = handler;
    irq::enable();
}

void unsubscribe(irq_num irqn)
{
    ecl_assert(static_cast<int>(irqn) >= 0);
    ecl_assert(static_cast<int>(irqn) < IRQ_COUNT);

    auto handlers = extract_handlers();
    irq::disable();
    handlers[irqn] = default_handler;
    irq::enable();
}

} // namespace irq

} // namespace ecl

#endif // !defined THECORE_NO_IRQ_MANAGER || THECORE_NO_IRQ_MANAGER == 0
