/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C IRQ handlers template.

#include <platform/exti_manager.hpp>
#include <platform/execution.hpp>
#include <common/console.hpp>

#include "aux/uart_cfg.hpp"
#include "aux/spi_cfg.hpp"

/*[[[cog

import cog
import json

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['menu-platform']['menu-tm4c']

def_isr  = 'ecl::abort();'

irqs = {
    'uart': {
        'ids': {
            'UART0': def_isr,
            'UART1': def_isr,
        },
        'isr_builder': (lambda u_id: 'ecl::uart_irq_proxy<ecl::uart_channel::ch%s>::deliver_irq();' % u_id[-1])
    },
    'spi': {
        'ids': {
            'SSI0': def_isr,
            'SSI1': def_isr,
            'SSI2': def_isr,
            'SSI3': def_isr,
        },
        'isr_builder': (lambda s_id: 'ecl::spi_irq_proxy<ecl::spi_channel::ch%s>::deliver_irq();' % s_id[-1])
    },
}

# Extract exact periphery for which IRQs must be generated

try:
    for uart in cfg['menu-uart']['table-uart']:
        if uart in irqs['uart']['ids']:
            irqs['uart']['ids'][uart] = irqs['uart']['isr_builder'](uart)
except:
    pass

try:
    for spi in cfg['menu-spi']['table-spi']:
        if spi in irqs['spi']['ids']:
            irqs['spi']['ids'][spi] = irqs['spi']['isr_builder'](spi)
except:
    pass

]]]*/
//[[[end]]]

namespace ecl
{

//! EXTI interrupt proxy.
struct exti_irq_proxy : public exti_manager
{
    template<gpio_hw::port Port>
    static void deliver_irq()
    {
        irq_port_handler<Port>();
    }
};

//! UART interrupt proxy.
template<uart_channel ch>
struct uart_irq_proxy : uart<ch>
{
    static void deliver_irq()
    {
        uart<ch>::irq_bus_handler();
    }
};

//! SPI interrupt proxy.
template<spi_channel ch>
struct spi_irq_proxy : spi<ch>
{
    static void deliver_irq()
    {
        spi<ch>::irq_handler();
    }
};

} // namespace ecl

/* 16 GPIO Port A */
extern "C"
void GPIOA_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::a>();
}

/* 17 GPIO Port B */
extern "C"
void GPIOB_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::b>();
}

/* 18 GPIO Port C */
extern "C"
void GPIOC_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::c>();
}

/* 19 GPIO Port D */
extern "C"
void GPIOD_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::d>();
}

/* 20 GPIO Port E */
extern "C"
void GPIOE_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::e>();
}

/* 21 UART0 */
extern "C"
void UART0_Handler()
{
    /*[[[cog

    cog.outl(irqs['uart']['ids']['UART0'])

    ]]]*/
    //[[[end]]]
}

/* 22 UART1 */
extern "C"
void UART1_Handler()
{
    /*[[[cog

    cog.outl(irqs['uart']['ids']['UART1'])

    ]]]*/
    //[[[end]]]
}

/* 23 SSI0 */
extern "C"
void SSI0_Handler()
{
    /*[[[cog

    cog.outl(irqs['spi']['ids']['SSI0'])

    ]]]*/
    //[[[end]]]
}

/* 24 I2C0 */
extern "C"
void I2C0_Handler()
{
    ecl::abort();
}

/* 25 PWM0 Fault */
extern "C"
void PWM0_FAULT_Handler()
{
    ecl::abort();
}

/* 26 PWM0 Generator 0 */
extern "C"
void PWM0_0_Handler()
{
    ecl::abort();
}

/* 27 PWM0 Generator 1 */
extern "C"
void PWM0_1_Handler()
{
    ecl::abort();
}

/* 28 PWM0 Generator 2 */
extern "C"
void PWM0_2_Handler()
{
    ecl::abort();
}

/* 29 QEI0 */
extern "C"
void QEI0_Handler()
{
    ecl::abort();
}

/* 30 ADC0 Sequence 0 */
extern "C"
void ADC0SS0_Handler()
{
    ecl::abort();
}

