cmake_minimum_required(VERSION 3.2)

# theCore framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Objvious.
project(host_hello)

# Required flags for this demo
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Our demo sources.
add_executable(hello main.cpp)

# Pick the host platform
set(CONFIG_PLATFORM host)

# Obvious
set(CONFIG_RUN_CPPCHECK_DURING_BUILD true)

# Some dependencies
target_link_libraries(hello the_core)
target_link_libraries(hello core_cpp)
target_link_libraries(hello host)

# Enable console.
set(CONFIG_USE_CONSOLE 1)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
