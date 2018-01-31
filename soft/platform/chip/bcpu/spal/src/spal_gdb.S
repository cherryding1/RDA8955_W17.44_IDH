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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/bcpu/spal/src/spal_gdb.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-10-11 14:24:51 +0800 (Mon, 11 Oct 2010) $                     //   
//    $Revision: 2715 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file spal_gdb.S
/// HAL GDB Assembly functions and variables
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "regdef.h"
#include "cp0.h"
#include "spalp_gdb_stub.h"

// =============================================================================
// spal_DbgGdbPrintf
// -----------------------------------------------------------------------------
/// Print to the gdb console
/// @param string Pointer to a string to print to a gdb console
// =============================================================================
///PUBLIC VOID spal_DbgGdbPrintf(CHAR *string);

//  Either mips16 or 32, no change needed 
    SET_P_MIPS16
    .globl    spal_DbgGdbPrintf
    .ent    spal_DbgGdbPrintf
    .frame    sp, 0, ra
spal_DbgGdbPrintf:
spal_DbgGdbPrintf:
    break    15
    jr        ra
    nop
    .type    spal_DbgGdbPrintf,@function
    .size    spal_DbgGdbPrintf,.-spal_DbgGdbPrintf
    .end    spal_DbgGdbPrintf



//  ********************************************************** 
//               GDB STUB                                      
//  ********************************************************** 
    .rdata
/*    .section "BB_SRAM_GLOBALS"*/


    .globl  spal_GdbGBreak
spal_GdbGBreak:
    .word   spal_GdbBreaks
    .word   spal_GdbBreakSoftBreakPoint
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreakBreakPoint
    .word   spal_GdbBreaks
    .word   spal_GdbBreakDivBy0
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreaks
    .word   spal_GdbBreakPrint

//  GDB DATA for bb 
/*    
.align 2
    .globl    __bb_gdb_context
__bb_gdb_context:
    .space SPAL_GDB_CONTEXT_SIZE
    */
