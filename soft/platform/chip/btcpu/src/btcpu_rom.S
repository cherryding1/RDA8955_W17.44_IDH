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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/btcpu/src/btcpu_rom.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2013-01-26 15:39:54 +0800 (Sat, 26 Jan 2013) $                     //   
//    $Revision: 18861 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file bcpu_rom.S                                                          //
/// That file provides the boot code                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "regdef.h"
#include "cp0.h"

#include "global_macros.h"

    
        
///////////////////////////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////////////////////////

    

.extern btcpu_stack_base
.extern btcpu_main_entry
.extern btcpu_int_entry

.text
.set at
//############################################
//# BTCPU BOOT at 0x4B00
//############################################
.globl  __btcpu_start
__btcpu_start:
    nop
    // ------------------------------------
    // Initialize caches
    // Flush both caches
    // ------------------------------------
    cache 0,0
    nop

    // ------------------------------------
    //  initialize stack pointer
    // ------------------------------------
    lw    sp, btcpu_stack_base


    la    v0, __btcpu_init
    jr    v0



// ##################################################################
// # Initialization function - Must be put after Uhandler definition
// ##################################################################
.globl  __btcpu_init
__btcpu_init:
    // barely nothing to do the xcpu does the setup of the system

//  Enable ITs with handler in rom
    li      t0,    Status_BEV | Status_IntMASK | Status_IEc
    mtc0    zero, Cause
    nop
    mtc0    t0, Status

    lw      ra, __btcpu_int_handler // if return from main... error
    lw      v0, btcpu_main_entry    // jump to the btcpu_main
    j       v0


. = (__btcpu_start + 0x80)

// #####################################
// # rom interrupt handler is at 0x4B80 (0x4B00 | 0x180)
// #####################################
.globl __btcpu_int_handler
__btcpu_int_handler:
    // ------------------------------------
    //  Get the irq handler
    // ------------------------------------
    lw      k0, btcpu_int_entry
    j       k0


