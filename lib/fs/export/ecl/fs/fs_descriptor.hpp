/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Filesystem descriptor.

#ifndef LIB_FS_FS_DESCRIPTOR_HPP_
#define LIB_FS_FS_DESCRIPTOR_HPP_

#include "types.hpp"

#include <ecl/err.hpp>

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

//! Creates mount point structure with given name and given mount point path.
//! \details Mounpoint struct then can be passed to the filesystem descriptor.
//! \param[in] struct_name Structure name.
//! \param[in] mnt_point_path Mounpoint path.
#define ECL_FS_MOUNT_POINT(struct_name, mnt_point_path) \
    struct struct_name \
    { \
        static constexpr auto mount_point() \
        { return mnt_point_path; } \
    }

//! Wraps filesystem class into correct descriptor, required by main FS module.
//! \tparam Mount Mounpoint for the  filesystem.
//! \tparam Fs Filesystem class.
template<class Mount, class Fs>
class fs_descriptor
{
public:
    //! Mounts a filesystem.
    //! \retval Status of operation.
    static ecl::err mount();
    //! Gets root node
    //! \retval Status of operation.
    static auto get_root();

    //! Holds mount point
    static constexpr auto mount_point = Mount::mount_point();

    // Couple of checks.
    static_assert(strchri_constexpr(mount_point, '/') == -1,
            "mountpoint must not contain slashes");
    static_assert(strlen_constexpr(mount_point) > 0,
            "mountpoint must not be empty");

private:
    //! Prevents static initialization fiasco.
    static inode_ptr &root() { static inode_ptr node; return node; }
};

//------------------------------------------------------------------------------

template<class Mount, class Fs>
ecl::err fs_descriptor<Mount, Fs>::mount()
{
    auto fs_root = Fs::mount();
    if (!fs_root) {
        return ecl::err::io;
    }

    root() = fs_root;

    return ecl::err::ok;
}

template<class Mount, class Fs>
auto fs_descriptor<Mount, Fs>::get_root()
{
    return root();
}

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_FS_DESCRIPTOR_HPP_
