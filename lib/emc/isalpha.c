#include "ctype.h"


int LIBC_FUNCTION(isalpha)(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
