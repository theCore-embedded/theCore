/* Just a memory regions */
MEMORY
{
	/* Values are taken from STM32 PM0214, 'STM32F3 and
	 * STM32F4 Series Cortex-M4 programming manual',
	 * section 2.2 'Memory model'
	 */
	rom (rx)     : ORIGIN = 0x00000000, LENGTH = 0x04000 /* 16K */
	ram (rwx)    : ORIGIN = 0x20000000, LENGTH = 0x2c000 /* 172 K */
	/* By default, stm32discovery is configured to start booting from flash.
	 * This behaviour is described in STM32 RM0090, 'Reference manual for
	 * STM32F405xx/07xx, STM32F415xx/17xx, STM32F42xxx and STM32F43xxx
	 * advanced ARM®-based 32-bit MCUs',
	 * section 2.4 'Boot configuration'
	 */
	flash (rwx)  : ORIGIN = 0x08000000, LENGTH = 1M
}

/* This is the actual start point, for ARM it is a
 * reset handler. Defined in main startup file
 */
ENTRY(irq_reset)

SECTIONS
{
	/* .text section goes to flash memory region, starts from 0x0,
	 * aligned by 4, contains vectors section inside
	 * (defined inside main source file) and the text section itself.
	 */

	. = ALIGN(4);


	.text :
	{
		/* Trace a start of the code */
		___text_start = .;
		
		KEEP(*(vectors))
		KEEP(*(.text*))

		. = ALIGN(4);

		/* Trace an end of the code */
		___text_end = .;
	} > flash


	/* Read only data goes to flash */
	.rodata :
	{
		*(.rodata*)
		. = ALIGN(4);
	} > flash

	/* Data section goes to ram and must be
	 * rellocated from flash 'manually'
	 * when board starts */

	/* Place of a data in the ROM */
	___data_load = .;

	.data :
	AT(___data_load)
	{
		/* Trace a start of the data */
		___data_start = .;

		*(.data*)

		. = ALIGN(4);

		/* Trace an end of the data */
		___data_end = .;
	} > ram 

	/* .bss goes to ram and must be zeroed
	 * 'manually' when board starts
	 */
	.bss  :
	{
		/* Trace a start of bss */
		___bss_start = .;

		*(.bss*)
		*(COMMON)

		. = ALIGN(4);
		/* An end of bss*/
		___bss_end = .;
	} > ram 


}
