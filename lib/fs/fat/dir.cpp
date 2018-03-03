/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fat/dir.hpp"
#include "fat/dir_inode.hpp"
#include "fat/file_inode.hpp"

#include <ecl/iostream.hpp>
#include <ecl/assert.h>

namespace fat
{

dir::dir(const fs::inode_ptr &node, FATFS *fs, const allocator &alloc,
         const DIR &fat_dir, path_ptr path)
    :fs::dir_descriptor{node}
    ,m_fs{fs}
    ,m_alloc{alloc}
    ,m_fdir{fat_dir}
    ,m_path{path}
    ,m_opened{true}
{
    ecl_assert(node);
}

dir::~dir()
{
}


fs::inode_ptr dir::next()
{
    if (m_opened) {
        FILINFO fno;
        FRESULT res = pf_readdir(m_fs, &m_fdir, &fno);

        if (res != FR_OK || fno.fname[0] == 0) {
            // End of dir
            return fs::inode_ptr{};
        }

        if (fno.fattrib & AM_DIR) {
            auto ptr = ecl::allocate_shared< dir_inode, decltype(m_alloc) >
                    (m_alloc, m_fs, m_alloc, m_path->get_path(), fno.fname);

            ptr->set_weak(ptr);

            return ptr;
        } else {
            auto ptr = ecl::allocate_shared< file_inode, decltype(m_alloc) >
                    (m_alloc, m_fs, m_alloc, m_path->get_path(), fno.fname);

            ptr->set_weak(ptr);

            return ptr;
        }
    }

    return fs::inode_ptr{};
}

int dir::rewind()
{
    if (m_opened) {
        FRESULT res = pf_readdir(m_fs, &m_fdir, nullptr);
        if (res == FR_OK) {
            return 0;
        }
    }

    return -1;
}

int dir::close()
{
    if (m_opened) {
        m_opened = false;
        return 0;
    }

    return -1;
}

} // namespace fat
