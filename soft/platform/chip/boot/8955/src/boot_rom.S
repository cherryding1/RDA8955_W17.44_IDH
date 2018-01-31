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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/boot/8809/src/boot_rom.S $ //
//    $Author: huazeng $                                                         // 
//    $Date: 2012-01-11 13:43:15 +0800 (Wed, 11 Jan 2012) $                     //   
//    $Revision: 13151 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_rom.S                                                          //
/// That file provides the boot code                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "regdef.h"
#include "cp0.h"
#include "global_macros.h"
#include "tcu_asm.h"
#include "mem_bridge_asm.h"
#include "sys_ctrl_asm.h"
#include "timer_asm.h"
#include "debug_uart_asm.h"
#include "bb_sram_asm.h"
#include "debug_host_asm.h"

#include "chip_id.h"

#include "boot_host.h"
#include "boot_sector_chip.h"
#include "boot_profile_codes.h"
#include "bootp_mode.h"

        
///////////////////////////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////////////////////////



/// Extern variable
.globl  boot_HstMonitor 
.globl  g_bootResetCause
.globl  g_bootBootMode


.text
.set at
//############################################
//# BOOT at 0x0
//############################################
.globl    __xcpu_start
__xcpu_start:


    // Historical reason: when the boot code was in flash, it was
    // fixing the "four nop" issue. As the flash was not ready on
    // time, we added some nops. This might not be useful with the ROM.
    nop
    nop
    nop
    nop

__lps_power_up:

    // Send an event on PXTS.
//    li t3, CP_BOOT_PU_END_START
//    sw t3, REG_DEBUG_PORT_PXTS_TAG_BOOT(a3)

    //----------------------------
    // Wait for power-up ready
    //----------------------------
    li  a0, REG_TCU_BASE_HI
    $L_WAIT_PU_DONE:
        lw t0, REG_TCU_LPS_PU_STATUS(a0)
        andi t0, t0, TCU_LPS_PU_READY
        beqz t0, $L_WAIT_PU_DONE

    //----------------------------
    // Go to done step and force the lines to avoid
    // shutdown
    //----------------------------
    li t0, TCU_LPS_PU_CPU_DONE \
          | TCU_LPS_PU_CLK_RF_ON \
          | TCU_LPS_PU_SPI_ON \
          | TCU_LPS_PU_ENABLE_ENABLE
    sw t0, REG_TCU_LPS_PU_CTRL(a0)

    // Send an event on PXTS.
//    li t3, CP_BOOT_PU_END_END
//    sw t3, REG_DEBUG_PORT_PXTS_TAG_BOOT(a3)


     // ######################################
    // Raise wakeup line
    // ######################################
    li  a0, REG_SYS_CTRL_BASE_HI
    li  t0, SYS_CTRL_FORCE_WAKEUP
    sw  t0, REG_SYS_CTRL_WAKEUP(a0)
    
   
    // ------------------------------------
    // Save context for Watchdog in BB SRAM
    // ------------------------------------
    la     k0, KSEG1(REG_BB_SRAM_BASE)
    .set noat
    sw  AT, 0x4(k0)
    .set at
    sw  a0, 0x10(k0)
    sw  a1, 0x14(k0)
    sw  t0, 0x20(k0)
    sw  t1, 0x24(k0)

    // We save those registers because they are used for PXTS code sending.
    sw  a3, 0x1c(k0)
    sw  t3, 0x2c(k0)

    
    //------------------------------------------------------
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ONLY USE AT, a0, a1, t0 and t1 until the full WD context is saved
    // a1 is used to store the boot mode pins value.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //------------------------------------------------------
   
    // ------------------------------------------------------
    // Testmode for the production test 
    // On the production line, the functional test mode is used
    // to tell the chip it is going to undergo the production 
    // test. The Boot mode has then a particular meaning dedicated
    // to the functional test:
    //  - A boot_mode = 0 ==> configure the EBC and jump a the beginning
    // of the flash, where further instruction (ie the functional 
    // test code) is waiting
    //  - A boot_mode != 0 ==> The boot mode is used by the romed 
    // functional test to do things, we jump at that romed function.
    // (This code is provided by the HW ASIC )
    // ------------------------------------------------------
    li      a0, REG_SYS_CTRL_BASE_HI
    lw      t0, REG_SYS_CTRL_RESET_CAUSE(a0)
    li      a1, SYS_CTRL_FONCTIONAL_TEST_MODE
    and     t1, t0, a1
    
    bnez    t1, __functional_test_boot

    // ------------------------------------------------------
    // Normal boot, we are not in functional test.
    // ------------------------------------------------------
    
