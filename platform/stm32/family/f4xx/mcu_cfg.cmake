# Notify whole system about which processor is used
set(TARGET_MCU_ARCH "arm_cm4" CACHE STRING "Processor arch")

message(STATUS "Checking [CONFIG_PLATFORM_DEVICE]...")

# Deprecated configuration value
if(CONFIG_PLATFORM_DEVICE STREQUAL STM32F40_41xxx)
    message(WARNING "Deprecated device config: STM32F40_41xxx"
            ", defaulting to STM32F407VG")
    set(CONFIG_PLATFORM_DEVICE STM32F407VG)
endif()

# Expose per-device definitions

if(CONFIG_PLATFORM_DEVICE STREQUAL STM32F407VG)
    # STM32 device identifier
    set(ST_DEVICE STM32F40_41xxx)

    # Memory layout
    # TODO: differentiate layout with respect to full device specification
    # TODO: #107 - allow user to override these values
    set(TARGET_MCU_ROM_START 0x08000000 CACHE STRING "Flash region start")
    set(TARGET_MCU_ROM_SIZE  0x100000   CACHE STRING "Flash region size")  # 1M of ROM
    set(TARGET_MCU_RAM_START 0x20000000 CACHE STRING "RAM region start")
    set(TARGET_MCU_RAM_SIZE  0x20000    CACHE STRING "RAM region size")    # 128K of RAM

    # IRQ count
    # TODO: differentiate layout with respect to full device specification
    # TODO: #107 - allow user to override these values
    # Count of user-overridable interrupts.
    # For this particular platform it doesn't include system exceptions,
    # but only peripheral interrupts. Yet.
    set(TARGET_MCU_IRQ_COUNT 82 CACHE STRING "Available interrupts")

    # SPI buses count. Essential only for stm32 platform code.
    set(TARGET_MCU_SPI_COUNT 3 CACHE STRING "SPI buses avaliable on the F4 MCU")

    # Direct EXTIs
    set(TARGET_MCU_EXTI_DIRECT_COUNT 5 CACHE STRING "EXTI direct lines")

    # Grouped EXTIs
    set(TARGET_MCU_EXTI_GROUPED_COUNT 2 CACHE STRING "EXTI grouped lines")

    # Present ADC modules
    set(TARGET_MCU_ADC_COUNT 3 CACHE STRING "ADC modules count")

    return()
endif()

# Additional implementation will be required to cover unsupported
# devices from stm32f4xx line.
message(FATAL_ERROR "Not supported device specified: ${CONFIG_PLATFORM_DEVICE}")
