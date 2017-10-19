# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# override warnings, since library contains a lot of them
set(CC_NO_EXTRA_WARNINGS "-Wno-pedantic -Wno-all -Wno-extra")
set(CXX_NO_EXTRA_WARNINGS "-Wno-effc++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CC_NO_EXTRA_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CXX_NO_EXTRA_WARNINGS}")

set(SPL_DIR ${CMAKE_CURRENT_LIST_DIR}/SPL/STM32L1xx_StdPeriph_Driver/)

add_library(stm32_spl STATIC
        ${SPL_DIR}src/misc.c
        ${SPL_DIR}src/stm32l1xx_adc.c
        ${SPL_DIR}src/stm32l1xx_aes.c
        ${SPL_DIR}src/stm32l1xx_aes_util.c
        ${SPL_DIR}src/stm32l1xx_comp.c
        ${SPL_DIR}src/stm32l1xx_crc.c
        ${SPL_DIR}src/stm32l1xx_dac.c
        ${SPL_DIR}src/stm32l1xx_dbgmcu.c
        ${SPL_DIR}src/stm32l1xx_dma.c
        ${SPL_DIR}src/stm32l1xx_exti.c
        ${SPL_DIR}src/stm32l1xx_flash.c
        ${SPL_DIR}src/stm32l1xx_flash_ramfunc.c
        ${SPL_DIR}src/stm32l1xx_fsmc.c
        ${SPL_DIR}src/stm32l1xx_gpio.c
        ${SPL_DIR}src/stm32l1xx_i2c.c
        ${SPL_DIR}src/stm32l1xx_iwdg.c
        ${SPL_DIR}src/stm32l1xx_lcd.c
        ${SPL_DIR}src/stm32l1xx_opamp.c
        ${SPL_DIR}src/stm32l1xx_pwr.c
        ${SPL_DIR}src/stm32l1xx_rcc.c
        ${SPL_DIR}src/stm32l1xx_rtc.c
        ${SPL_DIR}src/stm32l1xx_sdio.c
        ${SPL_DIR}src/stm32l1xx_spi.c
        ${SPL_DIR}src/stm32l1xx_syscfg.c
        ${SPL_DIR}src/stm32l1xx_tim.c
        ${SPL_DIR}src/stm32l1xx_usart.c
        ${SPL_DIR}src/stm32l1xx_wwdg.c
        )

# TODO:
# Report a bug.
# For whatever reason, 'SYSTEM' is working ONLY if path is absolute
# Why?

# STM32 peripherial library headers
target_include_directories(stm32_spl SYSTEM PUBLIC ${SPL_DIR}/inc)

target_link_libraries(stm32_spl PUBLIC ${PLATFORM_NAME}) # Let it be circular dependency for sake of simplicity
