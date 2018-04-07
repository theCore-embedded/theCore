/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FATFS dir descriptor interface.

#ifndef FATFS_DIR_HPP_
#define FATFS_DIR_HPP_

#include <ecl/fs/dir_descriptor.hpp>
#include <ecl/fs/inode.hpp>
#include <ecl/fs/path.hpp>

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

//! FATFS directory descriptor.
class dir : public fs::dir_descriptor
{
public:
    //! Constructs and opens dir descriptor.
    //! \param[in] node inode for directory.
    //! \param[in] fs Petite filesystem object.
    //! \param[in] alloc Allocator used for the filesystem.
    //! \param[in] fat_dir PetiteFAT directory structure.
    //! \param[in] path Directory path.
    dir(const fs::inode_ptr &node, FATFS *fs, const allocator &alloc,
        const DIR &fat_dir, fs::path_ptr path);

    //! \copydoc ecl::fs::dir_descriptor::~dir_descriptor()
    virtual ~dir();

    //! \copydoc ecl::fs::dir_descriptor::read()
    virtual fs::inode_ptr read() override;

    //! \copydoc ecl::fs::dir_descriptor::rewind()
    virtual ecl::err rewind() override;

    //! \copydoc ecl::fs::dir_descriptor::close()
    virtual ecl::err close() override;

    dir &operator=(dir&) = delete;
    dir(const dir&) = delete;

private:
    FATFS           *m_fs;      //!< Petite FATFS object
    allocator       m_alloc;    //!< Allocator to create new items.
    DIR             m_fdir;     //!< Petite FATFS directory object.
    fs::path_ptr    m_path;     //!< Path to a dir.
    bool            m_opened;   //!< Set to true if opened.
};

//! @}

//! @}

//! @}

} // namespace fat


} // namespace ecl

#endif // FATFS_DIR_HPP_
