#include "platform/irq_manager.hpp"
#include "platform/irq.hpp"
#include "platform/execution.h"
#include "misc.h"

#include <ecl/assert.h>
#include <new>

namespace ecl
{

using handler_storage =
std::aligned_storage_t<sizeof(irq::handler_type), alignof(irq::handler_type)>;

static handler_storage storage[CONFIG_IRQ_COUNT];

//! Unhandled interrupt must cause the abort
static void default_handler()
{
    ecl_abort();
}

//! Effectively casts IRQ storage
static constexpr auto extract_handlers()
{
    return reinterpret_cast<irq::handler_type *>(&storage);
}

//! Handles IRQ and visible to startup code
extern "C" __attribute__ ((used))
void core_isr()
{
    volatile int irqn = irq::get_current_irqn();
    auto handlers = extract_handlers();

    // TODO: Is it needed?
    irq::mask(static_cast< irq_num >(irqn));
    handlers[irqn]();
}

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

    auto handlers = extract_handlers();

    irq::disable();
    handlers[irqn] = handler;
    irq::enable();
}

void unsubscribe(irq_num irqn)
{
    ecl_assert(static_cast<int>(irqn) >= 0);

    auto handlers = extract_handlers();
    irq::disable();
    handlers[irqn] = default_handler;
    irq::enable();
}

} // namespace irq

} // namespace ecl
