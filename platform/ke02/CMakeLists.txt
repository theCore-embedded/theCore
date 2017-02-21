add_library(ke02 platform.cpp)

include(mcu_cfg.cmake)

set(TARGET_USE_NVIC_IRQ_NAMES 1 CACHE STRING "Platform uses NVIC IRQ names")
set(TARGET_MCU_ARCH "arm_cm0plus" CACHE STRING "MCU architecture")

target_include_directories(ke02 PUBLIC export)
target_link_libraries(ke02 PUBLIC arch)
