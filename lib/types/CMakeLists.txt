# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(types INTERFACE)
target_sources(types INTERFACE ${CMAKE_CURRENT_LIST_DIR}/err.cpp)
target_include_directories(types INTERFACE export)
