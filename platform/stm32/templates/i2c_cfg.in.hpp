//! \file
//! \brief Header provides template of I2C driver configuration.
#ifndef STM32XX_I2C_CFGS_HPP_
#define STM32XX_I2C_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/i2c_bus.hpp>

namespace ecl
{

// I2C configuration -----------------------------------------------------------

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

# Mapping between json-friendly and SPL/theCore definitions
i2c_cfg_map = {
    'mode' : {
        'IRQ'   : 'i2c_mode::IRQ',
        'POLL'  : 'i2c_mode::POLL'
    },
    'duty_cycle' : {
        '2/1'   : 'I2C_DutyCycle_2',
        '16/9'  : 'I2C_DutyCycle_16_9'
    },
    'ack' : {
        True    : 'I2C_Ack_Enable',
        False   : 'I2C_Ack_Disable'
    },
    'ack_addr_bit' : {
        7       : 'I2C_AcknowledgedAddress_7bit',
        10      : 'I2C_AcknowledgedAddress_10bit'
    }
}

# I2C defaults expressed in JSON-related format
i2c_defaults = {
    'mode'          : 'IRQ',
    'speed'         : 1000,
    'duty_cycle'    : '2/1',
    'ack'           : True,
    'own_addr'      : 51,
    'ack_addr_bit'  : 7,
}

# Extracts configuration parameter from config or return default
def extract_or_default(i2c_cfg, property):
    cog.msg('Extracting I2C property: ' + str(property))
    if property in i2c_cfg:
        value = i2c_cfg[property]
    elif property in i2c_defaults:
        cog.msg('Using default value')
        value = i2c_defaults[property]
    else:
        cog.error('No default or defined in config value for i2c property: '
            + str(property))

    cog.msg('Value extracted: ' + str(value))
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

i2c = {}
if 'i2c' in cfg:
    i2c = cfg['i2c']

for config in i2c:
    if 'comment' in config:
        cog.outl('/' + '* ' + config['comment'] + ' *' + '/')

    id              = config['id']
    bus_num         = int(id[-1])
    mode            = extract_or_default(config, 'mode')
    speed           = extract_or_default(config, 'speed')
    duty_cycle      = extract_or_default(config, 'duty_cycle')
    ack             = extract_or_default(config, 'ack')
    own_addr        = extract_or_default(config, 'own_addr')
    ack_addr_bit    = extract_or_default(config, 'ack_addr_bit')

    cog.outl(template_i2c_cfg % (id, bus_num, mode, speed,
        duty_cycle, own_addr, ack, ack_addr_bit))

    cog.outl(template_i2c_drv % (id, id))
    if 'alias' in config:
        cog.outl(template_i2c_alias % (config['alias'], id))

]]]*/
//[[[end]]]

} // namespace ecl


#endif // STM32XX_I2C_CFGS_HPP_
