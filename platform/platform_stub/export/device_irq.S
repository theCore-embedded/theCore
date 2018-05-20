#ifndef DEVICE_IRQ_DEFS_H_
#define DEVICE_IRQ_DEFS_H_

    // External Interrupts
    .long     Reserved16_IRQHandler  // Reserved interrupt 16
    .long     Reserved17_IRQHandler  // Reserved interrupt 17
    .long     Reserved18_IRQHandler  // Reserved interrupt 18
    .long     Reserved19_IRQHandler  // Reserved interrupt 19
    .long     Reserved20_IRQHandler  // Reserved interrupt 20
    .long     FTMRH_IRQHandler  // FTMRH command complete/read collision interrupt
    .long     LVD_LVW_IRQHandler  // Low Voltage Detect, Low Voltage Warning
    .long     IRQ_IRQHandler  // External interrupt
    .long     I2C0_IRQHandler  // I2C0 interrupt
    .long     Reserved25_IRQHandler  // Reserved interrupt 25
    .long     SPI0_IRQHandler  // SPI0 interrupt
    .long     SPI1_IRQHandler  // SPI1 interrupt
    .long     UART0_IRQHandler  // UART0 status/error interrupt
    .long     UART1_IRQHandler  // UART1 status/error interrupt
    .long     UART2_IRQHandler  // UART2 status/error interrupt
    .long     ADC0_IRQHandler  // ADC0 interrupt
    .long     ACMP0_IRQHandler  // ACMP0 interrupt
    .long     FTM0_IRQHandler  // FTM0 Single interrupt vector for all sources
    .long     FTM1_IRQHandler  // FTM1 Single interrupt vector for all sources
    .long     FTM2_IRQHandler  // FTM2 Single interrupt vector for all sources
    .long     RTC_IRQHandler  // RTC overflow
    .long     ACMP1_IRQHandler  // ACMP1 interrupt
    .long     PIT_CH0_IRQHandler  // PIT CH0 overflow
    .long     PIT_CH1_IRQHandler  // PIT CH1 overflow
    .long     KBI0_IRQHandler  // Keyboard interrupt 0
    .long     KBI1_IRQHandler  // Keyboard interrupt 1
    .long     Reserved42_IRQHandler  // Reserved interrupt 42
    .long     ICS_IRQHandler  // MCG interrupt
    .long     Watchdog_IRQHandler  // WDOG Interrupt
    .long     Reserved45_IRQHandler  // Reserved interrupt 45
    .long     Reserved46_IRQHandler  // Reserved interrupt 46
    .long     Reserved47_IRQHandler  // Reserved interrupt 47

    def_irq_handler Reserved16_IRQHandler
    def_irq_handler Reserved17_IRQHandler
    def_irq_handler Reserved18_IRQHandler
    def_irq_handler Reserved19_IRQHandler
    def_irq_handler Reserved20_IRQHandler
    def_irq_handler FTMRH_IRQHandler
    def_irq_handler LVD_LVW_IRQHandler
    def_irq_handler IRQ_IRQHandler
    def_irq_handler I2C0_IRQHandler
    def_irq_handler Reserved25_IRQHandler
    def_irq_handler SPI0_IRQHandler
    def_irq_handler SPI1_IRQHandler
    def_irq_handler UART0_IRQHandler
    def_irq_handler UART1_IRQHandler
    def_irq_handler UART2_IRQHandler
    def_irq_handler ADC0_IRQHandler
    def_irq_handler ACMP0_IRQHandler
    def_irq_handler FTM0_IRQHandler
    def_irq_handler FTM1_IRQHandler
    def_irq_handler FTM2_IRQHandler
    def_irq_handler RTC_IRQHandler
    def_irq_handler ACMP1_IRQHandler
    def_irq_handler PIT_CH0_IRQHandler
    def_irq_handler PIT_CH1_IRQHandler
    def_irq_handler KBI0_IRQHandler
    def_irq_handler KBI1_IRQHandler
    def_irq_handler Reserved42_IRQHandler
    def_irq_handler ICS_IRQHandler
    def_irq_handler Watchdog_IRQHandler
    def_irq_handler Reserved45_IRQHandler
    def_irq_handler Reserved46_IRQHandler
    def_irq_handler Reserved47_IRQHandler

#endif /* DEVICE_IRQ_DEFS_H_ */
