#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#include "types.hpp"

namespace fs
{

class file_descriptor
{
public:
    file_descriptor(const inode_weak &node);
	virtual ~file_descriptor();

    virtual ssize_t read(uint8_t *buf, size_t size) = 0;
    virtual ssize_t write(const uint8_t *buf, size_t size) = 0;
    virtual int seek(off_t offt) = 0;
    virtual off_t tell() = 0;
    virtual int close() = 0;

protected:
	// Associated inode
    inode_ptr m_inode;

};

}

#endif
