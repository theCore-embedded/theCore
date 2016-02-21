#include <os/utils.hpp>

#include <FreeRTOS.h>
#include <task.h>

void os::this_thread::yield()
{
    taskYIELD();
}

void os::this_thread::sleep_for(uint32_t msecs)
{
    vTaskDelay(msecs / portTICK_PERIOD_MS);
}
