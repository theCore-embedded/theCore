#include "string.h"
#include "ctype.h"

int strcmp(const char *str1, const char *str2)
{
	int c;
	while (!(c = *str1 - *str2++) && *str1++) { }

	return c;
}

int strcmpi(const char *str1, const char *str2)
{
    int c;
    while (!(c = tolower(*str1) - tolower(*str2++)) && *str1++) { }

    return c;
}
