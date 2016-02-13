#include "os.hpp"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>


extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
    (void) xTask;
    (void) pcTaskName;
    //ecl::cout << "Stack is overflowed by: " << (char *)pcTaskName << ecl::endl;
    for(;;);
}

extern "C" void vAssertCalled(const char *file, int line)
{
    (void) file;
    (void) line;
    //ecl::cout << "FreeRTOS assert failed: " << file << ':' << line;
    for(;;);
}

void* semaphore_create()
{
    return xSemaphoreCreateMutex();
}

void semaphore_take(void *sem)
{
    xSemaphoreTake(sem, portMAX_DELAY);
}

void semaphore_give(void *sem)
{
    xSemaphoreGive(sem);
}

void delay(unsigned int msecs)
{
    vTaskDelay(msecs / portTICK_PERIOD_MS);
}

int task_create(void(*routine)(void *), const char *name, int stack_sz)
{
    int ret = xTaskCreate(routine, name, stack_sz, NULL, tskIDLE_PRIORITY, NULL);
    if (ret != pdPASS)
        return -1;

    return 0;
}

void schedule_start()
{
    vTaskStartScheduler();
}
