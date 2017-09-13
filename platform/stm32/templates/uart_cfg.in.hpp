//! \file
//! \brief Header provides template of u(s)art driver configuration
#ifndef STM32XX_UART_CFGS_HPP_
#define STM32XX_UART_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/usart_bus.hpp>

/*[[[cog
import cog
import json
import os

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

# Some default UART configuration.
uart_defaults = {
    "baud"          : 115200,
    "wordlength"    : "USART_WordLength_8b",
    "stopbits"      : "USART_StopBits_1",
    "parity"        : "USART_Parity_No",
    "mode"          : "USART_Mode_Rx | USART_Mode_Tx",
    "hw_flow"       : "USART_HardwareFlowControl_None"
}

# Gets baud, fallbacks to default value.
def get_baud(uart_cfg):
    if "baud" in uart_cfg:
        return int(uart_cfg["baud"])
    else:
        return uart_defaults["baud"]

# Gets string representation of UART theCore enum.
def get_uart_enum(uart_cfg):
    num = uart_cfg['id'][-1]
    return "usart_device::dev" + str(num)

# Gets UART theCore driver instance name
# I.e. "USART3" -> "USART3_driver"
def get_inst_name(uart_cfg):
    return uart_cfg['id'] + '_driver'

# Gets UART theCore config instance name
# I.e. "USART3" -> "USART3_cfg"
def get_cfg_inst_name(uart_cfg):
    return uart_cfg['id'] + '_cfg'

# UART configuration struct.
template_uart_cfg = '''
template<>
struct usart_cfg<%s>
{
    static auto constexpr baudrate  = %d;
    static auto constexpr word_len  = USART_WordLength_8b;
    static auto constexpr stop_bit  = USART_StopBits_1;
    static auto constexpr parity    = USART_Parity_No;
    static auto constexpr mode      = USART_Mode_Rx | USART_Mode_Tx;
    static auto constexpr hw_flow   = USART_HardwareFlowControl_None;
};

using %s = usart_cfg<%s>;'''

# UART instance.
template_uart_instance = 'using %s = usart_bus<%s>;'

# UART name alias, if user requested
template_uart_alias = 'using %s = %s;'

# UART console name alias, used by theCore
template_console = 'using platform_console = %s;'

# UART bypass console name alias, used by theCore
template_bypass = '''
using bypass_console_bus = %s;
using bypass_config      = %s;
'''

]]]*/
//[[[end]]]

namespace ecl
{

// UART configuration ---------------------------------------------------------

/*[[[cog


# Will store console output lines (if needed)
console_str = ''

# Will store bypass console output lines (if needed)
bypass_console_str = ''

uart_cfgs = {}

if 'uart' in cfg:
    uart_cfgs = cfg['uart']

for uart_cfg in uart_cfgs:
    # Place comment line if needed
    if 'comment' in uart_cfg:
        # Avoid using begin and end comment section tokens
        cog.outl(('\n%s* ' + uart_cfg['comment'] + ' *%s') % ('/', '/'))

    # Config
    cog.outl(template_uart_cfg % (
        get_uart_enum(uart_cfg), get_baud(uart_cfg),
        get_cfg_inst_name(uart_cfg), get_uart_enum(uart_cfg)
        ))

    # Driver instance
    cog.outl(template_uart_instance % (get_inst_name(uart_cfg), get_uart_enum(uart_cfg)))
    # User-supplied alias
    if 'alias' in uart_cfg:
        cog.outl(template_uart_alias % (uart_cfg['alias'], get_inst_name(uart_cfg)))

    # Check if console need to be enabled for this device.
    if ('console' in cfg) and (cfg['console'] == uart_cfg['id']):
        console_str = template_console % get_inst_name(uart_cfg)

    if 'bypass_console' in cfg and cfg['bypass_console'] == uart_cfg['id']:
        bypass_console_str = template_bypass % (get_inst_name(uart_cfg),
                                                get_cfg_inst_name(uart_cfg))


]]]*/
//[[[end]]]

// Console configuration -------------------------------------------------------

/*[[[cog

if console_str:
    cog.outl(console_str)

]]]*/
//[[[end]]]

// Bypass console configuration ------------------------------------------------

/*[[[cog

if bypass_console_str:
    cog.outl(bypass_console_str)
    # Implementation is provided by the console.cpp module.
    cog.outl('void bypass_putc(char c);')
else:
    # Place stub here, if bypass is not needed
    cog.outl('static inline void bypass_putc(char c) { (void)c; }')

]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32XX_UART_CFGS_HPP_
