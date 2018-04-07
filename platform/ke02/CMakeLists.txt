add_library(ke02 platform.cpp)

include(mcu_cfg.cmake)

set(TARGET_USE_NVIC_IRQ_NAMES 1 CACHE STRING "Platform uses NVIC IRQ names")
set(TARGET_MCU_ARCH "arm_cm0plus" CACHE STRING "MCU architecture")

# Special protect section must be included in ASM
set(TARGET_KINETIS_FLASH_PROTECT_SECTION 1 CACHE STRING "Flash protect for Kinetis")

target_include_directories(ke02 PUBLIC export)
target_link_libraries(ke02 PUBLIC arch kexx_drv)
