# Add kernel to the build if it is really requested
if (DEFINED CONFIG_OS)
    if (CONFIG_OS STREQUAL "freertos")
		add_subdirectory(${CONFIG_OS})
	endif()
endif()
