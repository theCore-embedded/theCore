cmake_minimum_required(VERSION 3.4)

include(ExternalProject)

set(TESTCASES_DIR ${CMAKE_CURRENT_LIST_DIR}/cases)

# ------------------------------------------------------------------------------
# Empty main test case. Runs and hangs.

externalproject_add(empty_main_host
        SOURCE_DIR ${TESTCASES_DIR}/empty_main
        BUILD_ALWAYS 1
        INSTALL_COMMAND echo 'Install not needed, step skipped'
        CMAKE_ARGS
            -DTARGET_DIR=${CMAKE_CURRENT_LIST_DIR}/targets/host/
            -DCORE_DIR=${CMAKE_CURRENT_LIST_DIR}/../
        )

# ------------------------------------------------------------------------------
# 'Hello World' test case. Runs and prints `Hello World` string each second.

# Host platform
externalproject_add(hello_world_host
        SOURCE_DIR ${TESTCASES_DIR}/hello_world
        BUILD_ALWAYS 1
        INSTALL_COMMAND echo 'Install not needed, step skipped'
        CMAKE_ARGS
            -DTARGET_DIR=${CMAKE_CURRENT_LIST_DIR}/targets/host/
            -DCORE_DIR=${CMAKE_CURRENT_LIST_DIR}/../
            -DAPP_CFG=host
        )

