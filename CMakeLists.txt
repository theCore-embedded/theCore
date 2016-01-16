# ASM is crucial
enable_language(ASM-ATT)

# TODO: add same configuration mechanism for choosing right kernel
if (NOT DEFINED USE_PLATFORM)
	message(FATAL_ERROR "USE_PLATFORM must be set in order to use valid platform")
else()
	message("Platform will be used: ${USE_PLATFORM}")
	set(PLATFORM_NAME ${USE_PLATFORM}) # For convinience
endif()

# In order to pick proper definitions from the platform it is required
# that platform will be added in first place
add_subdirectory(platform)

# Rest of modules
add_subdirectory(sys)
add_subdirectory(lib)
add_subdirectory(dev)
add_subdirectory(kernel)

