# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include(${MCUS_DIR}/tm4c123gh6pm/mcu_defs.cmake)

# Enable only bypass console for this target to avoid conflicts.
set(CONFIG_USE_BYPASS_CONSOLE 1)

# UART0 visible trough USB.
set(CONFIG_CONSOLE_DEVICE_NUM 0)
