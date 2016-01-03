## Qt creator hack
file(GLOB_RECURSE ALL_PLATFORM_HEADERS *.hpp)
add_custom_target(platform_dummy_target SOURCES "${ALL_PLATFORM_HEADERS}")
## EO hack

add_library(${PLATFORM_NAME} STATIC platform.cpp)# ${ALL_PLATFORM_HEADERS})

target_include_directories(${PLATFORM_NAME} PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)
add_subdirectory(CMSIS)

# TODO: clarify essential libraries
target_link_libraries(${PLATFORM_NAME} startup)
target_link_libraries(${PLATFORM_NAME} spl)
