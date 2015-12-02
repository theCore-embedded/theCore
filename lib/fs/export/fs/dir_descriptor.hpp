#ifndef LIB_FS_DIR_DESCRIPTOR_HPP_
#define LIB_FS_DIR_DESCRIPTOR_HPP_


#include <ecl/memory.hpp>

namespace fs
{

class inode;

class dir_descriptor
{
public:
	// Next entity in a dir
	// nullptr returned if no more items
	virtual ecl::shared_ptr< inode > next() = 0;
	// Rewinds to the start of the dir
	// -1 if error, 0 otherwise
	virtual int rewind() = 0;

};

// TODO: move it somewhere
using dir_ptr = ecl::shared_ptr< dir_descriptor >;


}
#endif
