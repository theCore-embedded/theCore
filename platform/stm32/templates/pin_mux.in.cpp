/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_templates Auto-generated code, Python COG and CMake templates
//! @{

//! \file
//! \brief Pin multiplexer configurator.

#include <aux/platform_defines.hpp>
#include <platform/gpio_device.hpp>

/*[[[cog
import cog
import json
import re
from parse import *

cfg = json.load(open(JSON_CFG))
cfg = cfg['menu-platform']['menu-stm32']

pin_ids = cfg['menu-pins']['table-pins'] if 'menu-pins' in cfg else []

if len(pin_ids) > 0:
    cog.outl('#define PINMUX_REQUIRED')

# Periphery that should be enabled by RCC
rcc_ports = []

# Pin JSON to SPL definitions mapping
pin_defs_mapping = {
    'config-pull' : {
        'push down'         : 'GPIO_PuPd_DOWN',
        'pull up'           : 'GPIO_PuPd_UP',
        'no'                : 'GPIO_PuPd_NOPULL',
    },
    'config-mode' : {
        'af'                : 'GPIO_Mode_AF',
        'output'            : 'GPIO_Mode_OUT',
        'input'             : 'GPIO_Mode_IN'
    },
    'config-type':  {
        'push-pull'         : 'GPIO_OType_PP',
        'open drain'        : 'GPIO_OType_OD',
    },
    'config-speed': {
        2                   : 'GPIO_Speed_2MHz',
        25                  : 'GPIO_Speed_25MHz',
        50                  : 'GPIO_Speed_50MHz',
        100                 : 'GPIO_Speed_100MHz',
    }
}

# Gets property and converts a value
def extract_and_convert(pin_cfg, property):
    return pin_defs_mapping[property][pin_cfg[property]]

]]]*/
//[[[end]]]

//! Struct holding single pin info. Fields are self-explanatory.
struct pin_info
{
    GPIO_TypeDef       *port;
    uint32_t           pin;
    GPIOMode_TypeDef   mode;
    GPIOSpeed_TypeDef  speed;
    GPIOOType_TypeDef  otype;
    GPIOPuPd_TypeDef   pull;

    //! Pin AF descriptor. Must be set to valid AF SPL value,
    // if \ref mode field is set to GPIO_Mode_AF.
    uint8_t            af           = 0;

    //! Pinsource for AF mode. Must be set to valid AF pin source SPL value,
    // if \ref mode field is set to GPIO_Mode_AF.
    uint8_t            pin_src       = 0;
};

// Protect against zero-length array.
#ifdef PINMUX_REQUIRED
static constexpr pin_info pins[] = {
    /*[[[cog

    for pin_id in pin_ids:
        pin_cfg = cfg['menu-pins']['menu-' + pin_id]

        mode    = extract_and_convert(pin_cfg, 'config-mode')
        speed   = extract_and_convert(pin_cfg, 'config-speed')
        otype   = extract_and_convert(pin_cfg, 'config-type')
        pull    = extract_and_convert(pin_cfg, 'config-pull')
        afsel   = pin_cfg['config-afsel'] if 'config-afsel' in pin_cfg else None

        af = '0x0'
        if mode == 'GPIO_Mode_AF' and afsel:
            r = parse('AF{:d}:{}', afsel)
            af = hex(r[0])
            # Leave clarification comment
            af += ' /' + '* ' + r[1] + ' *' + '/ '

        if 'config-comment' in pin_cfg:
            cog.outl('/' + '* ' + pin_cfg['config-comment'] + ' *' + '/')

        r = parse('P{:1w}{:d}', pin_id)
        port = 'GPIO' + r[0]
        rcc_ports.append('RCC_AHB1Periph_GPIO' + r[0])
        pin_num = str(r[1])
        pin_def = 'GPIO_Pin_' + pin_num

        pin_src = '0'
        if mode == 'GPIO_Mode_AF':
            pin_src = 'GPIO_PinSource' + pin_num

        cog.outl('{ %s, %-12s, %-13s, %-16s, %s, %-17s, %-24s, %-16s },'
            % (port, pin_def, mode, speed, otype, pull, af, pin_src))

    ]]]*/
    //[[[end]]]
};
#endif

//! Init all GPIOs using generated code based on incoming JSON.
extern "C" void gpio_init_generated()
{
// Protect against including unnecessary code into compilation.
#ifdef PINMUX_REQUIRED
    RCC_AHB1PeriphClockCmd(
        /*[[[cog
        # Get only unique elements
        s = set(rcc_ports)
        cog.outl('\n| '.join(s))


        ]]]*/
        //[[[end]]]
        ,
        ENABLE
    );

    GPIO_InitTypeDef init;

    for (const auto &pin : pins) {
        init.GPIO_Pin   = pin.pin;
        init.GPIO_Mode  = pin.mode;
        init.GPIO_PuPd  = pin.pull;
        init.GPIO_OType = pin.otype;
        init.GPIO_Speed = pin.speed;

        GPIO_Init(pin.port, &init);

        if (pin.mode == GPIO_Mode_AF) {
            GPIO_PinAFConfig(pin.port, pin.pin_src, pin.af);
        }
    }
#endif // PINMUX_REQUIRED
}

//! @}

//! @}

//! @}
