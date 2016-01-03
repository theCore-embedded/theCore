cmake_minimum_required(VERSION 2.8)

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(target_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(${TARGET_NAME} STATIC target.cpp pinconfig.cpp ${ALL_TARGET_HEADERS})

target_include_directories(${TARGET_NAME} PUBLIC export)

target_link_libraries(${TARGET_NAME} libcpp)
target_link_libraries(${TARGET_NAME} emc)

# STM32 discovery is running with SMT32F407VG processor
target_compile_definitions(${TARGET_NAME} PUBLIC -DSTM32F40_41xxx)
# External oscillator has a 8 MHz out clock
target_compile_definitions(${TARGET_NAME} PUBLIC -DHSE_VALUE=8000000)

# Register platform as a base for this target
link_target_with_platform()
