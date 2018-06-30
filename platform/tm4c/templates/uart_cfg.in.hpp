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
cfg = cfg['menu-platform']['menu-tm4c']

# UART device alias, used internally by bypass console driver
template_uart_dev = '''
static constexpr uart_channel UART{CHANNEL_INDEX}_channel = uart_channel::ch{CHANNEL_INDEX};
using UART{CHANNEL_INDEX}_driver = uart<UART{CHANNEL_INDEX}_channel>;
'''

# Gets string representation of UART theCore enum.
def get_uart_enum(uart_cfg):
    return int(uart_name[-1])

]]]*/
//[[[end]]]

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_templates Auto-generated code, Python COG and CMake templates.
//! @{

// UART configuration ---------------------------------------------------------

/*[[[cog

uart_names = []

if 'menu-uart' in cfg:
    uart_names = cfg['menu-uart']['table-uart']

for uart_name in uart_names:
    # Place comment line if needed
    uart_cfg = cfg['menu-uart']['menu-' + uart_name]
    if 'config-comment' in uart_cfg:
        # Avoid using begin and end comment section tokens
        cog.outl(('\n%s* ' + uart_cfg['config-comment'] + ' *%s') % ('/', '/'))

    cog.outl(template_uart_dev.format(CHANNEL_INDEX = get_uart_enum(uart_name)))

    if 'config-alias' in uart_cfg:
        cog.outl('using {ALIAS_NAME} = UART{CHANNEL_INDEX}_driver;'.format(ALIAS_NAME = uart_cfg['config-alias'],
                                                                           CHANNEL_INDEX = get_uart_enum(uart_name)))

    if 'config-console' in cfg and cfg['config-console'] == uart_name:
        cog.outl('\n/' + '* Console configuration *' + '/\n')
        cog.outl('static constexpr auto console_channel = UART{CHANNEL_INDEX}_channel;'.format(CHANNEL_INDEX = get_uart_enum(uart_name)))
        cog.outl('using platform_console = {DRIVER_ID}_driver;'.format(DRIVER_ID = cfg['config-console']))


]]]*/
//[[[end]]]

//! @}

//! @}

//! @}

} // namespace ecl

#endif // TM4C_UART_DEFINES_
