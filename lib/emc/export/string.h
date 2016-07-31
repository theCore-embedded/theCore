#ifndef LIB_EMC_STRING_H_
#define LIB_EMC_STRING_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* To avoid collisions between hosted libc and existing implementation
 * names must be mangled */
#if EXTERNAL_LIBC
#define LIBC_FUNCTION(name) ecl_##name
#else
#define LIBC_FUNCTION(name) name
#endif

void *LIBC_FUNCTION(memmove) (void *dst, const void *src, size_t cnt);
void *LIBC_FUNCTION(memset) (void *s, int c, size_t n);
void *LIBC_FUNCTION(memcpy) (void *dst, const void *src, size_t cnt);

int LIBC_FUNCTION(strcmp) (const char *str1, const char *str2);
int LIBC_FUNCTION(strcmpi) (const char *str1, const char *str2);
const char* LIBC_FUNCTION(strchr) (const char *str, int character);
size_t LIBC_FUNCTION(strlen) (const char *str);

#ifdef __cplusplus
}
#endif

#endif
