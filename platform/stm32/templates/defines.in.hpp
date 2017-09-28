//! \file
//! \brief Platform definitions template
#ifndef PARTICLE_PLATFORM_DEFINES_
#define PARTICLE_PLATFORM_DEFINES_

// Some defintions are family-specific
#include <family_defines.hpp>

/*[[[cog
import cog
import json
]]]*/
//[[[end]]]

/*[[[cog

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['platform']

if 'console' in cfg:
    cog.outl('#define THECORE_CONFIG_USE_CONSOLE 1')

if 'bypass_console' in cfg:
    cog.outl('#define THECORE_CONFIG_USE_BYPASS_CONSOLE 1')

]]]*/
//[[[end]]]

#endif // PARTICLE_PLATFORM_DEFINES_
