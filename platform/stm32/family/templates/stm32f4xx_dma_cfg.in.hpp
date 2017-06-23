//! \file
//! \brief Header provides template of DMA driver configuration
#ifndef STM32F4XX_DMA_CFGS_HPP_
#define STM32F4XX_DMA_CFGS_HPP_

#include <stm32_device.hpp>

//namespace ecl
//{

/*[[[cog
import cog
import json

cfg = json.load(open(JSON_CFG))

dma = {}
if 'dma' in cfg:
    dma = cfg['dma']

template_dma = 'using %s = ' \
    'ecl::dma_wrap<ecl::dma_stream::dma%d_%d, ecl::dma_channel::ch%d>;'

for d in dma:
    cog.outl(template_dma % (d['alias'], d['num'], d['stream'], d['channel']))

]]]*/
//[[[end]]]

//} // namespace ecl

#endif // STM32F4XX_DMA_CFGS_HPP_