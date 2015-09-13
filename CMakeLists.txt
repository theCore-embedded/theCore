# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# TODO: bug?

# Objvious
project(mp3player)

# Makefile output for the debug purposes
set(CMAKE_VERBOSE_MAKEFILE on)

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

# Imports a module
# TODO: drop it?
function(import_module current_module imported_module)
	message(${current_module} " depends on " ${imported_module})

	# Make sure dependencies are correct
	add_dependencies(${current_module} ${imported_module})

	# Find configuration file and extract path

	# TODO: use more convinient way to find a module
	# file(GLOB_RECURSE module_path ${PROJECT_SOURCE_DIR}/*${imported_module}-config.cmake)
	# get_filename_component(module_dir ${module_path} DIRECTORY)
	# message("Imported module path: " ${module_dir})

	# TODO: this one not needed at all!
	# Find module itself
	# find_package(${imported_module} NO_DEFAULT_PATH PATHS ${module_dir})

	# TODO: easily avoided by using target_definitions() with
	# PUBLIC option
	# Set proper definitions
	# add_definitions(${${imported_module}_DEFS})

	# Set proper linkage
	target_link_libraries(${current_module} ${imported_module})
endfunction()

# Imports platform
# TODO

# Imports target
# TODO

# EO TODO
##########################

# Projects to be built
# TODO: think about using cmake's project instead of
# using self-made projects
register_project(mp3basic mp3basic stm32f4xx)

# For debug purposes only
# register_project(dummy dummy_target dummy_platform)
