#include <fs/file_descriptor.hpp>
#include <fs/inode.hpp>

using namespace fs;

file_descriptor::file_descriptor(const inode_weak &node)
    :m_inode(nullptr)
{
    assert(!node.expired());
    m_inode = node.lock();
    assert(m_inode);
}

file_descriptor::~file_descriptor()
{
}

