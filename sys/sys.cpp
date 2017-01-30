//!
//! \file
//! \brief The Core system initialization module.
//!
#include <stdint.h>
#include <stddef.h>

#include <common/irq.hpp>
#include <platform/console.hpp>
#include <common/execution.h>

// TODO: move it somewhere
void operator delete(void *) noexcept
{
    // TODO: call to abort routine
    for(;;);
}

// TODO: move it somewhere
void operator delete(void *, unsigned int) noexcept
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
    ecl::irq::disable();

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
    ecl::irq::enable();
}

extern "C" void platform_init();
extern "C" void board_init();
extern "C" void kernel_main();
int main();

//! Lowest level C-routine inside the Core
//! \details Performs essential initialization before construction of C++ objects
//! and kernel initialization.
extern "C" void core_main()
{
    platform_init();
    board_init();

#ifdef CONFIG_USE_BYPASS_CONSOLE
	// Dirty hack to make sure pin configuration is established before
	// bypass console will be used.
	// It should be fixed by configuring console GPIO directly in the platform,
    // not in the user's `board_init()` routine. See issue #151.
    ecl_spin_wait(50);
#endif // CONFIG_USE_BYPASS_CONSOLE
    kernel_main();
}

//! Early-main routine performs final initialization steps.
//! \details All global objects must be constructed before entering user's
//! main routine.
//! Kernel in responsible for calling early_main() after it (kernel) will be
//! completely ready.
//! \todo Consider specifying it with noreturn attribute.
extern "C" void early_main()
{
    // Platform console subsystem is ready at this stage
    for (auto c : "\r\nWelcome to theCore\r\n") { ecl::bypass_putc(c); }

    extern uint32_t __init_array_start;
    extern uint32_t __init_array_end;

    for (uint32_t *p = &__init_array_start; p < &__init_array_end; ++p) {
        // Iterator points to a memory which contains an address of a
        // initialization function.
        // Equivalent of:
        // void (*fn)() = p;
        // fn();
        ((void (*)()) *p)();
    }

    main();
    for(;;); // TODO: call to the abort routine
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
