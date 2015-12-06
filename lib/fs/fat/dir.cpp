#include <fat/dir.hpp>
#include <ecl/iostream.hpp>

using namespace fat;

dir::dir(const fs::inode_ptr &node, const DIR &fat_dir)
    :fs::dir_descriptor(node)
    ,m_fdir(fat_dir)
{
    ecl::cout << "Sect  " << (int) m_fdir.sect << ecl::endl;
    ecl::cout << "Clust " << (int) m_fdir.clust << ecl::endl;
}

dir::~dir()
{
}


fs::inode_ptr dir::next()
{
    return fs::inode_ptr{};
}

int dir::rewind()
{
    return -1;
}
