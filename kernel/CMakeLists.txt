# Add only requested kernel
if (DEFINED CONFIG_OS)
	add_subdirectory(${CONFIG_OS})
else()
	message(STATUS "Be aware: no OS was selected. Thread support will be disabled.")
endif()