__normal_boot:

    // ------------------------------------
    // Initialize caches
    // Flush both caches
    // ------------------------------------
    cache 0,0
    nop
    
    // ------------------------------------
    // Get the status of the BOOT MODE pins.
    // This will also save the status of the software boot modes.
    // ------------------------------------
    li  a0, REG_SYS_CTRL_BASE_HI
    lw  t0, REG_SYS_CTRL_RESET_CAUSE(a0)
    // t0's value not defined now (nop automatically added)
    srl a1, t0, SYS_CTRL_BOOT_MODE_SHIFT
    sw  a1, g_bootBootMode

    // Send an event on PXTS.
//    li t3, CP_BOOT_CACHE_FLUSHED
//    sw t3, REG_DEBUG_PORT_PXTS_TAG_BOOT(a3)


    // ------------------------------------
    // Save reset cause register
    // ------------------------------------
    li  a0, REG_SYS_CTRL_BASE_HI
    lw  t0, REG_SYS_CTRL_RESET_CAUSE(a0)
    li  t1, SYS_CTRL_WATCHDOG_RESET_HAPPENED \
          | SYS_CTRL_GLOBALSOFT_RESET_HAPPENED \
          | SYS_CTRL_HOSTDEBUG_RESET_HAPPENED \
          | SYS_CTRL_ALARMCAUSE_HAPPENED
    and t0, t0, t1
    sw  t0, g_bootResetCause

    // Clear the reset cause register, keeping the boot mode bits safe.
    lw  t0, REG_SYS_CTRL_RESET_CAUSE(a0)
    or t0, t0, t1
    sw  t0, REG_SYS_CTRL_RESET_CAUSE(a0)


    // ------------------------------------
    // Clear the reset out line
    // ------------------------------------
    // a0 is still REG_SYS_CTRL_BASE_HI
    li  t0, SYS_CTRL_CLR_RST_OUT
    sw  t0, REG_SYS_CTRL_SYS_RST_CLR(a0) 


//------------------------------------------
// End of boot
//------------------------------------------
_xcpu_end_boot:

    // -------------------------------------
    // Finish saving WD context
    // -------------------------------------
    sw  zero, 0x0(k0)
    // at already saved in 0x04
    sw  v0, 0x8(k0)
    sw  v1, 0xc(k0)
    // a0 already saved in 0x10
    // a1 already saved in 0x14
    sw  a2, 0x18(k0)
//    sw  a3, 0x1c(k0) // FIXME
    // t0 already saved in 0x20
    // t1 already saved in 0x24
    sw  t2, 0x28(k0)
//    sw  t3, 0x2c(k0)
    sw  t4, 0x30(k0)
    sw  t5, 0x34(k0)
    sw  t6, 0x38(k0)
    sw  t7, 0x3c(k0)
    sw  s0, 0x40(k0)
    sw  s1, 0x44(k0)
    sw  s2, 0x48(k0)
    sw  s3, 0x4c(k0)
    sw  s4, 0x50(k0)
    sw  s5, 0x54(k0)
    sw  s6, 0x58(k0)
    sw  s7, 0x5c(k0)
    sw  t8, 0x60(k0)
    sw  t9, 0x64(k0)
    sw  zero, 0x68(k0) // k0 is not saved - put 0
    sw  k1, 0x6c(k0)
    sw  gp, 0x70(k0)
    sw  sp, 0x74(k0)
    sw  fp, 0x78(k0)
    sw  ra, 0x7c(k0)
    sw  zero, 0x80(k0) // status is not saved     - put 0
    sw  zero, 0x84(k0) // lo is not saved         - put 0
    sw  zero, 0x88(k0) // hi is not saved         - put 0
    sw  zero, 0x8c(k0) // bad is not saved         - put 0
    sw  zero, 0x90(k0) // cause is not saved     - put 0
    // save PC
//    li    a0, REG_DEBUG_PORT_BASE_HI
//    lw    t0, REG_DEBUG_PORT_WD_ADDR_CFG(a0)
//    sw    t0, 0x94(k0)
    //---------------------------------------
    // WD context saved
    // no more limitation on register usage
    //---------------------------------------

    // ------------------------------------
    //  initialize stack pointer 
    // ------------------------------------
     lw    sp, initial_stack_top_var
     sw sp, sxr_IrqStack_pointer
//     la    fp, _ram_top
//    lw    gp, start_gp

    // enable it in risc
    li  t0, (Status_IntMASK | Status_IEc | 0x400000)
    mtc0    t0, Status

    // #######################################
    // Jump to monitor function - IF RETURNS FROM MONITOR ERROR
    // #######################################

    lw    t0, monitor_entry
    jr t0    

