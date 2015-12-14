## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(libcpp_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(libcpp STATIC arm_eabi.cpp)
target_include_directories(libcpp PUBLIC export)
target_link_libraries(libcpp target)
