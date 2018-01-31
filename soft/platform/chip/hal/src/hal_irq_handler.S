/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

//  HARDWARE DEFINITION 
#include "regdef.h"
#include "cp0.h"

#include "global_macros.h"
#include "sys_irq_asm.h"
#include "page_spy_asm.h"
#include "timer_asm.h"
#include "bb_irq_asm.h"
#include "spi_flash_asm.h"

#include "halp_gdb_stub.h"
#include "halp_irq_handler.h"


.extern initial_stack_top_var


#
# 
#ifdef __mips16
//  This file contains 32 bit assembly code.  
    .set nomips16
#endif

    .text

    .set    reorder

/// *********************************************************
///  * Function name: sxr_Irq
///  ***********************************************************
///  * Type     : API
///  * Purpose  : Irq handler
///  **********************************************************
//void _sxr_Irq_handler(void)
//{
    .section .irqtext, "awx", @progbits
    .balign 0x10

    .globl    _sxr_Irq_handler
    .ent    _sxr_Irq_handler
    .frame    sp, 0, ra
_sxr_Irq_handler:


#ifdef HAL_GDB_CONFIG
//  In this case, reliable reset is the most important.
//  So, we should avoid to use PSRAM (it is possible that sp is in PSRAM)
//  and enable watchdog forcedly in case that watchdog is disabled
//  intentionally or unintentionally. Then, only if this piece of codes
//  are not destroyed, system will be reset.

    mfc0    k0, Cause

#ifndef USE_GDB_PAGE_IRQ
//  extract exception code + debug irq + host irq
    andi    k0, k0, (XCPU_HIP_DEBUG|XCPU_HIP_HOST|Cause_ExcCode)
#else
//  extract exception code + debug irq + host irq + page irq
    andi    k0, k0, (XCPU_HIP_DEBUG|XCPU_HIP_HOST|XCPU_HIP_PAGE_SPY| Cause_ExcCode)
#endif

    beqz    k0, $L_normal_interrupt

    li      t0, REG_TIMER_BASE_HI
    li      t1, 0x100
    sw      t1, REG_TIMER_WDTIMER_LOADVAL(t0)
    li      t1, TIMER_RELOAD
    sw      t1, REG_TIMER_WDTIMER_CTRL(t0)
    li      t1, TIMER_START
    sw      t1, REG_TIMER_WDTIMER_CTRL(t0)

$L_dead_wait_watchdog:
    b       $L_dead_wait_watchdog

$L_normal_interrupt:
#endif


// ------------------------------------------------
// save context on current stack
// ------------------------------------------------

    subu sp, sp, TASK_EXTRA_CTX_SIZE

//  Put 4 instructions in the "stub" to make better use of the cache 
    .set noat
    sw  AT,FC_AT(sp)
    .set at
    sw  t0,FC_T0(sp)


    //----------------------------
    // Save extra context
    //----------------------------
//  make room on stack for extra context

//  save all registers 
    sw  v0,FC_V0(sp)        # save $2
    sw  v1,FC_V1(sp)        # save $3
    sw  a1,FC_A1(sp)        # save $5
    sw  a2,FC_A2(sp)        # save $6
    sw  a3,FC_A3(sp)        # save $7
    sw  t1,FC_T1(sp)        # save $9
    sw  t2,FC_T2(sp)        # save $10
#ifndef __REDUCED_REGS__
    sw  t3,FC_T3(sp)        # save $11
    sw  t4,FC_T4(sp)        # save $12
    sw  t5,FC_T5(sp)        # save $13
    sw  t6,FC_T6(sp)        # save $14
    sw  t7,FC_T7(sp)        # save $15
#endif
    sw  t8,FC_T8(sp)        # save $24
    sw  t9,FC_T9(sp)        # save $25
    sw  ra,FC_RA(sp)        # save $31

//  save HI and LO registers 
    mflo t0
    mfhi t1
    sw  t0, FC_LO(sp)
    sw  t1, FC_HI(sp)

    
    //----------------------------
    // Save small context
    //----------------------------
//  make room on stack for small context 
    subu sp, sp, TASK_SMALL_CTX_SIZE
    sw    s0,SC_S0(sp)           # save $16
    sw    s1,SC_S1(sp)           # save $17
#ifndef __REDUCED_REGS__
    sw    s2,SC_S2(sp)           # save $18
    sw    s3,SC_S3(sp)           # save $19
    sw    s4,SC_S4(sp)           # save $20
    sw    s5,SC_S5(sp)           # save $21
    sw    s6,SC_S6(sp)           # save $22
    sw    s7,SC_S7(sp)           # save $23
    sw    fp,SC_FP(sp)           # save $30
