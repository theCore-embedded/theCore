/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
