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
cfg = cfg['menu-platform']['menu-stm32']

]]]*/
//[[[end]]]

// GPIO drivers ----------------------------------------------------------------

/*[[[cog

# GPIO driver template
template_gpio = '\nusing %s = gpio<gpio_port::%s, gpio_num::pin%d>;'

pin_ids = cfg['menu-pins']['table-pins'] if 'menu-pins' in cfg else []

for pin_id in pin_ids:
    pin_cfg = cfg['menu-pins']['menu-' + pin_id]

    drv_name = pin_id + '_driver'
    desc = re.findall('P(\w)(\d+)', pin_id)
    pin_port = desc[-1][0].lower()
    pin_num = int(desc[-1][1])

    if 'config-comment' in pin_cfg:
        cog.outl('\n/' + '* ' + pin_cfg['config-comment'] + ' *' + '/\n')

    cog.outl(template_gpio % (drv_name, pin_port, pin_num))

    if 'config-gpio-alias' in pin_cfg:
        cog.outl('using %s = %s;' % (pin_cfg['config-gpio-alias'], drv_name))

]]]*/
//[[[end]]]

} // namespace ecl

//! @}

//! @}

//! @}

#endif // STM32XX_GPIO_CFGS_HPP_
