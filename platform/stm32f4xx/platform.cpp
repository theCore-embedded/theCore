#include <platform/irq_manager.hpp>
#include <platform/exti_manager.hpp>
#include <misc.h>
#include <core_cm4.h>

// TODO: decide if to make as a class member or not
extern "C" __attribute__((used)) void platform_init()
{
    // Required for FreeRTOS
    // TODO: find better place for it
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // IRQ must be ready before anything else will start work
    ecl::irq_manager::init();
    ecl::exti_manager::init();
}
