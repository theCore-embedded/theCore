#ifndef STM32F4XX_PLATFORM_UTILS_HPP_
#define STM32F4XX_PLATFORM_UTILS_HPP_

namespace ecl
{

//!
//! \brief Checks if current execution context is interrupt routine
//! \return true if execution happens in interrupt
//!
bool in_isr();

//!
//! \brief Disables interrupt.
//!
void disable_interrupts();

//!
//! \brief Enables interrupt.
//!
void enable_interrupts();

}

#endif // STM32_F4XX_PLATFORM_UTILS_HPP_
