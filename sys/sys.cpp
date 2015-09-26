#include <cstdint>
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

	extern uint32_t ___init_array_start;
	extern uint32_t ___init_array_end;


	extern "C" void static_init(void)
	{
		for (uint32_t *p = &___init_array_start; p < &___init_array_end; ++p) {
			// Iterator points to a memory which contains an address of a
			// initialization function.
			// Equivalent of:
			// void (*fn)() = p;
			// fn();
			((void (*)()) *p)();
		}
	}

