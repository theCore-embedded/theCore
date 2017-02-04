//! \file
//! \brief Mock IRQ header for FreeRTOS semaphore implementation.

#ifndef FREERTOS_COMMON_IRQ_HPP_
#define FREERTOS_COMMON_IRQ_HPP_

namespace ecl
{

namespace irq
{

bool in_isr()
{
    return false;
}

} // namespace irq

} // namespace ecl

#endif // FREERTOS_COMMON_IRQ_HPP_
