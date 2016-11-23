# override warnings, since library contains a lot of them
set(CC_NO_EXTRA_WARNINGS "-Wno-pedantic -Wno-all -Wno-extra")
set(CXX_NO_EXTRA_WARNINGS "-Wno-effc++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CC_NO_EXTRA_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CXX_NO_EXTRA_WARNINGS}")

set(SPL_DIR ${CMAKE_CURRENT_LIST_DIR}/SPL/STM32F4xx_StdPeriph_Driver/)

# note that not all sources are inlcuded, since not all stuff supported by STM32F40xxx.
# TODO: this should be fixed, and proper sources must be added depending which target processor is used.
# list of modules not included:
# ${SPL_DIR}src/stm32f4xx_fmc.c

add_library(stm32_spl STATIC
        ${SPL_DIR}src/stm32f4xx_dbgmcu.c
        ${SPL_DIR}src/stm32f4xx_cryp_tdes.c
        ${SPL_DIR}src/stm32f4xx_usart.c
        ${SPL_DIR}src/stm32f4xx_flash_ramfunc.c
        ${SPL_DIR}src/stm32f4xx_syscfg.c
        ${SPL_DIR}src/stm32f4xx_exti.c
        ${SPL_DIR}src/stm32f4xx_crc.c
        ${SPL_DIR}src/stm32f4xx_tim.c
        ${SPL_DIR}src/stm32f4xx_hash.c
        ${SPL_DIR}src/stm32f4xx_rng.c
        ${SPL_DIR}src/stm32f4xx_flash.c
        ${SPL_DIR}src/stm32f4xx_dcmi.c
        ${SPL_DIR}src/misc.c
        ${SPL_DIR}src/stm32f4xx_rtc.c
        ${SPL_DIR}src/stm32f4xx_spi.c
        ${SPL_DIR}src/stm32f4xx_adc.c
        ${SPL_DIR}src/stm32f4xx_dac.c
        ${SPL_DIR}src/stm32f4xx_dma.c
        ${SPL_DIR}src/stm32f4xx_cryp_aes.c
        ${SPL_DIR}src/stm32f4xx_cryp_des.c
        ${SPL_DIR}src/stm32f4xx_gpio.c
        ${SPL_DIR}src/stm32f4xx_hash_md5.c
        ${SPL_DIR}src/stm32f4xx_dma2d.c
        ${SPL_DIR}src/stm32f4xx_wwdg.c
        ${SPL_DIR}src/stm32f4xx_rcc.c
        ${SPL_DIR}src/stm32f4xx_sdio.c
        ${SPL_DIR}src/stm32f4xx_i2c.c
        ${SPL_DIR}src/stm32f4xx_iwdg.c
        ${SPL_DIR}src/stm32f4xx_pwr.c
        ${SPL_DIR}src/stm32f4xx_ltdc.c
        ${SPL_DIR}src/stm32f4xx_can.c
        ${SPL_DIR}src/stm32f4xx_hash_sha1.c
        ${SPL_DIR}src/stm32f4xx_sai.c
        ${SPL_DIR}src/stm32f4xx_fsmc.c
        ${SPL_DIR}src/stm32f4xx_cryp.c
        )

# TODO:
# Report a bug.
# For whatever reason, 'SYSTEM' is working ONLY if path is absolute
# Why?

# STM32 peripherial library headers
target_include_directories(stm32_spl SYSTEM PUBLIC ${SPL_DIR}/inc)

target_link_libraries(stm32_spl PUBLIC ${PLATFORM_NAME}) # Let it be circular dependency for sake of simplicity
