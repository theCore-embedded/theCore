# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Unity demo on STM32F4 Discovery target.

cmake_minimum_required(VERSION 3.4)

project(concurrency_suite)

include(${CORE_DIR}/build_api.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../target_defs.cmake)

add_executable(concurrency_suite ${AUTOGEN_DIR}/main.cpp suite_init.cpp)

target_link_libraries(concurrency_suite the_core unity thread)

# Suite setup not needed - all done by target

# Suite tests
include(${AUTOGEN_DIR}/suite_tests.cmake)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(concurrency_suite)
