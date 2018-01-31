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
















#ifndef vpp_PREVIEW_ASM_H
#define vpp_PREVIEW_ASM_H

#include "cs_types.h"

#define vpp_Preview_CODE_SIZE       1200*4

#define vpp__Preview_CODE_ENTRY 0
#define vpp__Preview_CRITICAL_SECTION_MIN 0
#define vpp__Preview_CRITICAL_SECTION_MAX 0
#define vpp__Preview_CONST_DATA_SIZE      (1536+104)*2

#define vpp__Preview_IN_STRUCT                                    (0   + 0x4000)
#define CONST_TabV2R_ADDR                                            (0) //768 short
#define CONST_TabU2G_ADDR                                   (14 + 0x4000) //256 short



const INT32 G_VppPreviewCode[] MICRON_ALIGN(32)=
{
#include "bilinerzoom.tab"
};

const INT32 G_VppCutzzomCode[] MICRON_ALIGN(32)=
{
#include "cutzoom.tab"
};

const INT16 G_CUTZOOMDATA_TABX_ADDR[] MICRON_ALIGN(32)=
{
#include "Cutzoom_contdata_x.tab"
};

const INT16 G_CUTZOOMDATA_TABY_ADDR[] MICRON_ALIGN(32)=
{
#include "Cutzoom_contdata_y.tab"
};

#endif  // vpp_PREVIEW_ASM_H

