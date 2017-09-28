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

# STM32 examples

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
