# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(__build_api_def)
    return()
endif()
set(__build_api_def YES)

# Might be reviewed
cmake_minimum_required(VERSION 3.2)

# Set for latter use
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR})

# Required to improve function management
include(CMakeParseArguments)

# Add modules dir to search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

# Colorize cmake output
string(ASCII 27 THECORE_ESC)
set(THECORE_INFO_COLOR "${THECORE_ESC}[96m")
set(THECORE_WARN_COLOR "${THECORE_ESC}[93m")
set(THECORE_TRACE_COLOR "${THECORE_ESC}[94m")
set(THECORE_FATAL_COLOR "${THECORE_ESC}[1;31m")
set(THECORE_COLOR_RESET "${THECORE_ESC}[m")

function(msg_info msg_body)
    message("${THECORE_INFO_COLOR}[INFO ]: ${msg_body} ${THECORE_COLOR_RESET}")
endfunction(msg_info)

function(msg_warn msg_body)
    message("${THECORE_WARN_COLOR}[WARN ]: ${msg_body} ${THECORE_COLOR_RESET}")
    message(WARNING)
endfunction(msg_warn)

function(msg_fatal msg_body)
    message("${THECORE_FATAL_COLOR}[FATAL]: ${msg_body} ${THECORE_COLOR_RESET}")
    message(FATAL_ERROR)
endfunction(msg_fatal)

function(msg_trace msg_body)
    message("${THECORE_TRACE_COLOR}[TRACE]: ${msg_body} ${THECORE_COLOR_RESET}")
endfunction(msg_trace)

# Add test only if not cross-compiling
if(CMAKE_HOST_SYSTEM_NAME STREQUAL CMAKE_SYSTEM_NAME)
    find_package(CppUTest)
    if(NOT ${CPPUTEST_FOUND})
        msg_warn("CppUTest library not present. Tests are disabled.")
    endif()
endif()

