/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of I2C driver configuration.
//! \ingroup stm32_templates
#ifndef STM32XX_I2C_CFGS_HPP_
#define STM32XX_I2C_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/i2c_bus.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

// I2C configuration -----------------------------------------------------------

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-stm32']

# Mapping between json-friendly and SPL/theCore definitions
i2c_cfg_map = {
    'config-mode' : {
        'IRQ'   : 'i2c_mode::IRQ',
        'POLL'  : 'i2c_mode::POLL'
    },
    'config-duty-cycle' : {
        '2/1'   : 'I2C_DutyCycle_2',
        '16/9'  : 'I2C_DutyCycle_16_9'
    },
    'config-ack' : {
        True    : 'I2C_Ack_Enable',
        False   : 'I2C_Ack_Disable'
    },
    'config-ack-addr-bit' : {
        7       : 'I2C_AcknowledgedAddress_7bit',
        10      : 'I2C_AcknowledgedAddress_10bit'
    }
}

# I2C defaults expressed in JSON-related format
i2c_defaults = {
    'config-mode'          : 'IRQ',
    'config-speed'         : 1000,
    'config-duty-cycle'    : '2/1',
    'config-ack'           : True,
    'config-own-addr'      : 51,
    'config-ack-addr-bit'  : 7,
}

# Extracts configuration parameter from config or return default
def extract_or_default(i2c_cfg, property):
    if property in i2c_cfg:
        value = i2c_cfg[property]
    elif property in i2c_defaults:
        value = i2c_defaults[property]
    else:
        cog.error('No default or defined in config value for i2c property: '
            + str(property))

    if property in i2c_cfg_map:
        if value in i2c_cfg_map[property]:
            return i2c_cfg_map[property][value]
        else:
            cog.error('No mapping for i2c property: ' + str(property)
                    + ' with value: ' + str(value))
    return value

# Template I2C config
template_i2c_cfg = '''
using %s_driver_cfg = i2c_config<
    i2c_device::bus%d,
    %s,
    %d,
    I2C_Mode_I2C,
    %s,
    %d,
    %s,
    %s
>;
'''

# Template I2C driver defintion and alias
template_i2c_drv = 'using %s_driver = i2c_bus<%s_driver_cfg>;'
template_i2c_alias = 'using %s = %s_driver;'

i2c_ids = cfg['menu-i2c']['table-i2c'] if 'menu-i2c' in cfg else []

for i2c_id in i2c_ids:
    i2c_cfg = cfg['menu-i2c']['menu-' + i2c_id]

    if 'config-comment' in i2c_cfg:
        cog.outl('/' + '* ' + i2c_cfg['config-comment'] + ' *' + '/')

    bus_num         = int(i2c_id[-1])
    mode            = extract_or_default(i2c_cfg, 'config-mode')
    speed           = extract_or_default(i2c_cfg, 'config-speed')
    duty_cycle      = extract_or_default(i2c_cfg, 'config-duty-cycle')
    ack             = extract_or_default(i2c_cfg, 'config-ack')
    own_addr        = extract_or_default(i2c_cfg, 'config-own-addr')
    ack_addr_bit    = extract_or_default(i2c_cfg, 'config-ack-addr-bit')

    cog.outl(template_i2c_cfg % (i2c_id, bus_num, mode, int(speed),
        duty_cycle, int(own_addr), ack, ack_addr_bit))

    cog.outl(template_i2c_drv % (i2c_id, i2c_id))
    if 'alias' in i2c_cfg:
        cog.outl(template_i2c_alias % (i2c_cfg['alias'], i2c_id))

]]]*/
//[[[end]]]

//! @}

//! @}

//! @}

} // namespace ecl

#endif // STM32XX_I2C_CFGS_HPP_
