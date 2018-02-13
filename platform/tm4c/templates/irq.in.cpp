/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief TM4C IRQ handlers template.

#include <platform/exti_manager.hpp>

/*[[[cog

import cog
import json

f = open(JSON_CFG)
cfg = json.load(f)
cfg = cfg['platform']

]]]*/
//[[[end]]]

namespace ecl
{

struct exti_event_receiver : public exti_manager
{
    template<gpio_hw::port Port>
    static void deliver_irq()
    {
        irq_port_handler<Port>();
    }
};

} // namespace ecl

/* 16 GPIO Port A */
extern "C"
void GPIOA_Handler()
{
    ecl::exti_event_receiver::deliver_irq<ecl::gpio_hw::port::a>();
}

/* 17 GPIO Port B */
extern "C"
void GPIOB_Handler()
{
    ecl::exti_event_receiver::deliver_irq<ecl::gpio_hw::port::b>();
}

/* 18 GPIO Port C */
extern "C"
void GPIOC_Handler()
{
    ecl::exti_event_receiver::deliver_irq<ecl::gpio_hw::port::c>();
}

/* 19 GPIO Port D */
extern "C"
void GPIOD_Handler()
{
    ecl::exti_event_receiver::deliver_irq<ecl::gpio_hw::port::d>();
}

/* 20 GPIO Port E */
extern "C"
void GPIOE_Handler()
{
    ecl::exti_event_receiver::deliver_irq<ecl::gpio_hw::port::e>();
}

/* 21 UART0 */
extern "C"
void UART0_Handler()
{
}

/* 22 UART1 */
extern "C"
void UART1_Handler()
{
}

/* 23 SSI0 */
extern "C"
void SSI0_Handler()
{
}

/* 24 I2C0 */
extern "C"
void I2C0_Handler()
{
}

/* 25 PWM0 Fault */
extern "C"
void PWM0_FAULT_Handler()
{
}

/* 26 PWM0 Generator 0 */
extern "C"
void PWM0_0_Handler()
{
}

/* 27 PWM0 Generator 1 */
extern "C"
void PWM0_1_Handler()
{
}

/* 28 PWM0 Generator 2 */
extern "C"
void PWM0_2_Handler()
{
}

/* 29 QEI0 */
extern "C"
void QEI0_Handler()
{
}

/* 30 ADC0 Sequence 0 */
extern "C"
void ADC0SS0_Handler()
{
}

/* 31 ADC0 Sequence 1 */
extern "C"
void ADC0SS1_Handler()
{
}

/* 32 ADC0 Sequence 2 */
extern "C"
void ADC0SS2_Handler()
{
}

/* 33 ADC0 Sequence 3 */
extern "C"
void ADC0SS3_Handler()
{
}

/* 34 Watchdog Timers 0 and 1 */
extern "C"
void WATCHDOG_Handler()
{
}

/* 35 16/32-Bit Timer 0A */
extern "C"
void TIMER0A_Handler()
{
}

/* 36 16/32-Bit Timer 0B */
extern "C"
void TIMER0B_Handler()
{
}

/* 37 16/32-Bit Timer 1A */
extern "C"
void TIMER1A_Handler()
{
}

/* 38 16/32-Bit Timer 1B */
extern "C"
void TIMER1B_Handler()
{
}

/* 39 16/32-Bit Timer 2A */
extern "C"
void TIMER2A_Handler()
{
}

/* 40 16/32-Bit Timer 2B */
extern "C"
void TIMER2B_Handler()
{
}

/* 41 Analog Comparator 0 */
extern "C"
void COMP0_Handler()
{
}

/* 42 Analog Comparator 1 */
extern "C"
void COMP1_Handler()
{
}

/* 44 System Control */
extern "C"
void SYSCTL_Handler()
{
}

/* 45 Flash Memory Control and EEPROM */
extern "C"
void FLASH_Handler()
{
}

/* 46 GPIO Port F */
extern "C"
void GPIOF_Handler()
{
}

/* 49 UART2 */
extern "C"
void UART2_Handler()
{
}

/* 50 SSI1 */
extern "C"
void SSI1_Handler()
{
}

/* 51 16/32-Bit Timer 3A */
extern "C"
void TIMER3A_Handler()
{
}

/* 52 Timer 3B */
extern "C"
void TIMER3B_Handler()
{
}

