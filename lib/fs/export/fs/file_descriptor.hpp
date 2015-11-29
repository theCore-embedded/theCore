#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace fs
{

class file_descriptor
{
public:
    file_descriptor();
    virtual ~file_descriptor();

    virtual int close();
    virtual ssize_t read(uint8_t *buf, size_t size) = 0;
    virtual ssize_t write(const uint8_t *buf, size_t size) = 0;
};

}

#endif
