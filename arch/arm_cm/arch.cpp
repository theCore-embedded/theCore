/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
