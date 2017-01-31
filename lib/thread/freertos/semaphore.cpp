#include <ecl/thread/semaphore.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/assert.h>

#include <common/irq.hpp>

#include <algorithm>

ecl::semaphore::semaphore()
        :m_semaphore{}
        ,m_cnt{0}
{
}

ecl::semaphore::~semaphore()
{
}

void ecl::semaphore::signal()
{
    if (m_cnt.fetch_add(1) < 0) {
        m_semaphore.signal();
    }
}

void ecl::semaphore::wait()
{
    if (m_cnt.fetch_sub(1) <= 0) {
        m_semaphore.wait();
    }
}

bool ecl::semaphore::try_wait()
{
    if (m_cnt.fetch_sub(1) > 0) {
        return true; // Got semaphore.
    }

    // Bring counter back
    ++m_cnt;

    return false;
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
    if (!ecl::irq::in_isr()) {
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

bool ecl::binary_semaphore::try_wait(std::chrono::milliseconds ms)
{
    // TODO: avoid potential overflow here
    auto ticks = portTICK_PERIOD_MS * ms.count();

    auto rc = xSemaphoreTake(m_semaphore,
                             std::min(ticks, static_cast<decltype(ticks)>(portMAX_DELAY)));
    return rc == pdTRUE;
}
