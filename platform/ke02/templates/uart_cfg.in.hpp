/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief KE02 UART definitions template

#ifndef KE02_UART_DEFINES_
#define KE02_UART_DEFINES_

#include "aux/platform_defines.hpp"
#include "aux/uart.hpp"


/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

# UART device alias, used internally by bypass console driver
template_console_dev = '''static constexpr uart_channel UART{CHANNEL_INDEX}_channel = uart_channel::ch{CHANNEL_INDEX};
using UART{CHANNEL_INDEX}_driver = uart<UART{CHANNEL_INDEX}_channel>;'''

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

    cog.outl(template_console_dev.format(CHANNEL_INDEX = get_uart_enum(uart_cfg)))

    if 'alias' in uart_cfg:
        cog.outl('using {ALIAS_NAME} = UART{CHANNEL_INDEX}_driver;'.format(ALIAS_NAME = uart_cfg['alias'],
                                                                           CHANNEL_INDEX = get_uart_enum(uart_cfg)))

    if 'console' in cfg and cfg['console'] == uart_cfg['id']:
        cog.outl('\n/' + '* Console configuration *' + '/\n')
        cog.outl('static constexpr auto console_channel = UART{CHANNEL_INDEX}_channel;'.format(CHANNEL_INDEX = get_uart_enum(uart_cfg)))
        cog.outl('using platform_console = {DRIVER_ID}_driver;'.format(DRIVER_ID = cfg['console']))

]]]*/
//[[[end]]]


} // namespace ecl

#endif // KE02_UART_DEFINES_
