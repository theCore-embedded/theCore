## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(fat_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(fat STATIC inode.cpp)
target_include_directories(fat PUBLIC export)
target_link_libraries(fat fs)
