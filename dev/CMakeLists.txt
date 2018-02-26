# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_subdirectory(pcd8544)
add_subdirectory(bus)
add_subdirectory(sensor)
add_subdirectory(cs43l22)
add_subdirectory(hm10)
add_subdirectory(sdspi)

# Proxy library to aggregate device drivers
add_library(thecore_dev INTERFACE)
target_link_libraries(thecore_dev INTERFACE sdspi)
