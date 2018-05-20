#ifndef KE02_PLATFORM_EXECUTION_H_
#define KE02_PLATFORM_EXECUTION_H_

#include <stdint.h>

__attribute__((noreturn))
void ecl_abort()
{
    for(;;);
}

static inline uint32_t get_clk_spd()
{
    // TODO
    return 100000;
}

static inline uint32_t get_clk()
{
    return 100000;
}




#endif // KE02_PLATFORM_EXECUTION_H_
