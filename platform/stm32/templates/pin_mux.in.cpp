//! \file
//! \brief Pin multiplexer configurator.

#include <aux/platform_defines.hpp>
#include <platform/gpio_device.hpp>

/*[[[cog
import cog
import json
import re

cfg = json.load(open(JSON_CFG))
cfg = cfg['platform']

pinmux = {}
if 'pinmux' in cfg:
    pinmux = cfg['pinmux']

if len(pinmux) > 0:
    cog.outl('#define PINMUX_REQUIRED')

# Periphery that should be enabled by RCC
rcc_ports = []

# Pin JSON to SPL definitions mapping
pin_defs_mapping = {
    'pull' : {
        'down'  : 'GPIO_PuPd_DOWN',
        'up'    : 'GPIO_PuPd_UP',
        'no'    : 'GPIO_PuPd_NOPULL',
    }
}

# Default pin configuration.
pin_default = {
    'mode'  : 'IN',
    'otype' : 'PP',
    'speed' : '2MHz',
    'pull'  : 'no'
}

# Gets property, or assigns default value
def extact_or_default(pin_cfg, property):
    if property in pin_cfg:
        return pin_cfg[property]
    else:
        return pin_default[property]
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

    for pins in pinmux:
        mode    = 'GPIO_Mode_'      + extact_or_default(pins, 'mode')
        speed   = 'GPIO_Speed_'     + extact_or_default(pins, 'speed')
        otype   = 'GPIO_OType_'     + extact_or_default(pins, 'otype')
        pull    =  pin_defs_mapping['pull'][extact_or_default(pins, 'pull')]

        af = '0'
        if mode == 'GPIO_Mode_AF':
            af = 'GPIO_AF_' + pins['function']

        if 'comment' in pins:
            cog.outl('/' + '* ' + pins['comment'] + ' *' + '/')

        for pin in pins['ids']:
            r = re.findall('P(\w)(\d+)', pin)
            port = 'GPIO' + r[0][0]
            rcc_ports.append('RCC_AHB1Periph_GPIO' + r[0][0])
            pin_num = int(r[0][1])
            pin_def = 'GPIO_Pin_' + str(pin_num)

            pin_src = '0'
            if mode == 'GPIO_Mode_AF':
                pin_src = 'GPIO_PinSource' + str(pin_num)

            cog.outl('{ %s, %-12s, %-13s, %-16s, %s, %-17s, %-15s, %-16s },'
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
#endif
}
