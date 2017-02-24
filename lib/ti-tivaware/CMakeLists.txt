if(NOT CONFIG_PLATFORM MATCHES "tm4c")
    return()
endif()

# override warnings, since library contains few of them
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")

add_library(tivaware STATIC
        ti-tivaware/driverlib/adc.c
        ti-tivaware/driverlib/aes.c
        ti-tivaware/driverlib/can.c
        ti-tivaware/driverlib/comp.c
        ti-tivaware/driverlib/cpu.c
        ti-tivaware/driverlib/crc.c
        ti-tivaware/driverlib/des.c
        ti-tivaware/driverlib/eeprom.c
        ti-tivaware/driverlib/emac.c
        ti-tivaware/driverlib/epi.c
        ti-tivaware/driverlib/flash.c
        ti-tivaware/driverlib/fpu.c
        ti-tivaware/driverlib/gpio.c
        ti-tivaware/driverlib/hibernate.c
        ti-tivaware/driverlib/i2c.c
        ti-tivaware/driverlib/interrupt.c
        ti-tivaware/driverlib/lcd.c
        ti-tivaware/driverlib/mpu.c
        ti-tivaware/driverlib/onewire.c
        ti-tivaware/driverlib/pwm.c
        ti-tivaware/driverlib/qei.c
        ti-tivaware/driverlib/shamd5.c
        ti-tivaware/driverlib/ssi.c
        ti-tivaware/driverlib/sw_crc.c
        ti-tivaware/driverlib/sysctl.c
        ti-tivaware/driverlib/sysexc.c
        ti-tivaware/driverlib/systick.c
        ti-tivaware/driverlib/timer.c
        ti-tivaware/driverlib/uart.c
        ti-tivaware/driverlib/udma.c
        ti-tivaware/driverlib/usb.c
        ti-tivaware/driverlib/watchdog.c
        )

if(CONFIG_PLATFORM_DEVICE MATCHES TM4C123GH6PM)
    target_compile_definitions(tivaware PUBLIC -DPART_TM4C123GH6PM)
else()
    message(FATAL_ERROR "Unknown device: ${CONFIG_PLATFORM_DEVICE}")
endif()

target_include_directories(tivaware SYSTEM PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ti-tivaware
        ${CMAKE_CURRENT_LIST_DIR}/ti-tivaware/inc
        ${CMAKE_CURRENT_LIST_DIR}/ti-tivaware/driverlib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(tivaware PUBLIC -DDEBUG)
    target_sources(tivaware PRIVATE debug_stubs.cpp)
    target_link_libraries(tivaware PRIVATE dbg)
endif()

if(CMAKE_C_COMPILER MATCHES "gcc")
    target_compile_definitions(tivaware PRIVATE -Dgcc)
endif()
