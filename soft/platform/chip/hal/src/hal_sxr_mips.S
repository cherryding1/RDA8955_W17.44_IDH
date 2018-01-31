////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_sxr_mips.S $ //
//    $Author: deqikong $                                                        // 
//    $Date: 2016-06-22 19:15:14 +0800 (Wed, 22 Jun 2016) $                     //   
//    $Revision: 32861 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file   hal_sxr_mips.S 
///     This file defines the low level functions for the OS.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



//  HARDWARE DEFINITION 
#include "regdef.h"
#include "cp0.h"
#include "global_macros.h"
#include "sys_irq_asm.h"


#include "halp_gdb_stub.h"
#include "halp_irq_handler.h"


//.extern initial_stack_top_var
.extern sxr_IrqStack_pointer

#ifdef CT_C_USE_GP
.extern sxr_Irq_TaskStack_pointer
#endif

#ifdef SX_PROFILING
// JobSwap will call void <JOB_SWAP_SPYBEFORE_FUNCTION> (UINT32 ** OldJobSp, UINT32 ** NewJobSp)
// when JOB_SWAP_SPYBEFORE_FUNCTION is undefined, the calling code will be removed
#define JOB_SWAP_SPYBEFORE_FUNCTION JobSwap_SpyBefore

// JobSwap will call void <JOB_SWAP_SPYAFTER_FUNCTION> (UINT32 ** OldJobSp, UINT32 ** NewJobSp)
// when JOB_SWAP_SPYAFTER_FUNCTION is undefined, the calling code will be removed
#define JOB_SWAP_SPYAFTER_FUNCTION JobSwap_SpyAfter
#endif




#ifdef __mips16
//  This file contains 32 bit assembly code.  
    .set nomips16
#endif


// ***********************************************************************
// ***********************************************************************
//                                                                        
//  Tasks utilities                                                       
//                                                                        
// ***********************************************************************
// ***********************************************************************




/// *********************************************************
///  * Function name: sxr_TaskSwap
///  ***********************************************************
///  * Type        : API
///  * Purpose    : Save current (old) task and restore new task
///  **********************************************************
//void sxr_TaskSwap   (sxr_TaskCtx_t *OldTaskCtx, sxr_TaskCtx_t *NewTaskCtx)
//{

#ifdef IRQ_IN_SRAM
    .section .sramtext, "awx", @progbits
#endif

    .balign 0x10

    .globl    sxr_TaskSwap
    .ent    sxr_TaskSwap
sxr_TaskSwap:    
    .frame    sp, 0, ra

    bnez k1, $L_TaskSwap_in_irq

    // get critical section status, 
    //  also the context is short so it wil be the CTX value
    li t0, REG_SYS_IRQ_BASE_HI
    lw t0, REG_SYS_IRQ_SC(t0)
    
    subu    sp, sp, TASK_SMALL_CTX_SIZE       # update task SP

    //----------------------------
    // Save small context
    //----------------------------
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
    // task param should be here
    sw    ra,SC_PC(sp)           # save Pc from ctx
    sw    t0,SC_CTX_TYPE(sp)     # save CTX Type (>=0 is from function - <0 is From IT) + Critical section flag

    // save sp in context
    sw    sp, 0(a0)         

    lw    sp,0(a1)           # load Sp from ctx

    //----------------------------
    // restore context
    //----------------------------

    li  t0, REG_SYS_IRQ_BASE_HI

    lw    t1,SC_CTX_TYPE(sp)       # restore CTX type and SC
    lw    s1,SC_S1(sp)           # restore $17
    lw    s0,SC_S0(sp)           # restore $16
#ifndef __REDUCED_REGS__
    lw    s2,SC_S2(sp)           # restore $18
    lw    s3,SC_S3(sp)           # restore $19
    lw    s4,SC_S4(sp)           # restore $20
    lw    s5,SC_S5(sp)           # restore $21
    lw    s6,SC_S6(sp)           # restore $22
    lw    s7,SC_S7(sp)           # restore $23
    lw    fp,SC_FP(sp)           # restore $30
#endif
    lw    a0,SC_A0(sp)           # restore task param


    // if context is small (=>0), we are done
    // if context is big (<0), restore other registers
    bltz t1, $L_TaskSwap_big

    lw    ra, SC_PC(sp)           # restore PC
    addu    sp, sp, TASK_SMALL_CTX_SIZE

    // restore SC status
    // store context direcly the hardware will check bit 0 only
    sw  t1, REG_SYS_IRQ_SC(t0)


    jr    ra

