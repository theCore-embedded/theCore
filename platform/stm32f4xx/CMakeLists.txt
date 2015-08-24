add_library(platform STATIC platform.cpp)

# Make sure appropriate MCU type is chosen
add_definitions("-DSTM32F40_41xxx")

include_directories(pins)

include_directories(SPL/inc)
include_directories(SPL/arm)
include_directories(SPL/st)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(pins)
add_subdirectory(SPL)

# TODO: clarify essential libraries
target_link_libraries(platform startup)
target_link_libraries(platform pins)
target_link_libraries(platform spl)
