cmake_minimum_required(VERSION 2.8)

## Qt creator hack
file(GLOB_RECURSE ALL_TARGET_HEADERS *.hpp)
add_custom_target(target_dummy_target SOURCES "${ALL_TARGET_HEADERS}")
## EO hack

add_library(target STATIC target.cpp pinconfig.cpp ${ALL_TARGET_HEADERS})

target_include_directories(target PUBLIC export)

# TODO: move it to register platform
import_module(target platform)

# Register platform as a base for this target
register_platform()
