add_library(bus INTERFACE)
target_include_directories(bus INTERFACE export)
target_link_libraries(bus INTERFACE dbg platform_common thread)

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

add_unit_host_test(NAME serial
		   SOURCES tests/serial_unit.cpp
		   INC_DIRS export tests/mocks)
