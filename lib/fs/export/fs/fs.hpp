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

    file_ptr open_file(const char *path);
    dir_ptr  open_dir(const char *path);

    /* TODO:
     copy(), create_dir(), rename(), remove(), move()
     */


private:
    class path_iter
    {
    public:
        path_iter(const char *path)
			:path{path}
            ,cur{0}
            ,component{0}
        { }

        const char* next(inode::type &next_type)
        {
            (void) next_type;
            return nullptr;
        }

    private:
        const char  *path;
        size_t      cur;
        char        component[16]; // TODO: clarify size
    };

    auto path_to_inode(const char *path);
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

    while (part = iter.next(&type)) {
        auto next = name_to_inode(root, part);

        if (!next) {
            // Such name is not found
            return nullptr;
        }

        if (next->get_type() != type) {
            // Type mismatch - e.g. requested dir name is a file on a device.
            return nullptr;
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
        if (next->get_name(inode_name, sizeof(inode_name)) >= sizeof(inode_name)) {
            // Name is too long
            assert(0);
        }

        if (!strcmp(inode_name, name)) {
            // Item found!
            return next;
        }
    }


    return nullptr;
}

}

#endif // LIB_FS_FS_HPP_
