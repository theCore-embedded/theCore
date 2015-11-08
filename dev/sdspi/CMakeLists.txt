## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(sdspi_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(sdspi STATIC sdspi.cpp)
target_include_directories(sdspi PUBLIC export)
target_link_libraries(sdspi target)
