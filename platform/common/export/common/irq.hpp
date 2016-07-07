//! \file
//! \brief IRQ common module.
//!
#ifndef COMMON_IRQ_HPP
#define COMMON_IRQ_HPP

#include <platform/irq.hpp>
#include <ecl/err.hpp>

#include <functional>
#include <type_traits>

namespace ecl
{

namespace irq
{

using handler_type = std::function<void()>;

//! Initializes storage for callbacks and setups default handler for every IRQ.
void init_storage();

//! Subscribes to the given IRQ.
//! \param[in] irqn     Valid IRQ number.
//! \param[in] handler  New IRQ handler for given IRQ.
//!
void subscribe(irq_num irqn, const irq::handler_type &handler);

//! Unsubscribes from the given IRQ.
//! \details Default handler will be used for given IRQ if this call succeed.
//! \param[in] irqn Valid IRQ number.
//!
void unsubscribe(irq_num irqn);

} // namespace ecl

} // namespace irq

#endif // COMMON_IRQ_HPP
