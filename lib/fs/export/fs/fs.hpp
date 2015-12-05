#ifndef LIB_FS_FS_HPP_
#define LIB_FS_FS_HPP_

#include "fs_descriptor.hpp"

#include <tuple>

namespace fs
{

template< class ...Fs >
class vfs
{
public:
    vfs();
    ~vfs();

    int mount_all();

    file_ptr open_file(const char *path);
    dir_ptr  open_dir(const char *path);

    /* TODO:
     copy(), create_dir(), rename(), remove(), move()
     */


private:
    // TODO: convert it to generic-purpose class
    class path_iter
    {
    public:
        path_iter(const char *path)
            :path{path}
            ,path_len{strlen(path)}
            ,cur{0}
            ,component{0}
        {
        }

        // Get next path token
        const char* next(inode::type &next_type)
        {
            size_t len;
            inode::type expected;

            if (cur == path_len)
                return nullptr;

            if (path[cur] == '/') {
                // Starts with root, skipping...
                cur++;
            }

            auto p = strchr(path + cur, '/');

            if (p) {
                len = p - (path + cur);
                // Dir is found in path
                if (len >= 16) {
                    // Name is too long
                    // TODO: fix it
                    assert(0);
                } else {
                    std::copy(path + cur, p, component);
                    component[len] = 0;
                }

                // Skip following '/'
                cur += len + 1;
                expected = inode::type::dir;

            } else {
                // Found a file
                len = path_len - cur - 1;

                if (len >= 16) {
                    // Name is too long
                    // TODO: fix it
                    assert(0);
                } else {
                    std::copy(path + cur, p, component);
                    component[len] = 0;
                }

                expected = inode::type::file;
            }

            next_type = expected;
            return component;
        }

    private:
        const char  *path;
        size_t      path_len;
        size_t      cur;
        char        component[16]; // TODO: clarify size
    };

    // Resolves path to inode
    auto path_to_inode(const char *path);
    // Resolves the name of item in current dir to the inode
    auto name_to_inode(inode_ptr cur_dir, const char *name);

    std::tuple< Fs... > m_fses;
};


template< class ...Fs >
vfs< Fs... >::vfs()
    :m_fses{}
{
}

template< class ...Fs >
vfs< Fs... >::~vfs()
{
}

template< class ...Fs >
int vfs< Fs... >::mount_all()
{
    auto &fs = std::get< 0 >(m_fses);
    fs.mount();
    return 0;
}

template< class ...Fs >
file_ptr vfs< Fs... >::open_file(const char *path)
{
    if (!path)
        return nullptr;

    auto node = path_to_inode(path);
    if (!node || node->get_type() != inode::type::file) {
        // Not found :(
        return nullptr;
    }

    return node->open();
}

template< class ...Fs >
dir_ptr vfs< Fs... >::open_dir(const char *path)
{
    // TODO: use specific mountpoint, instead of root
    return nullptr;
}

//------------------------------------------------------------------------------

template< class ...Fs >
auto vfs< Fs... >::path_to_inode(const char *path)
{
    assert(path);

    // TODO: use specific filesystem, instead of 0th fs
    auto &fs = std::get< 0 >(m_fses);
    auto root = fs.get_root();

    assert(root);

    path_iter iter{path};
    const char *part;
    inode::type type;

    while ((part = iter.next(type))) {
        auto next = name_to_inode(root, part);

        if (!next) {
            // Such name is not found
            return inode_ptr{nullptr};
        }

        if (next->get_type() != type) {
            // Type mismatch - e.g. requested dir name is a file on a device.
            return inode_ptr{nullptr};
        }

        // Move to the next inode in path
        root = next;
    }

    return root;
}

template< class ...Fs >
auto vfs< Fs... >::name_to_inode(inode_ptr cur_dir, const char *name)
{
    assert(cur_dir);
    assert(name);
    assert(cur_dir->get_type() == inode::type::dir);

    auto dd = cur_dir->open_dir();
    char inode_name[16]; // TODO: Clarify size

    // Iterate over directory and try to find proper item
    inode_ptr next;
    while (next = dd->next()) {
        ssize_t ret;
        ret = next->get_name(inode_name, sizeof(inode_name));

        if (ret < 0) {
            // Error occur
            return inode_ptr{nullptr};
        }

        if ((size_t) ret >= sizeof(inode_name)) {
            // Name is too long
            assert(0);
        }

        if (!strcmp(inode_name, name)) {
            // Item found!
            return next;
        }
    }


    return inode_ptr{nullptr};
}

}

#endif // LIB_FS_FS_HPP_
