/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C UART definitions template

#ifndef TM4C_UART_DEFINES_
#define TM4C_UART_DEFINES_

#include "aux/platform_defines.hpp"
#include "aux/uart.hpp"


/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

# UART device alias, used internally by bypass console driver
template_console_dev = 'static constexpr uart_channel console_channel = uart_channel::ch%d;'

# Gets string representation of UART theCore enum.
def get_uart_enum(uart_cfg):
    return int(uart_cfg['id'][-1])

]]]*/
//[[[end]]]

namespace ecl
{

// UART configuration ---------------------------------------------------------

/*[[[cog

uart_cfgs = {}

if 'uart' in cfg:
    uart_cfgs = cfg['uart']

for uart_cfg in uart_cfgs:
    # Place comment line if needed
    if 'comment' in uart_cfg:
        # Avoid using begin and end comment section tokens
        cog.outl(('\n%s* ' + uart_cfg['comment'] + ' *%s') % ('/', '/'))

    cog.outl(template_console_dev % get_uart_enum(uart_cfg))
    cog.outl('using platform_console = uart<console_channel>;')

]]]*/
//[[[end]]]


} // namespace ecl

#endif // TM4C_UART_DEFINES_
