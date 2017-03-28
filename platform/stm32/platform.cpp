#include "common/execution.hpp"
#include "platform/exti_manager.hpp"
#include "common/irq.hpp"

// Header from stm32 miscellaneous firmware library functions
// (add-on to CMSIS functions). See SPL.
#include <misc.h>

#if CONFIG_USE_BYPASS_CONSOLE
extern void bypass_console_init();
#endif // CONFIG_USE_BYPASS_CONSOLE


/* Required for STM32 Peripherial library */
extern "C"
void assert_param(int exp)
{
#ifdef NDEBUG
    (void)exp;
#else
    if (!exp) {
        ecl::abort();
    }
#endif
}

extern "C" void platform_init()
{
    // Required for FreeRTOS
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Magic here.
    // Logical priority of *any* user interrupt that use FreeRTOS API
    // must not be greater than configMAX_SYSCALL_INTERRUPT_PRIORITY
    for (int irqn = WWDG_IRQn; irqn < IRQ_COUNT; ++irqn) {
        NVIC_SetPriority(static_cast<IRQn_Type>(irqn), CONFIG_MAX_ISR_PRIORITY);
    }

    // IRQ must be ready before anything else will start work
    ecl::irq::init_storage();

    // EXTI manager depends on IRQ and must be initialized after.
    ecl::exti_manager::init();

    // Update clocks so it is visible to the rest of the system
    SystemCoreClockUpdate();

    // Initialize DWT, used by platform to count ticks.
    // See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#if CONFIG_USE_BYPASS_CONSOLE
    bypass_console_init();
#endif // CONFIG_USE_BYPASS_CONSOLE
}