$L_TaskSwap_big:

    // create a cpu side critical section, restored by rfe when exiting
    li  t9, (Status_IntMASK | Status_IEp | IRQ_LOCATION)
    mtc0  t9, Status

    // CANNOT USE MANUAL_EXIT_SC unless cpu SC started before short context restore to avoid s regs corruption
    // t0 is still  REG_SYS_IRQ_BASE_HI
    sw  t1, REG_SYS_IRQ_SC(t0)

    lw    t8, SC_PC(sp)           # restore PC
    addu    sp, sp, TASK_SMALL_CTX_SIZE
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
    lw  t9,FC_T9(sp)
    lw  ra,FC_RA(sp)
    .set noat
    lw  AT,FC_AT(sp)
    .set at

    // By now, we have : 
    // PC in t8
    // t0,t1 invalid and available
    // all others restored

    move k0,t8
    // t8 can be restored
    lw    t8,FC_T8(sp)


    // Finally restore t0 and t1
    lw  t0,FC_T0(sp)
    lw  t1,FC_T1(sp)
    // Free extra space on stack
    addu    sp, sp, TASK_EXTRA_CTX_SIZE

    .set noreorder
    jr    k0
    rfe
    .set reorder

$L_TaskSwap_in_irq:
#ifndef CT_C_USE_GP
    sw    gp,0(a0)           # save old Sp to ctx
    lw    gp,0(a1)           # load New Sp from ctx
    jr    ra
#else
    lw    t0,sxr_Irq_TaskStack_pointer
    sw    t0,0(a0)           # save old Sp to ctx
    lw    t0,0(a1)           # load New Sp from ctx
    sw    t0,sxr_Irq_TaskStack_pointer
    jr    ra
#endif

    
    .type    sxr_TaskSwap,@function
    .size    sxr_TaskSwap,.-sxr_TaskSwap
    .end    sxr_TaskSwap
//}




/// *********************************************************
///  * Function name: sxr_TaskSetUp
///  ***********************************************************
///  * Type        : API
///  * Purpose    : Set up a task context
///  **********************************************************
//void sxr_TaskSetUp  (sxr_TaskCtx_t *TaskCtx, void *TaskParam)
//{
#ifdef IRQ_IN_SRAM
    .text
#endif

    .globl    sxr_TaskSetUp
    .ent    sxr_TaskSetUp
sxr_TaskSetUp:    
    .frame    sp, 0, ra

    lw    t0,0(a0)           # load Sp from ctx
    lw    t1,4(a0)           # load Pc from ctx

    subu    t0,t0,16
    subu    t0, t0, TASK_SMALL_CTX_SIZE       # update task SP
    li  t2,1    // Short context + Interrupt Enable (out of SC)

    sw    a1,SC_A0(t0)           # save task param
    sw    t1,SC_PC(t0)           # save Pc from ctx
    sw    t2,SC_CTX_TYPE(t0)     # save CTX Type (>=0 is from function - <0 is From IT) + SC

    sw  t0,0(a0)           # save new Sp to ctx

    jr    ra

    .type    sxr_TaskSetUp,@function
    .size    sxr_TaskSetUp,.-sxr_TaskSetUp
    .end sxr_TaskSetUp
//}

// Unused
//.extern ct_ram_top

/// *********************************************************
///  * Function name: sxr_TaskFirst
///  ***********************************************************
///  * Type        : API
///  * Purpose    : Initiate the first task
///  **********************************************************
//void sxr_TaskFirst  (sxr_TaskCtx_t *TaskCtx) 
//{
    .globl    sxr_TaskFirst
    .ent    sxr_TaskFirst
sxr_TaskFirst:    
    .frame    sp, 0, ra

    // configure Irq Stack
//    lw    t0, initial_stack_top_var
    // Disable debug uart IRQ. This IRQ is only
    // used to wake-up the system when in idle,
    // we don't want to handle it dynamically.
#ifdef CHIP_DIE_8909
    li    t1, (SYS_IRQ_STATUS_EXTRA_MASK &(~ SYS_IRQ_SYS_IRQ_DEBUG_UART))
