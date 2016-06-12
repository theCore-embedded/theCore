cmake_minimum_required(VERSION 3.2)

# The Core framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Objvious.
project(exti_to_uart)

# Required flags for this demo
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Our demo sources.
add_executable(exti main.cpp target.cpp)

# Some dependencies
target_link_libraries(exti the_core)
target_link_libraries(exti libcpp)

# Configuraion parameters

# Target processor platform and its configuration
set(CONFIG_PLATFORM stm32f4xx)
# STM32 discovery is running with SMT32F407VG processor
set(CONFIG_PLATFORM_DEVICE STM32F40_41xxx)
# External oscillator has a 8 MHz out clock
set(CONFIG_PLATFORM_HSE_VAL 8000000)

# Enable console
set(CONFIG_USE_CONSOLE 1)
# UART #3 used for console output
set(CONFIG_CONSOLE_DEVICE_NUM 3)
# Baud rate of the console
set(CONFIG_CONSOLE_BAUD 115200)

# Notify core about our project.
register_project(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/exti)
