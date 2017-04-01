#include "arch/execution.hpp"

namespace ecl
{

#if USE_SYSTMR_SYSTICK

static uint32_t event_cnt;

namespace systmr
{

uint32_t events()
{
    return event_cnt;
}

} // namespace systmr

#endif // USE_SYSTMR_SYSTICK

} // namespace ecl

#if USE_SYSTMR_SYSTICK

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

#endif // USE_SYSTMR_SYSTICK
