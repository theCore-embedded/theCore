# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(NOT CONFIG_PLATFORM MATCHES "tm4c")
    return()
endif()

# master branch
theCore_get_thirdparty(ti-tivaware https://github.com/forGGe/ti-tivaware eaa4b2883)

# override warnings, since library contains few of them
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
set(TIVAWARE_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/ti-tivaware)

add_library(tivaware STATIC
        ${TIVAWARE_DIR}/driverlib/adc.c
        ${TIVAWARE_DIR}/driverlib/aes.c
        ${TIVAWARE_DIR}/driverlib/can.c
        ${TIVAWARE_DIR}/driverlib/comp.c
        ${TIVAWARE_DIR}/driverlib/cpu.c
        ${TIVAWARE_DIR}/driverlib/crc.c
        ${TIVAWARE_DIR}/driverlib/des.c
        ${TIVAWARE_DIR}/driverlib/eeprom.c
        ${TIVAWARE_DIR}/driverlib/emac.c
        ${TIVAWARE_DIR}/driverlib/epi.c
        ${TIVAWARE_DIR}/driverlib/flash.c
        ${TIVAWARE_DIR}/driverlib/fpu.c
        ${TIVAWARE_DIR}/driverlib/gpio.c
        ${TIVAWARE_DIR}/driverlib/hibernate.c
        ${TIVAWARE_DIR}/driverlib/i2c.c
        ${TIVAWARE_DIR}/driverlib/interrupt.c
        ${TIVAWARE_DIR}/driverlib/lcd.c
        ${TIVAWARE_DIR}/driverlib/mpu.c
        ${TIVAWARE_DIR}/driverlib/onewire.c
        ${TIVAWARE_DIR}/driverlib/pwm.c
        ${TIVAWARE_DIR}/driverlib/qei.c
        ${TIVAWARE_DIR}/driverlib/shamd5.c
        ${TIVAWARE_DIR}/driverlib/ssi.c
        ${TIVAWARE_DIR}/driverlib/sw_crc.c
        ${TIVAWARE_DIR}/driverlib/sysctl.c
        ${TIVAWARE_DIR}/driverlib/sysexc.c
        ${TIVAWARE_DIR}/driverlib/systick.c
        ${TIVAWARE_DIR}/driverlib/timer.c
        ${TIVAWARE_DIR}/driverlib/uart.c
        ${TIVAWARE_DIR}/driverlib/udma.c
        ${TIVAWARE_DIR}/driverlib/usb.c
        ${TIVAWARE_DIR}/driverlib/watchdog.c
        )

if(thecore_cfg.platform.device MATCHES "TM4C123GH6PM")
    target_compile_definitions(tivaware PUBLIC -DPART_TM4C123GH6PM)
else()
    msg_fatal("Unknown device: ${thecore_cfg.platform.device}")
endif()

target_include_directories(tivaware SYSTEM PUBLIC
        ${TIVAWARE_DIR}/
        ${TIVAWARE_DIR}/inc
        ${TIVAWARE_DIR}/driverlib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(tivaware PUBLIC -DDEBUG)
    target_sources(tivaware PRIVATE debug_stubs.cpp)
    target_link_libraries(tivaware PRIVATE dbg)
endif()

if(CMAKE_C_COMPILER MATCHES "gcc")
    target_compile_definitions(tivaware PRIVATE -Dgcc)
endif()
