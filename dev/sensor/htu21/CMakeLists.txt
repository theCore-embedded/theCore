## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(htu21_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(htu21 STATIC htu21.cpp)
target_include_directories(htu21 PUBLIC export)