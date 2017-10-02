# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(newlib_stubs INTERFACE)
target_sources(newlib_stubs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stubs.cpp)
target_link_libraries(newlib_stubs INTERFACE platform_common ${CONFIG_PLATFORM_NAME})
