# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Linker flags.
# TODO: check redundancy when using -msoft-float and -mfloat-abi=soft together

if(CONFIG_PLATFORM_DEVICE MATCHES "STM32F4")
    set(FAMILY_CORE "-mcpu=cortex-m4")
elseif(CONFIG_PLATFORM_DEVICE MATCHES "STM32L1")
    set(FAMILY_CORE "-mcpu=cortex-m3")
else()
    # Likely that additional implementation is required in order to add new stm32
    # family
    msg_fatal("Device ${CONFIG_PLATFORM_DEVICE} doesn't have any linker flags defined")
endif()

set(CMAKE_C_LINK_FLAGS
    "${FAMILY_CORE} -msoft-float -mfloat-abi=soft \
    -nostartfiles -nostdlib -mthumb -Wl,--gc-sections"
    CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
