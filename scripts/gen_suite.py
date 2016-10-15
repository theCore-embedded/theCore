#!/usr/bin/python3

import argparse
import re
import sys
import os

## Command line parser

parser = argparse.ArgumentParser(description='Generate test suite based on input test modules.')

parser.add_argument('-s', '--suite-name', metavar='suite', type=str,
                    help='suite name', required=True, dest='suite_name')
parser.add_argument('-i', '--in', metavar='file', type=argparse.FileType('r'), nargs='+',
                    help='file with tests inside', required=True, dest='input')
parser.add_argument('-o', '--out-cpp', metavar='file', type=argparse.FileType('w'),
                    help='output cpp file (default is stdout)', default=sys.stdout, dest='output_cpp')
parser.add_argument('-m', '--out-cmake', metavar='file', type=argparse.FileType('w'),
                    help='output cmake file (default is stdout)', default=sys.stdout, dest='output_cmake')

args = parser.parse_args()
print(args.input)

# Maps test groups to test lists
# Structure:
# group_name0 -> test0, test1, test2 ...
# group_name1 -> test0, test1, test2 ...
grouped_tests={}

for f in args.input:
    for line in f:
        m = re.search('TEST\((?P<group_name>\w[\w\d]*),\s*(?P<group_test>\w[\w\d]*)\)', line)
        if m is not None:
            gname = m.group('group_name')
            gtest = m.group('group_test')
            grouped_tests.setdefault(gname, []).append(gtest)

print('Tests found:')
for group, tests in grouped_tests.items():
    print('Group: ' + group + ' tests: ' + str(tests))

# Output templates

# Test runner

includes = '\n\n#include <unity.h>\n#include <unity_fixture.h>\n\n'

board_init = 'extern "C" void board_init()\n' \
             '{\n' \
             '    suite_board_init();\n' \
             '}\n\n'

suite_runner_start = 'static void suite_runner()\n{\n'

suite_runner_test_group = '    RUN_TEST_GROUP({test_group});\n'

suite_runner_end = '}\n\n'

group_runner_start = 'TEST_GROUP_RUNNER({test_group})\n{{\n'

group_runner_test_case = '    RUN_TEST_CASE({test_group}, {test_name});\n'

group_runner_end ='}\n\n'

suite_main = 'int main()\n' \
             '{\n' \
             '    UNITY_BEGIN();\n' \
             '    suite_runner();\n' \
             '    UNITY_END();\n' \
             '    // Suite completed...\n' \
             '    for (;;);\n ' \
             '}\n\n'

# CMake test list

cmake_test_template = '\nadd_subdirectory(${{TESTCASES_DIR}}/{test_name}/ ' \
                      '${{CMAKE_CURRENT_BINARY_DIR}}/{test_name}_case/)\n' \
                      'target_link_libraries({suite_name}, {test_name}_case)\n'

# Print generated file

output_cpp = args.output_cpp

# Prologue

output_cpp.write(includes)
output_cpp.write(board_init)

# List of tests cases called from group runners

for group, tests in grouped_tests.items():
    output_cpp.write(group_runner_start.format(test_group=group))
    for test in tests:
        output_cpp.write(group_runner_test_case.format(test_group=group, test_name=test))
    output_cpp.write(group_runner_end)

# List of groups called from suite runner

output_cpp.write(suite_runner_start)
for group, tests in grouped_tests.items():
    output_cpp.write(suite_runner_test_group.format(test_group=group))
output_cpp.write(suite_runner_end)

# Epilogue
output_cpp.write(suite_main)

# CMake test list

output_cmake = args.output_cmake

# Test case directory name should be the same as test case name
test_dirnames = [ os.path.basename(os.path.dirname(f.name)) for f in args.input ]
for test in test_dirnames:
    output_cmake.write(cmake_test_template.format(test_name=test, suite_name=args.suite_name))

