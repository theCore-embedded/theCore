#ifndef LIB_FS_INODE_HPP_
#define LIB_FS_INODE_HPP_

#include <ecl/memory.hpp>

namespace fs
{

class inode
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
    virtual int open() = 0;
};



}

#endif
