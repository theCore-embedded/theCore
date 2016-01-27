#include <platform/irq_manager.hpp>
#include <misc.h>


// TODO: move it elsewhere
std::function< void() > IRQ_manager::m_handlers[82];

// TODO: decide if to make as a class member or not
extern "C" __attribute__((used)) void platform_init()
{
    // Required for FreeRTOS
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
