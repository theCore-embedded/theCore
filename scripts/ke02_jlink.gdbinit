#*************************************************
#
# Connect to J-Link and debug application in flash on Cortex-M3
#
#*************************************************


# Connect to the J-Link gdb server
target remote localhost:2331
monitor speed 1000
monitor endian little

monitor flash device = MKE02Z64xxx2
monitor flash breakpoints = 1
monitor flash download = 1


def target_reset
    monitor reset 6

    # Initializing PC and stack pointer
    monitor reg r13 = (0x00000000)
    monitor reg pc = (0x00000004)
end

def target_load
    monitor halt

    load
    target_reset
end