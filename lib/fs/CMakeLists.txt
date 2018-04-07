# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Create directory for auto-generated filesystem confgiration
file(MAKE_DIRECTORY ${CORE_GEN_DIR}/export/fs)

add_library(fs STATIC inode.cpp file_descriptor.cpp dir_descriptor.cpp)
target_include_directories(fs PUBLIC export ${CORE_GEN_DIR}/export)
target_link_libraries(fs PUBLIC core_cpp dbg utils)

add_cppcheck(fs)

add_subdirectory(fat)

theCore_create_cog_runner(
        IN      ${CMAKE_CURRENT_LIST_DIR}/templates/fs_cfg.in.hpp
        OUT     ${CORE_GEN_DIR}/export/fs/fs_cfg.hpp
        DEPENDS ${THECORE_TARGET_CONFIG_FILE}
        ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

theCore_create_cog_runner(
        IN      ${CMAKE_CURRENT_LIST_DIR}/templates/fs_defines.in.h
        OUT     ${CORE_GEN_DIR}/export/fs/fs_defines.h
        DEPENDS ${THECORE_TARGET_CONFIG_FILE}
        ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

add_custom_target(fs_generated DEPENDS
        ${CORE_GEN_DIR}/export/fs/fs_cfg.hpp
        ${CORE_GEN_DIR}/export/fs/fs_defines.h)
add_dependencies(fs fs_generated)

# Unit tests
add_unit_host_test(NAME fs
        SOURCES
        tests/fs_unit.cpp
        INC_DIRS export
        DEPENDS core_cpp dbg)
