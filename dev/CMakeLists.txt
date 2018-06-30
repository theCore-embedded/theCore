# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Create directory for auto-generated device files
file(MAKE_DIRECTORY ${CORE_GEN_DIR}/export/dev)
file(MAKE_DIRECTORY ${CORE_GEN_DIR}/export/dev/sensor)

add_subdirectory(pcd8544)
add_subdirectory(bus)
add_subdirectory(sensor)
add_subdirectory(cs43l22)
add_subdirectory(hm10)
add_subdirectory(sdspi)

# Proxy library to aggregate device drivers
# TODO: aggregate ALL devices
add_library(thecore_dev INTERFACE)
target_include_directories(thecore_dev INTERFACE export)
target_link_libraries(thecore_dev INTERFACE sdspi cs43l22 htu21d hm10)
