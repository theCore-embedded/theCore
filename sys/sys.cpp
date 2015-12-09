#include "sys/pin_cfgs.h"

#include <cstdint>
#include <cstddef>

#include <platform/irq_manager.hpp>
#include <ecl/iostream.hpp>

extern "C" __attribute__((used))
void vTaskSwitchContext( void );

// TODO: decide if to keep this here or not

// TODO: move it somewhere
void operator delete(void *)
{
    // Abort - delete is forbidden
    for (;;);
}

// TODO: move it somewhere
void operator delete(void *, unsigned int)
{
    // Abort - delete is forbidden
    for (;;);
}


#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

extern "C" __attribute__((noreturn))
void __stack_chk_fail(void)
{
    ecl::cout << "Fail!!!" << ecl::endl;
    for(;;);
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

extern "C" void __cxa_pure_virtual()
{
    // Abort
    for(;;);
}

namespace std
{
void __throw_bad_function_call()
{
	// TODO: abort
	for (;;);
}
}
