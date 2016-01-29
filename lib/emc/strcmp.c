#include "string.h"

/* Rely upon actual function */
extern int tolower(int c);

int LIBC_FUNCTION(strcmp)(const char *str1, const char *str2)
{
	int c;
	while (!(c = *str1 - *str2++) && *str1++) { }

	return c;
}

int LIBC_FUNCTION(strcmpi)(const char *str1, const char *str2)
{
    int c;
    while (!(c = tolower(*str1) - tolower(*str2++)) && *str1++) { }

    return c;
}
