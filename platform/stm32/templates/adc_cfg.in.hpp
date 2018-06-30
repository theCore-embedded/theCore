/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of ADC driver configuration
//! \ingroup stm32_templates
#ifndef STM32XX_ADC_CFGS_HPP_
#define STM32XX_ADC_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/adc.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

/*[[[cog
import cog
import json
from parse import *

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-stm32']

]]]*/
//[[[end]]]

// ADC configuration -----------------------------------------------------------

/*[[[cog

# ADC instance template
template_adc_inst = 'using %s_driver = adc<adc_dev::dev%d>;'

# ADC DMA configuration template
template_adc_cfg_dma = '''
template<>
struct adc_cfg<adc_dev::dev%d>
{
    static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::dma;

    using dma = dma%s_stream%s_channel%s;
};
'''

# ADC IRQ configuration template
template_adc_cfg_irq = '''
template<>
struct adc_cfg<adc_dev::dev%d>
{
    static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::irq;
};
'''

adc_ids = []

if 'menu-adc' in cfg:
    adc_ids = cfg['menu-adc']['table-adc']

for adc_id in adc_ids:
    adc_cfg = cfg['menu-adc']['menu-' + adc_id]
    if 'config-comment' in adc_cfg:
        cog.outl('/' + '* ' + adc_cfg['config-comment'] + ' *' + '/\n')

    mode = adc_cfg['config-mode']
    adc_num = int(adc_id[-1])

    if mode == 'DMA':
        dma_descr = adc_cfg['config-dma-descriptor']
        r = parse('DMA{dma_module:d} Stream{dma_stream:d} Channel{dma_channel:d}', dma_descr)

        cog.outl(template_adc_cfg_dma % (adc_num,
            r['dma_module'], r['dma_stream'], r['dma_channel']))
    elif mode == 'IRQ':
        cog.outl(template_adc_cfg_irq % adc_num)

    cog.outl(template_adc_inst % (adc_id, adc_num))
    if 'config-alias' in adc_cfg:
        cog.outl('using %s = ADC%d_driver;' % (adc_cfg['config-alias'], adc_num))

]]]*/
//[[[end]]]

// ADC channels configuration --------------------------------------------------

/*[[[cog

# ADC channels with software trigger.
template_adc_chs_sw = '''
using %s = channel_group_sw_trigger<%s>;
'''

# ADC channels with external trigger.
template_adc_chs = '''
using %s = channel_group<%s, %s>;
'''

# ADC channels with EXTI trigger.
template_adc_chs_exti = '''
using %s = channel_group_exti_trigger<%s, %s>;
'''

chan_group_ids = []

if 'menu-adc-channels' in cfg:
    chan_group_ids = cfg['menu-adc-channels']['table-adc-channels']

for chan_group_id in chan_group_ids:
    chan_group_cfg = cfg['menu-adc-channels']['menu-' + chan_group_id]
    channels_str = ', '.join([('adc_channel::ch%d' % int(ch[-2:])) \
        for ch in chan_group_cfg['config-channels']])
    trigger = chan_group_cfg['config-trigger']

    # Alias here is mandatory. Imagine what name should be picked for random channel
    # combination? Better to force user to specify a name.
    alias = chan_group_id

    if 'comment' in chan_group_cfg:
        cog.outl('/' + '* ' + chan_group_cfg['comment'] + ' *' + '/\n')

    if trigger == 'software': # Software trigger
        cog.outl(template_adc_chs_sw % (alias, channels_str))
    else:
        cog.error('ADC channel config is not implemented for trigger: ' + trigger)

]]]*/
//[[[end]]]

//! @}

//! @}

//! @}

} // namespace ecl

#endif // STM32XX_ADC_CFGS_HPP_
