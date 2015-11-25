#ifndef LIB_ECL_FS_FILEHANDLE_HPP_
#define LIB_ECL_FS_FILEHANDLE_HPP_

#include <sys/types.h>
#include <cstdint>

namespace fat
{

class file_inode
{
	template< class T >
    friend class FAT;

public:
    file_inode();
    ~file_inode();

    int open();
    int close();

	ssize_t read(uint8_t *buf, size_t size);
	ssize_t write(const uint8_t *buf, size_t size);

	off_t tell() const;
	int seek(off_t pos);


private:
	// Must be constructed by filesystem class
	// Not intended to be copied or copy-constructed
    file_inode(const file_inode &) = delete;
    file_inode& operator=(const file_inode &) = delete;
};

}
#endif // LIB_ECL_FS_FILEHANDLE_HPP_
