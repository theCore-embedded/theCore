# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Registers a project
macro(register_project project_path platform_name)
	set(PROJECT_DIR ${project_path})
	set(PLATFORM_DIR ${CORE_DIR}/platform/${platform_name})

	message("--- > ${project_path}")
	message("--- > ${PLATFORM_DIR}")

	# Exports platform name
	set(PLATFORM_NAME ${platform_name})

	message("--- *> ${PLATFORM_NAME}")
	message("--- *> ${PROJECT_NAME}")

	# Pick proper compiler definitions
	include(${PLATFORM_DIR}/compiler/compiler.cmake)

	# System headers avaliable for all modules
	include_directories(${CORE_DIR}/sys/export)

	# Make sure core is included
	add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
	# Add platform to build
	add_subdirectory(${PLATFORM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/platform)

	# TODO: find better place for it
	#add_subdirectory(${CORE_DIR}/sys ${CMAKE_CURRENT_BINARY_DIR}/core/sys)
	#add_subdirectory(${CORE_DIR}/lib ${CMAKE_CURRENT_BINARY_DIR}/core/lib)
	#add_subdirectory(${CORE_DIR}/dev ${CMAKE_CURRENT_BINARY_DIR}/core/dev)
	#add_subdirectory(${CORE_DIR}/kernel ${CMAKE_CURRENT_BINARY_DIR}/core/kernel)

	# Make binary from the project object file
	add_custom_target(${PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_NAME}.bin
	DEPENDS ${PROJECT_NAME}
	COMMENT "Making binary ${PROJECT_NAME}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.bin)
endmacro()

