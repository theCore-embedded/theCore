/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FAT file descriptor implementation.
//! \todo rename it to file_descriptor?

#include "ecl/fat/file.hpp"

using namespace ecl::fat;

file::file(const fs::inode_weak &node, FATFS *fs)
    :fs::file_descriptor{node}
    ,m_fs{fs}
    ,m_opened{true} // When constructed it is already opened
{
}

file::~file()
{
}

ecl::err file::read(uint8_t *buf, size_t &size)
{
    ecl_assert(buf);
    ecl_assert(m_opened);

    if (!size) {
        return err::ok;
    }

    unsigned read;

    FRESULT res = pf_read(m_fs, reinterpret_cast<void*>(buf), size, &read);

    if (res == FR_OK) {
        size = read;
        return err::ok;
    }

    return err::io;
}

ecl::err file::write(const uint8_t *buf, size_t &size)
{
    ecl_assert(buf);
    ecl_assert(m_opened);

#if _USE_WRITE
    if (!size) {
        return err::ok;
    }

    UINT written;

    FRESULT res = pf_write(m_fs, reinterpret_cast<const void*>(buf), size, &written);

    if (res != FR_OK) {
        return ecl::err::io;
    }

    return ecl::err::ok;
#else
    (void)size;
    (void)buf;
    return ecl::err::notsup;
#endif
}

ecl::err file::seek(off_t offt, fs::seekdir whence)
{
    ecl_assert(m_opened);

#if _USE_LSEEK
    FRESULT res;

    off_t top_offt;

    switch (whence) {
    case fs::seekdir::beg:
        top_offt = offt;
        break;
    case fs::seekdir::cur:
        top_offt = m_fs->fptr + offt;
        break;
    case fs::seekdir::end:
        top_offt = m_fs->fsize + offt;
        break;
    default:
        ecl_assert(0); // Not supported seekdir
        break;
    }

    res = pf_lseek(m_fs, top_offt);
    if (res == FR_OK) {
        return ecl::err::ok;
    }

#else
    (void)offt;
    (void)whence;
#endif

    return ecl::err::notsup;
}

ecl::err file::tell(off_t &offt)
{
    ecl_assert(m_opened);

#if _USE_LSEEK
    offt = m_fs->fptr;
    return ecl::err::ok;
#else
    (void)offt;
    return ecl::err::notsup;
#endif
}

ecl::err file::close()
{
    ecl_assert(m_opened);

#if _USE_WRITE
    // Finalize write, if any
    pf_write(m_fs, nullptr, 0, nullptr);
#endif

    m_opened = false;
    return err::ok;
}
