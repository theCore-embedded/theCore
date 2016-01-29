#include "string.h"

size_t  LIBC_FUNCTION(strlen) (const char *str)
{
	size_t len = 0;
	while (*str++) { len++; }

	return len;
}
