#!/usr/bin/python3

# Script takes list of suite binaries and connected boards and tries to flash
# binaries into the boards connected

# Boards file example:
# {
#     "stm32f4discovery_simple": {
#         "debugger": {
#             "name": "openocd",
#             "flash_params": "init; reset halt; wait_halt; flash write_image erase {elf_path}; reset run; shutdown",
#             "conf_file" : "stm32f4discovery.cfg"
#         }
#     }
# }

import argparse
import csv
import json
import subprocess

#-------------------------------------------------------------------------------
# Flash executors

def do_openocd_flash(suite_data, board_data):
    flash_param = board_data['debugger']['flash_params'].format(elf_path = suite_data['elf_path'])
    conf_file = board_data['debugger']['conf_file']

    action = input('Suite: ' + suite_data['suite_name'] +
          ' is ready to be uploaded to ' + suite_data['target_name'] + '. Proceed? [y/N/a]: ')

    if action == 'y':
        cmd = [ 'openocd', '-f', conf_file, '-c', flash_param ]
        print(cmd)
        subprocess.run(cmd)
    elif action == 'n':
        print('Skipping to the next test...')
    elif action == 'a':
        print('Abort requested.')
        exit(0)
    else:
        print('Illegal input. Aborting.')
        exit(1)


flash_executors = { 'openocd' : do_openocd_flash }

#-------------------------------------------------------------------------------

parser = argparse.ArgumentParser(description='Flash connected devices with suite binaries provided.')

parser.add_argument('-l', '--list-file', metavar='list_file', type=argparse.FileType('r'),
                    help='file with lists of binaries inside', required=True, dest='list_file')
parser.add_argument('-b', '--boards-file', metavar='boards_file', type=argparse.FileType('r'),
                    help='file with boards connected', required=True, dest='boards_file')

args = parser.parse_args()
list_file = args.list_file
boards_file = args.boards_file

list_reader = csv.DictReader(list_file,
                             fieldnames=('suite_name', 'target_name', 'elf_path', 'bin_path'),
                             delimiter=',')

for row in list_reader:
    print('Found suite: ' + row['suite_name'] + ' target: ' + row['target_name'])

boards = json.load(boards_file)

for board_name, board_data in boards.items():
    print('Board is probably connected: ' + board_name + ' debugger: ' + board_data['debugger']['name'])

# Rewind back
list_file.seek(0)

for row in list_reader:
    if row['target_name'] in boards:
        board_data = boards[row['target_name']]
        debugger = board_data['debugger']['name']
        flash_executors[debugger](suite_data = row, board_data = board_data)

print('Test flasing finished')
