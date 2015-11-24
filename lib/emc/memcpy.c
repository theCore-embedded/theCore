#include "string.h"
#include <stdint.h>

__attribute__((used))
void *memcpy(void *dst, const void *src, size_t cnt)
{
    uint8_t         *d = (uint8_t *)        dst;
    const uint8_t   *s = (const uint8_t *)  src;

    while (cnt--) {
        *d++ = *s++;
    }

    return dst;
}
