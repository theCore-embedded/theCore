#include <cstdint>
#include <cstddef>

#include <platform/irq_manager.hpp>
#include <platform/utils.hpp>
#include <ecl/iostream.hpp>

// TODO: move it somewhere
void operator delete(void *)
{
    // TODO: call to abort routine
    for(;;);
}

// TODO: move it somewhere
void operator delete(void *, unsigned int)
{
    // TODO: call to abort routine
    for(;;);
}

// TODO: move this to toolchain-dependent module
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

// TODO: move this to toolchain-dependent module
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

// TODO: move this to toolchain-dependent module
extern "C" __attribute__((noreturn))
void __stack_chk_fail(void)
{
    // TODO: call to abort routine
    for(;;);
}

// TODO: move this to toolchain-dependent module
extern "C"
int atexit (void (*func)(void))
{
    (void) func;
    return 0;
}

// TODO: move this to toolchain-dependent module
extern "C"
int __cxa_guard_acquire(int *gv)
{
    // Disable interrupts to prevent concurent access
    ecl::disable_irq();

    if (*gv == 1) {
        // Already locked
        return 0;
    } else {
        *gv = 1;
        return 1;
    }
}

// TODO: move this to toolchain-dependent module
extern "C"
void __cxa_guard_release(int *gv)
{
    (void) gv;
    // Object constructed. It is safe to enable interrupts.
    ecl::enable_irq();
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

    // Due to undefined static init order, this initialization is placed here
    ecl::console_device.init();

    kernel_main();
}

// TODO: move this to toolchain-dependent module
extern "C" void __cxa_pure_virtual()
{
    // Abort
    for(;;);
}

namespace std
{
// TODO: move this to toolchain-dependent module
void __throw_bad_function_call()
{
    // TODO: abort
    for (;;);
}
}
