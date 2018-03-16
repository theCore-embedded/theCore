/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Filesystem definitions template
//! This file is intentionally made as C-header, to be able to include it
//! in FATFS, written in C.

#ifndef LIB_FS_DEFINES_H_
#define LIB_FS_DEFINES_H_

/*[[[cog

import cog
import json
import common

initial_config = {
    'fat': {
        'readonly': False,
        'enable_seek': True,
        'enable_lowercase': False
    }
}

defines_map = {
    'fat': {
        'readonly': 'THECORE_FATFS_READONLY',
        'enable_seek': 'THECORE_FATFS_USE_SEEK',
        'enable_lowercase': 'THECORE_FATFS_USE_LCC'
    }
}

def fat_generator(fat_cfg):
    for k,v in initial_config['fat'].items():
        if k in fat_cfg:
            initial_config['fat'][k] = fat_cfg[k]

        cog.outl('#define ' + defines_map['fat'][k] + ' '
            + str(int(initial_config['fat'][k])))

cfg = json.load(open(JSON_CFG))
fs_cfg = []

if 'lib' in cfg and 'fs' in cfg['lib']:
    fs_cfg = cfg['lib']['fs']

for fs in fs_cfg:
    if fs['type'] == 'fat':
        fat_generator(fs)

]]]*/
//[[[end]]]


#endif // LIB_FS_DEFINES_H_