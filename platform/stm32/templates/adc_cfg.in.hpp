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
//! \brief Header provides template of ADC driver configuration
#ifndef STM32XX_ADC_CFGS_HPP_
#define STM32XX_ADC_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/adc.hpp>

namespace ecl
{

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

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

    using dma = %s;
};
'''

# ADC IRQ configuration template
template_adc_cfg_irq = '''
template<>
struct adc_cfg<adc_dev::dev%d>
{
    static constexpr adc_mgmt_mode mgtm_mode = adc_mgmt_mode::irq;
}
'''

adc_cfgs = {}

if 'adc' in cfg:
    adc_cfgs = cfg['adc']

for adc_cfg in adc_cfgs:
    if 'comment' in adc_cfg:
        cog.outl('/' + '* ' + adc_cfg['comment'] + ' *' + '/\n')

    mode = adc_cfg['mode']
    adc_num = int(adc_cfg['id'][-1])

    if mode == 'DMA':
        cog.msg(str(adc_cfg))
        cog.outl(template_adc_cfg_dma % (adc_num, adc_cfg['dma_id']))
    elif mode == 'IRQ':
        cog.outl(template_adc_cfg_irq % adc_num)

    cog.outl(template_adc_inst % (adc_cfg['id'], adc_num))
    if 'alias' in adc_cfg:
        cog.outl('using %s = ADC%d_driver;' % (adc_cfg['alias'], adc_num))

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

channels_cfgs = {}

if 'adc_channels' in cfg:
    channels_cfgs = cfg['adc_channels']

for channels_cfg in channels_cfgs:
    channels_str = ', '.join([('adc_channel::ch%d' % ch) for ch in channels_cfg['nums']])
    trigger = channels_cfg['trigger']

    # Alias here is mandatory. Imagine what name should be picked for random channel
    # combination? Better to force user to specify a name.
    alias = channels_cfg['alias']

    if 'comment' in channels_cfg:
        cog.outl('/' + '* ' + channels_cfg['comment'] + ' *' + '/\n')

    if trigger == 'SW': # Software trigger
        cog.outl(template_adc_chs_sw % (alias, channels_str))
    else:
        cog.error('ADC channel config is not implemented for trigger: ' + trigger)


]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32XX_ADC_CFGS_HPP_

//! @}

//! @}

//! @}
