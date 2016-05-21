#include <platform/exti_manager.hpp>
#include <platform/irq_manager.hpp>
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

#include <common/pin.hpp>

#include <utility>

namespace ecl
{

exti_manager::mapping exti_manager::m_irq_to_exti;

void exti_manager::init()
{
    // TODO: avoid static init fiasco of m_irq_to_exti

    // Maps IRQ into appropriate handler call
    static constexpr std::pair< size_t, irq_num > irq_mapping[] =
    {
        // Direct IRQs
        { 0, EXTI0_IRQn     },
        { 1, EXTI1_IRQn     },
        { 2, EXTI2_IRQn     },
        { 3, EXTI3_IRQn     },
        { 4, EXTI4_IRQn     },

        // Grouped EXTI IRQs
        { 0, EXTI9_5_IRQn   },
        { 1, EXTI15_10_IRQn },
    };

    for (auto &p : irq_mapping)
    {
        irq_manager::subscribe(p.second, [&p]{
            p.second > EXTI4_IRQn
                    ? group_isr(p.first, p.second)
                    : direct_isr(p.first, p.second);
        });

        // Interrupts will be always on, at least at this stage of development
        irq_manager::unmask(p.second);
    }

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

void exti_manager::direct_isr(size_t idx, irq_num irqn)
{
    auto *h = m_irq_to_exti.direct[idx];

    if (!h) {
        // TODO Abort: IRQ must not be raised if direct EXTI handler isn't present.
        for(;;);
    }

    mask(*h);
    EXTI_ClearFlag(h->m_exti_line);
    h->operator ()();

    irq_manager::clear(irqn);
    irq_manager::unmask(irqn);
}

void exti_manager::group_isr(size_t idx, irq_num irqn)
{
    auto &handlers = m_irq_to_exti.grouped[idx];
    for (auto &h : handlers.lst) {
        auto exti = h.m_exti_line;
        // TODO: check EXTI and execute appropriate handler
        if ((EXTI->IMR | exti) && EXTI_GetITStatus(exti) == SET) {
            mask(h);
            EXTI_ClearFlag(h.m_exti_line);
            h();
        }
    }

    irq_manager::clear(irqn);
    irq_manager::unmask(irqn);
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
