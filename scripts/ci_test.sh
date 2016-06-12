#!/bin/sh
# Builds and executes tests.

set -e # Stop on error
set -v # Be verbose

# Main Core builds

mkdir build
cd build
cmake ..
make
ctest . --output-on-failure
cd ..

# Examples

mkdir build_examples
cd build_examples
cmake ../examples
make
