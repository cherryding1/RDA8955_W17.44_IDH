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






#include "cs_types.h"
#include "chip_id.h"
// chip/include
#include "global_macros.h"
#include "cordic.h"
// spal public header
#include "spal_cordic.h"
#include "spal_mem.h"
// spal private header
#include "spalp_private.h"
#include "spalp_debug.h"



//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_CordicAngle
//-----------------------------------------------------------------------------
/// Calculate angle(x+j*y): -PI->PI
/// pangle: angle(x+j*y)/PI*0x8000
/// return: amp
//=============================================================================

PUBLIC UINT16 spal_CordicAngle(INT32 x, INT32 y, INT16 *pangle)
{
    hwp_cordic->yin = y;
    hwp_cordic->xin = x;

    // command : 0->1:   up edge triger
    hwp_cordic->cmd = CORDIC_CMD_RST;
    hwp_cordic->cmd = CORDIC_CMD_START;


    while(hwp_cordic->status != CORDIC_OP_FREE);

    *pangle = (INT16)((hwp_cordic->dout) & 0xFFFF);

    return (UINT16)((hwp_cordic->dout)>>16);

}

