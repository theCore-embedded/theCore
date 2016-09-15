cmake_minimum_required(VERSION 3.4)

# Hello World basic case
project(thecore_hello_world_case)

include(${APP_CFG}/app_defs.cmake)

add_executable(hello_world main.cpp)
target_include_directories(hello_world PUBLIC
        ${TARGET_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/${APP_CFG})

target_link_libraries(hello_world the_core libcpp)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
