# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(fc28 INTERFACE)
target_include_directories(fc28 INTERFACE export)
target_link_libraries(fc28 INTERFACE platform_common types dbg)

add_unit_host_test(NAME fc28
        SOURCES tests/fc28_unit.cpp
        INC_DIRS export
        DEPENDS types platform_common dbg)
