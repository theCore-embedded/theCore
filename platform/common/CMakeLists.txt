# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(PLATFORM_NAME STREQUAL host)
    add_library(platform_common INTERFACE)
    target_include_directories(platform_common INTERFACE export)
else()
    # IRQ management must be enabled only if cross-compiling for embedded boards

    add_library(platform_common irq.cpp)

    target_include_directories(platform_common PUBLIC export)

    target_link_libraries(platform_common PUBLIC ${PLATFORM_NAME} dbg types utils)

    target_compile_definitions(platform_common PRIVATE -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})
endif()

add_unit_host_test(NAME mmio
        SOURCES tests/mmio_unit.cpp
        INC_DIRS export)



