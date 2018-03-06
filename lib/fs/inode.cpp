/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/fs/inode.hpp>
#include <ecl/fs/dir_descriptor.hpp>

using namespace ecl::fs;

inode::inode()
    :my_ptr{}
{
}

inode::~inode()
{
}

file_ptr inode::open()
{
	// Must not be called, if inode is a dir
    ecl_assert_msg(0, "Function call not permitted");
	return file_ptr{};
}

dir_ptr inode::open_dir()
{
	// Must not be called, if inode is a file
    ecl_assert_msg(0, "Function call not permitted");
    return dir_ptr{};
}

void inode::set_weak(const fs::inode_ptr &ptr)
{
    ecl_assert(my_ptr.expired());
    ecl_assert(ptr);
    my_ptr = ptr;
}