#endif
    sw    a0,SC_A0(sp)           # save $4

    mfc0    t0,EPC
    li     t1, CTX_TYPE_LONG | 1 // long context (<0) + 1 not in critical section
    sw    t0,SC_PC(sp)           # save Pc 

    blt k1, JOB_STACK_FLAG, $L_skip_save_job_flag
    or  t1, t1, CTX_TYPE_FROM_JOB
$L_skip_save_job_flag:

    sw    t1,SC_CTX_TYPE(sp)       # save CTX Type (>=0 is from function - <0 is From IT)

// ------------------------------------------------
//     End save context
// ------------------------------------------------


// ------------------------------------------------
// Go to Irq stack if needed
// ------------------------------------------------
    move  s0,sp                 # sp where the context has been saved stored in s0

    beqz k1,$L_comming_from_task
        blt k1, JOB_STACK_FLAG, $L_comming_from_irq
            .set noat
            li      AT, ~JOB_STACK_FLAG
            and     k1, k1, AT
            .set at
$L_comming_from_task:
        // coming from task
        lw t0, sxr_IrqStack_pointer
        nop
        move sp, t0

$L_comming_from_irq:

// ------------------------------------------------
// Get IRQ cause and select function to call
// ------------------------------------------------
    mfc0 t1,Cause

    addiu k1,k1,1

#ifndef HAL_GDB_CONFIG
//  read cause register 
#ifndef USE_GDB_PAGE_IRQ
//  extract exception code + debug irq + host irq 
    andi t0,t1,(XCPU_HIP_DEBUG|XCPU_HIP_HOST|Cause_ExcCode)
#else
//  extract exception code + debug irq + host irq + page irq 
    andi t0,t1,(XCPU_HIP_DEBUG|XCPU_HIP_HOST|XCPU_HIP_PAGE_SPY| Cause_ExcCode)
#endif
    
    bnez t0,_go_to_gdb
#endif

//---------------------------------
// regular IT function
//---------------------------------
#ifndef CT_C_USE_GP
    // create stack frame for function call + gp restoration
    subu    sp, sp, 24
    sw        gp, 20(sp)
    // get the stack to restore in gp
    move    gp, s0
#else
    // create stack frame for function call + task stack restoration
    subu    sp, sp, 24
    lw        t0, sxr_Irq_TaskStack_pointer
    sw        t0, 20(sp)
    // get the stack to restore in sxr_Irq_TaskStack_pointer
    sw        s0, sxr_Irq_TaskStack_pointer
#endif
    
        

    // disable it in ctrl
    li  t0, KSEG1(REG_SYS_IRQ_BASE_HI)
    lw  zero,   REG_SYS_IRQ_SC(t0)

    // enable it in risc
    li  t0, (Status_IntMASK | Status_IEc | IRQ_LOCATION)

.set noreorder
    jal hal_IrqDispatch
    mtc0    t0, Status
.set reorder



    // disable it in risc - set prev irq bit to 1 so that rfe will have irq enabled
    li  t0, (Status_IntMASK | Status_IEp | IRQ_LOCATION)
    mtc0  t0, Status

#ifndef CT_C_USE_GP
    // get the future stack from gp (this could have been updated in TaskSwap)
    move    v0, gp
    // restore stack frame
    lw        gp, 20(sp)
    // useless sp will be taken from v0 later
//    addu    sp, 24
#else
    // get the future stack from sxr_Irq_TaskStack_pointer
    lw        v0, sxr_Irq_TaskStack_pointer
    // restore stack frame
    lw        t0, 20(sp)
    sw        t0, sxr_Irq_TaskStack_pointer
    // useless sp will be taken from v0 later
//    addu    sp, 24
#endif


// hopefully from here we are no longer interruptible, but exception can occur
// don't step bellow this
_srx_irq_handler_end_dont_step_in_here:

// restore context associated to scheduled task
//----------------------------
// restore context
//----------------------------

    // v0 is setup to the sp addr of the scheduled task by TaskSwitch
    // or is the value saved before

    lw    s0,SC_S0(v0)           # restore $16
    lw    s1,SC_S1(v0)           # restore $17
#ifndef __REDUCED_REGS__
    lw    s2,SC_S2(v0)           # restore $18
    lw    s3,SC_S3(v0)           # restore $19
    lw    s4,SC_S4(v0)           # restore $20
    lw    s5,SC_S5(v0)           # restore $21
    lw    s6,SC_S6(v0)           # restore $22
    lw    s7,SC_S7(v0)           # restore $23
    lw    fp,SC_FP(v0)           # restore $30
