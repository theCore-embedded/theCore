//! \file
//! \brief FreeRTOSS semaphore mock header

#ifndef FREERTOS_SEMPHR_MOCK_H_
#define FREERTOS_SEMPHR_MOCK_H_

#include <semaphore.h>
#include <ecl/assert.h>
#include <time.h>

using SemaphoreHandle_t = sem_t *;

static inline SemaphoreHandle_t xSemaphoreCreateBinary()
{
    auto *sem = new sem_t;
    if (sem_init(sem, false, 0) < 0) {
        ecl_assert_msg(0, "Cannot init semaphore");
    }

    return sem;
}

static inline void vSemaphoreDelete(SemaphoreHandle_t sem)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    sem_destroy(sem);
    delete sem;
}

static inline void xSemaphoreGive(SemaphoreHandle_t sem)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    sem_post(sem);
}

static inline void xSemaphoreGiveFromISR(SemaphoreHandle_t sem, int *woken_hp_task)
{
    // Do not care about priority inversion
    (void)woken_hp_task;

    ecl_assert_msg(sem, "Invalid semaphore passed");

    sem_post(sem);
}

static inline bool xSemaphoreTake(SemaphoreHandle_t sem, int wait_time)
{
    ecl_assert_msg(sem, "Invalid semaphore passed");

    auto cur_time = time(nullptr);
    timespec deadline = { wait_time / 1000 + cur_time, 0 };

    return !(sem_timedwait(sem, &deadline) < 0);
}

#endif // FREERTOS_SEMPHR_MOCK_H_
