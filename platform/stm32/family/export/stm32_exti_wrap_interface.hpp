//!
//! \file
//! \brief Interface to EXTI STM32 wrappers.

#ifndef STM32_EXTI_WRAP_INTERFACE_HPP_
#define STM32_EXTI_WRAP_INTERFACE_HPP_

namespace ecl
{

//! Checks if given EXTI line is direct.
//! \tparam SplExti EXTI line in the SPL representation.
//! \return true if EXTI is direct and false if otherwise.
template<uint32_t SplExti>
constexpr auto direct_exti();

//! Checks if given EXTI line is grouped.
//! \tparam SplExti EXTI line in the SPL representation.
//! \return true if EXTI is grouped and false if otherwise.
template<uint32_t SplExti>
constexpr auto grouped_exti();

//! Gets EXTI index suitable for arranging EXTI handlers in an array.
//! \tparam SplExti EXTI line in the SPL representation.
//! \details Indexing is different for grouped and direct EXTI.
//! \return EXTI index randing from [0 ... CONFIG_ECL_EXTI_DIRECT_COUNT)
//! for direct EXTI and [0 ... CONFIG_ECL_EXTI_GROUPED_COUNT) for grouped EXTI.
template<uint32_t SplExti>
constexpr auto get_exti_idx();

} // namespace ecl

#endif // STM32_EXTI_WRAP_INTERFACE_HPP_
