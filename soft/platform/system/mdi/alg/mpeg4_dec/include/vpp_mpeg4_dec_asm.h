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




#ifndef VPP_MPEG4_DEC_ASM_H
#define VPP_MPEG4_DEC_ASM_H

#include "cs_types.h"
#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000
#include "voc_map_addr.h"


#ifdef SHEEN_VC_DEBUG

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFFFF)
#define NoCacheAddr
#else

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFF<<2)
#define NoCacheAddr(a) ((UINT32)(a)|0x20000000)
#endif

#endif

