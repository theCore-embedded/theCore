/* Note that M4 is capable only of THUMB instrucion set
 * TODO: validate algorithms
 */
.global		Reset_Handler				@ The first this that must be executed
.section	handlers
.syntax		unified
.align		2

.func		Reset_Handler, Reset_Handler
.type		Reset_Handler, %function
.thumb_func
.align
Reset_Handler:
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
			blx		core_main			@ Start a core
			b		board_stop			@ Infinite loop if returned
.size		Reset_Handler, . - Reset_Handler
.pool
.endfunc

/* TODO: assign proper handlers for first 16 interrupts */
.section	vectors
			.align	2 /* TODO: clarify */
			.long  0x20020000
			.long  Reset_Handler               /* Reset Handler                */
			.long  NMI_Handler                 /* NMI Handler                  */
			.long  HardFault_Handler           /* Hard Fault Handler           */
			.long  MemManage_Handler           /* MPU Fault Handler            */
			.long  BusFault_Handler            /* Bus Fault Handler            */
			.long  UsageFault_Handler          /* Usage Fault Handler          */
			.long  0                           /* Reserved                     */
			.long  0                           /* Reserved                     */
			.long  0                           /* Reserved                     */
			.long  0                           /* Reserved                     */
			.long  SVC_Handler                 /* SVCall Handler               */
			.long  DebugMon_Handler            /* Debug Monitor Handler        */
			.long  0                           /* Reserved                     */
			.long  PendSV_Handler              /* PendSV Handler               */
			.long  SysTick_Handler             /* SysTick Handler              */
			.rept  81							@ Repeat ASM statement
			.long  _ZN11IRQ_manager3isrEv		@ All other handlers
			.endr
			.align

/*
	TODO: define rest of handlers
	Below is not working, why?
*/
			@.skip	976, board_stop		@ Rest of IRQ handlers

board_stop:
			b		.					@ Infinite loop if returned

.weak NMI_Handler
.weak HardFault_Handler
.weak MemManage_Handler
.weak BusFault_Handler
.weak UsageFault_Handler
.weak SVC_Handler
.weak DebugMon_Handler
.weak PendSV_Handler
.weak SysTick_Handler

.thumb_func
NMI_Handler:
.thumb_func
HardFault_Handler:
.thumb_func
MemManage_Handler:
.thumb_func
BusFault_Handler:
.thumb_func
UsageFault_Handler:
.thumb_func
SVC_Handler:
.thumb_func
DebugMon_Handler:
.thumb_func
PendSV_Handler:
.thumb_func
SysTick_Handler:
	b	board_stop


.end
