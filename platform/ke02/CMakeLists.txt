# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(ke02 platform.cpp)

include(mcu_cfg.cmake)

target_include_directories(ke02 PUBLIC export)
target_link_libraries(ke02 PUBLIC arch kexx_drv)
