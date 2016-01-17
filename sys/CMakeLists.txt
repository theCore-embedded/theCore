## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(sys_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

# Placeholder
add_library(sys STATIC sys.cpp)

target_include_directories(sys PUBLIC export)

target_link_libraries(sys libcpp)
target_link_libraries(sys ${PLATFORM_NAME})
target_link_libraries(sys ${USE_OS})
