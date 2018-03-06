/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Directory FATFS inode interface.

#ifndef FATFS_DIR_INODE_HPP_
#define FATFS_DIR_INODE_HPP_

#include <ecl/fs/inode.hpp>
#include <ecl/fs/types.hpp>
#include <ecl/fs/path.hpp>

#include <cstdint>
#include <sys/types.h>

#include "src/pff.h"
#include "ecl/fat/types.hpp"

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

//! Metadata class for FATFS filesystem directories.
class dir_inode : public ecl::fs::inode
{
public:
    using type = typename ecl::fs::inode::type;

    //! Constructs FATFS inode for given directory.
    //! \details If both name and path are null then this inode
    //!          represents root node.
    //! \param[in] fs FATFS filesystem object,
    //! \param[in] alloc Allocator, used for internal  allocations.
    //! \param[in] path Path to a parent dir.
    //! \param[in] name Name of a dir represented by this inode.
    dir_inode(FATFS *fs, const allocator &alloc,
              const char *path = nullptr, const char *name = nullptr);

    //! \copydoc fs::inode::~inode()
    virtual ~dir_inode();

    //! \copydoc fs::inode::get_type()
    type get_type() const override;

    //! \copydoc fs::inode::open_dir()
    fs::dir_ptr open_dir() override;

    //! \copydoc fs::inode::size()
    err size(size_t &sz) const override;

    //! \copydoc fs::inode::get_name()
    err get_name(char *buf, size_t &buf_sz) const override;

    dir_inode &operator=(dir_inode&) = delete;
    dir_inode(const dir_inode&) = delete;

private:
    // Holds a reference to a path string and manages its deallocation
    allocator       m_alloc; //!< The allocator to create various objects
    fs::path_ptr    m_path;  //!< The path of this inode
    FATFS           *m_fs;   //!< The filesystem object
};

//! @}

//! @}

//! @}

} // namespace fat


} // namespace ecl

#endif  // FATFS_DIR_INODE_HPP_
