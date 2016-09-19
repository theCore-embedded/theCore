cmake_minimum_required(VERSION 3.4)

include(ExternalProject)

# Often used paths

set(MCUS_DIR ${CMAKE_CURRENT_LIST_DIR}/mcus)
set(TARGETS_DIR ${CMAKE_CURRENT_LIST_DIR}/targets)
set(TESTCASES_DIR ${CMAKE_CURRENT_LIST_DIR}/cases)

set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
set(TOOLCHAINS_DIR ${CORE_DIR}/toolchains)

# ------------------------------------------------------------------------------
# Empty main test case. Runs and hangs.

externalproject_add(empty_main_host
        SOURCE_DIR ${TESTCASES_DIR}/empty_main
        BUILD_ALWAYS 1
        INSTALL_COMMAND echo 'Install not needed, step skipped'
        CMAKE_ARGS
            -DTARGET_DIR=${TARGETS_DIR}/host/
            -DCORE_DIR=${CMAKE_CURRENT_LIST_DIR}/../
        )

externalproject_add(empty_main_stm32f4_discovery
        SOURCE_DIR ${TESTCASES_DIR}/empty_main
        BUILD_ALWAYS 1
        INSTALL_COMMAND echo 'Install not needed, step skipped'
        CMAKE_ARGS
            -DCMAKE_TOOLCHAIN_FILE=${CORE_DIR}/toolchains/arm-cm3-gnu.cmake
            -DMCU_DIR=${MCUS_DIR}/stm32f407
            -DTARGET_DIR=${TARGETS_DIR}/stm32f4discovery_simple/
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
            -DTARGET_DIR=${TARGETS_DIR}/host/
            -DCORE_DIR=${CORE_DIR}
            -DAPP_CFG=host
        )

# STM32 F4 Discovery board with USB <-> UART adapter
externalproject_add(hello_world_stm32f4_discovery
        SOURCE_DIR ${TESTCASES_DIR}/hello_world
        BUILD_ALWAYS 1
        INSTALL_COMMAND echo 'Install not needed, step skipped'
        CMAKE_ARGS
            -DCMAKE_TOOLCHAIN_FILE=${CORE_DIR}/toolchains/arm-cm3-gnu.cmake
            -DMCU_DIR=${MCUS_DIR}/stm32f407
            -DTARGET_DIR=${TARGETS_DIR}/stm32f4discovery_simple/
            -DCORE_DIR=${CORE_DIR}
            -DAPP_CFG=stm32f4disc_nonos
        )

