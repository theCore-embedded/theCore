/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of DMA driver configuration
#ifndef STM32_DMA_CFGS_HPP_
#define STM32_DMA_CFGS_HPP_

#include <stm32_device.hpp>

namespace ecl
{

/*[[[cog
import cog
import json
from parse import *

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-stm32']

template_dma = 'using dma%(dma_module)d_stream%(dma_stream)d_channel%(dma_channel)d = ' \
    'ecl::dma_wrap<ecl::dma_stream::dma%(dma_module)d_%(dma_stream)d, ecl::dma_channel::ch%(dma_channel)d>;'

def generate_dma(root):
    for k, v in root.items():
        if k.endswith('-dma-descriptor'):
            r = parse('DMA{dma_module:d} Stream{dma_stream:d} Channel{dma_channel:d}', v)
            cog.outl(template_dma % r)
        elif isinstance(v, dict):
            generate_dma(v)

generate_dma(cfg)

]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32_DMA_CFGS_HPP_
