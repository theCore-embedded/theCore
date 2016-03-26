if (__build_api_def)
    return()
endif ()
set(__build_api_def YES)


# Might be reviewed
cmake_minimum_required(VERSION 3.2)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Requried to improve function managament
include(CMakeParseArguments)

# Add modules dir to search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

# Registers a project
macro(register_project project_name path_to_exe_file)
	set(EXEC_PATH ${path_to_exe_file})

    # Make sure the core is included
	add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
endmacro()

# Creates a host unit test
#
# Syntax:
# add_unit_host_test(NAME test_name
#					 SOURCES test_sources_files...
#					 [DEPENDS list_of_dependencies...]
#					 [INC_DIRS list_of_include_directories...])
function(add_unit_host_test)
    # Add test only if not cross-compiling
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL ${CMAKE_SYSTEM_NAME})

        find_package(CppUTest REQUIRED)

        cmake_parse_arguments(
                UNIT_TEST
                "OPTIONAL"
                "NAME"
                "SOURCES;DEPENDS;INC_DIRS"
                ${ARGN}
        )

		if(DEFINED UNIT_TEST_NAME AND DEFINED UNIT_TEST_SOURCES)
            message("-----------------------------------------------")
            message("	Test added: ${UNIT_TEST_NAME}")
            message("	Test sources: ${UNIT_TEST_SOURCES}")

            add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
            add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST_NAME})
            target_link_libraries(${UNIT_TEST_NAME} CppUTest)
            target_link_libraries(${UNIT_TEST_NAME} CppUTestExt)
		else()
            message(FATAL_ERROR "Test sources and name must be defined!")
		endif()

		if(UNIT_TEST_DEPENDS)
            message("	Test dependencies: ${UNIT_TEST_DEPENDS}")
            target_link_libraries(${UNIT_TEST_NAME} ${UNIT_TEST_DEPENDS})
		endif()

		if(UNIT_TEST_INC_DIRS)
            message("	Test includes: ${UNIT_TEST_INC_DIRS}")
            target_include_directories(
                    ${UNIT_TEST_NAME}
                    PRIVATE
                    ${UNIT_TEST_INC_DIRS})
		endif()

        target_include_directories(${UNIT_TEST_NAME} PRIVATE ${CPPUTEST_INCLUDE_DIRS})
        message("-----------------------------------------------")
	endif()
endfunction()
