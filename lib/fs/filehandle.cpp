#include <ecl/filehandle.hpp>

using namespace ecl;

filehandle::filehandle()
{

}

filehandle::~filehandle()
{

}

ssize_t filehandle::read(uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

ssize_t filehandle::write(const uint8_t *buf, size_t size)
{
	(void) buf;
	(void) size;
	return -1;
}

off_t filehandle::tell() const
{
	return -1;
}

int filehandle::seek(off_t pos)
{
	(void) pos;
	return -1;
}

int filehandle::close()
{
	return -1;
}




