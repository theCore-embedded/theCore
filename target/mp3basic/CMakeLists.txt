cmake_minimum_required(VERSION 2.8)

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(target_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(target STATIC target.cpp pinconfig.cpp ${ALL_TARGET_HEADERS})

target_include_directories(target PUBLIC export)

# TODO: move it to register platform ?
target_link_libraries(target platform)

# STM32 discovery is running with SMT32F407VG processor
target_compile_definitions(target PUBLIC -DSTM32F40_41xxx)
# External oscillator has a 8 MHz out clock
target_compile_definitions(target PUBLIC -DHSE_VALUE=8000000)

# Register platform as a base for this target
register_platform()
