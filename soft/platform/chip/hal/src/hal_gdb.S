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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_gdb.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-10-11 14:24:51 +0800 (Mon, 11 Oct 2010) $                     //   
//    $Revision: 2715 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_gdb.S
/// HAL GDB Assembly functions and variables
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "regdef.h"
#include "cp0.h"
#include "halp_gdb_stub.h"

// =============================================================================
// hal_DbgGdbPrintf
// -----------------------------------------------------------------------------
/// Print to the gdb console
/// @param string Pointer to a string to print to a gdb console
// =============================================================================
///PUBLIC VOID hal_DbgGdbPrintf(CHAR *string);

//  Either mips16 or 32, no change needed 
    SET_P_MIPS16
    .globl    hal_DbgGdbPrintf
    .globl    dbg_GdbPrint
    .ent    hal_DbgGdbPrintf
    .frame    sp, 0, ra
hal_DbgGdbPrintf:
dbg_GdbPrint:
    break    15
    jr        ra
    nop
    .type    hal_DbgGdbPrintf,@function
    .size    hal_DbgGdbPrintf,.-hal_DbgGdbPrintf
    .end    hal_DbgGdbPrintf



//  ********************************************************** 
//               GDB STUB                                      
//  ********************************************************** 
    .data

    .globl  hal_GdbGBreak
hal_GdbGBreak:
    .word   hal_GdbBreaks
    .word   hal_GdbBreakSoftBreakPoint
    .word   hal_GdbBreakRaise
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreakBreakPoint
    .word   hal_GdbBreaks
    .word   hal_GdbBreakDivBy0
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreaks
    .word   hal_GdbBreakPrint

    /*
//  GDB DATA for bb 
    
.align 2
    .globl    __bb_gdb_context
__bb_gdb_context:
    .space HAL_GDB_CONTEXT_SIZE

    */

