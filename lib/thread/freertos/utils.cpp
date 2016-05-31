#include <ecl/thread/utils.hpp>
#include <platform/irq_manager.hpp>

#include <FreeRTOS.h>
#include <task.h>

// Thread Control Block, here is just used as an opaque type
typedef struct TCB TCB_t;

// Obtain current TCB from FreeRTOS
extern "C" TCB_t * volatile pxCurrentTCB;


void ecl::this_thread::yield()
{
    taskYIELD();
}

void ecl::this_thread::sleep_for(uint32_t msecs)
{
    vTaskDelay(msecs / portTICK_PERIOD_MS);
}

ecl::thread_handle ecl::this_thread::get_handle()
{
    TaskHandle_t handle = NULL;

    if (!ecl::irq_manager::in_isr()) {
        ecl::irq_manager::disable();
        handle = reinterpret_cast< TaskHandle_t >(pxCurrentTCB);
    }

    return handle;
}
