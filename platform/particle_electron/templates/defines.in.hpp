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

# Check systimer flag.
if 'use_systmr' in cfg and cfg['use_systmr']:
    cog.outl('#define USE_SYSTMR 1')

]]]*/

//[[[end]]]

#endif // PARTICLE_PLATFORM_DEFINES_
