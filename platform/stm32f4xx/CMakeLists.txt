## Qt creator hack
file(GLOB_RECURSE ALL_PLATFORM_HEADERS *.hpp)
add_custom_target(platform_dummy_target SOURCES "${ALL_PLATFORM_HEADERS}")
## EO hack

add_library(stm32f4xx STATIC platform.cpp)# ${ALL_PLATFORM_HEADERS})

target_include_directories(stm32f4xx PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)
add_subdirectory(CMSIS)

# TODO: clarify essential libraries
target_link_libraries(stm32f4xx startup)
target_link_libraries(stm32f4xx spl)

# Notify whole system about which processor is used
set(TARGET_PROCESSOR_ARCHITECTURE "arm_cm4" CACHE STRING "HELLO")
