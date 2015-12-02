#include <fs/dir_descriptor.hpp>


using namespace fs;

dir_descriptor::dir_descriptor(const inode_ptr &node)
    :m_inode(node)
{
}

dir_descriptor::~dir_descriptor()
{
}

