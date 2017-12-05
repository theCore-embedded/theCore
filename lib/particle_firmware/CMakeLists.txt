if(NOT PLATFORM_NAME STREQUAL "particle_electron")
    return()
endif()

add_library(particle_firmware INTERFACE)

# v0.5.1-rc.1-769-ged6f6640
theCore_get_thirdparty_cmd(particle_3rdparty
        https://github.com/forGGe/firmware.git ed6f66405af)

include(ExternalProject)

set(USER_FIRMWARE_IMAGE_LOCATION 0x8080000)
set(USER_FIRMWARE_IMAGE_SIZE 0x20000)

# Common definitions for all firmware projects
set(COMMON_DEFS
    -DSTM32_DEVICE
    -DSTM32F2XX
    -DPLATFORM_THREADING=1
    -DPLATFORM_ID=10
    -DPLATFORM_NAME=electron
    -DUSBD_VID_SPARK=0x2B04
    -DUSBD_PID_DFU=0xD00A
    -DUSBD_PID_CDC=0xC00A
    -DSPARK_PLATFORM
    -DINCLUDE_PLATFORM=1
    -DPRODUCT_ID=10
    -DPRODUCT_FIRMWARE_VERSION=65535
    -DUSE_STDPERIPH_DRIVER
    -DDFU_BUILD_ENABLE
    -DSYSTEM_VERSION_STRING=0.6.2
    -DRELEASE_BUILD
    -DSPARK=1
    -DPARTICLE=1
    -DSTART_DFU_FLASHER_SERIAL_SPEED=14400
    -DSTART_YMODEM_FLASHER_SERIAL_SPEED=28800
    -DSPARK_PLATFORM_NET=UBLOXSARA
    -DLOG_INCLUDE_SOURCE_INFO=1
    -DPARTICLE_USER_MODULE
    -DUSE_THREADING=0
    -DUSE_SPI=SPI
    -DUSE_CS=A2
    -DUSE_SPI=SPI
    -DUSE_CS=A2
    -DUSE_THREADING=0
    -DUSER_FIRMWARE_IMAGE_SIZE=${USER_FIRMWARE_IMAGE_SIZE}
    -DUSER_FIRMWARE_IMAGE_LOCATION=${USER_FIRMWARE_IMAGE_LOCATION}
    -DMODULAR_FIRMWARE=1
    -DMODULE_VERSION=4
    -DMODULE_FUNCTION=5
    -DMODULE_INDEX=1
    -DMODULE_DEPENDENCY=4,2,108
    -D_WINSOCK_H
    -D_GNU_SOURCE
)

set(PARTICLE_FIRMWARE_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/particle_3rdparty)
ExternalProject_Add(particle_build
    DOWNLOAD_COMMAND "${particle_3rdparty_get_cmd}"
    SOURCE_DIR "${PARTICLE_FIRMWARE_DIR}"
    BUILD_COMMAND PLATFORM=electron v=1 make main
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_ALWAYS 1
)

add_dependencies(particle_firmware particle_build)

target_include_directories(particle_firmware INTERFACE
    ${PARTICLE_FIRMWARE_DIR}/user/inc
    ${PARTICLE_FIRMWARE_DIR}/dynalib/inc
    ${PARTICLE_FIRMWARE_DIR}/services/inc
    ${PARTICLE_FIRMWARE_DIR}/hal/rtos/FreeRTOSv8.2.2/FreeRTOS/Source/include
    ${PARTICLE_FIRMWARE_DIR}/hal/rtos/FreeRTOSv8.2.2/FreeRTOS/Source/portable/GCC/ARM_CM3
    ${PARTICLE_FIRMWARE_DIR}/rt-dynalib/inc
    ${PARTICLE_FIRMWARE_DIR}/modules/shared/stm32f2xx/inc
    ${PARTICLE_FIRMWARE_DIR}/modules/electron/system-part2/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/shared/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/STM32_USB_Host_Driver/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/STM32_StdPeriph_Driver/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/STM32_USB_OTG_Driver/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/STM32_USB_Device_Driver/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/SPARK_Firmware_Driver/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/shared/STM32/inc
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/CMSIS/Include
    ${PARTICLE_FIRMWARE_DIR}/platform/MCU/STM32F2xx/CMSIS/Device/ST/Include
    ${PARTICLE_FIRMWARE_DIR}/wiring/inc
    ${PARTICLE_FIRMWARE_DIR}/system/inc
    ${PARTICLE_FIRMWARE_DIR}/services/inc
    ${PARTICLE_FIRMWARE_DIR}/communication/src
    ${PARTICLE_FIRMWARE_DIR}/hal/inc
    ${PARTICLE_FIRMWARE_DIR}/hal/shared
    ${PARTICLE_FIRMWARE_DIR}/hal/src/electron
    ${PARTICLE_FIRMWARE_DIR}/hal/src/stm32f2xx
    ${PARTICLE_FIRMWARE_DIR}/hal/src/stm32
    ${PARTICLE_FIRMWARE_DIR}/platform/shared/inc
    ${PARTICLE_FIRMWARE_DIR}/user/libraries
    ${PARTICLE_FIRMWARE_DIR}/user/src
)

target_link_libraries(particle_firmware INTERFACE
    -L${PARTICLE_FIRMWARE_DIR}/build/arm/linker/stm32f2xx
    -L${PARTICLE_FIRMWARE_DIR}/build/target/user/platform-10-m/src/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/services-dynalib/arm/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/hal-dynalib/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/system-dynalib/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/rt-dynalib/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/wiring/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/communication-dynalib/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/platform/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/target/wiring_globals/platform-10-m/
    -L${PARTICLE_FIRMWARE_DIR}/build/arm/linker
     ${PARTICLE_FIRMWARE_DIR}/build/target/user-part/platform-10-m/src/user_module.o
     ${PARTICLE_FIRMWARE_DIR}/build/target/user-part/platform-10-m/src/module_info.o
     ${PARTICLE_FIRMWARE_DIR}/build/target/user-part/platform-10-m/src/user_export.o
     ${PARTICLE_FIRMWARE_DIR}/build/target/user-part/platform-10-m/src/newlib_stubs.o
    -Wl,--whole-archive
    -lhal-dynalib -lservices-dynalib -lsystem-dynalib -lrt-dynalib -lwiring
    -lcommunication-dynalib -lplatform -lwiring_globals
    -Wl,--no-whole-archive
    -lnosys
    -L${PARTICLE_FIRMWARE_DIR}/modules/electron/system-part1
    -L${PARTICLE_FIRMWARE_DIR}/modules/electron/system-part3
    -L${PARTICLE_FIRMWARE_DIR}/modules/electron/system-part2
    -L${PARTICLE_FIRMWARE_DIR}/modules/electron/user-part/
    -T${PARTICLE_FIRMWARE_DIR}/modules/electron/user-part/linker.ld
    -Wl,--defsym,USER_FIRMWARE_IMAGE_SIZE=${USER_FIRMWARE_IMAGE_SIZE}
    -Wl,--defsym,USER_FIRMWARE_IMAGE_LOCATION=${USER_FIRMWARE_IMAGE_LOCATION}
)

target_compile_definitions(particle_firmware INTERFACE
    ${COMMON_DEFS}
)
