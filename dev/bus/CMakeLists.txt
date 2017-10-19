# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(bus INTERFACE)
target_include_directories(bus INTERFACE export)
target_link_libraries(bus INTERFACE dbg platform_common thread utils)

add_unit_host_test(NAME bus
                    SOURCES tests/bus_unit.cpp
                    # Use standart semaphore
                    ${CORE_DIR}/lib/thread/no_os/semaphore.cpp
                    # But mock mutex
                    tests/mocks/mutex.cpp
                    DEPENDS platform_common dbg
                    INC_DIRS export tests/mocks
                    # To provide headers for semaphore/mutex
                    ${CORE_DIR}/lib/thread/no_os/export)

find_package(Threads REQUIRED)

add_unit_host_test(NAME serial
                    SOURCES tests/serial_unit.cpp
                    ${CORE_DIR}/lib/types/err.cpp
                    DEPENDS thread dbg platform_common utils ${CMAKE_THREAD_LIBS_INIT}
                    INC_DIRS export tests/mocks)
