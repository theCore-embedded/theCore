# Add thread support for valid kernel
if (DEFINED CONFIG_OS)
	if (${CONFIG_OS} STREQUAL "freertos")
		add_subdirectory(freertos)
	else(${CONFIG_OS} STREQUAL "host")
		add_subdirectory(std_host)
	else()
		message(FATAL_ERROR "OS is selected: ${CONFIG_OS}, but no thread support exist")
	endif()
else()
	message(STATUS "Be aware: no OS was selected. Thread support will be disabled."
		"Stubs are provided for most synchronization primitives")
	add_subdirectory(empty)
endif()

add_subdirectory(common)
