#ifndef LIB_FS_INODE_HPP_
#define LIB_FS_INODE_HPP_

#include "file_inode.hpp"
#include "dir_inode.hpp"

namespace fs
{

class inode : public file_inode, public dir_inode
{
public:
	enum class type
	{
		file = 0,
		dir  = 1,
	};

	inode();
	virtual ~inode();
	virtual inode::type type() const = 0;

	// Notify inode about deletition
	virtual void release() = 0;
};



}

#endif
