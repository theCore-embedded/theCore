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
add_executable(tm4c_led main.cpp)

# Some dependencies
target_link_libraries(tm4c_led the_core core_cpp)

# Configuration parameters
theCore_set_target_config(${CMAKE_CURRENT_LIST_DIR}/tm4c_led.json)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(tm4c_led)