idle_loop:
    nop
    j   idle_loop


// ################################################################################
// Int handler at its fixed place, of needed
// ################################################################################

.align 7
// #####################################
// # rom interrupt handler is at 0x180
// #####################################

    .globl  _boot_IrqHandler
_boot_IrqHandler:

// ------------------------------------------------
// save context on current stack
// ------------------------------------------------

//  make room on stack for extra context
    subu sp, sp, 120

    .set noat
    sw  AT, 0(sp)
    .set at
    sw  t0, 4(sp)

    //----------------------------
    // Save extra context
    //----------------------------

//  save all registers
    sw  v0, 8(sp)        # save $2
    sw  v1,12(sp)        # save $3
    sw  a1,16(sp)        # save $5
    sw  a2,20(sp)        # save $6
    sw  a3,24(sp)        # save $7
    sw  t1,28(sp)        # save $9
    sw  t2,32(sp)        # save $10
    sw  t3,36(sp)        # save $11
    sw  t4,40(sp)        # save $12
    sw  t5,44(sp)        # save $13
    sw  t6,48(sp)        # save $14
    sw  t7,52(sp)        # save $15
    sw  t8,56(sp)        # save $24
    sw  t9,60(sp)        # save $25
    sw  ra,64(sp)        # save $31

//  save HI and LO registers
    mflo t0
    mfhi t1
    sw  t0,68(sp)
    sw  t1,72(sp)

    sw  s0,76(sp)           # save $16
    sw  s1,80(sp)           # save $17
    sw  s2,84(sp)           # save $18
    sw  s3,88(sp)           # save $19
    sw  s4,92(sp)           # save $20
    sw  s5,96(sp)           # save $21
    sw  s6,100(sp)           # save $22
    sw  s7,104(sp)           # save $23
    sw  fp,108(sp)           # save $30
    sw  a0,112(sp)           # save $4

    mfc0    t0,EPC
    sw  t0,116(sp)           # save Pc
   
// ------------------------------------------------
//  End save context
// ------------------------------------------------
    jal boot_FlushDCache
// ------------------------------------------------
// Get IRQ cause and select function to call
// ------------------------------------------------
    mfc0 t1,Cause

    //  While loop if is not a interruption
    // For example, we jump or directly execute _boot_IrqHandler
_boot_not_interrupt:
    beqz t1, _boot_not_interrupt

    
    // read cause register.
    // If this is not a regular interrupt, we are handling
    // an exception. In the rom code, we while(1).
    andi    t1, t1, (0x8000 | Cause_ExcCode)    // extract exception code + debug irq
_boot_romed_exception:
    bnez    t1, _boot_romed_exception 

//---------------------------------
// regular IT function
//---------------------------------
    // create stack frame for function call + gp restoration
    subu    sp, sp, 24

    jal boot_IrqDispatch

    addu    sp, 24

_boot_irq_restore_context:

// restore context associated to scheduled task
//----------------------------
// restore context
//----------------------------

    // v0 is setup to the sp addr of the scheduled task by TaskSwitch
    // or is the value saved before

    lw  k0,116(sp)           # restore PC
    lw  a0,112(sp)           # restore task param
    lw  fp,108(sp)           # restore $30
    lw  s7,104(sp)           # restore $23
    lw  s6,100(sp)           # restore $22
    lw  s5,96(sp)           # restore $21
    lw  s4,92(sp)           # restore $20
    lw  s3,88(sp)           # restore $19
    lw  s2,84(sp)           # restore $18
    lw  s1,80(sp)           # restore $17
    lw  s0,76(sp)           # restore $16

//  restore HI and LO registers 
    lw  t0, 68(sp)
    lw  t1, 72(sp)
    mtlo t0
    mthi t1

//  restore all registers 
    lw  ra,64(sp)
    lw  t9,60(sp)
    lw  t8,56(sp)
    lw  t7,52(sp)
    lw  t6,48(sp)
    lw  t5,44(sp)
    lw  t4,40(sp)
    lw  t3,36(sp)
    lw  t2,32(sp)
    lw  t1,28(sp)
    lw  a3,24(sp)
    lw  a2,20(sp)
    lw  a1,16(sp)
    lw  v1,12(sp)
    lw  v0, 8(sp)
    .set noat
    lw  AT,0(sp)
    .set at
    lw  t0,4(sp)

    // Free extra space on stack
    addu    sp, sp, 120

    .set noreorder
    jr  k0
    rfe
    .set reorder






