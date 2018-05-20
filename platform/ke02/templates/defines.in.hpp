/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief KE02 platform definitions template

#ifndef KE02_PLATFORM_DEFINES_
#define KE02_PLATFORM_DEFINES_

/*[[[cog
import cog
import json

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['platform']

]]]*/
//[[[end]]]

// Console configuration -------------------------------------------------------
/*[[[cog

if 'console' in cfg:
    cog.outl('#define THECORE_CONFIG_USE_CONSOLE 1')

]]]*/
//[[[end]]]

#endif // KE02_PLATFORM_DEFINES_
