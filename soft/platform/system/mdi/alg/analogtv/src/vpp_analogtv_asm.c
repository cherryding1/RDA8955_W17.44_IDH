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

PROTECTED CONST UINT16  g_vppAnalogTVConstX[1536+104] MICRON_ALIGN(32)=
{
#include "vpp_analogtv_const_x.tab"
};

PROTECTED CONST UINT16  g_vppAnalogTVConstY[1024] MICRON_ALIGN(32)=
{
#include "vpp_analogtv_const_y.tab"
};


PROTECTED CONST INT32 g_vppAnalogTVMainCode[] MICRON_ALIGN(32) =
{
#include "vpp_analogtv_asm_main.tab"
};


PROTECTED CONST INT32 g_vppAnalogTVMainCodeSize MICRON_ALIGN(32)= sizeof(g_vppAnalogTVMainCode);
PROTECTED CONST INT32 g_vppAnalogTVConstXSize MICRON_ALIGN(32)= sizeof(g_vppAnalogTVConstX);
PROTECTED CONST INT32 g_vppAnalogTVConstYSize MICRON_ALIGN(32)= sizeof(g_vppAnalogTVConstY);

