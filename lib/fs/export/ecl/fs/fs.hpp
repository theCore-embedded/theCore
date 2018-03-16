/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief VFS interface.

#ifndef LIB_FS_FS_HPP_
#define LIB_FS_FS_HPP_

#include "ecl/fs/fs_descriptor.hpp"
#include "ecl/fs/inode.hpp"
#include "ecl/fs/path.hpp"

#include <tuple>
#include <utility>
#include <cstring>
#include <algorithm>

#include <ecl/utils.hpp>
#include <ecl/iostream.hpp>
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

//! Virtual filesystem
//! \tparam List of filesystem descriptors.
template<class ...Fs>
class vfs
{
public:
    //! Mounts all filesystems to their mountpoints.
    //! \return Status of operation.
    static ecl::err mount_all();

    //! Opens file using given path.
    //! \return Pointer to the file descriptor, or nullptr if open failed.
    static file_ptr open_file(const char *path);

    //! Opens file using given path.
    //! \return Pointer to the dir descriptor, or nullptr if open failed.
    static dir_ptr  open_dir(const char *path);

    /* TODO:
     copy(), create_dir(), rename(), remove(), move()
     */

private:
    //! Helper struct to perform mount operation on concrete OS.
    template<class T>
    struct mounter { void operator()() { T::mount(); } };

    //! Gets root node by using mountpoint (1st segment of the path).
    static auto get_fs_node(const char *mountpoint, size_t mnt_point_len);

    //! Gets root node for given filesystem.
    template<typename T>
    static bool get_fs_node_impl(inode_ptr &root, const char *mnt_point, size_t mnt_point_len);

    //! Iterates over filesystems and resolves path to inode
    static auto path_to_inode(const char *path);

    //! Resolves the name of item in current dir to the inode with given name length
    static auto name_to_inode(inode_ptr cur_dir, const char *name, size_t name_len);
};

template<class ...Fs>
ecl::err vfs<Fs...>::mount_all()
{
    auto rc = ecl::err::ok;

    [](...){}((rc = is_ok(rc) ? Fs::mount() : rc, 0) ... );

    return rc;
}

template<class ...Fs>
file_ptr vfs<Fs...>::open_file(const char *path)
{
    ecl_assert(path);

    auto node = path_to_inode(path);
    if (!node || node->get_type() != inode::type::file) {
        // Not found :(
        return nullptr;
    }

    return node->open();
}

template<class ...Fs>
dir_ptr vfs<Fs...>::open_dir(const char *path)
{
    ecl_assert(path);

    // TODO: use specific mountpoint, instead of root
    auto node = path_to_inode(path);
    if (!node || node->get_type() != inode::type::dir) {
        // Not found :(
        return nullptr;
    }

    return node->open_dir();
}

//------------------------------------------------------------------------------

template<class ...Fs>
auto vfs<Fs...>::get_fs_node(const char *mnt_point, size_t mnt_point_len)
{
    ecl_assert(mnt_point);

    inode_ptr root;
    bool found = false;

    // Iterate over filesystems and pick most appropriate root inode of the underlying FS
    [](...){}((
        found = !found ?
            get_fs_node_impl<Fs>(root, mnt_point, mnt_point_len) : found, 0
    ) ... );

    return root;
}

template<class ...Fs>
template<typename T>
bool vfs<Fs...>::get_fs_node_impl(inode_ptr &root, const char *mnt_point, size_t mnt_point_len)
{
    constexpr auto fs_mnt = T::mount_point;
    constexpr auto fs_mnt_len = strlen_constexpr(fs_mnt);

    // using faster memcmp() instead of strncmp() since size is known.
    if (fs_mnt_len != mnt_point_len || memcmp(mnt_point, fs_mnt, fs_mnt_len)) {
        return false;
    }

    root = T::get_root();
    return true;
}

template<class ...Fs>
auto vfs<Fs...>::path_to_inode(const char *path)
{
    ecl_assert(path);

    path_iter iter{path};
    path_iter::segment seg;

    // First segment of the path must be a mount point directory
    if (!iter.next(seg)) {
        return inode_ptr{};
    }

    auto root = get_fs_node(seg.first, seg.second - seg.first);
    if (!root) {
        // Incorrect root directory specified
        return inode_ptr{};
    }

    while (iter.next(seg)) {
        // To iterate directory in name_to_inode() call, current root object
        // must be a directory, obviously.
        if (root->get_type() != inode::type::dir) {
            return inode_ptr{nullptr};
        }

        size_t len = seg.second - seg.first;
        auto next = name_to_inode(root, seg.first, len);

        if (!next) {
            // Such name is not found
            return inode_ptr{nullptr};
        }

        // Move to the next inode in path
        root = next;
    }

    return root;
}

template<class ...Fs>
auto vfs<Fs...>::name_to_inode(inode_ptr cur_dir, const char *name, size_t name_len)
{
    ecl_assert(cur_dir);
    ecl_assert(name);
    ecl_assert(cur_dir->get_type() == inode::type::dir);

    auto dd = cur_dir->open_dir();
    char inode_name[64]; // TODO: determine filename size from the filesystem settings

    // Iterate over directory and try to find proper item
    inode_ptr next;
    while (next = dd->next()) {
        size_t len = sizeof(inode_name);
        auto rc = next->get_name(inode_name, len);

        if (is_error(rc) || len >= sizeof(inode_name)) {
            // Error occur, or name is too long
            return inode_ptr{nullptr};
        }

        // TODO: most of FSes are case sensitive.
        // This must be optionally be supported by checking special flags
        if (!memcmp(inode_name, name, std::min(name_len, len))) {
            // Item found!
            return next;
        }
    }

    return inode_ptr{nullptr};
}

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_FS_HPP_
