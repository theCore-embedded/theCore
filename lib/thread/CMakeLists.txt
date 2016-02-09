# Add thread support for valid kernel
if (DEFINED CONFIG_OS)
	message(STATUS "OS selected: ${CONFIG_OS}")
	if (${CONFIG_OS} STREQUAL "freertos")
		add_subdirectory(freertos)
	else(${CONFIG_OS} STREQUAL "host")
		add_subdirectory(std_host)
	else()
		message(FATAL_ERROR "	No thread support exist for given OS")
	endif()
else()
	message(STATUS "Be aware: no OS was selected. Thread support will be disabled."
		" Stubs are provided for most synchronization primitives")
	add_subdirectory(default)
endif()

add_subdirectory(common)

# Tests.

# Semaphore test will be triggered for implementations based on the
# host environment only.
# This includes 'default' and 'std_host' implementations.
# TODO: add here 'if' statemenent to somehow emphasize that some tests
# are included depending on which library is used.
add_unit_host_test(
	NAME test_semaphore
	SOURCES tests/semaphore_unit.cpp
	# TODO: proper dependenices! What is about compilation on Win system?
	DEPENDS thread pthread
	)

