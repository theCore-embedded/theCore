/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Filesystem type utilities

#ifndef LIB_FS_TYPES_HPP_
#define LIB_FS_TYPES_HPP_

#include <ecl/memory.hpp>

namespace ecl
{

namespace fs
{

//! \addtogroup lib Libraries and utilities
//! @{

//! \defgroup fs Filesystem support
//! @{

//! \defgroup fs_iface Filesystem interfaces
//! @{

class inode;
class file_descriptor;
class dir_descriptor;

using inode_ptr = ecl::shared_ptr<inode>;
using inode_weak = ecl::weak_ptr<inode>;
using file_ptr = ecl::shared_ptr<file_descriptor>;
using dir_ptr = ecl::shared_ptr<dir_descriptor>;

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_TYPES_HPP_
