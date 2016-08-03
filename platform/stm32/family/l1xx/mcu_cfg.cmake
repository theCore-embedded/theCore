# Notify whole system about which processor is used
set(TARGET_MCU_ARCH "arm_cm3" CACHE STRING "Processor arch")

message(STATUS "Checking [CONFIG_PLATFORM_DEVICE]...")

# Depreceated configuration value
if(CONFIG_PLATFORM_DEVICE STREQUAL STM32L1XX_XL)
    message(WARNING "Depreceated device config: STM32L1XX_XL"
            ", defauluting to STM32L152RE")
    set(CONFIG_PLATFORM_DEVICE STM32L152RE)
endif()

# Expose per-device definitions

if(CONFIG_PLATFORM_DEVICE STREQUAL STM32L152RE)
    # STM32 device identifier
    set(ST_DEVICE STM32L1XX_XL)

    # Memory layout
    # TODO: differentiate layout with respect to full device specification
    # TODO: #107 - allow user to override these values
    set(TARGET_MCU_ROM_START 0x08000000 CACHE STRING "Flash region start")
    set(TARGET_MCU_ROM_SIZE  0x80000    CACHE STRING "Flash region size")  # 512k of ROM
    set(TARGET_MCU_RAM_START 0x20000000 CACHE STRING "RAM region start")
    set(TARGET_MCU_RAM_SIZE  0x14000    CACHE STRING "RAM region size")    # 80k of RAM

    # IRQ count
    # TODO: differentiate layout with respect to full device specification
    # TODO: #107 - allow user to override these values
    # Count of user-overridable interrupts.
    # For this particular platform it doesn't include system exceptions,
    # but only peripheral interrupts. Yet.
    set(TARGET_MCU_IRQ_COUNT 57 CACHE STRING "Avaliable interrupts")

    # SPI buses count. Essential only for stm32 platform code.
    set(TARGET_MCU_SPI_COUNT 3 CACHE STRING "SPI buses avaliable on the F4 MCU")

    # Direct EXTIs
    set(TARGET_MCU_EXTI_DIRECT_COUNT 5 CACHE STRING "EXTI direct lines")

    # Grouped EXTIs
    set(TARGET_MCU_EXTI_GROUPED_COUNT 5 CACHE STRING "EXTI grouped lines")

    return()
endif()

# Additional implementation will be required to cover unsupported
# devices from stm32f4xx line.
message(FATAL_ERROR "Not supported device specified: ${CONFIG_PLATFORM_DEVICE}")
