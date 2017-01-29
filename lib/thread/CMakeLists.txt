# Add thread support for valid kernel
message(STATUS "Checking [CONFIG_OS]...")
if(DEFINED CONFIG_OS)
    message(STATUS "OS selected: ${CONFIG_OS}")
    if(CONFIG_OS STREQUAL "freertos")
        add_subdirectory(freertos)
    elseif(CONFIG_OS STREQUAL "host")
        add_subdirectory(posix)
    else()
        message(FATAL_ERROR "No thread support exist for given OS: ${CONFIG_OS}")
    endif()
else()
    message(STATUS "Be aware: no OS was selected. Thread support will be disabled."
        " Stubs are provided for most synchronization primitives")
    add_subdirectory(no_os)
endif()

# Main library, acting like a proxy.
add_library(thread INTERFACE)

# Implementation of particular thread support resides in thread_impl.
target_link_libraries(thread INTERFACE thread_impl)

# Posix semaphore test. No need to mock anything - platform provides
# desired implementation.
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
)

# Semaphore for platforms without OS layer. No mocks needed, only atomic support.
add_unit_host_test(
    NAME semaphore_no_os
    SOURCES tests/semaphore_unit.cpp no_os/semaphore.cpp
    INC_DIRS no_os/export/
    DEPENDS pthread types
)

# FreeRTOS semaphore test.
add_unit_host_test(
    NAME semaphore_freertos
    SOURCES tests/semaphore_unit.cpp freertos/semaphore.cpp
    INC_DIRS freertos/export/ tests/mocks/freertos
    DEPENDS types dbg pthread
)

# Cover FreeRTOS thread with unit test - #211
