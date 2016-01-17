# Add only requested kernel
if (DEFINED USE_OS)
	add_subdirectory(${USE_OS})
else()
	message(STATUS "Be aware: no OS was selected. Thread support will be disabled.")
endif()
