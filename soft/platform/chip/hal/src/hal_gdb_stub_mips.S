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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_gdb_stub_mips.S $ //
//    $Author: deqikong $                                                        // 
//    $Date: 2016-10-20 12:58:45 +0800 (Thu, 20 Oct 2016) $                     //   
//    $Revision: 35440 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_gbd_stub_mips.S
///     GDB assembly functions for the GDB stub, 
///     about cache flushing
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "regdef.h"
#include "cp0.h"
#include "halp_irq_handler.h"
#include "sys_irq_asm.h"

.text

.globl hal_GdbInvalidCache
.ent   hal_GdbInvalidCache
.frame sp,0,ra
hal_GdbInvalidCache:
    .set noreorder
    .align 4
    cache 0,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end hal_GdbInvalidCache

.globl hal_GdbInvalidICache
.ent   hal_GdbInvalidICache
hal_GdbInvalidICache:
    .set noreorder
    .align 4
    cache 1,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end hal_GdbInvalidICache

.globl hal_GdbInvalidCacheData
.ent   hal_GdbInvalidCacheData
.frame sp,0,ra
hal_GdbInvalidCacheData:
    .set noreorder
    .align 4
    cache 2,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end hal_GdbInvalidCacheData

.globl hal_GdbDisableDebugIrq
.ent   hal_GdbDisableDebugIrq
.frame sp,0,ra
hal_GdbDisableDebugIrq:
    .align 4
    mfc0 t0, Status
    li t1, ~XCPU_HIP_DEBUG
    move v0, t0
    and t0, t0, t1
    mtc0 t0, Status
    jr ra
.end hal_GdbDisableDebugIrq

