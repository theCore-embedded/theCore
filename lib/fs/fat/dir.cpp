#include <fat/dir.hpp>

using namespace fat;

dir::dir()
{
}

dir::~dir()
{
}


fs::inode_ptr dir::next()
{
    return fs::inode_ptr{};
}

int dir::rewind()
{
    return -1;
}
