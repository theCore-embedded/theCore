#ifndef LIB_ECL_FS_FILEHANDLE_HPP_
#define LIB_ECL_FS_FILEHANDLE_HPP_

#include <sys/types.h>

namespace ecl
{

class filehandle
{
	template< class T >
	friend class filesystem;

public:
	ssize_t read(uint8_t *buf, size_t size);
	ssize_t write(const uint8_t *buf, size_t size);

	off_t tell() const;
	int seek(off_t pos);

	int close();

	~filehandle();

private:
	// Must be constructed by filesystem class
	filehandle();
	// Not intended to be copied or copy-constructed
	filehandle(const filehandle &) = delete;
	filehandle& operator=(const filehandle &) = delete;
};

}
#endif // LIB_ECL_FS_FILEHANDLE_HPP_