// ################################################################################
// Other functions. 
// ################################################################################
// ################################################################################
// _hstMonitorXCtx_start is the base address of the executing structure
// defined by the linker script

.align 4
.globl boot_HstMonitorX
.ent boot_HstMonitorX
.frame sp, 0, ra
boot_HstMonitorX:
    .set noreorder
    // Save sp,ra, s0 on the stack
    addiu sp, -24
    sw  ra, 16(sp)
    sw  s0, 20(sp)
    // sp saved in s0 in a delay slot later

    // read in monitor host execution context the 
    // value for pc and sp, and parameter.
    lw  t2, _hstMonitorXCtx_start+BOOT_HST_MONITOR_X_CTX_SP;
    
    // Pc loaded in t1 here because of undetermination
    // of the target of a lw a the time immediately 
    // following
    lw    t1, _hstMonitorXCtx_start+ BOOT_HST_MONITOR_X_CTX_PC;

    // if BOOT_HST_MONITOR_X_CTX_SP is 0, keep current sp
    beqz t2, $L_KEEP_SP
    move s0, sp             // sp saved in delay slot

    move    sp,    t2
$L_KEEP_SP:
    
    lw  a0, _hstMonitorXCtx_start + BOOT_HST_MONITOR_X_CTX_PARAM; 
    cache 2,0
    nop 
    jalr t1
    nop
    // parameter loaded in delay slot
    
    sw  v0, _hstMonitorXCtx_start + BOOT_HST_MONITOR_X_CTX_RETURNED_VALUE;

    // Recover previous state
    move sp, s0
    lw   ra, 16(sp)
    lw   s0, 20(sp)

    jr ra
    addiu sp, 24
    
    .set reorder
.end   boot_HstMonitorX



//.align 8 
// ============================================
// Function to trig EXL by jumping at a known 
// address (here)
// ============================================
.globl boot_DbgExlTrigFunction0
.ent   boot_DbgExlTrigFunction0
.frame sp,0,ra
boot_DbgExlTrigFunction0:
    .align 4
    jr ra
.end boot_DbgExlTrigFunction0




// ################################################################################
// ################################################################################
// ################################################################################
// ################################################################################

    // ------------------------------
    // Functional Test Boot
    // ------------------------------

__functional_test_boot:

    // Get the boot_mode[0] (keyin_0) from t0, which holds REG_SYS_CTRL_RESET_CAUSE
    srl     a1, t0, SYS_CTRL_BOOT_MODE_SHIFT 
    and     a1, a1, 0x1 

    // Test the boot mode
    // We jump in flash if boot_mode[0] is not equal to zero
    bnez    a1, __test_mode_jump_flash

    // If a1 null, go into the boot_FunctionalTest (Currently Ram test)
    // Jump into RAM test (romed) boot_FunctionalTest:

    // NOTE: BIST module has been removed from chip since gallite
boot_FunctionalTest:
    nop
    j       boot_FunctionalTest

__test_mode_jump_flash:
    j       __normal_boot


//.align 11
// #####################################
// # global variables accessible from outside  are at 0x800
// #####################################
.globl monitor_entry
.globl initial_stack_top_var






// ################################################################################
// Some of these functions might move if the previous code expands
// ################################################################################


// ============================================
// Some variables (in fact constants, since in ROM ...)
// ============================================
monitor_entry: .word    boot_Monitor
initial_stack_top_var: .word _sys_sram_non_romed_top



// ============================================
// Function to Invalid both caches
// ============================================
.globl boot_InvalidCache
.ent   boot_InvalidCache
.frame sp,0,ra
boot_InvalidCache:
    .set noreorder
    .align 4
    cache 0,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end boot_InvalidCache

// ============================================
// Function to Invalid I caches
// ============================================
.globl boot_InvalidICache
.ent   boot_InvalidICache
.frame sp,0,ra
boot_InvalidICache:
    .set noreorder
    .align 4
    cache 1,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end boot_InvalidICache

// ============================================
// Function to Invalid D caches
// ============================================
.globl boot_InvalidDCache
.ent   boot_InvalidDCache
.frame sp,0,ra
boot_InvalidDCache:
    .set noreorder
    .align 4
    cache 2,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end boot_InvalidDCache
// ============================================
// Disable interruption
// ============================================
.globl boot_DisableInterrupt
.ent   boot_DisableInterrupt
.frame sp,0,ra
boot_DisableInterrupt:
        li  t0, (Status_IntMASK | Status_IEp)
        mtc0  t0, Status
.end boot_DisableInterrupt



