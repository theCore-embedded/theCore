# ASM is crucial
enable_language(ASM-ATT)

# TODO: add same configuration mechanism for choosing right kernel
if (NOT DEFINED CONFIG_PLATFORM)
	message(FATAL_ERROR "CONFIG_PLATFORM must be set in order to use valid platform")
else()
	message(STATUS "Platform will be used: ${CONFIG_PLATFORM}")
	set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convinience
endif()

# This is the top-level target, placed here to control required build order.
# It depends on startup module, which is actual entry point (not main module).
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/null.cpp "")
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/null.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC startup)

# Make binary from the project object file
add_custom_target(${PROJECT_NAME}.bin ALL
COMMAND ${CMAKE_OBJCOPY} --output-format=binary
${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_NAME}.bin
DEPENDS ${PROJECT_NAME}
COMMENT "Making binary ${PROJECT_NAME}"
)

# Clean binary on 'make clean' call
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.bin)

# In order to pick proper definitions from the platform it is required
# that platform will be added in first place
add_subdirectory(platform)

# Rest of modules
add_subdirectory(sys)
add_subdirectory(lib)
add_subdirectory(dev)
add_subdirectory(kernel)

