#include <fat/inode.hpp>

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



