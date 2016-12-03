//!
//! \file
//! \brief EXTI wrapper implementation
//!
#ifndef STM32F4XX_EXTI_WRAP_HPP_
#define STM32F4XX_EXTI_WRAP_HPP_

#include <stm32_exti_wrap_interface.hpp>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>

namespace ecl
{

template<uint32_t SplExti>
constexpr auto direct_exti()
{
    static_assert(IS_GET_EXTI_LINE(SplExti), "Given EXTI line is invalid.");
    return SplExti < EXTI_Line5;
}

template<uint32_t SplExti>
constexpr auto grouped_exti()
{
    return !direct_exti<SplExti>();
}

template<uint32_t SplExti>
constexpr auto get_exti_idx()
{
    static_assert(IS_GET_EXTI_LINE(SplExti), "Given EXTI line is invalid.");

    switch (SplExti) {
        // Direct EXTI index.
        case EXTI_Line0:
            return 0;
        case EXTI_Line1:
            return 1;
        case EXTI_Line2:
            return 2;
        case EXTI_Line3:
            return 3;
        case EXTI_Line4:
            return 4;

        // Grouped EXTI indexes
        case EXTI_Line5:
        case EXTI_Line6:
        case EXTI_Line7:
        case EXTI_Line8:
        case EXTI_Line9:
            return 0;
        case EXTI_Line10:
        case EXTI_Line11:
        case EXTI_Line12:
        case EXTI_Line13:
        case EXTI_Line14:
        case EXTI_Line15:
            return 1;
    }
}

//! Maps direct EXTI IRQs into appropriate EXTI index.
// TODO: avoid static?
static constexpr
std::pair<size_t, IRQn> exti_irq_idx_direct_mapping[] =
{
    {get_exti_idx<EXTI_Line0>(), EXTI0_IRQn},
    {get_exti_idx<EXTI_Line1>(), EXTI1_IRQn},
    {get_exti_idx<EXTI_Line2>(), EXTI2_IRQn},
    {get_exti_idx<EXTI_Line3>(), EXTI3_IRQn},
    {get_exti_idx<EXTI_Line4>(), EXTI4_IRQn},
};

//! Maps grouped EXTI IRQs into appropriate EXTI index.
static constexpr
std::pair<size_t, IRQn> exti_irq_idx_grouped_mapping[] =
{
     {get_exti_idx<EXTI_Line9>(),  EXTI9_5_IRQn},
     {get_exti_idx<EXTI_Line15>(), EXTI15_10_IRQn},
};


} // namespace ecl


#endif // STM32F4XX_EXTI_WRAP_HPP_
