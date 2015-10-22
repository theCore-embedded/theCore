# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# TODO: bug?

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
	set(BASE_PROJECT_NAME ${project_name})
	set(BASE_TARGET_NAME ${base_target})
	set(BASE_PLATFORM_NAME ${base_platform})

	include(platform/${base_platform}/compiler/compiler.cmake)

	include_directories(sys/export)

	add_subdirectory(project/${project_name})

	# TODO: find better place for it
	add_subdirectory(sys)
	add_subdirectory(dev)
	add_subdirectory(kernel/freertos)

	# Make binary from the project object file
	add_custom_target(${project_name}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${CMAKE_CURRENT_BINARY_DIR}/project/${project_name}/${project_name} ${project_name}.bin
	DEPENDS ${project_name}
	COMMENT "Making binary ${project_name}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${Target}.bin)

endfunction()

# Registers base target for given project
# Requied to be called from the project module
function(register_target)
	add_subdirectory(${PROJECT_SOURCE_DIR}/target/${BASE_TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/target/)
	target_link_libraries(${BASE_PROJECT_NAME} target)
endfunction()

# Registers base platform for current target
# Required to be called from the target module
function(register_platform)
	add_subdirectory(${PROJECT_SOURCE_DIR}/platform/${BASE_PLATFORM_NAME} ${CMAKE_CURRENT_BINARY_DIR}/platform/)
	target_link_libraries(target platform)
endfunction()

# EO TODO
##########################

# Projects to be built
# TODO: think about using cmake's project instead of
# using self-made projects
register_project(mp3basic mp3basic stm32f4xx)

# For debug purposes only
# register_project(dummy dummy_target dummy_platform)
