# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(htu21d INTERFACE)
target_link_libraries(htu21d INTERFACE types)
target_include_directories(htu21d INTERFACE export)
