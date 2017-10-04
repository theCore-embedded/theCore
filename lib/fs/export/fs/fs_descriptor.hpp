/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_FS_FS_DESCRIPTOR_HPP_
#define LIB_FS_FS_DESCRIPTOR_HPP_

#include "types.hpp"

namespace fs
{

template< const char *mount_point, class Fs, typename ...Args >
class fs_descriptor
{
public:
	constexpr fs_descriptor(Args ...args);

    // Mounts a filesystem
    int mount();
    // Gets root node
    auto get_root() const;

    // Holds mount point
    // TODO: use it!
    static constexpr const char *mnt_point = mount_point;

private:
    Fs           m_fs;
    inode_ptr    m_root;

};


template< const char *mount_point, class Fs, typename ...Args >
constexpr fs_descriptor< mount_point, Fs, Args... >::fs_descriptor(Args ...args)
	:m_fs{args...}
	,m_root{}
{

}

template< const char *mount_point, class Fs, typename ...Args >
int fs_descriptor< mount_point, Fs, Args... >::mount()
{
    m_root = m_fs.mount();
    if (!m_root)
        return -1;

    return 0;
}

template< const char *mount_point, class Fs, typename ...Args >
auto fs_descriptor< mount_point, Fs, Args... >::get_root() const
{
    return m_root;
}

}

#endif // LIB_FS_FS_DESCRIPTOR_HPP_
