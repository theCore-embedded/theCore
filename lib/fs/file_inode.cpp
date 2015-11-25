#include <fs/file_inode.hpp>

using namespace fs;

file_inode::file_inode()
{
}

file_inode::~file_inode()
{
}

int file_inode::open()
{
	return -1;
}

int file_inode::close()
{
	return -1;
}

int file_inode::read(uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

int file_inode::write(const uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}
