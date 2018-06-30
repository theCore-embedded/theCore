/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of SPI and spi drivers configuration.
#ifndef PLATFORM_SPI_CFGS_HPP_
#define PLATFORM_SPI_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/spi.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_templates Auto-generated code, Python COG and CMake templates.
//! @{

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-tm4c']

]]]*/
//[[[end]]]

// SPI configuration -----------------------------------------------------------

/*[[[cog

# Maps SPI config values to correct definitions
spi_mapping = {
    'config-type' : {
        'master'        : 'ecl::spi_type::master',
        # TODO: slave mode is not implemented
        # 'slave'       : 'ecl::spi_type::slave'
    },
    'config-cpol': {
        1               : 'ecl::spi_cpol::high',
        0               : 'ecl::spi_cpol::low'
    },
    'config-cpha': {
        1               : 'ecl::spi_cpha::high',
        0               : 'ecl::spi_cpha::low'
    },
}

# Template for SPI config
spi_cfg_struct = '''
template<>
struct spi_cfg<spi_channel::ch%(spi_ch)d>
{
    static constexpr auto type    = %(config-type)s;
    static constexpr auto cpol    = %(config-cpol)s;
    static constexpr auto cpha    = %(config-cpha)s;
    static constexpr auto clk_div = %(clk_div)s;
};

'''

# Template for driver typedef
spi_typedef = 'using %s = spi<spi_channel::ch%d>;'
# Alias template (optional)
spi_alias = 'using %s = %s;'

spi_ids = []
try:
    spi_ids = cfg['menu-spi']['table-spi']
except:
    pass

for spi_id in spi_ids:
    # Missing configuration?
    try:
        spi_cfg = cfg['menu-spi']['menu-' + spi_id]
    except:
        continue

    spi_num =  int(spi_id[-1])
    spi_drv_name = 'SPI' + str(spi_num) + '_driver'
    spi_ssi_name = 'SSI' + str(spi_num) + '_driver'

    # Directly included values
    values = {
        'spi_ch' : spi_num,
    }

    # Values that require mapping
    for k, v in spi_mapping.items():
        json_value = spi_cfg[k]
        values[k] = v[json_value]

    values['clk_div'] = spi_cfg['config-clk-div']

    if 'config-comment' in spi_cfg:
        cog.outl('/' + '* ' + spi_cfg['config-comment'] + ' *' + '/')

    cog.outl(spi_cfg_struct % values)
    cog.outl(spi_typedef % (spi_drv_name, spi_num))
    # Alternative name
    cog.outl(spi_typedef % (spi_ssi_name, spi_num))

    if 'config-alias' in spi_cfg:
        cog.outl(spi_alias % (spi_cfg['config-alias'], spi_ssi_name))

]]]*/
//[[[end]]]

//! @}

//! @}

//! @}

} // namespace ecl

#endif // PLATFORM_SPI_CFGS_HPP_
