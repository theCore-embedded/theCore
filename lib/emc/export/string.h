#ifndef LIB_EMC_STRING_H_
#define LIB_EMC_STRING_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memmove(void *dst, const void *src, size_t cnt);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t cnt);

#ifdef __cplusplus
}
#endif

#endif