#else
    li    t1, (SYS_IRQ_STATUS_MASK &~ SYS_IRQ_SYS_IRQ_DEBUG_UART)
#endif
//    sw    t0, sxr_IrqStack_pointer

    // irq enter SC, set mask
    li  t0, REG_SYS_IRQ_BASE_HI
    lw  zero, REG_SYS_IRQ_SC(t0)
#ifdef CHIP_DIE_8909
    sw  t1, REG_SYS_IRQ_MASK_SET_EXTRA(t0)
    li  t1, (SYS_IRQ_STATUS_MASK)
#endif
    sw  t1, REG_SYS_IRQ_MASK_SET(t0)

    // xcpu enable irq
    mtc0  zero, Cause
    li  t8, (Status_IntMASK | Status_IEc | IRQ_LOCATION)
    mtc0  t8, Status

    // load task context
    lw    t1,0(a0)           # load Sp from ctx
    lw    a0,SC_A0(t1)           # restore task param
    lw    ra,SC_PC(t1)           # restore RA
    lw    t2,SC_CTX_TYPE(t1)   # We know we came from a function, need SC

    addu    sp, t1, TASK_SMALL_CTX_SIZE       # New Stack Pointer
//  tell the irq handler that we are know in a task
    li    k1, 0

    // exit SC
    // t0 still REG_SYS_IRQ_BASE_HI
    // store context direcly the hardware will check bit 0 only
    sw  t2, REG_SYS_IRQ_SC(t0)

    jr    ra

    .type    sxr_TaskFirst,@function
    .size    sxr_TaskFirst,.-sxr_TaskFirst
    .end sxr_TaskFirst
//}

// ***********************************************************************
// ***********************************************************************
//                                                                        
//  Jobs utilities                                                        
//                                                                        
// ***********************************************************************
// ***********************************************************************

/// *********************************************************
///  * Function name: sxr_JobSetUp
///  ***********************************************************
///  * Type     : API
///  * Purpose  : Set up a job context
///  **********************************************************
//   void sxr_JobSetUp (UINT8 JobNb, sxr_JbCtx_t *JobCtx)

        .globl  sxr_JobSetUp
        .ent    sxr_JobSetUp

sxr_JobSetUp:


        lw      t0, 0(a1)                       # Job Sp
        lw      t1, 4(a1)
        lw      t8, 8(a1)                       # Job data
        lw      t1, 0(t1)                       # Job Pc

        subu    t0, t0, JOB_CTX_SIZE
        sw      t8, JC_A0(t0)                   # Save job data
        sw      a0, JC_A1(t0)                   # Save job number
        sw      t1, JC_PC(t0)                   # Save job Pc
        sw      zero, JC_IS(t0)                 # Save job InteruptStack

        sw      t0, 0(a1)                       # Save Job Sp in job context.


        j       ra
    .type    sxr_JobSetUp,@function
    .size    sxr_JobSetUp,.-sxr_JobSetUp
    .end    sxr_JobSetUp


/// *********************************************************
///  * Function name: sxr_JobSwap
///  ***********************************************************
///  * Type     : API
///  * Purpose  : Save current (old) job and restore new job
///  **********************************************************
//   void sxr_JobSwap (UINT32 **OldJobSp, UINT32 **NewJobSp)

#ifdef IRQ_IN_SRAM
        .section .sramtext, "awx", @progbits
#endif

        .balign 0x10

        .globl  sxr_JobSwap
        .ent    sxr_JobSwap

sxr_JobSwap:
        
    // Enter CS to avoid dangerous behaviour
    // We don't care about the status, as the
    // SC is restored in the end ...
        li      t0, KSEG1(REG_SYS_IRQ_BASE_HI)
        lw      zero, REG_SYS_IRQ_SC(t0)
        


#-------------

#ifdef JOB_SWAP_SPYBEFORE_FUNCTION
        // reserve space on stack :
        // 16 (function call need to reserve space for the 4 parameters send by registers, this is the mips calling convention)
        // + 12 : keep a0, a1 and ra
        subu    sp, sp, 28
        // keep a0, a1 value
        sw      ra, 24(sp)
        sw      a0, 20(sp)
        sw      a1, 16(sp)
        // call spy function, parameters are still in a0, a1
        jal     JOB_SWAP_SPYBEFORE_FUNCTION
        // restore a0, a1 value
        lw      ra, 24(sp)
        lw      a0, 20(sp)
        lw      a1, 16(sp)
        // restore stack (so job context will be stored as if this call was never done
        addu    sp, sp, 28
