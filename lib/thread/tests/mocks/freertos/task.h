/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FreeRTOS task mock header
//! \details This mock is rather dead. Definitions below are used in try_wait()
//! FreeRTOS calls, which is not tested on host at this moment.
#ifndef FREERTOS_TASK_MOCK_H_
#define FREERTOS_TASK_MOCK_H_

#include <thread>

extern volatile uint32_t mock_time;

static inline int xTaskGetTickCount()
{
    return mock_time;
}

static inline void taskYIELD()
{
    std::this_thread::yield();
}

#endif // FREERTOS_TASK_MOCK_H_
