/* Avoid collisions with libc by using relative paths */
#include "export/string.h"
#include <stdint.h>

//! \todo Issue #9: this function get optimized out in certain cases
//! if flto is enabled
__attribute__((used))
void * LIBC_FUNCTION(memcpy) (void *dst, const void *src, size_t cnt)
{
    uint8_t         *d = (uint8_t *)        dst;
    const uint8_t   *s = (const uint8_t *)  src;

    while (cnt--) {
        *d++ = *s++;
    }

    return dst;
}
