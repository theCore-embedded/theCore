if(__build_api_def)
    return()
endif()
set(__build_api_def YES)

# Might be reviewed
cmake_minimum_required(VERSION 3.2)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Requried to improve function managament
include(CMakeParseArguments)

# Add modules dir to search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

# Add test only if not cross-compiling
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL ${CMAKE_SYSTEM_NAME})
    find_package(CppUTest)
    if(NOT ${CPPUTEST_FOUND})
        message(WARNING "CppUTest library not present. Tests are disabled.")
    endif()
endif()

# Creates a host unit test with name unit_test_${test_name}
# TODO: move this to separate module
#
# Syntax:
# add_unit_host_test(NAME test_name
#					 SOURCES test_sources_files...
#					 [DEPENDS list_of_dependencies...]
#					 [INC_DIRS list_of_include_directories...])
function(add_unit_host_test)
    # Check if tests are needed at all
    if(CONFIG_NO_THECORE_UNIT_TESTS)
        return()
    endif()

    # Set general flags for C\C++ compiler and linker
    set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

    # All test can use most recent standart
    set(CMAKE_CXX_STANDARD 14)

    # Protect from missing test utilities
    if(${CPPUTEST_FOUND})
        cmake_parse_arguments(
                UNIT_TEST
                ""
                "NAME"
                "SOURCES;DEPENDS;INC_DIRS"
                ${ARGN}
        )

        if(DEFINED UNIT_TEST_NAME AND DEFINED UNIT_TEST_SOURCES)
            set(UNIT_TEST_NAME unit_test_${UNIT_TEST_NAME})
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

# Strips executable, effectively making binary file from it.
# Use it, if binary is convinient form to deploy code into the board.
#
# Syntax:
# strip_executable(exe_name)
#   exec_name - name of the target, that was previously added via
#   add_executable call
function(strip_executable exec_name)
    # Make binary from the project object file
    add_custom_target(${exec_name}_bin ALL
            COMMAND ${CMAKE_OBJCOPY} --output-format=binary
            $<TARGET_FILE:${exec_name}>
            $<TARGET_FILE:${exec_name}>.bin
            DEPENDS ${exec_name}
            COMMENT "Making binary from ${exec_name} ..."
            )

    # Clean binary on 'make clean' call
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
            $<TARGET_FILE:${exec_name}>.bin)
endfunction()
