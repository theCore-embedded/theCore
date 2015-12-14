#include "fs/dir_descriptor.hpp"
#include "fs/inode.hpp"

using namespace fs;

dir_descriptor::dir_descriptor(const inode_weak &node)
    :m_inode(nullptr)
{
    assert(!node.expired());
    m_inode = node.lock();
    assert(m_inode);
}

dir_descriptor::~dir_descriptor()
{
}

