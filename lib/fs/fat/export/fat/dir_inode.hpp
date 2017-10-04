/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FATFS_DIR_INODE_HPP_
#define FATFS_DIR_INODE_HPP_

#include <sys/types.h>
#include <fs/inode.hpp>
#include <fs/types.hpp>
#include <cstdint>

#include "src/pff.h"
#include "fat/types.hpp"
#include "../../path.hpp"

namespace fat
{


class dir_inode : public fs::inode
{
public:
    using type = typename fs::inode::type;

    // Constructs this inode for given filesystem. Allocator is used for internal
    // allocations. The 'path' pointer should point to a path to a parent dir.
    // The name is a name of a dir represented by this inode.
    // If both name and path are null then this inode represents root node.
    dir_inode(FATFS *fs, const allocator &alloc,
              const char *path = nullptr, const char *name = nullptr);
    virtual ~dir_inode();

    virtual type get_type() const override;
    virtual fs::dir_ptr open_dir() override;
    virtual ssize_t size() const override;
    virtual ssize_t get_name(char *buf, size_t buf_sz) const override;

    dir_inode &operator=(dir_inode&) = delete;
    dir_inode(const dir_inode&) = delete;

private:
    // Holds a reference to a path string and manages its deallocation
    allocator  m_alloc; // The allocator to create various objects
    path_ptr   m_path;  // The path of this inodeho
    FATFS      *m_fs;   // The filesystem object
};

}

#endif  // FATFS_DIR_INODE_HPP_
