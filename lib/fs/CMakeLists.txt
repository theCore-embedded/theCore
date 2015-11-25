## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(fs_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(fs STATIC fs.cpp filehandle.cpp)
target_include_directories(fs PUBLIC export)
