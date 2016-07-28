if("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32F4")
    include(stm32f4xx_spl.cmake)
elseif("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32L1")
    include(stm32l1xx_spl.cmake)
elseif("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32")
    # Probably in future this fatal error message will be removed,
    # since there are a lot of STM32 devices does not supported by the SPL.
    message(FATAL_ERROR "Unsupported STM32 device: ${CONFIG_PLATFORM_DEVICE}")
endif()
