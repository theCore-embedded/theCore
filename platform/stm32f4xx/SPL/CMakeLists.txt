# override warnings, since library contains a lot of them
set(CC_NO_EXTRA_WARNINGS "-Wno-pedantic -Wno-all -Wno-extra")
set(CXX_NO_EXTRA_WARNINGS "-Wno-effc++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CC_NO_EXTRA_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CXX_NO_EXTRA_WARNINGS}")

# note that not all sources are inlcuded, since not all stuff supported by STM32F40xxx.
# TODO: this should be fixed, and proper sources must be added depending which target processor is used.
# list of modules not included:
# ./src/stm32f4xx_fmc.c

add_library(spl STATIC
        ./src/stm32f4xx_dbgmcu.c
        ./src/stm32f4xx_cryp_tdes.c
        ./src/stm32f4xx_usart.c
        ./src/stm32f4xx_flash_ramfunc.c
        ./src/stm32f4xx_syscfg.c
        ./src/stm32f4xx_exti.c
        ./src/stm32f4xx_crc.c
        ./src/stm32f4xx_tim.c
        ./src/stm32f4xx_hash.c
        ./src/stm32f4xx_rng.c
        ./src/stm32f4xx_flash.c
        ./src/stm32f4xx_dcmi.c
        ./src/misc.c
        ./src/stm32f4xx_rtc.c
        ./src/stm32f4xx_spi.c
        ./src/stm32f4xx_adc.c
        ./src/stm32f4xx_dac.c
        ./src/stm32f4xx_dma.c
        ./src/stm32f4xx_cryp_aes.c
        ./src/stm32f4xx_cryp_des.c
        ./src/stm32f4xx_gpio.c
        ./src/stm32f4xx_hash_md5.c
        ./src/stm32f4xx_dma2d.c
        ./src/stm32f4xx_wwdg.c
        ./src/stm32f4xx_rcc.c
        ./src/stm32f4xx_sdio.c
        ./src/stm32f4xx_i2c.c
        ./src/stm32f4xx_iwdg.c
        ./src/stm32f4xx_pwr.c
        ./src/stm32f4xx_ltdc.c
        ./src/stm32f4xx_can.c
        ./src/stm32f4xx_hash_sha1.c
        ./src/stm32f4xx_sai.c
        ./src/stm32f4xx_fsmc.c
        ./src/stm32f4xx_cryp.c
        )

# TODO:
# Report a bug.
# For whatever reason, 'SYSTEM' is working ONLY if path is absolute
# Why?

# STM32 peripherial library headers
target_include_directories(spl SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(spl PRIVATE stm32f4xx) # Let it be circular dependency for sake of simplicity
target_link_libraries(spl PUBLIC system_stm32f4xx)
