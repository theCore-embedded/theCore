#include <common/irq.hpp>
#include <platform/exti_manager.hpp>
#include <misc.h>
#include <core_cm4.h>
#include <stm32f4xx.h>

#if CONFIG_BYPASS_CONSOLE_ENABLED
extern void bypass_console_init();
#endif // CONFIG_BYPASS_CONSOLE_ENABLED

// TODO: decide if to make as a class member or not
extern "C" __attribute__((used)) void platform_init()
{
    // Required for FreeRTOS
    // TODO: find better place for it
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Magic here.
    // Logical priority of *any* user interrupt that use FreeRTOS API
    // must not be greater than configMAX_SYSCALL_INTERRUPT_PRIORITY
    for (int irqn = WWDG_IRQn; irqn < IRQ_COUNT; ++irqn) {
        NVIC_SetPriority(static_cast<IRQn_Type>(irqn), CONFIG_MAX_ISR_PRIORITY);
    }

    // IRQ must be ready before anything else will start work
    ecl::irq::init_storage();
    ecl::exti_manager::init();

    // Initialize DWT, used by platform to count ticks.
    // See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#if CONFIG_BYPASS_CONSOLE_ENABLED
    bypass_console_init();
#endif // CONFIG_BYPASS_CONSOLE_ENABLED
}
