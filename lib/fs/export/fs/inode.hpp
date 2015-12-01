#ifndef LIB_FS_INODE_HPP_
#define LIB_FS_INODE_HPP_

#include "file_descriptor.hpp"

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
    virtual file_ptr open() = 0;

    /*
     TODO:
     declare move(), copy(), create() and delete() operations
     */

    // Operations with a default implementation

    // Gets a name of an entity
    virtual int get_name(const char *buf, size_t buf_sz);
    // Returns size of a file if a type is file
    // or number of entries if the type is dir
    virtual size_t size();
    // Returns inode associated with an specific entity of a directory.
    // Meaningless for the file inode.
    // !!!TODO: move it to the dir descriptor!!!!
    // virtual ecl::shared_ptr< inode > operator[](size_t idx);

};

using inode_ptr = ecl::shared_ptr< inode >;


}

#endif
