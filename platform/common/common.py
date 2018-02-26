#!/bin/python3

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Resolves GPIO driver typedef based on given ID.
# Given ID can be either platform-related ID or user alias
def resolve_gpio_driver(json_cfg, gpio_id):
    # In case of tests and document generation platform object can be missing.
    if not 'platform' in json_cfg:
        return gpio_id + '_driver'

    json_cfg = json_cfg['platform']

    gpio_aliases = []
    pinmux_cfg = []

    if 'gpio_alias' in json_cfg:
        gpio_aliases = json_cfg['gpio_alias']

    if 'pinmux' in json_cfg:
        pinmux_cfg = json_cfg['pinmux']

    # Check aliases

    for alias in gpio_aliases:
        if gpio_id == alias['alias']:
            return gpio_id

    # If not found - assume common driver name
    return gpio_id + '_driver'

# Resolves SPI driver typedef based on given ID.
# Given ID can be either platform-related ID or user alias
def resolve_spi_driver(json_cfg, spi_id):
    # In case of tests and document generation platform object can be missing.
    if not 'platform' in json_cfg:
        return spi_id + '_driver'

    json_cfg = json_cfg['platform']

    assert 'spi' in json_cfg, "SPI configuration is missing"

    json_cfg = json_cfg['spi']

    for spi in json_cfg:
        if 'alias' in spi and spi_id == spi['alias']:
            return spi_id
        elif spi_id == spi['id']:
            return spi_id + '_driver'

    # If not found - assume regular driver name
    return spi_id + '_driver'
