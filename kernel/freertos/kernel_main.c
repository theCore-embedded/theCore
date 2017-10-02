/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FreeRTOS main module
#include <FreeRTOS.h>
#include <task.h>

#include <ecl/assert.h>

extern int early_main();

// Somehow linker drops this function if LTO is enabled
__attribute__((used))
void vTaskSwitchContext( void );

// TODO: find better place for these handlers?
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
    (void) xTask;
    (void) pcTaskName;
    //ecl::cout << "Stack is overflowed by: " << (char *)pcTaskName << ecl::endl;
    for(;;);
}

void vAssertCalled(const char *file, int line)
{
#ifndef NDEBUG
    ecl_assert_failed("",
                      "<see FreeRTOS sources>\n",
                      file,
                      NULL,
                      line);
#else
    (void)file;
    (void)line;
#endif
}

// Required since main prototype is not suitable for xTaskCreate()
void freertos_main_runner(void *arg)
{
    (void) arg;
    early_main();
}

void kernel_main()
{
    int ret = xTaskCreate(freertos_main_runner,
                          "main",
                          MAIN_STACK_SIZE / sizeof(StackType_t),
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL);

    if (ret == pdPASS) {
        vTaskStartScheduler();
    }

    // Shouldn't get here TODO: improve error handling
    for(;;);
}
