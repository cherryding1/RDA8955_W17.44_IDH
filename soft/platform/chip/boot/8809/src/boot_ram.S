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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/boot/8809/src/boot_ram.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-10-11 14:24:51 +0800 (Mon, 11 Oct 2010) $                     //   
//    $Revision: 2715 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_ram.S                                                          //
/// That file defines the global variables in memory used by the romed boot   //
/// code.                                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "regdef.h"
#include "cp0.h"

//#################################################################################
// Global variables in RAM
//#################################################################################
.data
// Xcpu variables
.globl xcpu_error_code
.globl xcpu_error_status
.globl xcpu_sp_context
.globl sxr_IrqStack_pointer

// Reset cause
.globl g_bootResetCause
// Boot mode
.globl g_bootBootMode

xcpu_error_code:         .word 0xc0010000
xcpu_error_status:       .word 0xc001c001
xcpu_sp_context:         .word 0x0
sxr_IrqStack_pointer:    .word _sys_sram_non_romed_top
g_bootResetCause:        .word 0x0
g_bootBootMode:          .word 0x0


