#include "ctype.h"

int LIBC_FUNCTION(islower)(int c)
{
    return (c >= 'a' && c <= 'z');
}
