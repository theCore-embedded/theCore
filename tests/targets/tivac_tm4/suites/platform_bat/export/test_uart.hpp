/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Test UART for platform BAT test
#ifndef TIVAC_PLATFORM_BAT_TEST_UART_HPP_
#define TIVAC_PLATFORM_BAT_TEST_UART_HPP_

#include <aux/uart_bus.hpp>

namespace ecl
{

using test_uart = uart_bus<uart_device::dev0>;

}

#endif // TIVAC_PLATFORM_BAT_TEST_UART_HPP_
