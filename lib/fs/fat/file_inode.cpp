#include <fat/file_inode.hpp>

using namespace fat;

file_inode::file_inode(FATFS *fs, const allocator &alloc,
                       const char *path, const char *name)
    :m_fs{fs}
    ,m_path{}
    ,m_alloc{alloc}
{
    m_path = allocate_path(path, name, m_alloc);
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

ssize_t file_inode::get_name(char *buf, size_t buf_sz) const
{
    assert(buf);
    assert(buf_sz);

    const char *path = m_path->get_path();

    size_t path_len = strlen(path);
    uint i;
    for (i = path_len; i > 0; --i) {
        if (path[i - 1] == '/') {
            i--; // Rest of the code assumes that 'i' is an index of array2
            break;
        }
    }

    const char *start = path + i;
    // Reserve place for null terminator
    size_t to_copy = std::min(buf_sz - 1, path_len - i);
    const char *end = path + i + to_copy;

    // Algorithm check
    assert(to_copy < buf_sz);
    assert(start < end);

    buf[to_copy] = 0;

    std::copy(start, end, buf);

    return path_len;
}

