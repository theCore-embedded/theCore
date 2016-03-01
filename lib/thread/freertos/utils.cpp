#include <os/utils.hpp>

#include <FreeRTOS.h>
#include <task.h>
#include <platform/utils.hpp>

// Thread Control Block, here is just used as an opaque type
typedef struct TCB TCB_t;

// Obtain current TCB from FreeRTOS
extern "C" TCB_t * volatile pxCurrentTCB;



void os::this_thread::yield()
{
    taskYIELD();
}

void os::this_thread::sleep_for(uint32_t msecs)
{
    vTaskDelay(msecs / portTICK_PERIOD_MS);
}

os::thread_handle os::this_thread::get_handle()
{
    TaskHandle_t handle = NULL;

    if (!ecl::in_isr()) {
        ecl::disable_interrupts();
        handle = reinterpret_cast< TaskHandle_t >(pxCurrentTCB);
    }

    return handle;
}
