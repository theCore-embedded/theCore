/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Directory FATFS inode implementation.

#include "ecl/fat/dir_inode.hpp"
#include "ecl/fat/dir.hpp"
#include <ecl/iostream.hpp>

using namespace ecl::fat;

dir_inode::dir_inode(FATFS *fs, const allocator &alloc, const char *path, const char *name)
    :fs::inode()
    ,m_alloc(alloc)
    ,m_path()
    ,m_fs(fs)
{
    if (!path && !name) {
        m_path = fs::allocate_path("/", nullptr, m_alloc);
    } else {
        m_path = fs::allocate_path(path, name, m_alloc);
    }
}

dir_inode::~dir_inode()
{
}

dir_inode::type dir_inode::get_type() const
{
    return dir_inode::type::dir;
}

ecl::fs::dir_ptr dir_inode::open_dir()
{
    ecl_assert(!my_ptr.expired());

    // Handle possible OOM condition
    if (!m_path) {
        return nullptr;
    }

    DIR fat_dir;

    FRESULT res = pf_opendir(m_fs, &fat_dir, m_path->get_path());
    // TODO: graceful error handing
    // ecl_assert(res == FR_OK);
    if (res != FR_OK) {
        ecl::cout << "Error opening dir: " << m_path->get_path()
                  << " : " << res << ecl::endl;
        return nullptr;
    }

    auto inode = my_ptr.lock();
    ecl_assert(inode);

    auto ptr = ecl::allocate_shared<dir, decltype(m_alloc)>
            (m_alloc, inode, m_fs, m_alloc, fat_dir, m_path);

    return ptr;
}

ecl::err dir_inode::size(size_t &sz) const
{
    (void)sz;
    // TODO
    return ecl::err::notsup;
}

ecl::err dir_inode::get_name(char *buf, size_t &buf_sz) const
{
    ecl_assert(buf);
    ecl_assert(buf_sz);

    // Handle possible OOM condition
    if (!m_path) {
        return err::nomem;
    }

    const char *path = m_path->get_path();

    size_t path_len = strlen(path);
    const char *start = path;
    const char *end;

    end = path + path_len;

    // Skip trailing '/'
    for (uint i = path_len - 1; i > 0; --i) {
        if (path[i - 1] == '/') {
            start = path + i; // Skipping forward slash
            break;
        }
    }

    // Reserve place for null terminator
    size_t to_copy = std::min(buf_sz - 1, (size_t) (end - start));

    // Algorithm check
    ecl_assert(to_copy < buf_sz);
    ecl_assert(start < end);

    buf[to_copy] = 0;

    std::copy(start, end, buf);

    buf_sz = path_len;
    return ecl::err::ok;
}

