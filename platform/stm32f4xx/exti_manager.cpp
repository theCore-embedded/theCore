#include <platform/exti_manager.hpp>
#include <common/irq.hpp>

#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

#include <utility>

namespace ecl
{

exti_manager::mapping_storage exti_manager::m_storage;

void exti_manager::init()
{
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
        irq::subscribe(p.second, [&p]{
            p.second > EXTI4_IRQn
                    ? group_isr(p.first, p.second)
                    : direct_isr(p.first, p.second);
        });

        // Interrupts will be always on, at least at this stage of development
        irq::unmask(p.second);
    }

    // Supply clocks for SYSCFG subsystem
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Init mapping storage
    new (&m_storage) mapping{};
}


void exti_manager::unsubscribe(handler &h)
{
    __disable_irq();

    // Will not appear in the exti list anymore
    h.m_node.unlink();

    // De-configure line

    EXTI_InitTypeDef exti;

    exti.EXTI_Line     = h.m_exti_line;
    exti.EXTI_Mode     = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger  = EXTI_Trigger_Rising_Falling;
    exti.EXTI_LineCmd  = DISABLE;
    EXTI_Init(&exti);

    // Prevent expose of the old events into the IRQ line
    mask(h);
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
    auto *h = map()->direct[idx];

    if (!h) {
        // TODO Abort: IRQ must not be raised if direct EXTI handler isn't present.
        for(;;);
    }

    mask(*h);
    EXTI_ClearFlag(h->m_exti_line);
    h->operator ()();

    irq::clear(irqn);
    irq::unmask(irqn);
}

void exti_manager::group_isr(size_t idx, irq_num irqn)
{
    auto &handlers = map()->grouped[idx];
    for (auto &h : handlers) {
        auto exti = h.m_exti_line;
        if ((EXTI->IMR | exti) && EXTI_GetITStatus(exti) == SET) {
            mask(h);
            EXTI_ClearFlag(h.m_exti_line);
            h();
        }
    }

    irq::clear(irqn);
    irq::unmask(irqn);
}

//------------------------------------------------------------------------------

exti_manager::handler::handler()
    :m_node{}
    ,m_ctx{nullptr}
    ,m_cb{nullptr}
    ,m_exti_line{0}
{

}

exti_manager::handler::~handler()
{
    // TODO
    irq::disable();
    m_node.unlink();
    irq::enable();
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
