#include "arch/execution.hpp"

namespace ecl
{

#if USE_SYSTMR

static volatile uint32_t event_cnt;

namespace systmr
{

uint32_t events()
{
    return event_cnt;
}

} // namespace systmr

#endif // USE_SYSTMR

} // namespace ecl

#if USE_SYSTMR

// User can override systimer handler to receive events on its side.
extern "C" void systmr_handler(void) __attribute__((weak));

extern "C" void systmr_handler(void)
{
}

extern "C" void SysTick_Handler(void)
{
    ecl::event_cnt++;
    systmr_handler();
}

extern "C"  void __attribute__((naked)) arch_delay(uint32_t loop_count)
{
    (void)loop_count;
#if defined ( __GNUC__ )
   __asm("    subs    r0, #1\n"
          "    bne    arch_delay\n"
          "    bx     lr");
#endif
}

#endif // USE_SYSTMR
