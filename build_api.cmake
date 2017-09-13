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
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL ${CMAKE_SYSTEM_NAME})
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
            target_link_libraries(${UNIT_TEST_NAME} CppUTest)
            target_link_libraries(${UNIT_TEST_NAME} CppUTestExt)
        else()
            msg_fatal("Test sources and name must be defined!")
        endif()

        if(UNIT_TEST_DEPENDS)
            msg_info("	Test dependencies: ${UNIT_TEST_DEPENDS}")
            target_link_libraries(${UNIT_TEST_NAME} ${UNIT_TEST_DEPENDS})
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
            target_compile_options(${UNIT_TEST_NAME} PUBLIC ${UNIT_TEST_COMPILE_OPTIONS})
        endif()

        target_compile_definitions(${UNIT_TEST_NAME} PUBLIC CORE_TESTS_ENABLED=1)

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

# Enables one of theCore platforms and exposes its API to the user, if present.
# Syntax:
# theCore_enable_platform(platform_name)
#   - platform_name - valid platform name. See `platform` directory to get
#                     insights on the list of available platforms.
macro(theCore_enable_platform PLATFORM_NAME)
    # TODO: rename this variable. See #261.
    set(CONFIG_PLATFORM "${PLATFORM_NAME}")

    set(THECORE_CONFIG_PLATFORM "${PLATFORM_NAME}" CACHE STRING "Desired platform" FORCE)

    set(PLATFORM_API_MODULE ${CORE_DIR}/platform/${PLATFORM_NAME}/platform_api.cmake)
    if(EXISTS ${PLATFORM_API_MODULE})
        include(${PLATFORM_API_MODULE})
    endif()

    msg_info("Requested to enable platform: ${PLATFORM_NAME}")
    unset(PLATFORM_API_MODULE)
endmacro()

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

    add_custom_command(
        OUTPUT ${COG_OUT}
        COMMAND cog.py ${COG_ARGS} -d -o ${COG_OUT} ${COG_IN}
        DEPENDS ${COG_IN} ${COG_DEPENDS}
        COMMENT "Generating: ${COG_IN} -> ${COG_OUT}"
    )
endmacro()
