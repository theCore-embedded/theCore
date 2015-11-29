#include <fs/file_descriptor.hpp>

using namespace fs;

file_descriptor::file_descriptor()
{
}

file_descriptor::~file_descriptor()
{
}

int file_descriptor::close()
{
	return -1;
}

ssize_t file_descriptor::read(uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

ssize_t file_descriptor::write(const uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}
