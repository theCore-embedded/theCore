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

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

dma = {}
if 'dma' in cfg:
    dma = cfg['dma']

template_dma = 'using %s = ' \
    'ecl::dma_wrap<ecl::dma_stream::dma%d_%d, ecl::dma_channel::ch%d>;'

for d in dma:
    # Place comment line if needed
    if 'comment' in d:
        # Avoid using begin and end comment section tokens
        cog.outl(('\n%s* ' + d['comment'] + ' *%s') % ('/', '/'))

    cog.outl(template_dma % (d['alias'], d['num'], d['stream'], d['channel']))

]]]*/
//[[[end]]]

} // namespace ecl

#endif // STM32_DMA_CFGS_HPP_
