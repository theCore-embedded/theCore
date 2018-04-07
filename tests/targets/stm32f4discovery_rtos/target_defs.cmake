# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include(${MCUS_DIR}/stm32f407/mcu_defs.cmake)

# Configuration for current target
theCore_set_target_config(${CMAKE_CURRENT_LIST_DIR}/target_defs.json)

# Enable FreeRTOS
theCore_enable_os(freertos)
theCore_freertos_header_config_dir(${CMAKE_CURRENT_LIST_DIR})
