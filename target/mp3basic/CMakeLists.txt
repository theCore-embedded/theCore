cmake_minimum_required(VERSION 2.8)

add_library(target STATIC target.cpp pinconfig.cpp)
import_module(target platform)

# Register platform as a base for this target
register_platform()
