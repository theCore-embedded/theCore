# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.2)

# theCore framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Obvious.
project(hm10_example)

# Our demo sources.
add_executable(hm10_example main.cpp)

# Some dependencies
target_link_libraries(hm10_example the_core hm10)

# STM32F4 Discovery configuration parameters
theCore_set_target_config(${CMAKE_CURRENT_LIST_DIR}/stm32f4_hm10.json)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(hm10_example)
