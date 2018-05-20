# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(CONFIG_PLATFORM STREQUAL "ke02")
    theCore_get_thirdparty(kexx_drv https://github.com/forGGe/kexx_driverlib 93eaf3f5d)
else()
    return()
endif()

set(KEXX_DRV_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/kexx_drv)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")

add_library(kexx_drv STATIC
        ${KEXX_DRV_DIR}/src/drivers/uart/uart.c
        ${KEXX_DRV_DIR}/src/drivers/gpio/gpio.c
        ${KEXX_DRV_DIR}/src/drivers/ics/ics.c
        ${KEXX_DRV_DIR}/src/drivers/sim/sim.c
        ${KEXX_DRV_DIR}/src/drivers/pit/pit.c
        ${KEXX_DRV_DIR}/src/drivers/nvm/flash.c
        ${KEXX_DRV_DIR}/src/drivers/ftm/ftm.c
        ${KEXX_DRV_DIR}/src/drivers/wdog/wdog.c
        ${KEXX_DRV_DIR}/src/drivers/adc/adc.c
        ${KEXX_DRV_DIR}/src/drivers/kbi/kbi.c
        ${KEXX_DRV_DIR}/src/cpu/sysinit.c
        ${KEXX_DRV_DIR}/src/common/assert.c
        # All I/O functions are intentionally avoided,
        # due to hard-coded output device. Instead, theCore provides its
        # own I/O implementation.
        # ${KEXX_DRV_DIR}/src/common/io.c
        ${KEXX_DRV_DIR}/src/common/printf.c
        ${KEXX_DRV_DIR}/src/common/alloc.c)

target_include_directories(kexx_drv SYSTEM PUBLIC
        ${KEXX_DRV_DIR}/src/drivers/uart/
        ${KEXX_DRV_DIR}/src/drivers/gpio/
        ${KEXX_DRV_DIR}/src/drivers/wdog/
        ${KEXX_DRV_DIR}/src/drivers/sim/
        ${KEXX_DRV_DIR}/src/drivers/ics/
        ${KEXX_DRV_DIR}/src/drivers/pit/
        ${KEXX_DRV_DIR}/src/drivers/nvm/
        ${KEXX_DRV_DIR}/src/drivers/ftm/
        ${KEXX_DRV_DIR}/src/drivers/adc/
        ${KEXX_DRV_DIR}/src/drivers/kbi/
        ${KEXX_DRV_DIR}/src/platforms/
        ${KEXX_DRV_DIR}/src/common/
        ${KEXX_DRV_DIR}/src/cpu/headers
        ${KEXX_DRV_DIR}/src/cpu/)

# TODO: move it to the header, rather than compile difinitions
target_compile_definitions(kexx_drv PUBLIC -DFRDM_KE02 -DCPU_KE02)
target_link_libraries(kexx_drv PUBLIC cmsis)
