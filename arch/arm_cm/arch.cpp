#include <stdint.h>

extern "C"  void __attribute__((naked)) arch_delay(uint32_t loop_count)
{
    (void)loop_count;
#if defined ( __GNUC__ )
   __asm("    subs    r0, #1\n"
          "    bne    arch_delay\n"
          "    bx     lr");
#else
#error Not yet implemented for your compiler
#endif
}
