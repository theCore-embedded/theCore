# Add kernel to the build if it is really requested
if (DEFINED CONFIG_OS)
	if (${CONFIG_OS_INTERNAL})
		add_subdirectory(${CONFIG_OS})
	else()
		message(INFO "Kernel is selected, but it is provdided externally")
	endif()
endif()
