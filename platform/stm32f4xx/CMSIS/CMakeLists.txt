add_library(cmsis st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC includes)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-function -Wno-unused-function")

# TODO: check if variables are defined
target_compile_definitions(cmsis PUBLIC "-D${CONFIG_PLATFORM_DEVICE}")
target_compile_definitions(cmsis PUBLIC "-DHSE_VALUE=${CONFIG_PLATFORM_HSE_VAL}")

# RCC related defines.
# Define CONFIG_PLATFORM_{PARAM} in your project file

# The {PARAM} can be one of:
# RCC_SYSCLK_SRC - HSE, HSI, PLL - main clock source
# RCC_PLL_SRC - HSE, HSI - clock source for PLL
# RCC_HCLK_DIV - AHB prescaler (1,2,4,..512)
# RCC_PCLK1_DIV - APB1 prescaler (1,2,4,8,16)
# RCC_PCLK2_DIV - APB2 prescaler (1,2,4,8,16)

# Where:
# HSI = 1
# HSE = 2
# PLL = 3

# NOTE:
# Please, be careful with changing of the PLL prescalers.
# Not all combinations are correct.
# Read Reference Manual (RM) first.
#
# RCC_PLL_M - main PLL division factor (2,3,..63), see RM for details
# RCC_PLL_N - main PLL multiplication factor for VCO (2,3..see RM for details)
# RCC_PLL_P - can be 2, 4, 6, 8 (see RM)
# RCC_PLL_Q - (1..15), see RM for details
#
# PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
# SYSCLK = PLL_VCO / PLL_P
# USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ

# In case defines were not provided, default will be used:
# sysclk = (HSE + PLL), AHB=sysclk, APB1=AHB/2, APB2=AHB

if (CONFIG_PLATFORM_RCC_SYSCLK_SRC)
target_compile_definitions(cmsis PUBLIC "-DRCC_SYSCLK_SRC=${CONFIG_PLATFORM_RCC_SYSCLK_SRC}")
endif()

if (CONFIG_PLATFORM_RCC_PLL_SRC)
target_compile_definitions(cmsis PUBLIC "-DRCC_PLL_SRC=${CONFIG_PLATFORM_RCC_PLL_SRC}")
endif()

if (CONFIG_PLATFORM_RCC_HCLK_DIV)
target_compile_definitions(cmsis PUBLIC "-DRCC_HCLK_DIV=${CONFIG_PLATFORM_RCC_HCLK_DIV}")
endif()

if (CONFIG_PLATFORM_RCC_PCLK1_DIV)
target_compile_definitions(cmsis PUBLIC "-DRCC_PCLK1_DIV=${CONFIG_PLATFORM_RCC_PCLK1_DIV}")
endif()

if (CONFIG_PLATFORM_RCC_PCLK2_DIV)
target_compile_definitions(cmsis PUBLIC "-DRCC_PCLK2_DIV=${CONFIG_PLATFORM_RCC_PCLK2_DIV}")
endif()

if (CONFIG_PLATFORM_RCC_PLL_M)
target_compile_definitions(cmsis PUBLIC "-DRCC_PLL_M=${CONFIG_PLATFORM_RCC_PLL_M}")
endif()

if (CONFIG_PLATFORM_RCC_PLL_N)
target_compile_definitions(cmsis PUBLIC "-DRCC_PLL_N=${CONFIG_PLATFORM_RCC_PLL_N}")
endif()

if (CONFIG_PLATFORM_RCC_PLL_P)
target_compile_definitions(cmsis PUBLIC "-DRCC_PLL_P=${CONFIG_PLATFORM_RCC_PLL_P}")
endif()

if (CONFIG_PLATFORM_RCC_PLL_Q)
target_compile_definitions(cmsis PUBLIC "-DRCC_PLL_Q=${CONFIG_PLATFORM_RCC_PLL_Q}")
endif()