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
#include "vppp_lzmadec_asm_map.h"


#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000


// ******************************
//            COMMON
// ******************************

CONST INT32 g_vppLzmaDecCommonCode[] MICRON_ALIGN(32)=
{
#include "vpp_lzmadec_asm_common.tab"
};

CONST INT32 g_vppLzmaDecCommonCodeSize MICRON_ALIGN(32)= sizeof(g_vppLzmaDecCommonCode);


