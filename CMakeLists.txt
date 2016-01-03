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
function(register_project project_name target platform)
	set(PROJECT_DIR ${PROJECT_SOURCE_DIR}/project/${project_name})
	set(TARGET_DIR ${PROJECT_SOURCE_DIR}/target/${target})
	set(PLATFORM_DIR ${PROJECT_SOURCE_DIR}/platform/${platform})

	set(PLATFORM_NAME ${platform})
	set(TARGET_NAME ${target}-${PLATFORM_NAME})
	set(PROJECT_NAME ${project_name}-${TARGET_NAME})

	include(platform/${platform}/compiler/compiler.cmake)

	include_directories(sys/export)

	add_subdirectory(project/${project_name}
		${CMAKE_CURRENT_BINARY_DIR}/project/${PROJECT_NAME}/)

	# TODO: find better place for it
	add_subdirectory(sys)
	add_subdirectory(lib)
	add_subdirectory(dev)
	add_subdirectory(kernel)

	# Make binary from the project object file
	add_custom_target(${PROJECT_NAME}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${CMAKE_CURRENT_BINARY_DIR}/project/${PROJECT_NAME}/${PROJECT_NAME} ${PROJECT_NAME}.bin
	DEPENDS ${PROJECT_NAME}
	COMMENT "Making binary ${PROJECT_NAME}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${Target}.bin)

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

# EO TODO
##########################

# Projects to be built
# TODO: think about using cmake's project instead of
# using self-made projects
register_project(mp3basic mp3basic stm32f4xx)

# For debug purposes only
# register_project(dummy dummy_target dummy_platform)
