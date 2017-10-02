# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(CONFIG_PLATFORM_DEVICE MATCHES "STM32F4")
    include(stm32f4xx_spl.cmake)
elseif(CONFIG_PLATFORM_DEVICE MATCHES "STM32L1")
    include(stm32l1xx_spl.cmake)
elseif(CONFIG_PLATFORM_DEVICE MATCHES "STM32")
    # Probably in future this fatal error message will be removed,
    # since there are a lot of STM32 devices does not supported by the SPL.
    msg_fatal("Unsupported STM32 device: ${CONFIG_PLATFORM_DEVICE}")
endif()
