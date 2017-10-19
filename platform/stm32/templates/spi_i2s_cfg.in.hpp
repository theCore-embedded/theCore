/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of SPI and I2S drivers configuration.
#ifndef STM32XX_SPI_I2S_CFGS_HPP_
#define STM32XX_SPI_I2S_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/spi_bus.hpp>

namespace ecl
{

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

]]]*/
//[[[end]]]

// SPI configuration -----------------------------------------------------------

// TODO

// I2S configuration -----------------------------------------------------------

/*[[[cog

# Maps I2S config values to SPL definitions
i2s_mapping = {
    'standard'  : {
        'phillips'      : 'I2S_Standard_Phillips',
        'MSB'           : 'I2S_Standard_MSB',
        'LSB'           : 'I2S_Standard_LSB',
        'PCMShort'      : 'I2S_Standard_PCMShort',
        'PCMLong'       : 'I2S_Standard_PCMLong'
    },
    'data_bits' : {
        16              : 'I2S_DataFormat_16b',
        24              : 'I2S_DataFormat_24b',
        32              : 'I2S_DataFormat_32b',
        '16ext'         : 'I2S_DataFormat_16bextended'
    },
    'master_clk': {
        True            : 'I2S_MCLKOutput_Enable',
        False           : 'I2S_MCLKOutput_Disable'
    },
    'audio_freq': {
        8               : 'I2S_AudioFreq_8k',
        11              : 'I2S_AudioFreq_11k',
        16              : 'I2S_AudioFreq_16k',
        22              : 'I2S_AudioFreq_22k',
        32              : 'I2S_AudioFreq_32k',
        44              : 'I2S_AudioFreq_44k',
        48              : 'I2S_AudioFreq_48k',
        96              : 'I2S_AudioFreq_96k',
        192             : 'I2S_AudioFreq_192k'
    },
    'CPOL'      : {
        'high'          : 'I2S_CPOL_High'
    }
}

# Template for i2s config
template_i2s_cfg = '''
template<>
struct spi_i2s_cfg<spi_device::bus%(i2s_num)d>
{
    static constexpr auto bus_type = spi_bus_type::i2s;

    using dma_tx = %(dma_tx)s;
    using dma_rx = %(dma_tx)s;

    static constexpr I2S_InitTypeDef init_obj = {
        I2S_Mode_MasterTx,
        %(standard)s,
        %(data_bits)s,
        %(master_clk)s,
        %(audio_freq)s,
        %(CPOL)s,
    };
};

'''

# Template for driver itself
template_i2s_drv = 'using %s = spi_i2s_bus<spi_device::bus%d>;'
# Alias template (optional)
template_i2s_alias = 'using %s = %s;'

i2s_cfgs = {}
if 'i2s' in cfg:
    i2s_cfgs = cfg['i2s']

for i2s in i2s_cfgs:
    id = i2s['id']
    i2s_num = int(id[-1])
    i2s_name = id + '_driver'

    dma_tx = i2s['dma_tx']

    # Directly included values
    values = {
        'i2s_num' : i2s_num,
        'dma_tx'  : i2s['dma_tx'],
        'dma_rx'  : i2s['dma_rx'],
    }

    # Values that require mapping
    for k, v in i2s_mapping.items():
        json_value = i2s[k]
        values[k] = v[json_value]

    if 'comment' in i2s:
        cog.outl('/' + '* ' + i2s['comment'] + ' *' + '/')

    cog.outl(template_i2s_cfg % values)
    cog.outl(template_i2s_drv % (i2s_name, i2s_num))

    if 'alias' in i2s:
        cog.outl(template_i2s_alias % (i2s['alias'], i2s_name))

]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32XX_SPI_I2S_CFGS_HPP_


