#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <ecl/memory.hpp>

namespace fs
{

class file_descriptor
{
public:
    virtual ~file_descriptor();

    virtual ssize_t read(uint8_t *buf, size_t size) = 0;
    virtual ssize_t write(const uint8_t *buf, size_t size) = 0;
    virtual int seek(off_t offt) = 0;
    virtual off_t tell() = 0;

protected:
    // Can be constructed only by descedance
    file_descriptor();
};

using file_ptr = ecl::shared_ptr< file_descriptor >;

}

#endif
