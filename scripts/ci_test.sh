#!/bin/sh
# Builds and executes tests.

set -e # Stop on error
set -v # Be verbose

# Main Core builds

mkdir -p build
cd build
cmake ..
make
ctest . --output-on-failure
cd ..

# Examples

mkdir -p build_examples
cd build_examples
cmake ../examples
make
cd ..

# Test cases

mkdir -p build_test_cases
cd build_test_cases
cmake ../tests
make
cd ..

# Documentation build

mkdir -p docs
cd docs
cmake ../doc
make core_doc
cd ..
