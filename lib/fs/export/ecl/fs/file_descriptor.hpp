/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief File descriptor interface

#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

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

//! Offset changing direction.
//! \sa file_descriptor::seek()
enum class seekdir
{
    beg, //! Set new offset from the beginning.
    cur, //! Set new offset to its current location plus given offset.
    end, //! Set new offset to the size of the file plus given offset.
};

//! Abstract file descriptor
class file_descriptor
{
public:
    //! Constructs the file descriptor from given weak pointer to inode.
    //! \details Constructing the file descriptor acts as file opening.
    //! \param[in] node Weak smart pointer to the respective file inode.
    file_descriptor(const inode_weak &node);

    //! Closes and destructs the file descriptor.
    virtual ~file_descriptor();

    //! Reads data from a file to a buffer with given size.
    //! \pre Valid and opened file descriptor.
    //! \details Current read/write offset is advanced to an amount
    //!          of bytes read.
    //! \param[in]      buf  Buffer to store data into. Must not be NULL.
    //! \param[in,out]  size Size of a buffer on entry,
    //!                      bytes read from a file on exit.
    //! \return Status of operation.
    //! \retval err::eof    End-of-file is reached.
    //! \retval err::notsup Operation is not supported on a given filesystem.
    virtual err read(uint8_t *buf, size_t &size) = 0;

    //! Writes data to a file from buffer with given size.
    //! \pre Valid and opened file descriptor.
    //! \details Current read/write offset is advanced to an amount
    //!          of bytes written.
    //! \param[in]      buf  Buffer to copy data from. Must not be NULL.
    //! \param[in,out]  size Size of a buffer on entry,
    //!                      bytes written to a file on exit.
    //! \return Status of operation.
    //! \retval err::notsup Operation is not supported on a given filesystem.
    virtual err write(const uint8_t *buf, size_t &size) = 0;

    //! Sets the file offset to a given value.
    //! \pre Valid and opened file descriptor.
    //! \param[in] offt New offset value.
    //! \param[in] whence
    //!     Directs how to change offset:
    //!     * If set to seekdir::beg, the file offset shall be set to
    //!       offset bytes.
    //!     * If set to seekdir::cur, the file offset shall be set to
    //!       its current location plus offset.
    //!     * If set to is seekdir::end, the file offset shall be set to
    //!       the size of the file plus offset.
    //! \return Status of operation.
    //! \retval err::notsup Operation is not supported on a given filesystem.
    virtual err seek(off_t offt, seekdir whence = seekdir::beg) = 0;

    //! Gets current file offset.
    //! \pre Valid and opened file descriptor.
    //! \param[out] offt Parameter to store offset into.
    //! \return Status of operation.
    //! \retval err::notsup Operation is not supported on a given filesystem.
    virtual err tell(off_t &offt) = 0;

    //! Closes the file descriptor.
    //! \pre Valid and opened file descriptor.
    //! \post File descriptor is closed.
    //! \return Status of operation.
    virtual err close() = 0;

protected:
    inode_ptr m_inode; 	//!< Associated inode
};

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_FILE_DESCRIPTOR_HPP_
