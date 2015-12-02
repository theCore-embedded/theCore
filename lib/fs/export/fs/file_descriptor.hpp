#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <ecl/memory.hpp>

namespace fs
{

class inode;

class file_descriptor
{
public:
	file_descriptor(const ecl::shared_ptr< inode > &node);
	virtual ~file_descriptor();

    virtual ssize_t read(uint8_t *buf, size_t size) = 0;
    virtual ssize_t write(const uint8_t *buf, size_t size) = 0;
    virtual int seek(off_t offt) = 0;
    virtual off_t tell() = 0;

protected:
	// Associated inode
	ecl::shared_ptr< inode > m_inode;

};

// TODO: move it somewhere
using file_ptr = ecl::shared_ptr< file_descriptor >;

}

#endif
