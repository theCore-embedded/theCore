#include "fat/file.hpp"

using namespace fat;

file::file(const fs::inode_weak &node, FATFS *fs)
    :fs::file_descriptor{node}
    ,m_fs{fs}
    ,m_opened{true} // When constructed it is already opened
{
}

file::~file()
{
}

ssize_t file::read(uint8_t *buf, size_t size)
{
    assert(buf);
    assert(size); // TODO: for now

    if (m_opened) {
        size_t read;

        FRESULT res = pf_read(m_fs, reinterpret_cast< void* >(buf), size, &read);

        if (res == FR_OK)
            return read;
    }

    return -1;
}

ssize_t file::write(const uint8_t *buf, size_t size)
{
    assert(buf);
    assert(size); // TODO: for now

#if _USE_WRITE
    if (m_opened) {
        size_t written;

        FRESULT res = pf_write(m_fs, reinterpret_cast< const void* >(buf), size, &written);

        if (res != FR_OK)
            return -1;

        return written;
    }
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
    if (m_opened) {
        m_opened = false;
        return 0;
    }

    // Already closed
    return -1;
}
