# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(bh1750 INTERFACE)
target_link_libraries(bh1750 INTERFACE types)
target_link_libraries(bh1750 INTERFACE thread)
target_include_directories(bh1750 INTERFACE export)
