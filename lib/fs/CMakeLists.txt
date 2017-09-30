# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Some platforms may not feel well with definitions in FAT headers
# TODO: fix it.
if(THECORE_FORCE_NO_FS_MODULE)
    return()
endif()

add_library(fs STATIC fs.cpp inode.cpp file_descriptor.cpp dir_descriptor.cpp)
target_include_directories(fs PUBLIC export)
target_link_libraries(fs PUBLIC core_cpp dbg utils)

add_cppcheck(fs)

add_subdirectory(fat)

