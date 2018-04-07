/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief File FATFS inode implementation.

#include "ecl/fat/file_inode.hpp"
#include "ecl/fat/file.hpp"

using namespace ecl::fat;

file_inode::file_inode(FATFS *fs, const allocator &alloc,
                       const char *path, const char *name)
    :m_fs{*fs}
    ,m_path{}
    ,m_alloc{alloc}
{
    m_path = fs::allocate_path(path, name, m_alloc);
}

file_inode::~file_inode()
{

}

file_inode::type file_inode::get_type() const
{
    return file_inode::type::file;
}

ecl::fs::file_ptr file_inode::open()
{
    // Handle possible OOM condition
    if (!m_path) {
        return nullptr;
    }

    // Petite FAT does not support multiple opened files.
    // This can be workarounded by keeping copy of whole FATFS internal object
    // in every file descriptor.
    FRESULT res = pf_open(&m_fs, m_path->get_path());
    if (res == FR_OK) {
        auto ptr = ecl::allocate_shared<file, allocator>(m_alloc, my_ptr, &m_fs);
        return ptr;
    }

    return fs::file_ptr{};
}

ecl::err file_inode::size(size_t &sz) const
{
    (void)sz;
    return ecl::err::notsup; // TODO: implement
}

ecl::err file_inode::get_name(char *buf, size_t &buf_sz) const
{
    // Handle possible OOM condition
    if (!m_path) {
        return err::nomem;
    }

    ecl_assert(buf);
    ecl_assert(buf_sz);

    const char *path = m_path->get_path();

    size_t path_len = strlen(path);
    uint i;
    for (i = path_len; i > 0; --i) {
        if (path[i - 1] == '/') {
            i--; // Rest of the code assumes that 'i' is an index of array
            break;
        }
    }

    size_t name_len = path_len - i;

    // Skip forward slash
    const char *start = path + i + 1;
    // Reserve place for null terminator
    size_t to_copy = std::min(buf_sz - 1, name_len);
    // One-beyond-end iterator
    const char *end = start + to_copy + 1;

    // Algorithm check
    // TODO: handle case when buffer is too small more gracefully
    ecl_assert(to_copy < buf_sz);
    ecl_assert(start < end);

    buf[to_copy] = 0;

    // Copy [start;end) to buf
    std::copy(start, end, buf);
    buf_sz = name_len;

    return ecl::err::ok;
}

