#include "string.h"

const char*  LIBC_FUNCTION(strchr) (const char *str, int character)
{
    const char *p;
    while (*(p = str++)) {
        if (*p == character) {
            return p;
        }
    }

    return NULL;
}
