cmake_minimum_required(VERSION 2.8)

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(mp3basic_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

# Our project sources
add_executable(mp3basic main.cpp)

# Register project with 'mp3basic' target
register_target()

# TODO: combine it with register_target()
target_link_libraries(mp3basic target)
target_link_libraries(mp3basic pcd8544)
target_link_libraries(mp3basic sys)
target_link_libraries(mp3basic sdspi)
target_link_libraries(mp3basic fat)
target_link_libraries(mp3basic allocators)
target_link_libraries(mp3basic fs)
