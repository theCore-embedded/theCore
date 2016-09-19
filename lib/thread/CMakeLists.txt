# Add thread support for valid kernel
message(STATUS "Checking [CONFIG_OS]...")
if(DEFINED CONFIG_OS)
	message(STATUS "OS selected: ${CONFIG_OS}")
	if(${CONFIG_OS} STREQUAL "freertos")
		add_subdirectory(freertos)
	else(${CONFIG_OS} STREQUAL "host")
		if(${UNIX})
			add_subdirectory(posix)
		else(${WIN32})
			# Currently missing
			add_subdirectory(win32)
		else()
			# Please, implement it if you see this message
			message(FATAL_ERROR "	Your host system has no known thread support")
		endif()
	else()
		message(FATAL_ERROR "	No thread support exist for given OS")
	endif()
else()
	message(STATUS "Be aware: no OS was selected. Thread support will be disabled."
		" Stubs are provided for most synchronization primitives")
	add_subdirectory(default)
endif()

add_subdirectory(common)

# TODO: comment about lib names
add_library(thread lock_guard.cpp)
target_include_directories(thread PUBLIC export)
target_link_libraries(thread PUBLIC thread_os)

# Tests.

# Semaphore test will be triggered for implementations based on the
# host environment only.
# This includes 'default' and 'std_host' implementations.
# TODO: add here 'if' statemenent to somehow emphasize that some tests
# are included depending on which library is used.
add_unit_host_test(
	NAME semaphore
	SOURCES tests/semaphore_unit.cpp
	# TODO: proper dependenices! What is about compilation on Win system?
	DEPENDS thread_os pthread
)

if ("${CONFIG_OS}" STREQUAL "host")
	add_unit_host_test(
		NAME thread
		SOURCES tests/thread_unit.cpp
		# TODO: proper dependenices! What is about compilation on Win system?
		DEPENDS thread_os pthread
	)
endif()
