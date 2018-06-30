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

// SDSPI is only block device supported in theCore right now
// So, to make things easier just include it right away
#include <dev/sdspi.hpp>

/*[[[cog

import cog
import json
import common

cfg = json.load(open(JSON_CFG))
fs_cfg = {}
fatfs_menu = {}
fatfs_ids = []

try:
    fs_cfg = cfg['menu-lib']['menu-filesystem']
except:
    pass

if 'menu-fatfs' in fs_cfg:
    cog.outl('#include <ecl/fat/fs.hpp>')
    fatfs_menu = fs_cfg['menu-fatfs']
    fatfs_ids = fatfs_menu['table-fatfs']

]]]*/
//[[[end]]]


namespace ecl
{

// File system definitions

/*[[[cog

fs_descriptors = []

def fat_generator(fat_id, fat_cfg):
    fat_mount_name = fat_id + '_mount'
    fat_descr_name = fat_id + '_descriptor'

    mountpoint = fat_cfg['config-mountpoint']
    block = fat_cfg['config-block']

    cog.outl('')

    cog.outl('ECL_FS_MOUNT_POINT(%s, "%s");'
        % (fat_mount_name, mountpoint))
    cog.outl('using %s = ecl::fat::petit<%s>;'
        % (fat_id, block))
    cog.outl('using %s = ecl::fs::fs_descriptor<%s, %s>;'
        % (fat_descr_name, fat_mount_name, fat_id))

    fs_descriptors.append(fat_descr_name)

for fatfs_id in fatfs_ids:
    fat_generator(fatfs_id, fatfs_menu['menu-' + fatfs_id])

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
