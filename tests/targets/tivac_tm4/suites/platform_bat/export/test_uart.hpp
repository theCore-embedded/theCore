//! \file
//! \brief Test UART for gpio_case test
#ifndef TIVAC_PLATFORM_BAT_TEST_UART_HPP_
#define TIVAC_PLATFORM_BAT_TEST_UART_HPP_

#include <aux/uart_bus.hpp>

namespace ecl
{

using test_uart = uart_bus<uart_device::dev0>;

}

#endif // TIVAC_PLATFORM_BAT_TEST_UART_HPP_
