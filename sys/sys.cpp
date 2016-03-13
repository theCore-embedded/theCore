#include <cstdint>
#include <cstddef>

#include <platform/irq_manager.hpp>
#include <ecl/iostream.hpp>

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

__attribute__((used))
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

extern "C" __attribute__((noreturn)) __attribute__((used))
void __stack_chk_fail(void)
{
    ecl::cout << "Fail!!!" << ecl::endl;
    for(;;);
}

extern "C" void platform_init();
extern "C" void board_init();
extern "C" void kernel_init();
extern "C" void kernel_main();

namespace ecl {
extern typename istream< console_driver >::device_type console_device;
}

extern "C" void core_main(void)
{
    platform_init();
    board_init();
    kernel_init();

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

    IRQ_manager::init();

    // Due to undefined static init order, this initialization is placed here
    ecl::console_device.init();


    kernel_main();
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
