/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/thread/mutex.hpp>
#include <ecl/assert.h>

ecl::mutex::mutex()
    :m_mutex{xSemaphoreCreateMutex()}
{
    ecl_assert(m_mutex);
}

ecl::mutex::~mutex()
{
    vSemaphoreDelete(m_mutex);
}

void ecl::mutex::lock()
{
    auto rc = xSemaphoreTake(m_mutex, portMAX_DELAY);
    ecl_assert(rc == pdTRUE);
}

void ecl::mutex::unlock()
{
    auto rc = xSemaphoreGive(m_mutex);
    ecl_assert(rc == pdTRUE);

}

bool ecl::mutex::try_lock()
{
    auto rc = xSemaphoreTake(m_mutex, 0);
    return rc == pdTRUE;
}
