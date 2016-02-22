# Might be reviewed
cmake_minimum_required(VERSION 3.3)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Requried to improve function managament
include(CMakeParseArguments)

# Registers a project
macro(register_project project_name)
	# This check is intentionally copied from core's listfile
	# Reason is that if build API used a proper platform must be set
	# _before_ compiler definitions and core inclusion.
	message(STATUS "Checking [CONFIG_PLATFORM]...")
	if (NOT DEFINED CONFIG_PLATFORM)
		message(FATAL_ERROR "CONFIG_PLATFORM must be set in order to use valid platform")
	else()
		message(STATUS "Platform will be used: ${CONFIG_PLATFORM}")
		set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convinience
	endif()

	set(PROJECT_DIR ${CMAKE_CURRENT_DIR}/${project_name})
	set(PLATFORM_DIR ${CORE_DIR}/platform/${PLATFORM_NAME})

	# Pick common linker definitions for given platform
	include(${CORE_DIR}/platform/common/linker.cmake)

	# Make sure the core is included
	add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
endmacro()

# Creates a host unit test
function(add_unit_host_test)
	# Add test only if not cross-compiling
	if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL ${CMAKE_SYSTEM_NAME})
		cmake_parse_arguments(
			UNIT_TEST
			"OPTIONAL"
			"NAME"
			"SOURCES;DEPENDS;INC_DIRS"
			${ARGN}
			)

		if (DEFINED UNIT_TEST_NAME AND DEFINED UNIT_TEST_SOURCES)
			message("-----------------------------------------------")
			message("	Test added: ${UNIT_TEST_NAME}")
			message("	Test sources: ${UNIT_TEST_SOURCES}")

			add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
			add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST_NAME})
			target_link_libraries(${UNIT_TEST_NAME} CppUTest)
		else()
			message(FATAL_ERROR "Test sources and name must be defined!")
		endif()

		if (UNIT_TEST_DEPENDS)
			message("	Test dependencies: ${UNIT_TEST_DEPENDS}")
			target_link_libraries(${UNIT_TEST_NAME} ${UNIT_TEST_DEPENDS})
		endif()

		if (UNIT_TEST_INC_DIRS)
			message("	Test includes: ${UNIT_TEST_INC_DIRS}")
			target_include_directories(
			${UNIT_TEST_NAME}
			${UNIT_TEST_INC_DIRS})
		endif()
		message("-----------------------------------------------")
	endif()
endfunction()
