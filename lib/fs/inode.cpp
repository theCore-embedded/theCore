#include <fs/inode.hpp>
#include <fs/dir_descriptor.hpp>

using namespace fs;

inode::inode()
{
}

inode::~inode()
{
}

file_ptr inode::open()
{
	// Must not be called, if inode is a dir
	assert(0);
	return file_ptr{};
}

dir_ptr inode::open_dir()
{
	// Must not be called, if inode is a file
	assert(0);
	return dir_ptr{};
}

