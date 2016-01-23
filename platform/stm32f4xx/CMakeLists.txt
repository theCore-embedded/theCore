add_library(stm32f4xx STATIC platform.cpp)
add_library(stm32f4xx_utils STATIC utils.c)

target_include_directories(stm32f4xx PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)
add_subdirectory(CMSIS)

# TODO: clarify essential libraries
target_link_libraries(stm32f4xx spl)
target_link_libraries(stm32f4xx common_spi)
target_link_libraries(stm32f4xx common_pin)
target_link_libraries(stm32f4xx common_usart)

# Notify whole system about which processor is used
set(TARGET_PROCESSOR_ARCHITECTURE "arm_cm4" CACHE STRING "Processor arch")
