/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

    .set        nomips16

    .global     bl_run_with_stack
    .ent        bl_run_with_stack
bl_run_with_stack:

    addiu       sp, sp, -24
    sw          s0, 16(sp)
    sw          ra, 20(sp)
    move        s0, a2
    move        sp, a2
    jalr        a1
    move        sp, s0
    lw          a3, 20(sp)
    lw          s0, 16(sp)
    jr          a3

    .end        bl_run_with_stack
