# Might be reviwed.
cmake_minimum_required(VERSION 3.4)

# Special module will separate builds for each example project.
include(ExternalProject)

# Root project for bunch of The Core examples.
project(example_hub)

################################################################################
# Hello World host example.
################################################################################

ExternalProject_Add(host_hello
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/host_hello
        BUILD_ALWAYS 1 # Allows to rebuild external project if its files changes
        INSTALL_COMMAND echo "Install isn't required, skipping...")
