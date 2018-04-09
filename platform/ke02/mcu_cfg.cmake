# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(TARGET_USE_NVIC_IRQ_NAMES 1 CACHE STRING "Platform uses NVIC IRQ names")
set(TARGET_MCU_ARCH "arm_cm0plus" CACHE STRING "MCU architecture")

# Special protect section must be included in ASM
set(TARGET_KINETIS_FLASH_PROTECT_SECTION 1 CACHE STRING "Flash protect for Kinetis")

# Memory layout
# TODO: differentiate layout with respect to full device specification
# TODO: #107 - allow user to override these values
set(TARGET_MCU_ROM_START 0x00000000 CACHE STRING "Flash region start")
set(TARGET_MCU_ROM_SIZE  0xffff     CACHE STRING "Flash region size")  # 64k of ROM
set(TARGET_MCU_RAM_START 0x20000000 CACHE STRING "RAM region start")
set(TARGET_MCU_RAM_SIZE  0xc00      CACHE STRING "RAM region size")    # 3072 bytes of RAM

# TODO: Provide two-segment memory values. (see RM)

set(TARGET_STACK_SIZE    0x100      CACHE STRING "Stack size")
set(TARGET_HEAP_SIZE     0x200      CACHE STRING "Heap size")
