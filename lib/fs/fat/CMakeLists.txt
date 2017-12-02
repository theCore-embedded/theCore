# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

theCore_get_thirdparty(fat https://github.com/forGGe/petite-fat 5dca528)

set(FAT_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/fat)
add_library(fat STATIC
    dir.cpp
    file.cpp
    dir_inode.cpp
    file_inode.cpp
    path.cpp
    ${FAT_DIR}/src/diskio.c
    ${FAT_DIR}/src/pff.c
    ${FAT_DIR}/src/diskio.c
)

target_include_directories(fat PUBLIC export)
target_include_directories(fat PUBLIC ${FAT_DIR}/)
target_link_libraries(fat fs)
target_link_libraries(fat allocators)
