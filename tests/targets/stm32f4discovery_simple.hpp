//!
//! \file
//! \brief STM32 F4 discovery board without any additional attachments.
//!
#ifndef TEST_STM32F4DISCOVERY_SIMPLE_HPP_
#define TEST_STM32F4DISCOVERY_SIMPLE_HPP_

#include <stm32f4xx_defs.hpp>

namespace ecl
{

namespace test
{

// User button
using usr_btn = gpio_pa0;

} // namespace test

} // namespace ecl


#endif // TEST_STM32F4DISCOVERY_SIMPLE_HPP_
