# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(cs43l22 INTERFACE)
target_link_libraries(cs43l22 INTERFACE types)
target_include_directories(cs43l22 INTERFACE export)
