#ifndef ARCH_ASM_DEFS_H_
#define ARCH_ASM_DEFS_H_

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro	def_irq_handler	handler_name
    .weak	\handler_name
    .set	\handler_name, Default_Handler
    .endm

#endif /* ARCH_ASM_DEFS_H_ */
