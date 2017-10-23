# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.7)

# Handle case when documentation CMake file is a top-level file.
# Thus, CORE_DIR will not be defined.
if(NOT DEFINED CORE_DIR)
set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../)

# Add modules dir to search path
list(APPEND CMAKE_MODULE_PATH ${CORE_DIR}/modules)
endif()

include(FindDoxygen)
include(FindSphinx)
include(${CORE_DIR}/build_api.cmake)

find_package(cmake-doxygen REQUIRED COMPONENTS plantuml)

get_filename_component(ABSOLUTE_CORE_DIR "${CORE_DIR}" ABSOLUTE)

if(DOXYGEN_FOUND)
    # All sources
    file(GLOB_RECURSE THECORE_SOURCES
        "${ABSOLUTE_CORE_DIR}/*.cpp"
        "${ABSOLUTE_CORE_DIR}/*.hpp"
        "${ABSOLUTE_CORE_DIR}/*.h"
        "${ABSOLUTE_CORE_DIR}/*.c")

    # 3rd-parties that should be omitted.
    # Inspired by Oleh Kravchenko.

    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/kernel/freertos/freertos/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/stm32-spl/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/cmsis/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/particle_firmware/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/ti-tivaware/ti-tivaware/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/unity/Unity/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/system_stm32f4xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/stm32f4xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/system_stm32f4xx.c")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/system_stm32l1xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/stm32l1xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/system_stm32l1xx.c")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/fs/fat/pff/.*")

    set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

    doxygen(TARGET "core_doc"
        OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen"
        SOURCES
            ${THECORE_SOURCES}
        NO
            GENERATE_LATEX
            VERBATIM_HEADERS
        YES
            CALL_GRAPH
            EXTRACT_ALL
            EXTRACT_STATIC
            GENERATE_TREEVIEW
            HAVE_DOT
            INTERACTIVE_SVG
            JAVADOC_AUTOBRIEF
            OPTIMIZE_OUTPUT_FOR_C
            WARN_AS_ERROR
            WARN_NO_PARAMDOC
        PREDEFINED
            __DOXYGEN
        EXCLUDE_SYMBOLS
            __stringify_1
        TAB_SIZE 4
    )
endif()

# Taken from
# https://eb2.co/blog/2012/03/sphinx-and-cmake-beautiful-documentation-for-c-projects/

find_package(Sphinx)

# Convenient function to generate examples from schema files.
function(generate_source CONFIG_PATH IN_PATH OUT_PATH)
list(APPEND GENERATED_FILES ${OUT_PATH})
set(GENERATED_FILES ${GENERATED_FILES} PARENT_SCOPE)

theCore_create_cog_runner(
    IN      ${IN_PATH}
    OUT     ${OUT_PATH}
    DEPENDS ${CONFIG_PATH} ${IN_PATH}
    ARGS    "-DJSON_CFG=${CONFIG_PATH}")
endfunction()

################################################################################
# STM32 examples
################################################################################

set(STM32_DOC_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/platform/stm32)
set(STM32F4_DOC_DIR ${STM32_DOC_DIR}/f4xx)

set(STM32_TEMPLATE_DIR ${CORE_DIR}/platform/stm32/templates)
set(STM32F4_TEMPLATE_DIR ${CORE_DIR}/platform/stm32/family/f4xx/templates)

set(STM32_OUT_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/_static/generated/stm32)
set(STM32F4_OUT_DIR ${STM32_OUT_DIR}/f4xx)

# UART basic example.
generate_source(${STM32_DOC_DIR}/uart_example.json
    ${STM32_TEMPLATE_DIR}/uart_cfg.in.hpp
    ${STM32_OUT_DIR}/uart_example.hpp
)

# ADC example.
generate_source(${STM32_DOC_DIR}/adc_example.json
    ${STM32_TEMPLATE_DIR}/adc_cfg.in.hpp
    ${STM32_OUT_DIR}/adc_example.hpp
)

# I2C example.
generate_source(${STM32_DOC_DIR}/i2c_example.json
    ${STM32_TEMPLATE_DIR}/i2c_cfg.in.hpp
    ${STM32_OUT_DIR}/i2c_example.hpp
)

