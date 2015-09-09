## Qt creator hack
file(GLOB_RECURSE ALL_PLATFORM_HEADERS *.hpp)
add_custom_target(platform_dummy_target SOURCES "${ALL_PLATFORM_HEADERS}")
## EO hack

add_library(platform STATIC platform.cpp)# ${ALL_PLATFORM_HEADERS})

target_include_directories(platform PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)

# TODO: clarify essential libraries
target_link_libraries(platform startup)
target_link_libraries(platform spl)
