#ifndef LIB_FS_FILE_INODE_HPP_
#define LIB_FS_FILE_INODE_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace fs
{

class file_inode
{
public:
	file_inode();
	virtual ~file_inode();

	virtual int open();
	virtual int close();
	virtual int read(uint8_t *buf, size_t size);
	virtual int write(const uint8_t *buf, size_t size);
};

}

#endif
