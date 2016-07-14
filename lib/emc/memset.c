/* Avoid collisions with libc by using relative paths */
#include "export/string.h"

#include <stdint.h>

__attribute__((used))
void * LIBC_FUNCTION(memset) (void *s, int c, size_t n)
{
    uint8_t *dest = (uint8_t *) s;

    while (n--) {
        *dest++ = c;
    }

    return s;
}
