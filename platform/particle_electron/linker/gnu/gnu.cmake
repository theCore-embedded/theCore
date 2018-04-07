# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(CMAKE_C_LINK_FLAGS
        "--specs=nano.specs -fno-strict-aliasing -ffunction-sections \
        -fdata-sections -mcpu=cortex-m3 -mthumb -nostartfiles -fno-builtin \
        -nostartfiles -Xlinker --gc-sections"
    CACHE STRING "Linker C flags" FORCE)
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags" FORCE)
