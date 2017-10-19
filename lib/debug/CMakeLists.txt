# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(dbg INTERFACE)
target_sources(dbg INTERFACE ${CMAKE_CURRENT_LIST_DIR}/assert.cpp)
target_include_directories(dbg INTERFACE export types)
target_link_libraries(dbg INTERFACE ${PLATFORM_NAME})
