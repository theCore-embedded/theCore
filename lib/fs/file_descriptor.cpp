/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief File descriptor implementation

#include <ecl/fs/file_descriptor.hpp>
#include <ecl/fs/inode.hpp>

using namespace ecl::fs;

file_descriptor::file_descriptor(const inode_weak &node)
    :m_inode(nullptr)
{
    ecl_assert(!node.expired());
    m_inode = node.lock();
    ecl_assert(m_inode);
}

file_descriptor::~file_descriptor()
{
}

