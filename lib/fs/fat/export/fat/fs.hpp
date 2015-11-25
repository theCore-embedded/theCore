#ifndef LIB_ECL_FS_FILESYSTEM_HPP_
#define LIB_ECL_FS_FILESYSTEM_HPP_

#include "inode.hpp"

namespace fs
{

template< class Block >
class FAT
{
    friend class fat::file_inode;
public:
	static int init();

    static fat::file_inode&& open(const char *path)
	{
		(void) path;
        return std::move(fat::file_inode{});
	}

private:
    // Singleton
    FAT();

    // These methods are used by filehandle

};


}

#endif // LIB_ECL_FS_FILESYSTEM_HPP_

