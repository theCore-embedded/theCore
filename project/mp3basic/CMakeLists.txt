cmake_minimum_required(VERSION 2.8)

# Our project sources
add_executable(mp3basic main.cpp)

# Register project with 'mp3basic' target
register_target()

# TODO: combine it with register_target()
target_link_libraries(mp3basic target)
target_link_libraries(mp3basic pcd8544)
target_link_libraries(mp3basic sdspi)