#endif
    lw    a0,SC_A0(v0)           # restore task param
    lw    k0,SC_PC(v0)           # restore PC
    lw    t1,SC_CTX_TYPE(v0)           # restore CTX type

    and   t0,t1,CTX_TYPE_FROM_JOB
    beqz t0,$L_skip_restore_job_flag
    or    k1,k1,JOB_STACK_FLAG
$L_skip_restore_job_flag:

    // Restore Critical Section Status
    li  t0, KSEG1(REG_SYS_IRQ_BASE_HI)
    // store context direcly the hardware will check bit 0 only
    sw  t1, REG_SYS_IRQ_SC(t0)

// ------------------------------------------------
// Go to task stack if needed
// ------------------------------------------------
    addiu k1,k1,-1

    addiu    sp, v0, TASK_SMALL_CTX_SIZE

    // if context is small, we are done
    // if context is big, restore other registers
    bgez t1, $L_CTx_restored 

    //----------------------------------------------
    // Long restore - this task has been interrupted
    //----------------------------------------------
//  restore HI and LO registers 
    lw  t0, FC_LO(sp)
    lw  t1, FC_HI(sp)
    mtlo t0
    mthi t1
//  restore all registers 
    lw  v0,FC_V0(sp)
    lw  v1,FC_V1(sp)
    lw  a1,FC_A1(sp)
    lw  a2,FC_A2(sp)
    lw  a3,FC_A3(sp)
    lw  t2,FC_T2(sp)
#ifndef __REDUCED_REGS__
    lw  t3,FC_T3(sp)
    lw  t4,FC_T4(sp)
    lw  t5,FC_T5(sp)
    lw  t6,FC_T6(sp)
    lw  t7,FC_T7(sp)
#endif
    lw  ra,FC_RA(sp)
    lw  t8,FC_T8(sp)
    lw  t9,FC_T9(sp)
    .set noat
    lw  AT,FC_AT(sp)
    .set at
    lw  t0,FC_T0(sp)
    lw  t1,FC_T1(sp)

    // Free extra space on stack
    addu    sp, sp, TASK_EXTRA_CTX_SIZE

$L_CTx_restored :
    .set noreorder
    jr    k0
    rfe
    .set reorder

//  ***** 
//   GDB  
//  ***** 
#define GDB_REG00 0x00
#define GDB_REG01 0x04
#define GDB_REG02 0x08
#define GDB_REG03 0x0c
#define GDB_REG04 0x10
#define GDB_REG05 0x14
#define GDB_REG06 0x18
#define GDB_REG07 0x1c
#define GDB_REG08 0x20
#define GDB_REG09 0x24
#define GDB_REG10 0x28
#define GDB_REG11 0x2c
#define GDB_REG12 0x30
#define GDB_REG13 0x34
#define GDB_REG14 0x38
#define GDB_REG15 0x3c
#define GDB_REG16 0x40
#define GDB_REG17 0x44
#define GDB_REG18 0x48
#define GDB_REG19 0x4c
#define GDB_REG20 0x50
#define GDB_REG21 0x54
#define GDB_REG22 0x58
#define GDB_REG23 0x5c
#define GDB_REG24 0x60
#define GDB_REG25 0x64
#define GDB_REG26 0x68
#define GDB_REG27 0x6c
#define GDB_REG28 0x70
#define GDB_REG29 0x74
#define GDB_REG30 0x78
#define GDB_REG31 0x7c
#define GDB_REGSR 0x80
#define GDB_REGLO 0x84
#define GDB_REGHI 0x88
#define GDB_REGBAD 0x8c
#define GDB_REGCAUSE 0x90
#define GDB_REGEPC 0x94
#define GDB_CMD 0x98

#define GDB_CMD_BREAKPOINT 0x10
#define GDB_EVENT 0x9db00000

#ifndef HAL_GDB_CONFIG

_go_to_gdb:
    // MISSING BREAK , DBG_IT, EXCEPTION, HOST_IT
    // s0 is context_sp
    // t0, at are invalid
    // t1 is Cause


//  make room on stack 
    subu sp, sp, HAL_GDB_CONTEXT_SIZE

//  store Cause and EPC registers 
    mfc0    t8, EPC
    sw      t1, GDB_REGCAUSE(sp)
    sw      t8, GDB_REGEPC(sp)
    // t8 must hold EPC until later

//  r1 = at from context 
    lw        t0,  TASK_SMALL_CTX_SIZE+FC_AT(s0)
    sw        r0,  GDB_REG00(sp) // this is always 0 but we want it for gdb
    .set noat
    sw        t0,  GDB_REG01(sp) 
    .set at
    sw        r2,  GDB_REG02(sp)
    sw        r3,  GDB_REG03(sp)
    sw        r4,  GDB_REG04(sp)
    sw        r5,  GDB_REG05(sp)
    sw        r6,  GDB_REG06(sp)
    sw        r7,  GDB_REG07(sp)