#endif

#-------------

        lw      t0, sxr_Job
        subu    sp, sp, JOB_CTX_SIZE
        lb      t1, 0(t0)                       # Get current Job number

        sw      s0, JC_S0(sp)
        sw      s1, JC_S1(sp)
#ifndef __REDUCED_REGS__
        sw      s2, JC_S2(sp)
        sw      s3, JC_S3(sp)
        sw      s4, JC_S4(sp)
        sw      s5, JC_S5(sp)
        sw      s6, JC_S6(sp)
        sw      s7, JC_S7(sp)
        sw      fp, JC_FP(sp)
#endif
        sw      t1, JC_A1(sp)                   # Save job number
        sw      ra, JC_PC(sp)                   # Save Job Pc

        sw      sp, 0(a0)                       # Save Job Sp in old job context.

#-------------
        move    t1, zero                        # context Interrupt stack value
        bge     k1, JOB_STACK_FLAG, $L_JS_comming_from_job
            lw  t1, sxr_IrqStack_pointer
            beqz    k1, $L_JS_comming_from_task
                sw  sp, sxr_IrqStack_pointer        
$L_JS_comming_from_task:
$L_JS_comming_from_job:
        sw  t1, JC_IS(sp)
#-------------

        lw      sp, 0(a1)                       # Get Job Sp form new job context.

#ifndef JOB_SWAP_SPYAFTER_FUNCTION
        // while we wait for sp
        li      t8, REG_SYS_IRQ_BASE_HI
#endif

#-------------
        lw      t1, JC_IS(sp)                   # context Interrupt stack value
        li      t9, JOB_STACK_FLAG
        or      k1, k1, t9
        beqz    t1, $L_JS_going_to_job
            sw  t1, sxr_IrqStack_pointer
            #li      t9, ~JOB_STACK_FLAG
            not      t9
            and      k1, k1, t9
$L_JS_going_to_job:
#-------------

        lw      s0, JC_S0(sp)
        lw      s1, JC_S1(sp)
#ifndef __REDUCED_REGS__
        lw      s2, JC_S2(sp)
        lw      s3, JC_S3(sp)
        lw      s4, JC_S4(sp)
        lw      s5, JC_S5(sp)
        lw      s6, JC_S6(sp)
        lw      s7, JC_S7(sp)
        lw      fp, JC_FP(sp)
#endif

#ifdef JOB_SWAP_SPYAFTER_FUNCTION
        // keep a0 value for JOB_SWAP_SPYAFTER_FUNCTION call
        move    t9, a0
#endif

        lw      a0, JC_A0(sp)                   # Restore Job data
        lw      t1, JC_A1(sp)                   # Restore job number
        lw      ra, JC_PC(sp)                   # Restore Job PC
        sb      t1, 0(t0)
        addu    sp, sp, JOB_CTX_SIZE
        
#-------------

#ifdef JOB_SWAP_SPYAFTER_FUNCTION
        // reserve space on stack :
        // 16 (function call need to reserve space for the 4 parameters send by registers, this is the mips calling convention)
        // + 12 : keep a0, a1 and ra
        subu    sp, sp, 28
        // keep a0, a1 value
        sw      ra, 24(sp)
        sw      a0, 20(sp)
        sw      a1, 16(sp)
        // restore a0 parameter
        move    a0, t9
        // call spy function, parameter is still in a1
        jal     JOB_SWAP_SPYAFTER_FUNCTION
        // restore a0, a1 value
        lw      ra, 24(sp)
        lw      a0, 20(sp)
        lw      a1, 16(sp)
        // restore stack (so job context will be stored as if this call was never done
        addu    sp, sp, 28
        li      t8, REG_SYS_IRQ_BASE_HI
#endif

#-------------

        // exit SC so that job can be interrupted
        li      t1, 1
        sw      t1, REG_SYS_IRQ_SC(t8)


        jr      ra
    .type    sxr_JobSwap,@function
    .size    sxr_JobSwap,.-sxr_JobSwap
    .end    sxr_JobSwap




