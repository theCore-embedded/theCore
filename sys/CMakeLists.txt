## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(sys_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

# TODO: move it to a separate module
add_library(common_io INTERFACE)
target_include_directories(common_io INTERFACE export)

# Placeholder
add_library(sys STATIC sys.cpp)

target_link_libraries(sys libcpp)
target_link_libraries(sys ${PLATFORM_NAME})
target_link_libraries(sys main)