//  r8-r9 = t0-t1 from context 
    lw        t0,  TASK_SMALL_CTX_SIZE+FC_T0(s0)
    sw        t0,  GDB_REG08(sp)
    lw        t0,  TASK_SMALL_CTX_SIZE+FC_T1(s0)
    sw        t0,  GDB_REG09(sp)
//  
    sw        r10, GDB_REG10(sp)
    sw        r11, GDB_REG11(sp)
    sw        r12, GDB_REG12(sp)
    sw        r13, GDB_REG13(sp)
    sw        r14, GDB_REG14(sp)
    sw        r15, GDB_REG15(sp)
//  r16 = s0 from context 
    lw        t0, SC_S0(s0)
    sw        r17, GDB_REG17(sp)
    sw        t0, GDB_REG16(sp)
    sw        r18, GDB_REG18(sp)
    sw        r19, GDB_REG19(sp)
    sw        r20, GDB_REG20(sp)
    sw        r21, GDB_REG21(sp)
    sw        r22, GDB_REG22(sp)
    sw        r23, GDB_REG23(sp)
//  r24 = t8 from context 
    lw        t0,  TASK_SMALL_CTX_SIZE+FC_T8(s0)
    sw        t0,  GDB_REG24(sp)
//  
    sw        r25, GDB_REG25(sp)
//  k0 is destroyed saved 0 for gdb 
    sw        r0, GDB_REG26(sp)
    addiu    t0,k1,-1
    sw        t0, GDB_REG27(sp)
    sw        r28, GDB_REG28(sp)
//  r29 original sp calculated here 
    addu    s0, s0, TASK_SMALL_CTX_SIZE+TASK_EXTRA_CTX_SIZE
    sw        s0, GDB_REG29(sp)
    sw        r30, GDB_REG30(sp)
    sw        r31, GDB_REG31(sp)
//  store HI and LO registers 
    .set noat
    mflo    t0
    mfhi    AT
    sw        t0, GDB_REGLO(sp)
    sw        AT, GDB_REGHI(sp)
    .set at
//  store Status, BadVAddr 
    .set noat
    mfc0    t0, Status
    mfc0    AT, BadVAddr
    sw      t0, GDB_REGSR(sp)
    sw      AT, GDB_REGBAD(sp)
    .set at

//  write GDB CMD, it may not needed any more
    li      t0, GDB_CMD_BREAKPOINT
    sw      t0, GDB_CMD(sp)

//  save stack pointer position when context saved 
    la        t0, xcpu_sp_context
    sw        sp, 0(t0) 

//  frame for paramters 
    subu    sp, sp, 16

//  clear AHB_READ_DISABLE
    li      t0, REG_SPI_FLASH_BASE_HI
    lw      a0, REG_SPI_FLASH_SPI_CS_SIZE(t0)
    li      t1, ~SPI_FLASH_AHB_READ_DISABLE_DISABLE_AHB_READ
    and     a0, a0, t1
    sw      a0, REG_SPI_FLASH_SPI_CS_SIZE(t0)

//  trigger BCPU debug IRQ
    li      t0, BB_IRQ_DEBUG_IRQ
    li      a0, REG_BB_IRQ_BASE_HI
    sw      t0, REG_BB_IRQ_NONMASKABLE(a0)

//  send event to notify PC
    li      a0, GDB_EVENT
    jal     mon_Event

#if (XCPU_CACHE_MODE_WRITE_BACK==1)
    li      a0, 1
    jal     boot_FlushDCache
#endif

$L_gdb_dead:
    b       $L_gdb_dead

#endif // !HAL_GDB_CONFIG


    .type    _sxr_Irq_handler,@function
    .size    _sxr_Irq_handler,.-_sxr_Irq_handler
    .end    _sxr_Irq_handler

//}



//  ********************************************************** 
//               END INTERRUPT TREATMENT                       
//  ********************************************************** 


//  ********************************************************** 
//               INTERRUPT DRIVER                              
//  ********************************************************** 
// IRQ initialization is done by the sxr_TaskFirst function.


//  ********************************************************* 
//                     Global Vars                            
//  ********************************************************* 

    .section .sramdata, "aw", @progbits

.globl sxr_IrqStack_pointer

#ifdef CT_C_USE_GP
.globl sxr_Irq_TaskStack_pointer
sxr_Irq_TaskStack_pointer:    .word 0
#endif