/* 31 ADC0 Sequence 1 */
extern "C"
void ADC0SS1_Handler()
{
    ecl::abort();
}

/* 32 ADC0 Sequence 2 */
extern "C"
void ADC0SS2_Handler()
{
    ecl::abort();
}

/* 33 ADC0 Sequence 3 */
extern "C"
void ADC0SS3_Handler()
{
    ecl::abort();
}

/* 34 Watchdog Timers 0 and 1 */
extern "C"
void WATCHDOG_Handler()
{
    ecl::abort();
}

/* 35 16/32-Bit Timer 0A */
extern "C"
void TIMER0A_Handler()
{
    ecl::abort();
}

/* 36 16/32-Bit Timer 0B */
extern "C"
void TIMER0B_Handler()
{
    ecl::abort();
}

/* 37 16/32-Bit Timer 1A */
extern "C"
void TIMER1A_Handler()
{
    ecl::abort();
}

/* 38 16/32-Bit Timer 1B */
extern "C"
void TIMER1B_Handler()
{
    ecl::abort();
}

/* 39 16/32-Bit Timer 2A */
extern "C"
void TIMER2A_Handler()
{
    ecl::abort();
}

/* 40 16/32-Bit Timer 2B */
extern "C"
void TIMER2B_Handler()
{
    ecl::abort();
}

/* 41 Analog Comparator 0 */
extern "C"
void COMP0_Handler()
{
    ecl::abort();
}

/* 42 Analog Comparator 1 */
extern "C"
void COMP1_Handler()
{
    ecl::abort();
}

/* 44 System Control */
extern "C"
void SYSCTL_Handler()
{
    ecl::abort();
}

/* 45 Flash Memory Control and EEPROM */
extern "C"
void FLASH_Handler()
{
    ecl::abort();
}

/* 46 GPIO Port F */
extern "C"
void GPIOF_Handler()
{
    ecl::exti_irq_proxy::deliver_irq<ecl::gpio_hw::port::f>();
}

/* 49 UART2 */
extern "C"
void UART2_Handler()
{
    ecl::abort();
}

/* 50 SSI1 */
extern "C"
void SSI1_Handler()
{
    /*[[[cog

    cog.outl(irqs['spi']['ids']['SSI1'])

    ]]]*/
    //[[[end]]]
}

/* 51 16/32-Bit Timer 3A */
extern "C"
void TIMER3A_Handler()
{
    ecl::abort();
}

/* 52 Timer 3B */
extern "C"
void TIMER3B_Handler()
{
    ecl::abort();
}

/* 53 I2C1 */
extern "C"
void I2C1_Handler()
{
    ecl::abort();
}

/* 54 QEI1 */
extern "C"
void QEI1_Handler()
{
    ecl::abort();
}

/* 55 CAN0 */
extern "C"
void CAN0_Handler()
{
    ecl::abort();
}

/* 56 CAN1 */
extern "C"
void CAN1_Handler()
{
    ecl::abort();
}

/* 59 Hibernation Module */
extern "C"
void HIBERNATE_Handler()
{
    ecl::abort();
}

/* 60 USB */
extern "C"
void USB0_Handler()
{
    ecl::abort();
}

/* 61 PWM Generator 3 */
extern "C"
void PWM0_3_Handler()
{
    ecl::abort();
}

/* 62 uDMA Software */
extern "C"
void UDMA_Handler()
{
    ecl::abort();
}

/* 63 uDMA Error */
extern "C"
void UDMAERR_Handler()
{
    ecl::abort();
}

/* 64 ADC1 Sequence 0 */
extern "C"
void ADC1SS0_Handler()
{
    ecl::abort();
}

/* 65 ADC1 Sequence 1 */
extern "C"
void ADC1SS1_Handler()
{
    ecl::abort();
}

/* 66 ADC1 Sequence 2 */
extern "C"
void ADC1SS2_Handler()
{
    ecl::abort();
}

/* 67 ADC1 Sequence 3 */
extern "C"
void ADC1SS3_Handler()
{
    ecl::abort();
}

