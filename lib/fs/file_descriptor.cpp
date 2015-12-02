#include <fs/file_descriptor.hpp>
#include <fs/inode.hpp>

using namespace fs;

file_descriptor::file_descriptor(const inode_ptr &node)
	:m_inode(node)
{
}

file_descriptor::~file_descriptor()
{
}

