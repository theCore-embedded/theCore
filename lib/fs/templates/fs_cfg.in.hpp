/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Filesystem configuration code template

#ifndef LIB_FS_CFG_HPP_
#define LIB_FS_CFG_HPP_

#include <ecl/fs/fs_descriptor.hpp>
#include <ecl/fs/fs.hpp>

#include <fs/fs_defines.h>

/*[[[cog

import cog
import json
import common

cfg = json.load(open(JSON_CFG))
fs_cfg = []

if 'lib' in cfg and 'fs' in cfg['lib']:
    fs_cfg = cfg['lib']['fs']

for fs in fs_cfg:
    if fs['type'] == 'fat':
        cog.outl('#include <ecl/fat/fs.hpp>')

]]]*/
//[[[end]]]


namespace ecl
{

// File system definitions

/*[[[cog

fs_descriptors = []

def fat_generator(fat_cfg):
    i = 0
    for dev in fat_cfg['dev']:
        fat_name = 'fatfs' + str(i)
        fat_mount_name = 'fatfs_mount' + str(i)
        fat_descr_name = 'fatfs_descr' + str(i)

        cog.outl('')

        cog.outl('ECL_FS_MOUNT_POINT(%s, "%s");'
            % (fat_mount_name, dev['mountpoint']))
        cog.outl('using %s = ecl::fat::petit<%s>;'
            % (fat_name, dev['id']))
        cog.outl('using %s = ecl::fs::fs_descriptor<%s, %s>;'
            % (fat_descr_name, fat_mount_name, fat_name))

        i += 1
        fs_descriptors.append(fat_descr_name)

for fs in fs_cfg:
    if fs['type'] == 'fat':
        fat_generator(fs)
]]]*/
//[[[end]]]

// Virtual file system alias

/*[[[cog

if len(fs_descriptors) != 0:
    fs_descriptors_str = ', '.join(fs_descriptors)
    cog.outl('using filesystem = ecl::fs::vfs<%s>;' % (fs_descriptors_str))

]]]*/
//[[[end]]]

} // namespace ecl

#endif // LIB_FS_CFG_HPP_