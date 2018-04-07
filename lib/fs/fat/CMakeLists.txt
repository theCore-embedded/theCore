# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

theCore_get_thirdparty(fat https://github.com/forGGe/petite-fat 49a5b17)

set(FAT_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/fat)

# Some headers must be substituted, thus first copied

set(FAT_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/export/pff/)
set(FAT_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/export/pff/src)

file(COPY
        ${FAT_DIR}/src/diskio.h
        ${FAT_DIR}/src/integer.h
        ${FAT_DIR}/src/pff.h
        ${CMAKE_CURRENT_LIST_DIR}/pffconf.h
        ${FAT_DIR}/src/diskio.c
        ${FAT_DIR}/src/pff.c
    DESTINATION ${FAT_SRC_DIR}
)

add_library(fat STATIC
    dir.cpp
    file.cpp
    dir_inode.cpp
    file_inode.cpp
    ${FAT_SRC_DIR}/diskio.c
    ${FAT_SRC_DIR}/pff.c
)

target_include_directories(fat PUBLIC export ${FAT_INC_DIR})
target_link_libraries(fat fs allocators)