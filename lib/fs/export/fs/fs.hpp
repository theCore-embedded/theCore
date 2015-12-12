#ifndef LIB_FS_FS_HPP_
#define LIB_FS_FS_HPP_

#include "fs_descriptor.hpp"

#include <tuple>
#include <ecl/utils.hpp>

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


            // Find a start of the segment
            while (path[cur] == '/') {
                cur++;
            }

            if (cur == path_len)
                return nullptr;

            const char *p = strchr(path + cur, '/');

            // TODO: compact this
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
                cur += len;
                expected = inode::type::dir;

            } else {
                // File is found in the path
                len = path_len - cur;

                if (len >= 16) {
                    // Name is too long
                    // TODO: fix it
                    assert(0);
                } else {
                    std::copy(path + cur, path + cur + len, component);
                    component[len] = 0;
                }

                cur += len;

                expected = inode::type::file;
            }

            next_type = expected;
            return component;
        }

        // Obvious
        size_t get_cur_idx() const
        {
            return cur;
        }

        size_t get_len() const
        {
            return path_len;
        }

    private:
        const char  *path;
        size_t      path_len;
        size_t      cur;
        char        component[16]; // TODO: clarify size
    };

    // Get root node by the path to its element.
    // It changes the path pointer so it will point to the root
    // of a filesystem.
    auto get_root_node(const char *&path);
    // Iterates over tuple
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
    auto mounter = [](auto &fs) {
         fs.mount();
    };

    ecl::for_each(m_fses, mounter);

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
    auto node = path_to_inode(path);
    if (!node || node->get_type() != inode::type::dir) {
        // Not found :(
        ecl::cout << "Not found!\n";
        return nullptr;
    }

    return node->open_dir();
}

//------------------------------------------------------------------------------

template< class ...Fs >
auto vfs< Fs... >::get_root_node(const char *&path)
{
    assert(path);

    inode_ptr root;
    size_t last_match = 0;

    // Picks most appropriate inode
    auto fn = [&root, &last_match, path](auto &fs) {
        constexpr const char *mnt_point = fs.mnt_point;

        // Iterate over two paths, trying to find best match

        auto dummy_type = inode::type::dir;
        path_iter m{mnt_point};
        path_iter p{path};

        const char *mnt_seg;
        const char *path_seg;

        while ((mnt_seg = m.next(dummy_type)) && (path_seg = p.next(dummy_type))) {
            if (strcmp(mnt_seg, path_seg)) {
                // Token mismatch
                break;
            }
        }

        // Whole mount path was iterated, meaning that it
        // can be a possible match
        if (!mnt_seg) {
            // An incoming path can contain multiple slashes
            // thus current index must be taken from it rather than from
            // a mount point.
            auto match = p.get_cur_idx();

            // Check if it is a better match.
            if (match > last_match) {
                root = fs.get_root();
                last_match = match;
            }
        }
    };

    ecl::for_each(m_fses, fn);

    // Properly advance a path
    path += last_match;

    return root;
}

template< class ...Fs >
auto vfs< Fs... >::path_to_inode(const char *path)
{
    assert(path);
    auto root = get_root_node(path);
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

        // If we reached here then last node is a file and its name is
        // the same as requested.
        if (next->get_type() == fs::inode::type::file)
            break;
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
