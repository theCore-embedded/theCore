#include "fat/dir.hpp"
#include "fat/dir_inode.hpp"
#include "fat/file_inode.hpp"

#include <ecl/iostream.hpp>

using namespace fat;

dir::dir(const fs::inode_ptr &node, FATFS *fs, const allocator &alloc,
         const DIR &fat_dir, path_ptr path)
    :fs::dir_descriptor{node}
    ,m_fs{fs}
    ,m_alloc{alloc}
    ,m_fdir{fat_dir}
    ,m_path{path}
{
    assert(node);
}

dir::~dir()
{
}


fs::inode_ptr dir::next()
{
    FILINFO fno;
    FRESULT res = pf_readdir(m_fs, &m_fdir, &fno);

    if (res != FR_OK || fno.fname[0] == 0) {
        // End of dir
        return fs::inode_ptr{};
    }

    if (fno.fattrib & AM_DIR) {
        auto ptr = ecl::allocate_shared< dir_inode, decltype(m_alloc) >
                (m_alloc, m_fs, m_alloc, m_path->get_path(), fno.fname);

        ptr->set_weak(ptr);

        return ptr;
    } else {
        auto ptr = ecl::allocate_shared< file_inode, decltype(m_alloc) >
                (m_alloc, m_fs, m_alloc, m_path->get_path(), fno.fname);

        return ptr;
    }

    return fs::inode_ptr{};

}

int dir::rewind()
{
    FRESULT res = pf_readdir(m_fs, &m_fdir, nullptr);
    if (res != FR_OK) {
        return -1;
    } else {
        return 0;
    }
}
