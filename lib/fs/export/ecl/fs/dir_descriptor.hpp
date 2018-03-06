/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Dir descriptor interface.

#ifndef LIB_FS_DIR_DESCRIPTOR_HPP_
#define LIB_FS_DIR_DESCRIPTOR_HPP_

#include <ecl/memory.hpp>
#include <ecl/err.hpp>

#include "types.hpp"

namespace ecl
{

namespace fs
{

//! \addtogroup lib Libraries and utilities
//! @{

//! \defgroup fs Filesystem support
//! @{

//! \defgroup fs_iface Filesystem interfaces
//! @{

//! Directory descriptor interface.
class dir_descriptor
{
public:
    //! Constructs and opens directory descriptor from corresponding inode.
    dir_descriptor(const inode_weak &node);

    //! Closes and destructs dir descriptor.
    virtual ~dir_descriptor();

    //! Gets next entity inode (directory or file) from current directory.
    //! \pre Opened descriptor.
    //! \return Next item in the directory, or nullptr if no more items.
    virtual inode_ptr next() = 0;

    //! Rewindes to the start of the directory.
    //! \pre Opened descriptor.
    //! \retval Status of operation.
    virtual ecl::err rewind() = 0;

    //! Closes a descriptor
    //! \pre Opened descriptor.
    //! \retval Status of operation.
    virtual ecl::err close() = 0;

protected:
    inode_ptr m_inode; //!< Associated inode
};

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_DIR_DESCRIPTOR_HPP_
