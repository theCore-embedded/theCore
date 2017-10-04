/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/thread/semaphore.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/assert.h>

#include <common/irq.hpp>

#include <task.h>

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

bool ecl::semaphore::try_wait(std::chrono::milliseconds ms)
{
    using std::chrono::milliseconds;
    // Keep track of cycles wasted during loop operation
    auto end = xTaskGetTickCount() + ms.count() / portTICK_PERIOD_MS;

    int cnt;

    do {
        if ((cnt = m_cnt.load()) > 0) {
            // Now we almost sure that it is safe to decrement counter - it is
            // positive. However, other thread(s) can decrement before we will.
            // CAS will ensure that counter is the same.
            if (m_cnt.compare_exchange_weak(cnt, cnt - 1)) {
                // We've got a semaphore. Cool.
                return true;
            }
        }

        // Either counter is <= 0, which means that someone else blocked and waiting
        // for event, or we failed with decrementing positive counter.
        // To let implementation be simple, we remain graceful and do not
        // wait on binary semaphore. It means that every other thread
        // doing wait() will be able to get semaphore event in the first place.
        // Tradeoff is that we always at the end of a queue.

        // Make sure MCU is not stall with executing this loop.
        taskYIELD();
    } while (xTaskGetTickCount() > end); // Are we wasted all time?

    // We still here, means that counter was not obtained within given time
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
    using std::chrono::milliseconds;
    // TODO: avoid potential overflow here
    auto ticks = ms > milliseconds(0) ? ms.count() / portTICK_PERIOD_MS : 0;

    auto rc = xSemaphoreTake(m_semaphore,
                             std::min(ticks, static_cast<decltype(ticks)>(portMAX_DELAY)));
    return rc == pdTRUE;
}
