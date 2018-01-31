////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_lps_utils.S $ //
//    $Author: kongdq $                                                        // 
//    $Date: 2015-12-28 18:47:10 +0800 (Mon, 28 Dec 2015) $                     //   
//    $Revision: 26982 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file   hal_lps_utils.S 
///     This file defines the low level functions for lps Deep Sleep.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



//  HARDWARE DEFINITION 
#include "regdef.h"
#include "cp0.h"
#include "global_macros.h"
#include "sys_irq_asm.h"


#include "halp_gdb_stub.h"
#include "halp_irq_handler.h"


.extern sxr_IrqStack_pointer
.extern hal_LpsDeepSleep
.extern hal_LpsLightSleep
#ifdef __mips16
//  This file contains 32 bit assembly code.  
    .set nomips16
#endif

// =============================================================================
// hal_LpsDeepSleepWrapper
// -----------------------------------------------------------------------------
/// Have the XCPU entered in critical section.
/// Set the stack to the IRQ Stack (in internal ram)
/// call hal_LpsDeepSleep() with unchanged parameters
///   hal_LpsDeepSleep() does the low power sleep sequence
/// Resore Stack & critical section
/// basically the prototype should be the one of hal_LpsDeepSleep() as this
/// wrapper is only some piece of assembly code.
///
/// Note: The critical section used here is the CPU's one not the IRQ Controller
/// to prevent all IRQ source (debug interrupts, page interrupt...) during low
/// power. So that the power up sequence is executed before executing the
/// interrupt code.
///
/// @param die : If TRUE, the sytem will "die" in a low power state
/// This is for Idle power test purpose one. In regular usage always use FALSE
/// @return the irq cause register value, 0 is a normal LPS wake up
/// =============================================================================
// UINT32 hal_LpsDeepSleepWrapper (BOOL die)
// {
    .globl    hal_LpsDeepSleepWrapper
    .ent    hal_LpsDeepSleepWrapper
hal_LpsDeepSleepWrapper:    
    .frame    sp, 32, ra
//    .frame    t0, 32, ra
    // Have the XCPU entered in critical section.
    // use XCPU critical section (avoid debug irqs and page irq while in lp)
    mfc0    t1, Status
    li      t0, ~1
    and     t0, t1
    mtc0    t0, Status
    .set noat
    li      AT, LPS_STACK_FLAG
    or      k1, k1, AT
    .set at
    // reusing IRQ Stack Pointer
    lw      t0, sxr_IrqStack_pointer
    addiu     t0, t0, -32
    sw      t1, 24(t0)
    sw      sp, 20(t0)
    sw      ra, 16(t0)
    move    sp, t0
    // call the LPS function
    jal     hal_LpsDeepSleep
    // restore
    .set noat
    li      AT, ~LPS_STACK_FLAG
    and     k1, k1, AT
    .set at
    lw      t1, 24(sp)
    lw      ra, 16(sp)
    lw      sp, 20(sp)
    mtc0    t1, Status
    jr  ra

    .type    hal_LpsDeepSleepWrapper,@function
    .size    hal_LpsDeepSleepWrapper,.-hal_LpsDeepSleepWrapper
    .end    hal_LpsDeepSleepWrapper
// }

// =============================================================================
// hal_LpsLightSleepWrapper
// -----------------------------------------------------------------------------
/// Have the XCPU entered in critical section.
/// Set the stack to the IRQ Stack (in internal ram)
/// call hal_LpsDeepSleep() with unchanged parameters
///   hal_LpsDeepSleep() does the low power sleep sequence
/// Resore Stack & critical section
/// basically the prototype should be the one of hal_LpsDeepSleep() as this
/// wrapper is only some piece of assembly code.
///
/// Note: The critical section used here is the CPU's one not the IRQ Controller
/// to prevent all IRQ source (debug interrupts, page interrupt...) during low
/// power. So that the power up sequence is executed before executing the
/// interrupt code.
///
/// @param die : If TRUE, the sytem will "die" in a low power state
/// This is for Idle power test purpose one. In regular usage always use FALSE
/// @return the irq cause register value, 0 is a normal LPS wake up
/// =============================================================================
// UINT32 hal_LpsLightSleepWrapper (BOOL die)
// {
    .globl    hal_LpsLightSleepWrapper
    .ent    hal_LpsLightSleepWrapper
hal_LpsLightSleepWrapper:    
    .frame    sp, 32, ra
//    .frame    t0, 32, ra
    // Have the XCPU entered in critical section.
    // use XCPU critical section (avoid debug irqs and page irq while in lp)
    mfc0    t1, Status
    li      t0, ~1
    and     t0, t1
    mtc0    t0, Status
    .set noat
    li      AT, LPS_STACK_FLAG
    or      k1, k1, AT
    .set at
    // reusing IRQ Stack Pointer
    lw      t0, sxr_IrqStack_pointer
    addiu     t0, t0, -32
    sw      t1, 24(t0)
    sw      sp, 20(t0)
    sw      ra, 16(t0)
    move    sp, t0
    // call the LPS function
    jal     hal_LpsLightSleep
    // restore
    .set noat
    li      AT, ~LPS_STACK_FLAG
    and     k1, k1, AT
    .set at
    lw      t1, 24(sp)
    lw      ra, 16(sp)
    lw      sp, 20(sp)
    mtc0    t1, Status
    jr  ra

    .type    hal_LpsLightSleepWrapper,@function
    .size    hal_LpsLightSleepWrapper,.-hal_LpsLightSleepWrapper
    .end    hal_LpsLightSleepWrapper
// }
