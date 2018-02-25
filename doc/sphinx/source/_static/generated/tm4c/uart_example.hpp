/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C UART definitions template

#ifndef TM4C_UART_DEFINES_
#define TM4C_UART_DEFINES_

#include "aux/platform_defines.hpp"
#include "aux/uart.hpp"



namespace ecl
{

// UART configuration ---------------------------------------------------------


/* UART-over-USB console output */
static constexpr uart_channel template_console_dev = uart_channel::ch0;
using platform_console = uart<template_console_dev>;

/* Example UART */
static constexpr uart_channel template_console_dev = uart_channel::ch1;
using platform_console = uart<template_console_dev>;


} // namespace ecl

#endif // TM4C_UART_DEFINES_
