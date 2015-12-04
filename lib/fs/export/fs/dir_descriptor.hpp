#ifndef LIB_FS_DIR_DESCRIPTOR_HPP_
#define LIB_FS_DIR_DESCRIPTOR_HPP_


#include <ecl/memory.hpp>
#include "types.hpp"

namespace fs
{

class dir_descriptor
{
public:
    dir_descriptor(const inode_ptr &node);
    virtual ~dir_descriptor();


	// Next entity in a dir
	// nullptr returned if no more items
    virtual inode_ptr next() = 0;
	// Rewinds to the start of the dir
	// -1 if error, 0 otherwise
	virtual int rewind() = 0;

protected:
    // Associated inode
    inode_ptr m_inode;
};

}

#endif
