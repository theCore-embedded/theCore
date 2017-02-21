message(STATUS "Checking [CONFIG_PLATFORM_DEVICE]...")

if(CONFIG_PLATFORM_DEVICE STREQUAL "TM4C123GH6PM")
    # All members of the TivaTM C Series, including the TM4C123GH6PM
    # microcontroller, are designed around an ARM Cortex-M4 processor core.
    set(TARGET_MCU_ARCH "arm_cm4" CACHE STRING "Processor arch")

    # Memory layout

    # Separate ROM region at address 0x01000000 is also present on a device,
    # but it is not yet used here.

    # Use flash instead of ROM
    set(TARGET_MCU_ROM_START 0x00000000 CACHE STRING "Flash region start")
    set(TARGET_MCU_ROM_SIZE  0x40000    CACHE STRING "Flash region size")  # 256k of flash
    set(TARGET_MCU_RAM_START 0x20000000 CACHE STRING "RAM region start")
    set(TARGET_MCU_RAM_SIZE  0x8000     CACHE STRING "RAM region size")    # 32k of RAM

    # IRQ count
    # TODO: differentiate layout with respect to full device specification
    # TODO: #107 - allow user to override these values
    # Count of user-overridable interrupts.
    # For this particular platform it doesn't include system exceptions,
    # but only peripheral interrupts. Yet.
    set(TARGET_MCU_IRQ_COUNT 139 CACHE STRING "Available interrupts")
    return()
endif()

message(FATAL_ERROR "${CONFIG_PLATFORM_DEVICE} is not supported")
