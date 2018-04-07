/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of GPIO driver configuration.
//! \details Pin assignment is configured in pinmux.in.cpp.
//! \ingroup stm32_templates
#ifndef STM32XX_GPIO_CFGS_HPP_
#define STM32XX_GPIO_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <platform/gpio_device.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

/*[[[cog
import cog
import json
import re

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

]]]*/
//[[[end]]]

// GPIO drivers ----------------------------------------------------------------

/*[[[cog

# GPIO driver template
template_gpio = '\nusing %s = gpio<gpio_port::%s, gpio_num::pin%d>;'

pinmux = []
gpio_alias = []

if 'pinmux' in cfg:
    pinmux = cfg['pinmux']

if 'gpio_alias' in cfg:
    gpio_alias = cfg['gpio_alias']

for group in pinmux:
    if group['mode'] != 'AF':
        for pin in group['ids']:
            drv_name = pin + '_driver'
            desc = re.findall('P(\w)(\d+)', pin)
            pin_port = desc[-1][0].lower()
            pin_num = int(desc[-1][1])

            cog.outl(template_gpio % (drv_name, pin_port, pin_num))

for gpio in gpio_alias:
    desc = re.findall('(\w)(\d+)', gpio['id'])
    pin_port = desc[-1][0].lower()
    pin_num = int(desc[-1][1])
    drv_name = 'GPIO_P%s%d_driver' % (pin_port.upper(), pin_num)

    if 'comment' in gpio:
        cog.outl('/' + '* ' + gpio['comment'] + ' *' + '/\n')

    cog.outl(template_gpio % (drv_name, pin_port, pin_num))

    if 'alias' in gpio:
        cog.outl('using %s = %s;' % (gpio['alias'], drv_name))


]]]*/
//[[[end]]]

} // namespace ecl

//! @}

//! @}

//! @}

#endif // STM32XX_GPIO_CFGS_HPP_
