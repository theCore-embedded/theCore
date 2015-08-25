# Make sure appropriate MCU type is chosen
add_definitions("-DSTM32F40_41xxx")

include_directories(SYSTEM SPL/inc)
include_directories(SYSTEM SPL/arm)
include_directories(SYSTEM SPL/st)
include_directories(includes)

## Qt creator hack
file(GLOB_RECURSE ALL_PLATFORM_HEADERS *.hpp)
add_custom_target(platform_dummy_target SOURCES "${ALL_PLATFORM_HEADERS}")

add_library(platform STATIC platform.cpp ${ALL_PLATFORM_HEADERS})

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)

# TODO: clarify essential libraries
target_link_libraries(platform startup)
target_link_libraries(platform spl)
