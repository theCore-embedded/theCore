#!/usr/bin/env sh
# Builds and executes tests, examples, docs

set -e # Stop on error
set -v # Be verbose

# Main Core builds

mkdir -p build
cd build

mkdir -p units
cd units
cmake ../..
make
ctest . --output-on-failure
cd ..

# System test cases

# TODO: test it and make sure that it supports new configurator
# mkdir -p build_test_cases
# cd build_test_cases
# cmake ../../tests
# make
# cd ..

# Documentation build

mkdir -p docs
cd docs
cmake ../../doc
# Doxygen will be built automatically
make core_sphinx
cd ..