# I2S example.
generate_source(${STM32_DOC_DIR}/i2s_example.json
    ${STM32_TEMPLATE_DIR}/spi_i2s_cfg.in.hpp
    ${STM32_OUT_DIR}/i2s_example.hpp
)

# DMA for STM32F4 example.
generate_source(${STM32F4_DOC_DIR}/dma_example.json
    ${STM32F4_TEMPLATE_DIR}/dma_cfg.in.hpp
    ${STM32F4_OUT_DIR}/dma_example.hpp
)

################################################################################
# Particle electron examples
################################################################################

set(PARTICLE_DOC_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/platform/particle_electron)
set(PARTICLE_TEMPLATE_DIR ${CORE_DIR}/platform/particle_electron/templates)
set(PARTICLE_OUT_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/_static/generated/particle_electron)

# Serial for a particle electron example.
generate_source(${PARTICLE_DOC_DIR}/serial_example.json
    ${PARTICLE_TEMPLATE_DIR}/serial_cfgs.in.hpp
    ${PARTICLE_OUT_DIR}/serial_example.hpp
)

# I2C/Wire for a particle electron example.
generate_source(${PARTICLE_DOC_DIR}/i2c_example.json
    ${PARTICLE_TEMPLATE_DIR}/i2c_wire_cfgs.in.hpp
    ${PARTICLE_OUT_DIR}/i2c_example.hpp
)

################################################################################
# TM4C examples
################################################################################

set(TM4C_DOC_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/platform/tm4c)
set(TM4C_TEMPLATE_DIR ${CORE_DIR}/platform/tm4c/templates)
set(TM4C_OUT_DIR ${CMAKE_CURRENT_LIST_DIR}/sphinx/source/_static/generated/tm4c)

# UART basic example.
generate_source(${TM4C_DOC_DIR}/uart_example.json
    ${TM4C_TEMPLATE_DIR}/uart_cfg.in.hpp
    ${TM4C_OUT_DIR}/uart_example.hpp
)

# GPIO alias example.
generate_source(${TM4C_DOC_DIR}/gpio_alias_example.json
    ${TM4C_TEMPLATE_DIR}/gpio_cfg.in.hpp
    ${TM4C_OUT_DIR}/gpio_alias_example.hpp
)

# Pin multiplexing example
generate_source(${TM4C_DOC_DIR}/pin_mux_example.json
    ${TM4C_TEMPLATE_DIR}/pin_mux.in.cpp
    ${TM4C_OUT_DIR}/pin_mux_example.cpp
)


if(Sphinx_FOUND)
    # configured documentation tools and intermediate build results
    set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/sphinx/theCore")

    # HTML output directory
    set(SPHINX_HTML_DIR "${BINARY_BUILD_DIR}/")

    # Single-page HTML output directory
    set(SPHINX_SINGLE_HTML_DIR "${BINARY_BUILD_DIR}/singlehtml")

    # Title page output directory for forgge.github.io index page
    set(SPHINX_TITLE_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/sphinx/")

    add_custom_target(core_sphinx
        # Multi-page HTML build
        ${CMAKE_COMMAND} -E env SPHINX_ROOT_URL=http://forgge.github.io/theCore
        ${SPHINX_EXECUTABLE}
        -b html
        -c "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
        "${SPHINX_HTML_DIR}"
        # Make sure additional files are generated first (like examples from config JSON)
        DEPENDS ${GENERATED_FILES}

        COMMAND # Single HTML build
        ${CMAKE_COMMAND} -E env SPHINX_ROOT_URL=http://forgge.github.io/theCore
        ${SPHINX_EXECUTABLE}
        -b singlehtml
        -c "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
        "${SPHINX_SINGLE_HTML_DIR}"
        COMMENT "Building HTML documentation with Sphinx"

        COMMAND # Title HTML build
        ${CMAKE_COMMAND} -E env SPHINX_ROOT_URL=http://forgge.github.io/theCore
        ${SPHINX_EXECUTABLE}
        -b singlehtml
        -c "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source_title"
        "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source_title"
        "${SPHINX_TITLE_HTML_DIR}"
        COMMENT "Building title page documentation with Sphinx"
    )

endif()
