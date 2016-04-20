#include "string.h"

void * LIBC_FUNCTION(memmove) (void *dst_void, const void *src_void, size_t length)
{
    // Grabbed from newlib implementation
    char *dst = dst_void;
    const char *src = src_void;

    if (src < dst && dst < src + length)
    {
        /* Have to copy backwards */
        src += length;
        dst += length;
        while (length--)
        {
            *--dst = *--src;
        }
    }
    else
    {
        while (length--)
        {
            *dst++ = *src++;
        }
    }

    return dst_void;
}
