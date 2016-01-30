#include <fs/file_descriptor.hpp>
#include <fs/inode.hpp>

using namespace fs;

file_descriptor::file_descriptor(const inode_weak &node)
    :m_inode(nullptr)
{
    ecl_assert(!node.expired());
    m_inode = node.lock();
    ecl_assert(m_inode);
}

file_descriptor::~file_descriptor()
{
}

