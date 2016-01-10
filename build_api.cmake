# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Registers a project
macro(register_project project_path platform_name)
	# TODO: check that platform name is a valid platform name
	# TODO: check that project path is valid project path

	set(PROJECT_DIR ${project_path})
	set(PLATFORM_DIR ${CORE_DIR}/platform/${platform_name})

	# Export the platform name
	set(PLATFORM_NAME ${platform_name})

	# Pick proper compiler definitions
	include(${PLATFORM_DIR}/compiler/compiler.cmake)

	# System headers avaliable for all modules
	include_directories(${CORE_DIR}/sys/export)

	# Make sure the core is included
	add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

	# Make binary from the project object file
	add_custom_target(${PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_NAME}.bin
	DEPENDS ${PROJECT_NAME}
	COMMENT "Making binary ${PROJECT_NAME}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.bin)
endmacro()

