/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FATFS_DIR_HPP_
#define FATFS_DIR_HPP_

#include <fs/dir_descriptor.hpp>
#include <fs/inode.hpp>

#include "src/pff.h"
#include "fat/types.hpp"
#include "fat/path.hpp"

namespace fat
{

class dir : public fs::dir_descriptor
{
public:
    dir(const fs::inode_ptr &node, FATFS *fs, const allocator &alloc,
        const DIR &fat_dir, fat::path_ptr path);
    virtual ~dir();

    //! Next entity in a dir
    //! \details nullptr returned if no more items
    virtual fs::inode_ptr next() override;

    //! Rewinds to the start of the dir
    //! -1 if error, 0 otherwise
    virtual int rewind() override;

    //! Closes a dir
    virtual int close() override;

    dir &operator=(dir&) = delete;
    dir(const dir&) = delete;

private:
    // TBD
    FATFS       *m_fs;
    allocator   m_alloc;
    DIR         m_fdir;
    path_ptr    m_path;
    bool        m_opened;
};


}

#endif
