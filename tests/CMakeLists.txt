cmake_minimum_required(VERSION 3.4)

include(ExternalProject)
include(CMakeParseArguments)

# Adds theCore test suite and sets suite parameters.
#
# Syntax:
# add_unit_host_test(test_name
#					 [TOOLCHAIN_NAME toolchain_name]
#					 [TARGET_NAME target_name]
function(add_suite suite_name)

    cmake_parse_arguments(TEST
            ""
            "TOOLCHAIN_NAME;TARGET_NAME"
            ""
            ${ARGN})

    # Often used paths

    set(MCUS_DIR ${CMAKE_CURRENT_LIST_DIR}/mcus)
    set(TARGETS_DIR ${CMAKE_CURRENT_LIST_DIR}/targets)
    set(TESTCASES_DIR ${CMAKE_CURRENT_LIST_DIR}/cases)
    set(SUITE_SOURCE_DIR ${TARGETS_DIR}/${TEST_TARGET_NAME}/suites/${suite_name})

    set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
    set(TOOLCHAINS_DIR ${CORE_DIR}/toolchains)

    # Arguments for the test project
    set(CMAKE_ARGS)

    # Partially optional arguments

    if(DEFINED TEST_TOOLCHAIN_NAME)
        list(APPEND CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAINS_DIR}/${TEST_TOOLCHAIN_NAME})
    endif()

    list(APPEND CMAKE_ARGS -DCORE_DIR=${CORE_DIR})
    list(APPEND CMAKE_ARGS -DTESTCASES_DIR=${TESTCASES_DIR})
    list(APPEND CMAKE_ARGS -DMCUS_DIR=${MCUS_DIR})

    set(COMPLETE_TEST_NAME "${suite_name}_${TEST_TARGET_NAME}")

    message("-----------------------------------------------")
    message("   Suite added:   ${COMPLETE_TEST_NAME}")
    message("   Suite dir:     ${SUITE_SOURCE_DIR}")
    message("   MCU:           ${TEST_MCU_NAME}")
    message("   Target:        ${TEST_TARGET_NAME}")
    message("-----------------------------------------------")

    externalproject_add(${COMPLETE_TEST_NAME}
            SOURCE_DIR ${SUITE_SOURCE_DIR}
            BUILD_ALWAYS 1
            INSTALL_COMMAND echo 'Install not needed, step skipped'
            CMAKE_ARGS ${CMAKE_ARGS}
            )

endfunction()


#-------------------------------------------------------------------------------
# Platform BAT suite

add_suite(platform_bat
        TOOLCHAIN_NAME      arm-cm4-gnu.cmake
        TARGET_NAME         tivac_tm4)



#-------------------------------------------------------------------------------
# Unity demo test suite

add_suite(unity_demo
        TARGET_NAME         host)

add_suite(unity_demo
        TOOLCHAIN_NAME      arm-cm3-gnu.cmake
        TARGET_NAME         stm32f4discovery_simple)

add_suite(unity_demo
        TOOLCHAIN_NAME      arm-cm4-gnu.cmake
        TARGET_NAME         tivac_tm4)
