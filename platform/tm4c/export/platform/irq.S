/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DEVICE_IRQ_DEFS_H_
#define DEVICE_IRQ_DEFS_H_

.long GPIOA_Handler /* 16 GPIO Port A */
.long GPIOB_Handler /* 17 GPIO Port B */
.long GPIOC_Handler /* 18 GPIO Port C */
.long GPIOD_Handler /* 19 GPIO Port D */
.long GPIOE_Handler /* 20 GPIO Port E */
.long UART0_Handler /* 21 UART0 */
.long UART1_Handler /* 22 UART1 */
.long SSI0_Handler /* 23 SSI0 */
.long I2C0_Handler /* 24 I2C0 */
.long PWM0_FAULT_Handler /* 25 PWM0 Fault */
.long PWM0_0_Handler /* 26 PWM0 Generator 0 */
.long PWM0_1_Handler /* 27 PWM0 Generator 1 */
.long PWM0_2_Handler /* 28 PWM0 Generator 2 */
.long QEI0_Handler /* 29 QEI0 */
.long ADC0SS0_Handler /* 30 ADC0 Sequence 0 */
.long ADC0SS1_Handler /* 31 ADC0 Sequence 1 */
.long ADC0SS2_Handler /* 32 ADC0 Sequence 2 */
.long ADC0SS3_Handler /* 33 ADC0 Sequence 3 */
.long WATCHDOG_Handler /* 34 Watchdog Timers 0 and 1 */
.long TIMER0A_Handler /* 35 16/32-Bit Timer 0A */
.long TIMER0B_Handler /* 36 16/32-Bit Timer 0B */
.long TIMER1A_Handler /* 37 16/32-Bit Timer 1A */
.long TIMER1B_Handler /* 38 16/32-Bit Timer 1B */
.long TIMER2A_Handler /* 39 16/32-Bit Timer 2A */
.long TIMER2B_Handler /* 40 16/32-Bit Timer 2B */
.long COMP0_Handler /* 41 Analog Comparator 0 */
.long COMP1_Handler /* 42 Analog Comparator 1 */
.long 0 /* Reserved */
.long SYSCTL_Handler /* 44 System Control */
.long FLASH_Handler /* 45 Flash Memory Control and EEPROM */
.long GPIOF_Handler /* 46 GPIO Port F */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long UART2_Handler /* 49 UART2 */
.long SSI1_Handler /* 50 SSI1 */
.long TIMER3A_Handler /* 51 16/32-Bit Timer 3A */
.long TIMER3B_Handler /* 52 Timer 3B */
.long I2C1_Handler /* 53 I2C1 */
.long QEI1_Handler /* 54 QEI1 */
.long CAN0_Handler /* 55 CAN0 */
.long CAN1_Handler /* 56 CAN1 */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long HIBERNATE_Handler /* 59 Hibernation Module */
.long USB0_Handler /* 60 USB */
.long PWM0_3_Handler /* 61 PWM Generator 3 */
.long UDMA_Handler /* 62 uDMA Software */
.long UDMAERR_Handler /* 63 uDMA Error */
.long ADC1SS0_Handler /* 64 ADC1 Sequence 0 */
.long ADC1SS1_Handler /* 65 ADC1 Sequence 1 */
.long ADC1SS2_Handler /* 66 ADC1 Sequence 2 */
.long ADC1SS3_Handler /* 67 ADC1 Sequence 3 */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long SSI2_Handler /* 73 SSI2 */
.long SSI3_Handler /* 74 SSI3 */
.long UART3_Handler /* 75 UART3 */
.long UART4_Handler /* 76 UART4 */
.long UART5_Handler /* 77 UART5 */
.long UART6_Handler /* 78 UART6 */
.long UART7_Handler /* 79 UART7 */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long I2C2_Handler /* 84 I2C2 */
.long I2C3_Handler /* 85 I2C3 */
.long TIMER4A_Handler /* 86 16/32-Bit Timer 4A */
.long TIMER4B_Handler /* 87 16/32-Bit Timer 4B */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long TIMER5A_Handler /* 108 16/32-Bit Timer 5A */
.long TIMER5B_Handler /* 109 16/32-Bit Timer 5B */
.long WTIMER0A_Handler /* 110 32/64-Bit Timer 0A */
.long WTIMER0B_Handler /* 111 32/64-Bit Timer 0B */
.long WTIMER1A_Handler /* 112 32/64-Bit Timer 1A */
.long WTIMER1B_Handler /* 113 32/64-Bit Timer 1B */
.long WTIMER2A_Handler /* 114 32/64-Bit Timer 2A */
.long WTIMER2B_Handler /* 115 32/64-Bit Timer 2B */
.long WTIMER3A_Handler /* 116 32/64-Bit Timer 3A */
.long WTIMER3B_Handler /* 117 32/64-Bit Timer 3B */
.long WTIMER4A_Handler /* 118 32/64-Bit Timer 4A */
.long WTIMER4B_Handler /* 119 32/64-Bit Timer 4B */
.long WTIMER5A_Handler /* 120 32/64-Bit Timer 5A */
.long WTIMER5B_Handler /* 121 32/64-Bit Timer 5B */
.long SYSEXC_Handler /* 122 System Exception (imprecise) */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long 0 /* Reserved */
.long PWM1_0_Handler /* 150 PWM1 Generator 0 */
.long PWM1_1_Handler /* 151 PWM1 Generator 1 */
.long PWM1_2_Handler /* 152 PWM1 Generator 2 */
.long PWM1_3_Handler /* 153 PWM1 Generator 3 */
.long PWM1_FAULT_Handler /* 154 PWM1 Fault */

#endif /* DEVICE_IRQ_DEFS_H_ */
