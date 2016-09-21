cmake_minimum_required(VERSION 3.4)

include(ExternalProject)
include(CMakeParseArguments)

# Adds theCore functional test and sets test parameters.
#
# Syntax:
# add_unit_host_test(test_name
#					 [TOOLCHAIN_NAME toolchain_name]
#					 [MCU_NAME mcu_name]
#					 [TARGET_NAME target_name]
#					 [CONFIG_NAME test_configuration_name...])
function(add_functional_test test_name)

    cmake_parse_arguments(TEST
            ""
            "TOOLCHAIN_NAME;MCU_NAME;TARGET_NAME;CONFIG_NAME"
            ""
            ${ARGN})

    # Often used paths

    set(MCUS_DIR ${CMAKE_CURRENT_LIST_DIR}/mcus)
    set(TARGETS_DIR ${CMAKE_CURRENT_LIST_DIR}/targets)
    set(TESTCASES_DIR ${CMAKE_CURRENT_LIST_DIR}/cases)
    set(TEST_SOURCE_DIR ${TESTCASES_DIR}/${test_name})

    set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
    set(TOOLCHAINS_DIR ${CORE_DIR}/toolchains)

    # Arguments for the test project
    set(CMAKE_ARGS)

    # Partially optional arguments

    if(DEFINED TEST_MCU_NAME)
        list(APPEND CMAKE_ARGS -DMCU_DIR=${MCUS_DIR}/${TEST_MCU_NAME}/)
    endif()

    if(DEFINED TEST_TARGET_NAME)
        list(APPEND CMAKE_ARGS -DTARGET_DIR=${TARGETS_DIR}/${TEST_TARGET_NAME}/)
    endif()

    if(DEFINED TEST_TOOLCHAIN_NAME)
        list(APPEND CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAINS_DIR}/${TEST_TOOLCHAIN_NAME})
    endif()

    # Let case config be explicit
    if(NOT DEFINED TEST_CONFIG_NAME)
        set(TEST_CONFIG_NAME "default")
    else()
        list(APPEND CMAKE_ARGS -DAPP_CFG=${TEST_CONFIG_NAME})
    endif()

    list(APPEND CMAKE_ARGS -DCORE_DIR=${CORE_DIR})

    set(COMPLETE_TEST_NAME "${test_name}_${TEST_TARGET_NAME}_${TEST_CONFIG_NAME}")

    message("-----------------------------------------------")
    message("   Functional test added: ${COMPLETE_TEST_NAME}")
    message("   Source dir:    ${TEST_SOURCE_DIR}")
    message("   MCU:           ${TEST_MCU_NAME}")
    message("   Target:        ${TEST_TARGET_NAME}")
    message("   Config:        ${TEST_CONFIG_NAME}")
    message("-----------------------------------------------")

    externalproject_add(${COMPLETE_TEST_NAME}
            SOURCE_DIR ${TEST_SOURCE_DIR}
            BUILD_ALWAYS 1
            INSTALL_COMMAND echo 'Install not needed, step skipped'
            CMAKE_ARGS ${CMAKE_ARGS}
            )

endfunction()

#------------------------------------------------------------------------------
# Empty main test group. Runs and hangs on all targets.

add_functional_test(empty_main
        TARGET_NAME         host)

add_functional_test(empty_main
        TOOLCHAIN_NAME      arm-cm3-gnu.cmake
        TARGET_NAME         stm32f4discovery_simple
        MCU_NAME            stm32f407)

add_functional_test(empty_main
        TOOLCHAIN_NAME      arm-cm4-gnu.cmake
        TARGET_NAME         tivac_tm4
        MCU_NAME            tm4c123gh6pm)

#------------------------------------------------------------------------------
# Hello World test group. Prints 'Hello, World' and exits.

add_functional_test(hello_world
        TARGET_NAME         host
        CONFIG_NAME         host)

add_functional_test(hello_world
        TOOLCHAIN_NAME      arm-cm3-gnu.cmake
        MCU_NAME            stm32f407
        TARGET_NAME         stm32f4discovery_simple
        CONFIG_NAME         stm32f4disc_nonos
        )
