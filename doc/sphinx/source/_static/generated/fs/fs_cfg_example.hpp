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

#include <ecl/fat/fs.hpp>


namespace ecl
{

// File system definitions


ECL_FS_MOUNT_POINT(fatfs_mount0, "main_fs");
using fatfs0 = ecl::fat::petit<sdspi0>;
using fatfs_descr0 = ecl::fs::fs_descriptor<fatfs_mount0, fatfs0>;

ECL_FS_MOUNT_POINT(fatfs_mount1, "backup_fs");
using fatfs1 = ecl::fat::petit<other_sdspi_driver>;
using fatfs_descr1 = ecl::fs::fs_descriptor<fatfs_mount1, fatfs1>;

// Virtual file system alias

using filesystem = ecl::fs::vfs<fatfs_descr0, fatfs_descr1>;

} // namespace ecl

#endif // LIB_FS_CFG_HPP_