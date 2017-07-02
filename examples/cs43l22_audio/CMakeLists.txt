cmake_minimum_required(VERSION 3.2)

# theCore framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Objvious.
project(cs43l22_audio)

# Our demo sources.
add_executable(audio main.cpp target.cpp)

# Some dependencies
target_link_libraries(audio the_core core_cpp cs43l22)

# Configuraion parameters

# Target processor platform and its configuration
set(CONFIG_PLATFORM stm32)
# STM32 discovery is running with STM32F407VG processor
set(CONFIG_PLATFORM_DEVICE STM32F407VG)
# External oscillator has a 8 MHz out clock
set(CONFIG_PLATFORM_HSE_VAL 8000000)

# Enable console
set(CONFIG_USE_CONSOLE 1)
# UART #3 used for console output
set(CONFIG_CONSOLE_DEVICE_NUM 3)
# Baud rate of the console
set(CONFIG_CONSOLE_BAUD 115200)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(audio)
