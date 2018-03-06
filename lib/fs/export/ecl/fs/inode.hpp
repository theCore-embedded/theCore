/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief inode interface.

#ifndef LIB_FS_INODE_HPP_
#define LIB_FS_INODE_HPP_

#include "file_descriptor.hpp"
#include "dir_descriptor.hpp"

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

//! Abstract class holding metadata about the file in the filesystem.
class inode
{
public:
    //! Possible filesystem entity types
    enum class type
    {
        file = 0, //!< File entity.
        dir  = 1, //!< Dir entity.
    };

    //! Constructs inode.
    inode();

    //! Destroys inode.
    virtual ~inode();

    //! Gets type of entity, represented by the curent inode.
    //! \return Type of the entity.
    virtual inode::type get_type() const = 0;

    //! Returns the file descriptor, if this inode represents file.
    //! \pre inode representing file entity.
    //! \details Meaningless if used on the dir inode.
    //! \return Pointer to file descriptor.
    virtual file_ptr open();

    //! Returns the descriptor associated with this directory.
    //! \pre inode representing dir entity.
    //! \details Meaningless if used on file inode.
    //! \return Pointer to dir descriptor.
    virtual dir_ptr open_dir();

    //! Returns size of a file or number of entries in the directory.
    //! \param[out] sz Parameter to store either file size (if this inode
    //!                   represents a file) or count of etries in the directory
    //!                   (if this inode represents a dir).
    //! \return Status of operation.
    virtual err size(size_t &sz) const = 0;

    //! Gets a name of an entity.
    //! \param[in]     buf    Buffer to store name to.
    //! \param[in,out] buf_sz On entry: size of a buffer.
    //!                       On exit: bytes written excluding null character.
    //!                       If truncation occur then it will store amount
    //!                       of bytes that would have been written if enough
    //!                       space had been avaliable.
    //! \return Status of operation.
    virtual err get_name(char *buf, size_t &buf_sz) const = 0;

    //! Assigns weak reference to the inode.
    //! \details Every file or dir descriptor is associated with inode.
    //! To simplify memory management, descriptors holds a smart pointer
    //! to inode. But descriptor creation is managed inside the same inode,
    //! which reference should be passed to the descriptor. Thus,
    //! self-reference is required. Setting weak reference helps avoid
    //! cyclic dependency. It then can be passed to descriptors in open()
    //! and open_dir() methods.
    void set_weak(const fs::inode_ptr &ptr);

    /*
     TODO:
     declare move(), copy(), create() and delete() operations?
     */
protected:
    // Allows to pass this reference to a dir descriptor without increasing
    // a reference counter to prevent cycle dependency.
    fs::inode_weak my_ptr; //!< Weak self-pointer.
};

//! Alias to inode pointer.
//! \todo move it somewhere
using inode_ptr = ecl::shared_ptr<inode>;

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_INODE_HPP_
