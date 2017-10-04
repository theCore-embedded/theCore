# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Platform BAT tests for TivaC target.

cmake_minimum_required(VERSION 3.4)

project(platform_bat)

include(${CMAKE_CURRENT_LIST_DIR}/../../target_defs.cmake)

add_executable(platform_bat ${AUTOGEN_DIR}/main.cpp suite_init.cpp)

target_link_libraries(platform_bat the_core unity utils)

# Avoid particular test. Required mode(s) is not implemented in UART driver yet
# or implemented with some details.
target_compile_definitions(platform_bat PUBLIC
        -DUART_TEST_NO_LISTEN_MODE)

# Expose suite headers to the test cases
include_directories(${CMAKE_CURRENT_LIST_DIR}/export)

# Suite setup not needed - all done by target

# Suite tests
include(${AUTOGEN_DIR}/suite_tests.cmake)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

# Produce complete binary, suitable for flashing to the target
strip_executable(platform_bat)
