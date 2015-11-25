#include <fat/inode.hpp>

using namespace fat;

file_inode::file_inode()
{

}

file_inode::~file_inode()
{

}

ssize_t file_inode::read(uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

ssize_t file_inode::write(const uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

off_t file_inode::tell() const
{
	return -1;
}

int file_inode::seek(off_t pos)
{
	(void) pos;
	return -1;
}

int file_inode::close()
{
	return -1;
}




