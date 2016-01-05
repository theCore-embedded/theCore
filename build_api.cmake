# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Registers a project
function(register_project project_path target_path platform_name)
	set(PROJECT_DIR ${project_path})
	set(TARGET_DIR ${target_path})
	set(PLATFORM_DIR ${CORE_DIR}/platform/${platform_name})

	message("--- > ${project_path}")
	message("--- > ${target_path}")
	message("--- > ${PLATFORM_DIR}")

	# Exports platform name
	set(PLATFORM_NAME ${platform_name})

	# Exports target name
	get_filename_component(TARGET_NAME ${TARGET_DIR} NAME)
	set(TARGET_NAME ${TARGET_NAME}-${PLATFORM_NAME})

	# Exports project name
	get_filename_component(PROJECT_NAME ${PROJECT_DIR} NAME)
	set(PROJECT_NAME ${PROJECT_NAME}-${TARGET_NAME})

	message("--- *> ${PLATFORM_NAME}")
	message("--- *> ${TARGET_NAME}")
	message("--- *> ${PROJECT_NAME}")

	# Pick proper compiler definitions
	include(${PLATFORM_DIR}/compiler/compiler.cmake)

	# System headers avaliable for all modules
	include_directories(${CORE_DIR}/sys/export)

	#add_subdirectory(${PROJECT_DIR}
	#	${CMAKE_CURRENT_BINARY_DIR}/project/${PROJECT_NAME}/)

	# TODO: find better place for it
	add_subdirectory(${CORE_DIR}/sys)
	add_subdirectory(${CORE_DIR}/lib)
	add_subdirectory(${CORE_DIR}/dev)
	add_subdirectory(${CORE_DIR}/kernel)

	# Make binary from the project object file
	add_custom_target(${PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary
	${CMAKE_CURRENT_BINARY_DIR}/project/${PROJECT_NAME}/${PROJECT_NAME} ${PROJECT_NAME}.bin
	DEPENDS ${PROJECT_NAME}
	COMMENT "Making binary ${PROJECT_NAME}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.bin)
endfunction()

# Registers base target for given project
# Requied to be called _only_ by the project module
function(link_project_with_target)
	add_subdirectory(${TARGET_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/target/)
	target_link_libraries(${PROJECT_NAME} ${TARGET_NAME})
endfunction()

# Registers base platform for current target
# Required to be called _only_ by the target module
function(link_target_with_platform)
	add_subdirectory(${PLATFORM_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/platform/)
	target_link_libraries(${TARGET_NAME} ${PLATFORM_NAME})
endfunction()

