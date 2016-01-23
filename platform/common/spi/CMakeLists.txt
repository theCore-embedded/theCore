add_library(common_spi INTERFACE)
target_include_directories(common_spi INTERFACE export)
if (DEFINED CONFIG_OS)
	target_link_libraries(common_spi INTERFACE ${CONFIG_OS})
else()
	# TODO: remove that hack!
	target_compile_definitions(common_spi INTERFACE -DSPI_NO_OS)
endif()
