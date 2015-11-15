## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(libcpp_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(libcpp STATIC cpp.cpp)
target_include_directories(libcpp PUBLIC export)
