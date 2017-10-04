/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_FS_TYPES_HPP_
#define LIB_FS_TYPES_HPP_

#include <ecl/memory.hpp>

namespace fs
{
    class inode;
    class file_descriptor;
    class dir_descriptor;


    using inode_ptr = ecl::shared_ptr< inode >;
    using inode_weak = ecl::weak_ptr< inode >;
    using file_ptr = ecl::shared_ptr< file_descriptor >;
    using dir_ptr = ecl::shared_ptr< dir_descriptor >;
}

#endif
