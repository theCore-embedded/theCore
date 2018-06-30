/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

//! \file
//! \brief Header provides template of SPI and I2S drivers configuration.
#ifndef STM32XX_SPI_I2S_CFGS_HPP_
#define STM32XX_SPI_I2S_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/spi_i2s_bus.hpp>

namespace ecl
{

/*[[[cog
import cog
import json
from parse import *

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-stm32']
menu_spi_i2s = cfg['menu-spi-i2s'] if 'menu-spi-i2s' in cfg else {}

]]]*/
//[[[end]]]

// SPI configuration -----------------------------------------------------------

// TODO

// I2S configuration -----------------------------------------------------------

/*[[[cog

# Maps I2S config values to SPL definitions
i2s_mapping = {
    'config-standard'  : {
        'phillips'      : 'I2S_Standard_Phillips',
        'MSB'           : 'I2S_Standard_MSB',
        'LSB'           : 'I2S_Standard_LSB',
        'PCMShort'      : 'I2S_Standard_PCMShort',
        'PCMLong'       : 'I2S_Standard_PCMLong'
    },
    'config-data-bits' : {
        16              : 'I2S_DataFormat_16b',
        24              : 'I2S_DataFormat_24b',
        32              : 'I2S_DataFormat_32b',
        '16ext'         : 'I2S_DataFormat_16bextended'
    },
    'config-master-clk': {
        True            : 'I2S_MCLKOutput_Enable',
        False           : 'I2S_MCLKOutput_Disable'
    },
    'config-audio-freq': {
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
    'config-cpol'      : {
        1               : 'I2S_CPOL_High',
        0               : 'I2S_CPOL_Low'
    }
}

# Template for i2s config
template_i2s_cfg = '''
template<>
struct spi_i2s_cfg<spi_device::bus%(i2s_num)d>
{
    static constexpr auto bus_type = spi_bus_type::i2s;

    using dma_tx = %(dma_tx)s;
    using dma_rx = %(dma_rx)s;

    static constexpr I2S_InitTypeDef init_obj = {
        I2S_Mode_MasterTx,
        %(config-standard)s,
        %(config-data-bits)s,
        %(config-master-clk)s,
        %(config-audio-freq)s,
        %(config-cpol)s,
    };
};

'''

# Template for driver itself
template_i2s_drv = 'using %s = spi_i2s_bus<spi_device::bus%d>;'
# Alias template (optional)
template_i2s_alias = 'using %s = %s;'

i2s_ids = menu_spi_i2s['table-i2s'] if 'table-i2s' in menu_spi_i2s else []

for i2s_id in i2s_ids:
    i2s_cfg = menu_spi_i2s['menu-' + i2s_id]

    i2s_num = int(i2s_id[-1])
    i2s_name = i2s_id + '_driver'

    dma_tx_descr = i2s_cfg['config-tx-dma-descriptor']
    dma_rx_descr = i2s_cfg['config-rx-dma-descriptor']

    dtx = parse('DMA{dma_module:d} Stream{dma_stream:d} Channel{dma_channel:d}', dma_tx_descr)
    drx = parse('DMA{dma_module:d} Stream{dma_stream:d} Channel{dma_channel:d}', dma_rx_descr)

    dtx_str = 'dma%s_stream%s_channel%s' % (dtx['dma_module'], dtx['dma_stream'], dtx['dma_channel'])
    drx_str = 'dma%s_stream%s_channel%s' % (drx['dma_module'], drx['dma_stream'], drx['dma_channel'])

    # Directly included values
    values = {
        'i2s_num' : i2s_num,
        'dma_tx'  : dtx_str,
        'dma_rx'  : drx_str,
    }

    # Values that require mapping
    for k, v in i2s_mapping.items():
        json_value = i2s_cfg[k]
        values[k] = v[json_value]

    if 'config-comment' in i2s_cfg:
        cog.outl('/' + '* ' + i2s_cfg['config-comment'] + ' *' + '/')

    cog.outl(template_i2s_cfg % values)
    cog.outl(template_i2s_drv % (i2s_name, i2s_num))

    if 'config-alias' in i2s_cfg:
        cog.outl(template_i2s_alias % (i2s_cfg['config-alias'], i2s_name))

]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32XX_SPI_I2S_CFGS_HPP_

//! @}

//! @}

//! @}
