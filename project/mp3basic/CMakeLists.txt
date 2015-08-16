cmake_minimum_required(VERSION 2.8)

# Include special definitions
include(project.cmake)

# Our project sources
add_executable(mp3basic main.cpp)

# Register project with 'mp3basic' target
register_target()
