#include <cstdint>
#include <cstddef>

#include "sys/pin_cfgs.h"

// TODO: decide if to keep this here or not
extern "C" int  __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
	(void) object;
	(void) destroyer;
	(void) dso_handle;
	return 0;
}

// TODO: decide if to keep this here or not

extern "C" void vAssertCalled(const char *file, int line)
{
    (void) file;
    (void) line;
    for(;;);
}

extern "C"
void * memset(void *s, int c, size_t n)
{
    uint8_t *dest = (uint8_t *) s;

    while (n--) {
        *dest++ = c;
    }

    return s;
}

extern "C"
// TODO: HACK - move it somewhere
void *memcpy(void *dst, const void *src, size_t cnt)
{
    uint8_t         *d = (uint8_t *)        dst;
    const uint8_t   *s = (const uint8_t *)  src;

    while (cnt--) {
        *d++ = *s++;
    }

    return dst;
}

extern "C" void static_init(void)
{
	extern uint32_t ___init_array_start;
	extern uint32_t ___init_array_end;

	for (uint32_t *p = &___init_array_start; p < &___init_array_end; ++p) {
		// Iterator points to a memory which contains an address of a
		// initialization function.
		// Equivalent of:
		// void (*fn)() = p;
		// fn();
		((void (*)()) *p)();
	}
}

namespace std
{
void __throw_bad_function_call()
{
	// TODO: abort
	for (;;);
}
}
