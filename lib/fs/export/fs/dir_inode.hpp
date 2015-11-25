#ifndef LIB_FS_DIR_INODE_HPP_
#define LIB_FS_DIR_INODE_HPP_

#include "inode_ptr.hpp"

namespace fs
{

class dir_inode
{
public:
	dir_inode();
	virtual ~dir_inode();

	virtual inode_ptr next();
	virtual int rewind();
};

}

#endif
