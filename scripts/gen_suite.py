#!/usr/bin/python3

import argparse
import re
import sys

## Command line parser

parser = argparse.ArgumentParser(description='Generate test suite based on input test modules.')

parser.add_argument('-i', '--in', metavar='file', type=argparse.FileType('r'), nargs='+',
                    help='file with tests inside', required=True, dest='input')
parser.add_argument('-o', '--out', metavar='file', type=argparse.FileType('w'), nargs=1,
                    help='output file (default is stdout)', default=sys.stdout, dest='output')

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

includes = '\n\n#include <unity.h>\n#include <unity_fixture.h>\n\n'

board_init = '\
extern "C" void board_init()\n\
{\n\
    suite_board_init();\n\
}\n\n'

suite_runner_start = 'static void suite_runner()\n{\n'

suite_runner_test_group = '    RUN_TEST_GROUP({test_group});\n'

suite_runner_end = '}\n\n'

group_runner_start = 'TEST_GROUP_RUNNER({test_group})\n{{\n'

group_runner_test_case = '    RUN_TEST_CASE({test_group}, {test_name});\n'

group_runner_end ='}\n\n'

suite_main = '\
int main()\n\
{\n\
    UNITY_BEGIN();\n\
    suite_runner();\n\
    UNITY_END();\n\
\n\
    // Suite completed...\n\
    for (;;);\n\
}\n\n'

# Print generated file

output = args.output

# Prologue

output.write(includes)
output.write(board_init)

# List of tests cases called from group runners

for group, tests in grouped_tests.items():
    output.write(group_runner_start.format(test_group=group))
    for test in tests:
        output.write(group_runner_test_case.format(test_group=group, test_name=test))
    output.write(group_runner_end)

# List of groups called from suite runner

output.write(suite_runner_start)
for group, tests in grouped_tests.items():
    output.write(suite_runner_test_group.format(test_group=group))
output.write(suite_runner_end)

# Epilogue
output.write(suite_main)
