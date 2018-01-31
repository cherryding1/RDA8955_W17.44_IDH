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

#include "regdef.h"

// =============================================================================
// PUBLIC VOID hal_CallWithStack(VOID *stack,
//                               VOID *function,
//                               UINT32 par1,
//                               UINT32 par2,
//                               UINT32 par3,
//                               UINT32 par4,
//                               UINT32 par5,
//                               UINT32 par6,
//                               UINT32 par7,
//                               UINT32 par8);
// =============================================================================
//      a0:     stack
//      a1:     function
//      a2:     par1
//      a3:     par2
//      16(sp): par3
//      20(sp): par4
//      24(sp): par5
//      28(sp): par6
//      32(sp): par7
//      36(sp): par8
// =============================================================================
// a usage as following.
//  hal_DcacheFlushAll(TRUE);
// hal_CallWithStack(hal_GetSramScratchTop(), thecalledfuntion, thecalledfuntion_param1, thecalledfuntion_param2, thecalledfuntion_param3, 0, 0, 0, 0, 0)


    .text
    .set    nomips16
    .global hal_CallWithStack
    .ent    hal_CallWithStack

hal_CallWithStack:

#define BACK_SP     40
#define BACK_RA     44
#define BACK_SIZE   48

    move    t0, sp      // old stack
    move    sp, a0      // new stack

    subu    sp, sp, BACK_SIZE
    sw      t0, BACK_SP(sp)
    sw      ra, BACK_RA(sp)

    move    t1, a1      // function
    move    a0, a2      // par1
    move    a1, a3      // par2
    lw      a2, 16(t0)  // par3
    lw      a3, 20(t0)  // par4
    lw      v0, 24(t0)  // par5
    sw      v0, 16(sp)
    lw      v0, 28(t0)  // par6
    sw      v0, 20(sp)
    lw      v0, 32(t0)  // par7
    sw      v0, 24(sp)
    lw      v0, 36(t0)  // par8
    sw      v0, 28(sp)
    jal     t1

    lw      ra, BACK_RA(sp)
    lw      sp, BACK_SP(sp)
    jr      ra

    .end    hal_CallWithStack
