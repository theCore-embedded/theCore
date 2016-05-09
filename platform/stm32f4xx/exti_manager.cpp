#include <platform/exti_manager.hpp>
#include <platform/irq_manager.hpp>
#include <common/pin.hpp>

#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

namespace ecl
{

exti_manager::mapping exti_manager::m_irq_to_exti;


void exti_manager::init()
{
    // TODO: static init fiasco of m_irq_to_exti?

    // Direct IRQs
    irq_manager::subscribe(EXTI0_IRQn, direct_isr< 0, EXTI0_IRQn >);
    irq_manager::subscribe(EXTI1_IRQn, direct_isr< 1, EXTI1_IRQn >);
    irq_manager::subscribe(EXTI2_IRQn, direct_isr< 2, EXTI2_IRQn >);
    irq_manager::subscribe(EXTI3_IRQn, direct_isr< 3, EXTI3_IRQn >);
    irq_manager::subscribe(EXTI4_IRQn, direct_isr< 4, EXTI4_IRQn >);

    // Grouped EXTI IRQs
    irq_manager::subscribe(EXTI9_5_IRQn, group_isr< 0, EXTI9_5_IRQn >);
    irq_manager::subscribe(EXTI15_10_IRQn, group_isr< 1, EXTI15_10_IRQn >);

    // Interrupts will be always on, at least at this stage of development
    irq_manager::unmask(EXTI0_IRQn);
    irq_manager::unmask(EXTI1_IRQn);
    irq_manager::unmask(EXTI2_IRQn);
    irq_manager::unmask(EXTI3_IRQn);
    irq_manager::unmask(EXTI4_IRQn);
    irq_manager::unmask(EXTI9_5_IRQn);
    irq_manager::unmask(EXTI15_10_IRQn);

    // Supply clocks for SYSCFG subsystem
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}


void exti_manager::unsubscribe(handler &h)
{
    (void)h;
    __disable_irq();
    // TODO
    __enable_irq();
}

void exti_manager::clear(handler &h)
{
    __disable_irq();
    EXTI_ClearFlag(h.m_exti_line);
    __enable_irq();
}

void exti_manager::mask(handler &h)
{
    __disable_irq();
    // Mask EXTI line
    EXTI->IMR &= ~h.m_exti_line;
    __enable_irq();
}

void exti_manager::unmask(handler &h)
{
    __disable_irq();
    // Unmask EXTI line
    EXTI->IMR |= h.m_exti_line;
    __enable_irq();
}


//------------------------------------------------------------------------------

void exti_manager::irq_handler()
{
    // TODO
}

//------------------------------------------------------------------------------

exti_manager::handler::handler()
    :m_node{}
    ,m_ctx{nullptr}
    ,m_cb{nullptr}
{

}

exti_manager::handler::~handler()
{
    // TODO
    irq_manager::disable();
    m_node.unlink();
    irq_manager::enable();
}

void exti_manager::handler::set_cb(callback cb)
{
    // TODO: reuse not_null policy:
    // https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Ri-nullptr
    m_cb = cb;
}

void exti_manager::handler::set_ctx(void *ctx)
{
    m_ctx = ctx;
}

void exti_manager::handler::operator ()()
{
    if (m_cb) {
        m_cb(m_ctx);
    }
}

}
