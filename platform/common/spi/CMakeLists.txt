add_library(common_spi INTERFACE)
target_include_directories(common_spi INTERFACE export)
target_link_libraries(common_spi INTERFACE ${USE_OS})
