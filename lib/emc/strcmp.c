#include "string.h"

int strcmp(const char *str1, const char *str2)
{
	int c;
	while (!(c = *str1 - *str2++) && *str1++) { }

	return c;
}
