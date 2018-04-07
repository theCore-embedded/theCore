/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FAT file descriptor interface.
//! \todo rename it to file_descriptor?

#ifndef FATFS_FILE_HPP_
#define FATFS_FILE_HPP_

#include <ecl/fs/file_descriptor.hpp>
#include <ecl/fs/inode.hpp>

#include "src/pff.h"

namespace ecl
{

namespace fat
{

//! \addtogroup lib Libraries and utilities
//! @{

//! \defgroup fs Filesystem support
//! @{

//! \defgroup fat FAT filesystem
//! @{

//! FATFS file descriptor.
class file : public fs::file_descriptor
{
public:
    //! Constructs the FATFS file descriptor.
    //! \param[in] node Weak smart pointer to the respective file inode.
    //! \param[in] fs Petite FATFS object.
    file(const fs::inode_weak &node, FATFS *fs);

    //! \copydoc ecl::fs::file_descriptor::~file_descriptor()
    ~file();

    //! \copydoc ecl::fs::file_descriptor::read()
    err read(uint8_t *buf, size_t &size) override;
    //! \copydoc ecl::fs::file_descriptor::write()
    err write(const uint8_t *buf, size_t &size) override;
    //! \copydoc ecl::fs::file_descriptor::seek()
    err seek(off_t offt, fs::seekdir whence = fs::seekdir::beg) override;
    //! \copydoc ecl::fs::file_descriptor::tell()
    err tell(off_t &offt) override;
    //! \copydoc ecl::fs::file_descriptor::close()
    err close() override;

    file &operator=(file&) = delete;
    file(const file&) = delete;

private:
    FATFS *m_fs;    //! Pointer to petite FATFS object
    bool  m_opened; //! Set to true if opened. \todo: remove it and use FATFS::flag instead
};

//! @}

//! @}

//! @}

} // namespace fat

} // namespace ecl

#endif // FATFS_FILE_HPP_
