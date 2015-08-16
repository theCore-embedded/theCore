# Might be reviewed
cmake_minimum_required(VERSION 2.8)

# Objvious
project(mp3player)

# Makefile output for the debug purposes
set(CMAKE_VERBOSE_MAKEFILE off)

# Useful routines TODO: move all these routines in the separate include

# Registers a project
function(register_project project_name base_target base_platform)
	set(BASE_PROJECT_NAME ${project_name})
	set(BASE_TARGET_NAME ${base_target})
	set(BASE_PLATFORM_NAME ${base_platform})

	include(platform/${base_platform}/compiler/compiler.cmake)

	add_subdirectory(project/${project_name})

	# Make binary from the project object file
	add_custom_target(${project_name}.bin ALL
	COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${CMAKE_CURRENT_BINARY_DIR}/project/${project_name}/${project_name} ${project_name}.bin
	DEPENDS ${project_name}
	COMMENT "Making binary ${project_name}"
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${Target}.bin)

endfunction(register_project)

# Registers base target for given project
# Requied to be called from the project module
function(register_target)
	add_subdirectory(${PROJECT_SOURCE_DIR}/target/${BASE_TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/target/)
	target_link_libraries(${BASE_PROJECT_NAME} target)
endfunction(register_target)

# Registers base platform for current target
# Required to be called from the target module
function(register_platform)
	add_subdirectory(${PROJECT_SOURCE_DIR}/platform/${BASE_PLATFORM_NAME} ${CMAKE_CURRENT_BINARY_DIR}/platform/)
	target_link_libraries(target platform)
endfunction(register_platform)

# Projects to be built
# add_subdirectory(project/mp3basic)
register_project(mp3basic mp3basic stm32f4xx)

# For debug purposes only
#register_project(dummy dummy_target dummy_platform)
