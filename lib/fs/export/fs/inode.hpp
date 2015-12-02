#ifndef LIB_FS_INODE_HPP_
#define LIB_FS_INODE_HPP_

#include "file_descriptor.hpp"
#include "dir_descriptor.hpp"

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
    virtual inode::type get_type() const = 0;
	// Returns the file descriptor, if this inode represents file.
	// Meaningless if used on the dir inode.
	virtual file_ptr open();
	// If this inode is directory, then it returns the descriptor associated
	// with this directory. Meaningless if used on file inode.
	virtual dir_ptr open_dir();
	// Returns size of a file if a type is file
	// or number of entries if the type is dir
	virtual ssize_t size() const = 0;
	// Gets a name of an entity, returns bytes written
	virtual ssize_t get_name(const char *buf, size_t buf_sz) const = 0;

    /*
     TODO:
     declare move(), copy(), create() and delete() operations
	 */

    // Returns inode associated with an specific entity of a directory.
    // Meaningless for the file inode.
    // !!!TODO: move it to the dir descriptor!!!!
    // virtual ecl::shared_ptr< inode > operator[](size_t idx);

};

// TODO: move it somewhere
using inode_ptr = ecl::shared_ptr< inode >;


}

#endif
