cmake_minimum_required(VERSION 2.8)

# Our project sources
add_executable(mp3basic main.cpp)

# Register project with 'mp3basic' target
register_target()

# TODO: combine it with register_target()
import_module(mp3basic target)
