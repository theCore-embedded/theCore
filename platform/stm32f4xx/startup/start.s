/* Note that M4 is capable only of THUMB instrucion set
 * TODO: validate algorithms
 */
.global		_resetHandler				@ The first this that must be executed
.section	handlers
.syntax		unified
.align		2


.func		_resetHandler, _resetHandler
.type		_resetHandler, %function
.thumb_func
.align
_resetHandler:
			cpsid	i					@ Disable interrupts
			ldr		r0, =___data_load	@ Obtain data address in ROM
			ldr		r1, =___data_start  @ Obtain data start address in RAM
			ldr		r2, =___data_end	@ Obtain data end address in RAM
			subs	r2, r1				@ Setup a counter
			beq		copy_data_end		@ If nothing to do
copy_data:
/* TODO: what if r2 == 0 ? */
			subs	r2, r2, #4			@ Decrement a counter
			ldr		r3, [r0, r2]		@ Load a word of data from ROM
			str		r3, [r1, r2]		@ Store it to the RAM
			bne		copy_data			@ If counter greather than 0

copy_data_end:							@ Prepare for bss zeroing
			ldr		r0, =___bss_start	@ Obtain bss address in RAM
			ldr		r1, =___bss_end		@ Obtain bss address in ROM
			subs	r1, r1, r0			@ Setup a counter
			beq		clear_bss_end		@ If nothing to do
			mov		r2, #0				@ Use it as a output buffer
clear_bss:
/* TODO: what if r1 == 0 ? */
			subs	r1, r1, #4			@ Decrement a counter
			str		r2, [r0, r1]		@ Store a zero word to the RAM
			bne		clear_bss			@ If counter greather than 0

clear_bss_end:
			bl		SystemInit			@ Initialize a system
			blx		platform_init		@ Initialize a platform // TODO: remove
			blx		target_init			@ Initialize a target
			blx		early_main			@ Do the last step before entering main routine
			blx		main				@ Allow return, for now
			b		board_stop			@ Infinite loop if returned
.size		_resetHandler, . - _resetHandler
.pool
.endfunc

board_stop:
			b		.					@ Infinite loop if returned



/* TODO: assign proper handlers for first 16 interrupts */
.section	vectors
			.align	2 /* TODO: clarify */
			.long   0x20020000
			.long	_resetHandler					@ Reset handler
			.long	board_stop						@ NMI
			.long	board_stop						@ Hard fault
			.long	board_stop						@ Memory management fault
			.long	board_stop						@ Bus fault
			.long	board_stop						@ Usage fault
			.long	0								@ Reserved bytes
			.long	0								@ Reserved bytes
			.long	0								@ Reserved bytes
			.long	0								@ Reserved bytes
			.long	vPortSVCHandler					@ SVC handler
			.long	board_stop						@ Debug monitor
			.long	0								@ Reserved bytes
			.long	xPortPendSVHandler				@ PendSV handler
			.long	xPortSysTickHandler				@ SysTick handler
			.rept	81								@ Repeat ASM statement
			.long	_ZN11IRQ_manager3ISREv			@ All other handlers
			.endr
			.align

/*
	TODO: define rest of handlers
	Below is not working, why?
*/
			@.skip	976, board_stop		@ Rest of IRQ handlers

.end
