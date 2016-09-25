cmake_minimum_required(VERSION 3.4)

# Hello World basic case
project(unity_demo)

include(${APP_CFG}/app_defs.cmake)

add_executable(unity_demo main.cpp)
target_include_directories(unity_demo PUBLIC
        ${TARGET_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/${APP_CFG})

target_link_libraries(unity_demo the_core unity)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
