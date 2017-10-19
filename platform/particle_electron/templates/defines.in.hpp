/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Platform definitions template
#ifndef PARTICLE_PLATFORM_DEFINES_
#define PARTICLE_PLATFORM_DEFINES_

/*[[[cog
import cog
import json
import os

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

# Check if console has to be enabled

if 'console' in cfg and cfg['console']:
    cog.outl('#define THECORE_CONFIG_USE_CONSOLE 1')

# Check timer ownership

systmr_freq_hz = 20

if 'systmr_cfg' in cfg:
    systmr_cfg = cfg['systmr_cfg']

    if systmr_cfg['freq_hz']:
        systmr_freq_hz = systmr_cfg['freq_hz']

    if systmr_cfg['owner'] and systmr_cfg['owner'] == 'thecore':
        cog.outl('#define THECORE_OWNS_SYSTMR 1')

cog.outl('#define THECORE_SYSTMR_FREQ %d' % systmr_freq_hz)

]]]*/

//[[[end]]]

#endif // PARTICLE_PLATFORM_DEFINES_
