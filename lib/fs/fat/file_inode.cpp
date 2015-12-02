#include <fat/file_inode.hpp>

using namespace fat;

file_inode::file_inode()
{

}

file_inode::~file_inode()
{

}

file_inode::type file_inode::get_type() const
{
    return file_inode::type::file;
}

fs::file_ptr file_inode::open()
{
    return fs::file_ptr{};
}

ssize_t file_inode::size() const
{
    return -1;
}

ssize_t file_inode::get_name(const char *buf, size_t buf_sz) const
{
    (void) buf;
    (void) buf_sz;
    return -1;
}

