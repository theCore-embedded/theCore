#include <ecl/thread/semaphore.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/assert.h>

#include <platform/irq_manager.hpp>

ecl::semaphore::semaphore()
    :m_semaphore{nullptr}
{
    // TODO: find better way to implement semaphore, rather than via
    // xSemaphoreCreateCounting(). Problem is that xSemaphoreCreateCounting()
    // limits maximum count of a semaphore. See FreeRTOS docs for details.
    m_semaphore = xSemaphoreCreateCounting(10, 0);
    ecl_assert(m_semaphore);
}

ecl::semaphore::~semaphore()
{
    vSemaphoreDelete(m_semaphore);
}

void ecl::semaphore::signal()
{
    // HACK:
    // Give operation fails if there is no more room in FreeRTOS queue.
    // Idea is to postpone a giving until consumer will free some space for us.
    if (!ecl::irq_manager::in_isr()) {
        while (xSemaphoreGive(m_semaphore) != pdTRUE) {
            this_thread::sleep_for(100);
        }
    } else {
        // Can't block an ISR, event will be lost
        xSemaphoreGiveFromISR(m_semaphore, nullptr);
    }
}

void ecl::semaphore::wait()
{
    auto rc = xSemaphoreTake(m_semaphore, portMAX_DELAY);
    ecl_assert(rc == pdTRUE);
}

ecl::err ecl::semaphore::try_wait()
{
    auto rc = xSemaphoreTake(m_semaphore, 0);
    return rc == pdTRUE ? ecl::err::ok : ecl::err::again;
}

//------------------------------------------------------------------------------

ecl::binary_semaphore::binary_semaphore()
    :m_semaphore{nullptr}
{
    m_semaphore = xSemaphoreCreateBinary();
    ecl_assert(m_semaphore);
}

ecl::binary_semaphore::~binary_semaphore()
{
    vSemaphoreDelete(m_semaphore);
}

void ecl::binary_semaphore::signal()
{
    // No need to check that operation fails or not simply because it is
    // a binary semaphore. If signal called multiple times without waiting
    // for a semaphore it is expected that only one event will be delivered to
    // a task when it will call wait()
    if (!ecl::irq_manager::in_isr()) {
        xSemaphoreGive(m_semaphore);
    } else {
        // Don't care about priority inversion thus skipping second argument
        xSemaphoreGiveFromISR(m_semaphore, nullptr);
    }
}

void ecl::binary_semaphore::wait()
{
    auto rc = xSemaphoreTake(m_semaphore, portMAX_DELAY);
    ecl_assert(rc == pdTRUE);
}

ecl::err ecl::binary_semaphore::try_wait()
{
    auto rc = xSemaphoreTake(m_semaphore, 0);
    return rc == pdTRUE ? ecl::err::ok : ecl::err::again;
}
