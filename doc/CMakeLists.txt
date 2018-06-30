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
    file(GLOB_RECURSE THECORE_INCLUDES
       "${ABSOLUTE_CORE_DIR}/*.hpp"
       "${ABSOLUTE_CORE_DIR}/*.h"
    )

    # Stay only with exported includes
    LIST(FILTER THECORE_INCLUDES INCLUDE REGEX "${ABSOLUTE_CORE_DIR}/.*/(export|templates)/.*\.h")

    file(GLOB_RECURSE THECORE_SOURCES
       "${ABSOLUTE_CORE_DIR}/*.cpp"
       "${ABSOLUTE_CORE_DIR}/*.c"
       "${ABSOLUTE_CORE_DIR}/*.dox"
    )

    list(APPEND THECORE_SOURCES ${THECORE_INCLUDES})

    # 3rd-parties that should be omitted.
    # Inspired by Oleh Kravchenko.

    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/.thirdparty/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/build/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/lib/unity/Unity/.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/doc/spinhx.*")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/system_stm32f4xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/stm32f4xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/system_stm32f4xx.c")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/system_stm32l1xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/stm32l1xx.h")
    LIST(FILTER THECORE_SOURCES EXCLUDE REGEX "${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/system_stm32l1xx.c")

    set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(PROJECT_NAME "\"theCore C++ embedded framework\"")

    doxygen(TARGET "core_doxygen"
        OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/doxygen"
        SOURCES
            ${THECORE_SOURCES}
        NO
            GENERATE_LATEX
            VERBATIM_HEADERS
            FULL_PATH_NAMES
            SORT_MEMBER_DOCS
        YES
            QUIET
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

if(Sphinx_FOUND)
    # configured documentation tools and intermediate build results
    set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/sphinx/theCore")

    # HTML output directory
    set(SPHINX_HTML_DIR "${BINARY_BUILD_DIR}/")

    # Single-page HTML output directory
    set(SPHINX_SINGLE_HTML_DIR "${BINARY_BUILD_DIR}/singlehtml")

    add_custom_target(core_sphinx
        # create directories beforehand
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${SPHINX_HTML_DIR}"
            "${SPHINX_SINGLE_HTML_DIR}"

        # sphinx pages will contain doxygen docs links.
        # adding a symlink will allow to validate that links and copy
        # doxygen docs along with sphinx
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_CURRENT_BINARY_DIR}/doxygen/html"
            "${SPHINX_HTML_DIR}/doxygen"

        COMMAND # Multi-page HTML build
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
    )

    add_dependencies(core_sphinx core_doxygen)
endif()
