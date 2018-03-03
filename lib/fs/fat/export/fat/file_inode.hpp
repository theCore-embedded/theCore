/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FATFS_FILE_INODE_HPP_
#define FATFS_FILE_INODE_HPP_

#include <sys/types.h>
#include <cstdint>
#include <fs/inode.hpp>

#include "fat/types.hpp"
#include "src/pff.h"
#include "fat/path.hpp"

namespace fat
{

class file_inode : public fs::inode
{
public:
    using type = typename fs::inode::type;

    file_inode(FATFS *fs, const allocator &alloc, const char *path, const char *name);
    virtual ~file_inode();
    virtual type get_type() const override;
    virtual fs::file_ptr open() override;
    virtual ssize_t size() const override;
    virtual ssize_t get_name(char *buf, size_t buf_sz) const override;

private:
    // TBD
    FATFS       m_fs;
    path_ptr    m_path;     // Full path to a file
    allocator   m_alloc;    // Allocator for internal use

};

}

#endif // FATFS_FILE_INODE_HPP_
