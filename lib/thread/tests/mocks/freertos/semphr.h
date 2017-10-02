/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FreeRTOS semaphore mock header

#ifndef FREERTOS_SEMPHR_MOCK_H_
#define FREERTOS_SEMPHR_MOCK_H_

#include <ecl/assert.h>
#include <condition_variable>
#include <atomic>

// Implementation taken from POSIX semaphores.

class helper_semphr
{
public:
    helper_semphr()
        :m_mutex{}
        ,m_cond{}
        ,m_flag{false}
    {
    }

    void signal()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_flag = true;
        lock.unlock();

        m_cond.notify_one();
    }

    bool try_wait(std::chrono::milliseconds ms)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Already rised
        if (m_flag) {
            m_flag = false;
            return true;
        }

        // Give a chance to raise flag
        auto rc = m_cond.wait_for(lock, ms, [&] { return m_flag.load(); });

        m_flag = false;
        return rc;
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Already rised
        if (m_flag) {
            m_flag = false;
            return;
        }

        // Give a chance to raise flag
        m_cond.wait(lock, [&] { return m_flag.load(); });

        m_flag = false;
    }

private:
    std::mutex                  m_mutex;
    std::condition_variable     m_cond;
    std::atomic_bool            m_flag;
};


using SemaphoreHandle_t = helper_semphr *;

static inline SemaphoreHandle_t xSemaphoreCreateBinary()
{
    return new helper_semphr;
}

static inline void vSemaphoreDelete(SemaphoreHandle_t sem)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    delete sem;
}

static inline void xSemaphoreGive(SemaphoreHandle_t sem)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    sem->signal();
}

static inline void xSemaphoreGiveFromISR(SemaphoreHandle_t sem, int *woken_hp_task)
{
    // Do not care about priority inversion
    (void)woken_hp_task;

    ecl_assert_msg(sem, "Invalid semaphore passed");

    sem->signal();
}

static inline bool xSemaphoreTake(SemaphoreHandle_t sem, int wait_time)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    // Assuming that portTICK_PERIOD_MS == 1, see FreeRTOS.h test header

    return sem->try_wait(std::chrono::milliseconds(wait_time));
}

#endif // FREERTOS_SEMPHR_MOCK_H_
