#include "fs/dir_descriptor.hpp"
#include "fs/inode.hpp"

using namespace fs;

dir_descriptor::dir_descriptor(const inode_weak &node)
    :m_inode(nullptr)
{
    ecl_assert(!node.expired());
    m_inode = node.lock();
    ecl_assert(m_inode);
}

dir_descriptor::~dir_descriptor()
{
}

