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

get_filename_component(ABSOLUTE_CORE_DIR "${CORE_DIR}" ABSOLUTE)

set(EXCLUDE_DIRS
"EXCLUDE = ${ABSOLUTE_CORE_DIR}/kernel/freertos/freertos/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/stm32-spl/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/cmsis/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/particle_firmware/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/ti-tivaware/ti-tivaware/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/unity/Unity/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/system_stm32f4xx.h/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/export/stm32f4xx.h/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/f4xx/system_stm32f4xx.c/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/system_stm32l1xx.h/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/export/stm32l1xx.h/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/platform/stm32/family/l1xx/system_stm32l1xx.c/\n\
EXCLUDE += ${ABSOLUTE_CORE_DIR}/lib/fs/fat/pff/\n\
")

if(DOXYGEN_FOUND)
	set(INPUT_DIR ${CORE_DIR})
	set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg.in
	${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg)

	add_custom_target(core_doc
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
		WORKING_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating doxygen docs"
		VERBATIM
		)
endif()

# Taken from
# https://eb2.co/blog/2012/03/sphinx-and-cmake-beautiful-documentation-for-c-projects/

find_package(Sphinx)

if(Sphinx_FOUND)
    # configured documentation tools and intermediate build results
	set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/sphinx/theCore")

	# HTML output directory
	set(SPHINX_HTML_DIR "${BINARY_BUILD_DIR}/")

	# Single-page HTML output directory
	set(SPHINX_SINGLE_HTML_DIR "${BINARY_BUILD_DIR}/singlehtml")

	add_custom_target(core_sphinx
		# Multi-page HTML build
		${CMAKE_COMMAND} -E env SPHINX_ROOT_URL=http://forgge.github.io/theCore
		${SPHINX_EXECUTABLE}
			-b html
			-c "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
			"${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
			"${SPHINX_HTML_DIR}"
		COMMAND # Single HTML build
		${CMAKE_COMMAND} -E env SPHINX_ROOT_URL=http://forgge.github.io/theCore
		${SPHINX_EXECUTABLE}
			-b singlehtml
			-c "${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
			"${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
			"${SPHINX_SINGLE_HTML_DIR}"
		COMMENT "Building HTML documentation with Sphinx")
endif()
