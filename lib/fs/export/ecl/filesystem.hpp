#ifndef LIB_ECL_FS_FILESYSTEM_HPP_
#define LIB_ECL_FS_FILESYSTEM_HPP_

#include "filehandle.hpp"

namespace ecl
{

// Only FAT is supported for now
template< class Block >
class filesystem
{
	friend class filehandle;
public:
	static int init();

	static filehandle&& open(const char *path)
	{
		(void) path;
		return std::move(filehandle());
	}

private:
	// Filesystem from which descriptor was obtained
	enum class FS_type
	{
		FAT = 0
	};

	// Aggregate for all possible file descriptors
	struct filedescr
	{
		FS_type type;
		union
		{
			int fd;
		};
	};

	// Singleton
	filesystem();

	// These methods are used by filehandle

};


}

#endif // LIB_ECL_FS_FILESYSTEM_HPP_

