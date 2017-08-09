#include "arch/execution.hpp"

namespace ecl
{

#if USE_SYSTMR
namespace systmr
{

static volatile uint32_t event_cnt;

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
    ecl::systmr::event_cnt++;
    systmr_handler();
}

#endif // USE_SYSTMR
