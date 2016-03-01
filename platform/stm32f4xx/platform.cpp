#include <platform/irq_manager.hpp>
#include <platform/utils.hpp>
#include <misc.h>
#include <core_cm4.h>

// TODO: move it elsewhere
std::function< void() > IRQ_manager::m_handlers[82];

// TODO: decide if to make as a class member or not
extern "C" __attribute__((used)) void platform_init()
{
    // Required for FreeRTOS
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

namespace ecl
{

bool in_isr()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

void disable_irq()
{
    __disable_irq();
}

void enable_irq()
{
    __enable_irq();
}

}
