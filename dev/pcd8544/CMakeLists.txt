## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(pcd_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(pcd8544 STATIC pcd8544.cpp)
target_include_directories(pcd8544 PUBLIC export)
