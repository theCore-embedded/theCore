add_library(utils INTERFACE)
target_include_directories(utils INTERFACE export)

add_unit_host_test(
    NAME utils
    SOURCES tests/utils_unit.cpp
    INC_DIRS export
)
