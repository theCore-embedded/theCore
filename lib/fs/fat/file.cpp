#include "fat/file.hpp"

using namespace fat;

file::file(const fs::inode_weak &node, FATFS *fs)
    :fs::file_descriptor{node}
    ,m_fs{fs}
{
}

file::~file()
{
}

ssize_t file::read(uint8_t *buf, size_t size)
{
    assert(buf);
    assert(size); // TODO: for now

    size_t read;

    FRESULT res = pf_read(m_fs, reinterpret_cast< void* >(buf), size, &read);

    if (res != FR_OK)
        return -1;

    return read;
}

ssize_t file::write(const uint8_t *buf, size_t size)
{
    assert(buf);
    assert(size); // TODO: for now

#if _USE_WRITE
    size_t written;

    FRESULT res = pf_write(m_fs, reinterpret_cast< const void* >(buf), size, &written);

    if (res != FR_OK)
        return -1;

    return written;
#else
    return -1;
#endif
}

int file::seek(off_t offt)
{
#if _USE_LSEEK
    FRESULT res;

    res = pf_lseek(m_fs, offt);
    if (res == FR_OK)
        return 0;
#else
    (void) offt;
#endif

    return -1;
}

off_t file::tell()
{
    return -1;
}

int file::close()
{
    return -1;
}
