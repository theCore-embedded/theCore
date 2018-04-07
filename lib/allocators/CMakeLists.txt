# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(allocators INTERFACE)
target_sources(allocators INTERFACE ${CMAKE_CURRENT_LIST_DIR}/alloc.cpp)
target_include_directories(allocators INTERFACE export)

# Requires assert
target_link_libraries(allocators INTERFACE dbg types)
# Static analysis
add_cppcheck(allocators UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)

# Unit tests
add_unit_host_test(NAME pool_allocator
        SOURCES
        tests/pool_base_unit.cpp
        tests/pool_unit.cpp
        tests/pool_main.cpp
        alloc.cpp
        INC_DIRS export
        DEPENDS core_cpp dbg)