# Creates a host unit test with name unit_test_${test_name}
# TODO: move this to separate module
#
# Syntax:
# add_unit_host_test(NAME test_name
#                    SOURCES test_sources_files...
#                    [DEPENDS list_of_dependencies...]
#                    [INC_DIRS list_of_include_directories...]
#                    [COMPILE_OPTIONS list_of_compiler_options...])
function(add_unit_host_test)
    set(CC_WARN_FLAGS -Wall -Wextra -Wpedantic -Werror)
    set(CXX_WARN_FLAGS ${CC_WARN_FLAGS})

    # All test can use most recent standart
    set(CMAKE_CXX_STANDARD 14)

    # Debug mode flags, for maximum debug info
    set(CC_CXX_FLAGS_DEBUG -O0 -g3 -ggdb)

    # Protect from missing test utilities
    if(CPPUTEST_FOUND)
        cmake_parse_arguments(
                UNIT_TEST
                ""
                "NAME"
                "SOURCES;DEPENDS;INC_DIRS;COMPILE_OPTIONS"
                ${ARGN}
        )

        if(DEFINED UNIT_TEST_NAME AND DEFINED UNIT_TEST_SOURCES)
            set(UNIT_TEST_NAME unit_test_${UNIT_TEST_NAME})
            msg_info("-----------------------------------------------")
            msg_info("	Test added: ${UNIT_TEST_NAME}")
            msg_info("	Test sources: ${UNIT_TEST_SOURCES}")

            add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
            add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST_NAME})
            target_link_libraries(${UNIT_TEST_NAME} PRIVATE CppUTest)
            target_link_libraries(${UNIT_TEST_NAME} PRIVATE CppUTestExt)
        else()
            msg_fatal("Test sources and name must be defined!")
        endif()

        if(UNIT_TEST_DEPENDS)
            msg_info("	Test dependencies: ${UNIT_TEST_DEPENDS}")
            target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${UNIT_TEST_DEPENDS})
        endif()

        if(UNIT_TEST_INC_DIRS)
            msg_info("	Test includes: ${UNIT_TEST_INC_DIRS}")
            target_include_directories(
                    ${UNIT_TEST_NAME}
                    PRIVATE
                    ${UNIT_TEST_INC_DIRS})
        endif()

        if(UNIT_TEST_COMPILE_OPTIONS)
            msg_info("	Test compile options: ${UNIT_TEST_COMPILE_OPTIONS}")
            target_compile_options(${UNIT_TEST_NAME} PRIVATE ${UNIT_TEST_COMPILE_OPTIONS})
        endif()

        target_compile_definitions(${UNIT_TEST_NAME} PRIVATE CORE_TESTS_ENABLED=1)

        target_include_directories(${UNIT_TEST_NAME} PRIVATE ${CPPUTEST_INCLUDE_DIRS})

        target_compile_options(${UNIT_TEST_NAME} PUBLIC $<$<CONFIG:Debug>:${CC_CXX_FLAGS_DEBUG}>)
        target_compile_options(${UNIT_TEST_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${CXX_WARN_FLAGS}>)
        target_compile_options(${UNIT_TEST_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:${CC_WARN_FLAGS}>)
        msg_info("-----------------------------------------------")
    endif()
endfunction()

# Strips executable, effectively making binary file from it.
# Use it, if binary is convenient form to deploy code into the board.
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

#-------------------------------------------------------------------------------

# Enables particular OS.
# Syntax:
# theCore_enable_os(os_name)
#   - os_name - valid OS name. Right now only FreeRTOS is supported.
macro(theCore_enable_os OS_NAME)
    # TODO: rename this variable. See #261.
    set(CONFIG_OS "${OS_NAME}")

    # Rest of API will be included based on selected OS.
    include(${CORE_DIR}/lib/thread/thread_api.cmake)
endmacro()

# Exposes hardware configuration to theCore in json form.
# Syntax:
# theCore_set_hw_config(jsonpath)
#   - jsonpath - path to a JSON file, describing target hardware.
function(theCore_set_target_config jsonpath)
    set(THECORE_TARGET_CONFIG_FILE "${jsonpath}" CACHE STRING "Path to a json config" FORCE)
endfunction()

#-------------------------------------------------------------------------------

# Create custom command - COG runner
# See https://nedbatchelder.com/code/cog/ for details.
# Syntax:
# theCore_create_cog_runner(IN input_file
#                           OUT output_file
#                           [ARGS args ...]
#                           [DEPENDS dependency ...])
#   - input_file    - input file for COG
#   - output_file   - resulting file for COG
#   - args          - optional arguments for COG
#   - dependency    - optional. One or more additional files
#                     that output file is depends on.
macro(theCore_create_cog_runner)
    cmake_parse_arguments(
            COG
            ""
            "IN;OUT"
            "ARGS;DEPENDS"
            ${ARGN}
    )

    if(NOT DEFINED COG_IN OR NOT DEFINED COG_OUT)
        msg_fatal("Input or output files are not specified")
    endif()

    set_source_files_properties(${COG_OUT} PROPERTIES GENERATED TRUE)

    # Additional paths to include platform-level modules
    set(COG_INCLUDES
        "-I${CORE_DIR}/platform/${PLATFORM_NAME}" "-I${CORE_DIR}/platform/common/")

    add_custom_command(
        OUTPUT ${COG_OUT}
        COMMAND cog.py ${COG_INCLUDES} ${COG_ARGS} -d -o ${COG_OUT} ${COG_IN}
        DEPENDS ${COG_IN} ${COG_DEPENDS}
        COMMENT "Generating: ${COG_IN} -> ${COG_OUT}"
    )

    unset(COG_INCLUDES)
endmacro()

# Creates custom target - JSON validation runner against provided schema.
# theCore_create_json_validator_runner(NAME      target_name
#                                      JSON      path_to_json
#                                      SCHEMA    path_to_json_schema
#                                      SUBOBJECT subobject_to_validate
#                                      WORKDIR   working_directory)
#   - target_name           - name for a CMake target to be created
#   - path_to_json          - path to JSON file for validation
#   - path_to_json_schema   - path to JSON schema
#   - subobject_to_validate - JSON subobject to validate (optional)
#   - working_directory     - path to working directory, required to correctly
#                             process schema references with relative paths.
macro(theCore_create_json_validator_runner)
    cmake_parse_arguments(
        JSON_VALIDATOR
        ""
        "NAME;JSON;SCHEMA;SUBOBJECT;WORKDIR"
        ""
        ${ARGN}
    )

    msg_info("${JSON_VALIDATOR_JSON}")

    if(NOT DEFINED JSON_VALIDATOR_NAME
        OR NOT DEFINED JSON_VALIDATOR_JSON
        OR NOT DEFINED JSON_VALIDATOR_SCHEMA
        OR NOT DEFINED JSON_VALIDATOR_WORKDIR)
        msg_fatal("Incorrect arguments passed to validator function: ${ARGN}")
    endif()

    set(VALIDATOR_SCRIPT "${CORE_DIR}/scripts/validate.py")

    add_custom_target(${JSON_VALIDATOR_NAME}
        COMMAND python ${VALIDATOR_SCRIPT}
            ${JSON_VALIDATOR_JSON}
            ${JSON_VALIDATOR_SCHEMA}
            ${JSON_VALIDATOR_SUBOBJECT}
        WORKING_DIRECTORY ${JSON_VALIDATOR_WORKDIR}
        VERBATIM
        DEPENDS ${VALIDATOR_SCRIPT} ${JSON_VALIDATOR_JSON} ${JSON_VALIDATOR_SCHEMA}
        COMMENT "Validating ${JSON_VALIDATOR_SUBOBJECT} configuration"
    )

    unset(VALIDATOR_SCRIPT)

endmacro()

# Constructs command to download thirdparty from git, place it under thirdparty
# directory and make copy of it into the builddir.
# Command constructed in returned in a ${target_name}_get_cmd variable.
#
# theCore_get_thirdparty(target_name git_remote git_commit)
#   - name         - name for 3rdparty directory that will reside under
#                    ${THECORE_THIRDPARTY_DIR}/ and will contain 3rdparty sources.
#                    Also, name of resuling worktree dir during build, that will
#                    be placed in ${THECORE_BUILD_THIRDPARTY_DIR}
#   - git_remote   - git remote URL to fetch 3rdparty from
#   - git_commit   - valid git commit to fetch
function(theCore_get_thirdparty_cmd NAME GIT_REMOTE GIT_COMMIT)
    if(NOT DEFINED NAME
        OR NOT DEFINED GIT_REMOTE
        OR NOT DEFINED GIT_COMMIT)
        msg_fatal("Incorrect arguments passed to theCore_get_thirdparty")
    endif()

    msg_trace("Requested thirdparty: ${NAME} from: ${GIT_REMOTE} commit: ${GIT_COMMIT}")

    if(NOT THECORE_THIRDPARTY_DIR)
        msg_fatal("THECORE_THIRDPARTY_DIR must be specified in order to persist thirdparty packages")
    endif()

    if(NOT THECORE_BUILD_THIRDPARTY_DIR)
        msg_fatal("THECORE_BUILD_THIRDPARTY_DIR must be specified in order to persist thirdparty packages")
    endif()

    set(${NAME}_get_cmd
        "${CORE_DIR}/scripts/git_thirdparty.sh"
        "${THECORE_THIRDPARTY_DIR}/${NAME}"
        "${THECORE_BUILD_THIRDPARTY_DIR}/${NAME}"
        "${GIT_REMOTE}"
        "${GIT_COMMIT}" PARENT_SCOPE)
endfunction()

# Downloads thirdparty from GIT, places it under thirdparty directory and
# makes copy of it into the builddir
function(theCore_get_thirdparty NAME GIT_REMOTE GIT_COMMIT)
    if(NOT DEFINED NAME
        OR NOT DEFINED GIT_REMOTE
        OR NOT DEFINED GIT_COMMIT)
        msg_fatal("Incorrect arguments passed to theCore_add_thirdparty")
    endif()

    theCore_get_thirdparty_cmd(${NAME} ${GIT_REMOTE} ${GIT_COMMIT})

    execute_process(COMMAND ${${NAME}_get_cmd}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()
