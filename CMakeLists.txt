message(STATUS "Determining project configuration...")

# Pick console driver
if (DEFINED CONSOLE_DRIVER_LIB)
	if (TARGET "${CONSOLE_DRIVER_LIB}")
		message(STATUS "Console library is provided by the project: ${CONSOLE_DRIVER_LIB}")
	else()
		message(FATAL_ERROR "${CONSOLE_DRIVER_LIB} is not a valid console library!")
	endif()
else()
	message(STATUS "Console library is not provided by the project, so default is used")
endif()

# TODO: add same configuration mechanism for choosing right kernel

# Add platform to the build
add_subdirectory(${PLATFORM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/platform)

add_subdirectory(./sys ${CMAKE_CURRENT_BINARY_DIR}/core/sys)
add_subdirectory(./lib ${CMAKE_CURRENT_BINARY_DIR}/core/lib)
add_subdirectory(./dev ${CMAKE_CURRENT_BINARY_DIR}/core/dev)
add_subdirectory(./kernel ${CMAKE_CURRENT_BINARY_DIR}/core/kernel)
