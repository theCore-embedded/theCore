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
