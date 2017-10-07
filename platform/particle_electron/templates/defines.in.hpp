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

]]]*/

//[[[end]]]

// Particle Electron timers is always avaliable for theCore.
#define USE_SYSTMR 1

#endif // PARTICLE_PLATFORM_DEFINES_
