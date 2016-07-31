// TODO: rename it to 'file descriptor'
#ifndef FATFS_FILE_HPP_
#define FATFS_FILE_HPP_

#include <fs/file_descriptor.hpp>
#include <fs/inode.hpp>

#include "src/pff.h"

namespace fat
{

class file : public fs::file_descriptor
{
public:
    file(const fs::inode_weak &node, FATFS *fs);
    virtual ~file();

    virtual ssize_t read(uint8_t *buf, size_t size);
    virtual ssize_t write(const uint8_t *buf, size_t size);
    virtual int seek(off_t offt, fs::seekdir way = fs::seekdir::beg);
    virtual off_t tell();
    virtual int close();

private:
    FATFS *m_fs;
    bool  m_opened; // TODO: remove it and use FATFS::flag instead
};

}

#endif // FATFS_FILE_HPP_
