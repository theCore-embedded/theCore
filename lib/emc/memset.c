#include "string.h"

#include <stdint.h>

// Hack to avoid errors when using LTO
__attribute__((used))
void *memset(void *s, int c, size_t n)
{
    uint8_t *dest = (uint8_t *) s;

    while (n--) {
        *dest++ = c;
    }

    return s;
}
