# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(dbg STATIC ${CMAKE_CURRENT_LIST_DIR}/assert.cpp)
target_include_directories(dbg PUBLIC export types)
target_link_libraries(dbg PUBLIC platform_common utils ${PLATFORM_NAME})
