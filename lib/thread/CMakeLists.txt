# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Add thread support for valid kernel
msg_trace("Checking [CONFIG_OS]...")
if(DEFINED CONFIG_OS)
    msg_info("OS selected: ${CONFIG_OS}")
    if(CONFIG_OS STREQUAL "freertos")
        add_subdirectory(freertos)
    elseif(CONFIG_OS STREQUAL "host")
        add_subdirectory(posix)
    else()
        msg_fatal("No thread support exist for given OS: ${CONFIG_OS}")
    endif()
else()
    msg_info("Be aware: no OS was selected. Thread support will be disabled."
        " Stubs are provided for most synchronization primitives")
    add_subdirectory(no_os)
endif()

# Main library, acting like a proxy.
add_library(thread INTERFACE)

# Implementation of particular thread support resides in thread_impl.
target_link_libraries(thread INTERFACE thread_impl)

# Posix semaphore test.
add_unit_host_test(
    NAME semaphore_posix
    SOURCES tests/semaphore_unit.cpp posix/semaphore.cpp
    INC_DIRS posix/export/
    DEPENDS pthread types
)

# Posix thread tests. Requires semaphores as well.
add_unit_host_test(
    NAME thread_posix
    SOURCES tests/thread_unit.cpp posix/thread.cpp posix/semaphore.cpp
    INC_DIRS posix/export/
    DEPENDS utils types dbg pthread
    COMPILE_OPTIONS -Wno-error=strict-aliasing
)

# Semaphore for platforms without OS layer. Mocking time routines.
add_unit_host_test(
    NAME semaphore_no_os
    SOURCES tests/semaphore_unit.cpp no_os/semaphore.cpp
    INC_DIRS no_os/export/ tests/mocks/
    DEPENDS pthread types
    COMPILE_OPTIONS -DSEMAPHORE_TEST_MOCKED_TIME=1
)

# FreeRTOS semaphore test.
add_unit_host_test(
    NAME semaphore_freertos
    SOURCES tests/semaphore_unit.cpp freertos/semaphore.cpp
    INC_DIRS freertos/export/ tests/mocks/freertos tests/mocks/
    DEPENDS types dbg pthread
)

# Cover FreeRTOS thread with unit test - #211
