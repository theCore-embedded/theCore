include(FindDoxygen)

if(DOXYGEN_FOUND)
	set(INPUT_DIR ${CORE_DIR})
	set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
	set(EXCLUDE_DIRS
		"${CORE_DIR}/kernel/freertos/freertos/ \
		${CORE_DIR}/platform/stm32f4xx/CMSIS \
		${CORE_DIR}/platform/stm32f4xx/SPL")

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg.in
	${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg @ONLY)

	add_custom_target(core_doc
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
		WORKING_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating doxygen docs"
		VERBATIM
		)

endif()
