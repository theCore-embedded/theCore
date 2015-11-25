#include <fs/dir_inode.hpp>

using namespace fs;

dir_inode::dir_inode()
{
}

dir_inode::~dir_inode()
{
}

inode_ptr dir_inode::next()
{
	return inode_ptr{};
}

int dir_inode::rewind()
{
	return -1;
}

