//! \file
//! \brief Test UART for uart_case test
#ifndef PARTICLE_PLATFORM_BAT_TEST_UART_HPP_
#define PARTICLE_PLATFORM_BAT_TEST_UART_HPP_

#include <aux/uart_bus.hpp>

namespace ecl
{

using test_uart = uart_bus<uart_device::serial0>;

} // namespace ecl

#endif // PARTICLE_PLATFORM_BAT_TEST_UART_HPP_
