add_library(platform platform.cpp)

# Make sure appropriate MCU type is chosen
add_definitions("-DSTM32F40_41xxx")

include_directories(pins)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(pins)
add_subdirectory(SPL)

# TODO: clarify essential libraries
target_link_libraries(platform startup)
target_link_libraries(platform pins)
target_link_libraries(spl)
