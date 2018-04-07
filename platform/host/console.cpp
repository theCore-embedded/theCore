/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Console class implementation for the host platform

#include "platform/console.hpp"
#include <common/console.hpp>

namespace ecl
{

// Static member declarations

const uint8_t *platform_console::m_tx;
uint8_t *platform_console::m_rx;
size_t platform_console::m_tx_size;
size_t platform_console::m_rx_size;
std::aligned_storage_t<sizeof(typename platform_console::handler_fn),
    alignof(typename platform_console::handler_fn)> platform_console::m_fn_storage;

//! Greeting executor, prints message during static initialization
static struct greeter
{
    greeter() {
        bypass_greeting();
    }
} greeter_obj;

} // namespace ecl
