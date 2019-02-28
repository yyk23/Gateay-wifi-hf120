.text
.code 32
.align  0

.global relocate

.set  UART_DATAX,  		0xc0000c00

	.LC7:
	.word   0x30000		//192K,Older version may use 160K,We now enlarge sram from 160K to 192K.

relocate:
//##### for debugging #####
//	ldr r1, =UART_DATAX
//	mov r2,#0x62	//'b'
//	str r2,[r1]

	/* Stack Sizes */
    .set  UND_STACK_SIZE, 0x00000040
    .set  ABT_STACK_SIZE, 0x00000040
    .set  FIQ_STACK_SIZE, 0x00000400
    .set  IRQ_STACK_SIZE, 0X00000800
    .set  SVC_STACK_SIZE, 0x00001000

	/* This TOTAL_STACK_SIZE will affect the malloc's heap end */	
    .set  TOTAL_STACK_SIZE, SVC_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + ABT_STACK_SIZE + UND_STACK_SIZE 

	/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
    .set  MODE_USR, 0x10            /* User Mode */
    .set  MODE_FIQ, 0x11            /* FIQ Mode */
    .set  MODE_IRQ, 0x12            /* IRQ Mode */
    .set  MODE_SVC, 0x13            /* Supervisor Mode */
    .set  MODE_ABT, 0x17            /* Abort Mode */
    .set  MODE_UND, 0x1B            /* Undefined Mode */
    .set  MODE_SYS, 0x1F            /* System Mode */

    .equ  I_BIT, 0x80               /* when I bit is set, IRQ is disabled */
    .equ  F_BIT, 0x40               /* when F bit is set, FIQ is disabled */

	/* Setup a stack for each mode - note that this only sets up a usable stack
	for system/user, SWI and IRQ modes.   Also each mode is setup with
	interrupts initially disabled. */
	ldr   r0, .LC7
	msr   CPSR_c, #MODE_UND|I_BIT|F_BIT /* Undefined Instruction Mode */
	mov   sp, r0
	sub   r0, r0, #UND_STACK_SIZE
	msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT /* Abort Mode */
	mov   sp, r0
	sub   r0, r0, #ABT_STACK_SIZE
	msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT /* FIQ Mode */
	mov   sp, r0
	sub   r0, r0, #FIQ_STACK_SIZE
	msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT /* IRQ Mode */
	mov   sp, r0
	sub   r0, r0, #IRQ_STACK_SIZE
	msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT /* Supervisor Mode */
	mov   sp, r0
	sub   r0, r0, #SVC_STACK_SIZE
	msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT /* System Mode */
	mov   sp, r0

	/* We want to start in supervisor mode.  Operation will switch to system
 	mode when the first task starts. */
 	msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT

//##### for debugging #####
// 	ldr r1, =UART_DATAX
//    mov r2,#0x63	//'c'
//    str r2,[r1]

	mov pc,lr 
