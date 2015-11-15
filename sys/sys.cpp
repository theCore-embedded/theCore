#include "sys/pin_cfgs.h"

#include <cstdint>
#include <cstddef>

#include <platform/irq_manager.hpp>
#include <ecl/iostream.hpp>

extern "C" __attribute__((used))
void vTaskSwitchContext( void );


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

extern "C" __attribute__((used))
void *memset(void *s, int c, size_t n)
{
    uint8_t *dest = (uint8_t *) s;

    while (n--) {
        *dest++ = c;
    }

    return s;
}

extern "C" __attribute__((used))
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

namespace ecl {
// TODO: avoid this somehow.
// See https://isocpp.org/wiki/faq/ctors#static-init-order
typename istream< console_driver >::device_type cin_device;
typename istream< console_driver >::device_type cout_device;
typename istream< console_driver >::device_type cerr_device;

ecl::istream< console_driver >  cin{&cin_device};
ecl::ostream< console_driver >  cout{&cout_device};
ecl::ostream< console_driver >  cerr{&cout_device};
}


extern "C" void early_main(void)
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

    // Due to undefined static init order, this initialization is put here
    // See note above
    ecl::cin_device.init();
    ecl::cin_device.open();
    ecl::cout_device.init();
    ecl::cout_device.open();
    ecl::cerr_device.init();
    ecl::cerr_device.open();

    // Make sure IRQ is ready
    IRQ_manager::init();
}

namespace std
{
void __throw_bad_function_call()
{
	// TODO: abort
	for (;;);
}
}
