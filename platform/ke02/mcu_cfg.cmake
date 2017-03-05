# Memory layout
# TODO: differentiate layout with respect to full device specification
# TODO: #107 - allow user to override these values
set(TARGET_MCU_ROM_START 0x00000000 CACHE STRING "Flash region start")
set(TARGET_MCU_ROM_SIZE  0xffff     CACHE STRING "Flash region size")  # 64k of ROM
set(TARGET_MCU_RAM_START 0x20000000 CACHE STRING "RAM region start")
set(TARGET_MCU_RAM_SIZE  0xc00      CACHE STRING "RAM region size")    # 3072 bytes of RAM

# TODO: Provide two-segment memory values. (see RM)

set(TARGET_STACK_SIZE    0xff     CACHE STRING "Stack size")
set(TARGET_HEAP_SIZE     0xff     CACHE STRING "Heap size")
