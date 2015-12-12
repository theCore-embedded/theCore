#include "fat/file_inode.hpp"
#include "fat/file.hpp"

using namespace fat;

file_inode::file_inode(FATFS *fs, const allocator &alloc,
                       const char *path, const char *name)
    :m_fs{*fs}
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
    // Petite FAT does not support multiple opened files,
    // so it is unnecesary to keep any state inside file descriptor.
    // However, the filesystem object itself works as an obscured
    // file descriptors
    FRESULT res = pf_open(&m_fs, m_path->get_path());
    if (res == FR_OK) {
        auto ptr = ecl::allocate_shared< file, allocator >(m_alloc, my_ptr, &m_fs);

        return ptr;
    }

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

    // Skip forward slash
    const char *start = path + i + 1;
    // Reserve place for null terminator
    size_t to_copy = std::min(buf_sz - 1, path_len - i);
    // One-beyond-end iterator
    const char *end = start + to_copy + 1;

    // Algorithm check
    assert(to_copy < buf_sz);
    assert(start < end);

    buf[to_copy] = 0;

    // Copy [start;end) to buf
    std::copy(start, end, buf);

    return path + path_len - start;
}

