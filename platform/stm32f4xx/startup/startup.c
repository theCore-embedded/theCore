/*
 * This file contains startup script for STM32F4
 *
 * TODO: extend interrupt vector table to support ALL interrupts
 */

/* Initial stack pointer value */
#define STACK_START 0x20006000

/* Module's main routine */
extern int main(void);

/* Handler called in the start of the system*/
void irq_reset(void);

/* IRQ handler routine generic prototype */
typedef void (*irq_handler)(void);

/**
 * \brief Interrupt vector table
 *
 * According to ARM spec. first word is an initial stack pointer
 * Second is a pointer to reset handler, which executes immediately
 * after stack pointer was set.
 *
 * Ideally, all interrupt handlers must be set in order to catch
 * any fault
 */
irq_handler ___interrupt_vectors[] __attribute__ ((section("vectors"))) = {
	(irq_handler) STACK_START,		/** Initial SP value */
	(irq_handler) irq_reset,		/** Reset */

	/** \TODO add handlers according
	 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABIFJFG.html
	 */
};

void irq_reset(void) {
	/* Don't care about anything, just launch 'main' for a demo */
	main();
	/* TODO perform chip reset */
	for (;;);
}
