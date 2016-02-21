#include <ecl/thread/semaphore.hpp>
#include <ecl/assert.h>

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
    while (xSemaphoreGive(m_semaphore) != pdTRUE){
        portYIELD();
    }
}

void ecl::semaphore::wait()
{
    auto rc = xSemaphoreTake(m_semaphore, portMAX_DELAY);
    ecl_assert(rc == pdTRUE);
}
