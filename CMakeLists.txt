# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# Objvious
project(mp3player)

# Makefile output for the debug purposes
set(CMAKE_VERBOSE_MAKEFILE on)

# ASM is crucial
enable_language(ASM-ATT)


##########################
# Useful routines TODO: move all these routines in the separate include

# Registers a project
function(register_project project_name base_target base_platform)
	set(BASE_PROJECT_DIR ${PROJECT_SOURCE_DIR}/project/${project_name})
	set(BASE_TARGET_DIR ${PROJECT_SOURCE_DIR}/target/${base_target})
	set(BASE_PLATFORM_DIR ${PROJECT_SOURCE_DIR}/platform/${base_platform})

	set(BASE_PLATFORM_NAME ${base_platform})
	set(BASE_TARGET_NAME ${base_target}-${BASE_PLATFORM_NAME})
	set(BASE_PROJECT_NAME ${project_name}-${BASE_TARGET_NAME})

	include(platform/${base_platform}/compiler/compiler.cmake)

	include_directories(sys/export)

	add_subdirectory(project/${project_name}
	    ${CMAKE_CURRENT_BINARY_DIR}/project/${BASE_PROJECT_NAME}/)

	# TODO: find better place for it
	add_subdirectory(sys)
	add_subdirectory(lib)
	add_subdirectory(dev)
	add_subdirectory(kernel)

	# Make binary from the project object file
	add_custom_target(${BASE_PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${CMAKE_CURRENT_BINARY_DIR}/project/${BASE_PROJECT_NAME}/${BASE_PROJECT_NAME} ${BASE_PROJECT_NAME}.bin
	DEPENDS ${BASE_PROJECT_NAME}
	COMMENT "Making binary ${BASE_PROJECT_NAME}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${Target}.bin)

endfunction()

# Registers base target for given project
# Requied to be called _only_ by the project module
function(link_project_with_target)
	add_subdirectory(${BASE_TARGET_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/target/)
	target_link_libraries(${BASE_PROJECT_NAME} ${BASE_TARGET_NAME})
endfunction()

# Registers base platform for current target
# Required to be called _only_ by the target module
function(link_target_with_platform)
	add_subdirectory(${BASE_PLATFORM_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/platform/)
	target_link_libraries(${BASE_TARGET_NAME} ${BASE_PLATFORM_NAME})
endfunction()

# EO TODO
##########################

# Projects to be built
# TODO: think about using cmake's project instead of
# using self-made projects
register_project(mp3basic mp3basic stm32f4xx)

# For debug purposes only
# register_project(dummy dummy_target dummy_platform)
