#include <fat/file.hpp>

using namespace fat;

file::file(const fs::inode_ptr &node)
    :fs::file_descriptor{node}
{
}

file::~file()
{
}

ssize_t file::read(uint8_t *buf, size_t size)
{
    (void) buf;
    (void) size;
    return -1;
}

ssize_t file::write(const uint8_t *buf, size_t size)
{
    (void) buf;
    (void) size;

    return -1;
}

int file::seek(off_t offt)
{
    (void) offt;
    return -1;
}

off_t file::tell()
{
    return -1;
}
