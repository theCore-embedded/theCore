# Might be reviwed.
cmake_minimum_required(VERSION 3.4)

# Special module will separate builds for each example project.
include(ExternalProject)

# Root project for bunch of theCore examples.
project(example_hub)

# Convinient alias
set(ARM_CM4_GNU_TOOLCHAIN ${CMAKE_CURRENT_LIST_DIR}/../toolchains/arm-cm4-gnu.cmake)

################################################################################
# Hello World host example.
################################################################################

ExternalProject_Add(host_hello
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/host_hello
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_ALWAYS 1 # Allows to rebuild external project if its files changes
        INSTALL_COMMAND echo "Install isn't required, skipping...")

################################################################################
# External interrupt stm32 demo.
################################################################################

ExternalProject_Add(stm32f4_exti
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/stm32f4_exti
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE=${ARM_CM4_GNU_TOOLCHAIN}
        BUILD_ALWAYS 1 # Allows to rebuild external project if its files changes
        INSTALL_COMMAND echo "Install isn't required, skipping...")


################################################################################
# Audio stm32 and cs43l22 demo.
################################################################################

ExternalProject_Add(stm32f4_cs43l22_audio
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/cs43l22_audio
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${ARM_CM4_GNU_TOOLCHAIN}
        BUILD_ALWAYS 1 # Allows to rebuild external project if its files changes
        INSTALL_COMMAND echo "Install isn't required, skipping...")

################################################################################
# STM32 and HTU21D demo.
################################################################################

ExternalProject_Add(stm32f4_htu21d_example
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/htu21d
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE=${ARM_CM4_GNU_TOOLCHAIN}
        BUILD_ALWAYS 1 # Allows to rebuild external project if its files changes
        INSTALL_COMMAND echo "Install isn't required, skipping...")