/* 53 I2C1 */
extern "C"
void I2C1_Handler()
{
}

/* 54 QEI1 */
extern "C"
void QEI1_Handler()
{
}

/* 55 CAN0 */
extern "C"
void CAN0_Handler()
{
}

/* 56 CAN1 */
extern "C"
void CAN1_Handler()
{
}

/* 59 Hibernation Module */
extern "C"
void HIBERNATE_Handler()
{
}

/* 60 USB */
extern "C"
void USB0_Handler()
{
}

/* 61 PWM Generator 3 */
extern "C"
void PWM0_3_Handler()
{
}

/* 62 uDMA Software */
extern "C"
void UDMA_Handler()
{
}

/* 63 uDMA Error */
extern "C"
void UDMAERR_Handler()
{
}

/* 64 ADC1 Sequence 0 */
extern "C"
void ADC1SS0_Handler()
{
}

/* 65 ADC1 Sequence 1 */
extern "C"
void ADC1SS1_Handler()
{
}

/* 66 ADC1 Sequence 2 */
extern "C"
void ADC1SS2_Handler()
{
}

/* 67 ADC1 Sequence 3 */
extern "C"
void ADC1SS3_Handler()
{
}

/* 73 SSI2 */
extern "C"
void SSI2_Handler()
{
}

/* 74 SSI3 */
extern "C"
void SSI3_Handler()
{
}

/* 75 UART3 */
extern "C"
void UART3_Handler()
{
}

/* 76 UART4 */
extern "C"
void UART4_Handler()
{
}

/* 77 UART5 */
extern "C"
void UART5_Handler()
{
}

/* 78 UART6 */
extern "C"
void UART6_Handler()
{
}

/* 79 UART7 */
extern "C"
void UART7_Handler()
{
}

/* 84 I2C2 */
extern "C"
void I2C2_Handler()
{
}

/* 85 I2C3 */
extern "C"
void I2C3_Handler()
{
}

/* 86 16/32-Bit Timer 4A */
extern "C"
void TIMER4A_Handler()
{
}

/* 87 16/32-Bit Timer 4B */
extern "C"
void TIMER4B_Handler()
{
}

/* 108 16/32-Bit Timer 5A */
extern "C"
void TIMER5A_Handler()
{
}

/* 109 16/32-Bit Timer 5B */
extern "C"
void TIMER5B_Handler()
{
}

/* 110 32/64-Bit Timer 0A */
extern "C"
void WTIMER0A_Handler()
{
}

/* 111 32/64-Bit Timer 0B */
extern "C"
void WTIMER0B_Handler()
{
}

/* 112 32/64-Bit Timer 1A */
extern "C"
void WTIMER1A_Handler()
{
}

/* 113 32/64-Bit Timer 1B */
extern "C"
void WTIMER1B_Handler()
{
}

/* 114 32/64-Bit Timer 2A */
extern "C"
void WTIMER2A_Handler()
{
}

/* 115 32/64-Bit Timer 2B */
extern "C"
void WTIMER2B_Handler()
{
}

/* 116 32/64-Bit Timer 3A */
extern "C"
void WTIMER3A_Handler()
{
}

/* 117 32/64-Bit Timer 3B */
extern "C"
void WTIMER3B_Handler()
{
}

/* 118 32/64-Bit Timer 4A */
extern "C"
void WTIMER4A_Handler()
{
}

/* 119 32/64-Bit Timer 4B */
extern "C"
void WTIMER4B_Handler()
{
}

/* 120 32/64-Bit Timer 5A */
extern "C"
void WTIMER5A_Handler()
{
}

/* 121 32/64-Bit Timer 5B */
extern "C"
void WTIMER5B_Handler()
{
}

/* 122 System Exception (imprecise) */
extern "C"
void SYSEXC_Handler()
{
}

/* 150 PWM1 Generator 0 */
extern "C"
void PWM1_0_Handler()
{
}

/* 151 PWM1 Generator 1 */
extern "C"
void PWM1_1_Handler()
{
}

/* 152 PWM1 Generator 2 */
extern "C"
void PWM1_2_Handler()
{
}

/* 153 PWM1 Generator 3 */
extern "C"
void PWM1_3_Handler()
{
}

/* 154 PWM1 Fault */
extern "C"
void PWM1_FAULT_Handler()
{
}