/* 73 SSI2 */
extern "C"
void SSI2_Handler()
{
    /*[[[cog

    cog.outl(irqs['spi']['ids']['SSI2'])

    ]]]*/
    //[[[end]]]
}

/* 74 SSI3 */
extern "C"
void SSI3_Handler()
{
    /*[[[cog

    cog.outl(irqs['spi']['ids']['SSI3'])

    ]]]*/
    //[[[end]]]
}

/* 75 UART3 */
extern "C"
void UART3_Handler()
{
    ecl::abort();
}

/* 76 UART4 */
extern "C"
void UART4_Handler()
{
    ecl::abort();
}

/* 77 UART5 */
extern "C"
void UART5_Handler()
{
    ecl::abort();
}

/* 78 UART6 */
extern "C"
void UART6_Handler()
{
    ecl::abort();
}

/* 79 UART7 */
extern "C"
void UART7_Handler()
{
    ecl::abort();
}

/* 84 I2C2 */
extern "C"
void I2C2_Handler()
{
    ecl::abort();
}

/* 85 I2C3 */
extern "C"
void I2C3_Handler()
{
    ecl::abort();
}

/* 86 16/32-Bit Timer 4A */
extern "C"
void TIMER4A_Handler()
{
    ecl::abort();
}

/* 87 16/32-Bit Timer 4B */
extern "C"
void TIMER4B_Handler()
{
    ecl::abort();
}

/* 108 16/32-Bit Timer 5A */
extern "C"
void TIMER5A_Handler()
{
    ecl::abort();
}

/* 109 16/32-Bit Timer 5B */
extern "C"
void TIMER5B_Handler()
{
    ecl::abort();
}

/* 110 32/64-Bit Timer 0A */
extern "C"
void WTIMER0A_Handler()
{
    ecl::abort();
}

/* 111 32/64-Bit Timer 0B */
extern "C"
void WTIMER0B_Handler()
{
    ecl::abort();
}

/* 112 32/64-Bit Timer 1A */
extern "C"
void WTIMER1A_Handler()
{
    ecl::abort();
}

/* 113 32/64-Bit Timer 1B */
extern "C"
void WTIMER1B_Handler()
{
    ecl::abort();
}

/* 114 32/64-Bit Timer 2A */
extern "C"
void WTIMER2A_Handler()
{
    ecl::abort();
}

/* 115 32/64-Bit Timer 2B */
extern "C"
void WTIMER2B_Handler()
{
    ecl::abort();
}

/* 116 32/64-Bit Timer 3A */
extern "C"
void WTIMER3A_Handler()
{
    ecl::abort();
}

/* 117 32/64-Bit Timer 3B */
extern "C"
void WTIMER3B_Handler()
{
    ecl::abort();
}

/* 118 32/64-Bit Timer 4A */
extern "C"
void WTIMER4A_Handler()
{
    ecl::abort();
}

/* 119 32/64-Bit Timer 4B */
extern "C"
void WTIMER4B_Handler()
{
    ecl::abort();
}

/* 120 32/64-Bit Timer 5A */
extern "C"
void WTIMER5A_Handler()
{
    ecl::abort();
}

/* 121 32/64-Bit Timer 5B */
extern "C"
void WTIMER5B_Handler()
{
    ecl::abort();
}

/* 122 System Exception (imprecise) */
extern "C"
void SYSEXC_Handler()
{
    ecl::abort();
}

/* 150 PWM1 Generator 0 */
extern "C"
void PWM1_0_Handler()
{
    ecl::abort();
}

/* 151 PWM1 Generator 1 */
extern "C"
void PWM1_1_Handler()
{
    ecl::abort();
}

/* 152 PWM1 Generator 2 */
extern "C"
void PWM1_2_Handler()
{
    ecl::abort();
}

/* 153 PWM1 Generator 3 */
extern "C"
void PWM1_3_Handler()
{
    ecl::abort();
}

/* 154 PWM1 Fault */
extern "C"
void PWM1_FAULT_Handler()
{
    ecl::abort();
}
