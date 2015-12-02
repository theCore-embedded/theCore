#ifndef FATFS_FILE_HPP_
#define FATFS_FILE_HPP_

#include <fs/file_descriptor.hpp>
#include <fs/inode.hpp>

namespace fat
{

class file : public fs::file_descriptor
{
public:
    file(const fs::inode_ptr &node);
    virtual ~file();

    virtual ssize_t read(uint8_t *buf, size_t size);
    virtual ssize_t write(const uint8_t *buf, size_t size);
    virtual int seek(off_t offt);
    virtual off_t tell();
};

}

#endif // FATFS_FILE_HPP_
