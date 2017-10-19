# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(fat STATIC
dir.cpp
file.cpp
dir_inode.cpp
file_inode.cpp
path.cpp
pff/src/diskio.c
pff/src/pff.c
pff/src/diskio.c
)

target_include_directories(fat PUBLIC export)
target_include_directories(fat PUBLIC pff)
target_link_libraries(fat fs)
target_link_libraries(fat allocators)
