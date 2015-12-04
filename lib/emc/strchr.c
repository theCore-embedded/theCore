#include "string.h"

const char* strchr(const char *str, int character)
{
    const char *p;
    while (*(p = str++)) {
        if (*p == character) {
            return p;
        }
    }

    return NULL;
}
