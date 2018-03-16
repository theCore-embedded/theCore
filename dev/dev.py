#!/bin/python3

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import common
import json

# Resolves SDSPI driver typedef based on SPI and GPIO IDs.
# Given IDs can be either platform-related ID or user alias
def resolve_sdspi_driver(json_cfg, spi_id, gpio_id):
    spi_drv = common.resolve_spi_driver(json_cfg, spi_id)
    cs_gpio = common.resolve_gpio_driver(json_cfg, gpio_id)
    return 'sdspi_' + spi_drv + '_' + cs_gpio
