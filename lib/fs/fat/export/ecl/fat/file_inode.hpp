/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief File FATFS inode interface.

#ifndef FATFS_FILE_INODE_HPP_
#define FATFS_FILE_INODE_HPP_

#include <ecl/fs/inode.hpp>
#include <ecl/fs/path.hpp>

#include <cstdint>
#include <sys/types.h>

#include "ecl/fat/types.hpp"
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

//! FATFS file inode
class file_inode : public fs::inode
{
public:
    using type = typename fs::inode::type;

    //! Constructs FATFS inode for given file.
    //! \param[in] fs FATFS filesystem object,
    //! \param[in] alloc Allocator, used for internal  allocations.
    //! \param[in] path Path to a parent dir.
    //! \param[in] name Name of a file represented by this inode.
    file_inode(FATFS *fs, const allocator &alloc, const char *path, const char *name);

    //! \copydoc ecl::fs::inode::~inode()
    virtual ~file_inode();

    //! \copydoc ecl::fs::inode::get_type()
    type get_type() const override;

    //! \copydoc ecl::fs::inode::open()
    fs::file_ptr open() override;

    //! \copydoc ecl::fs::inode::size()
    err size(size_t &sz) const override;

    //! \copydoc ecl::fs::inode::get_name()
    err get_name(char *buf, size_t &buf_sz) const override;

private:
    FATFS           m_fs;       //!< Petite FATFS object
    fs::path_ptr    m_path;     //!< Full path to a file
    allocator       m_alloc;    //!< Allocator for internal use

};

//! @}

//! @}

//! @}

} // namespace fat

} // namespace ecl

#endif // FATFS_FILE_INODE_HPP_
