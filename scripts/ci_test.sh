#!/bin/sh
# Builds and executes tests.

set -e # Stop on error
set -v # Be verbose

mkdir build
cd build
cmake ..
make
ctest . --output-on-failure
