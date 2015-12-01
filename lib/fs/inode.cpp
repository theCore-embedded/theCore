#include <fs/inode.hpp>

using namespace fs;

inode::inode()
{
}

inode::~inode()
{
}

int inode::get_name(const char *buf, size_t buf_sz)
{
    (void) buf;
    (void) buf_sz;
    return -1;
}

