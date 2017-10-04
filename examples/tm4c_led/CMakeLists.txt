# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.2)

# Objvious.
project(tm4c_led)

# theCore framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Our demo sources.
add_executable(tm4c_led main.cpp target.cpp)

# Some dependencies
target_link_libraries(tm4c_led the_core core_cpp)

# Configuraion parameters

# Target processor platform and its configuration
set(CONFIG_PLATFORM tm4c)
# STM32 discovery is running with STM32F407VG processor
set(CONFIG_PLATFORM_DEVICE TM4C123GH6PM)

# Enable console
set(CONFIG_USE_CONSOLE 1)
# USART #2 used for console output
set(CONFIG_CONSOLE_DEVICE_NUM 0)
# Baud rate of the console
set(CONFIG_CONSOLE_BAUD 115200)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(tm4c_led)
