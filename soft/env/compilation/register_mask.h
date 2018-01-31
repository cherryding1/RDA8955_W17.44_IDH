#ifndef REGISTER_MASK_H
#define REGISTER_MASK_H

/*
 * These registers will not be used by GCC to 
 * output mips32 code, when WITH_REDUCED_REGS 
 * is set.
 * **** DO NOT CHANGE THIS WITHOUT MODIFYING
 * **** THE IRQ HANDLER ACCORDINGLY !
 */

#ifdef __REDUCED_REGS__
/* 
* t-registers are also used as pseudo stack 
* for register save/restore in mips16
* 3 registers seem enough for most functions 
*/
   
/*
register unsigned long masked_t0 asm("$8");
register unsigned long masked_t1 asm("$9");
register unsigned long masked_t2 asm("$10");
*/

register unsigned long masked_t3 asm("$11");
register unsigned long masked_t4 asm("$12");
register unsigned long masked_t5 asm("$13");
register unsigned long masked_t6 asm("$14");
register unsigned long masked_t7 asm("$15");

/* t9 ($25) has a specific meaning from GCC's point-of-view
 * (PIC_FN_REGISTER, used in abi calls) and is used explicitly by
 * ct-gcc for mips16 compatible sibling calls in 32 bits,
 * therefore IT MUST NOT BE MASKED (if it is compilation will fail
 * anyway)
 */

register unsigned long masked_s2 asm("$18");
register unsigned long masked_s3 asm("$19");
register unsigned long masked_s4 asm("$20");
register unsigned long masked_s5 asm("$21");
register unsigned long masked_s6 asm("$22");
register unsigned long masked_s7 asm("$23");

register unsigned long masked_fp asm("$30");
#endif

#endif /* REGISTER_MASK_H */
