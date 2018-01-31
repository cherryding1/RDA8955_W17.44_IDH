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

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _XVID_HW_H
#define _XVID_HW_H

#define REG_MP4_BASE           0X1C0F0000           //up to 0xa000

#define MP4_CTRL_VLD                  REG_MP4_BASE + 0x00  //
#define MP4_STAT_VLD                  REG_MP4_BASE + 0x04  // 
#define MP4_STMBDRY                   REG_MP4_BASE + 0x08  // 
#define MP4_CTRL_IQ_IDCT              REG_MP4_BASE + 0x0C  // 
#define MP4_STAT_IQ_IDCT              REG_MP4_BASE + 0x10  //
#define MP4_CTRL_VLD_IQ_IDCT          REG_MP4_BASE + 0x14  // 
#define MP4_INTS_VLD                  REG_MP4_BASE + 0x18  //
#define MP4_INTM_VLD            REG_MP4_BASE + 0x1C  //
#define MP4_VLD_DATA            REG_MP4_BASE + 0x104
#define MP4_VLD_LENGTH          REG_MP4_BASE + 0x10C
#define MP4_VLD_BIAS            REG_MP4_BASE + 0x110

#define MP4_VLD_BUF                   0x1a000000    //
#define MP4_IDCT_BUF                  0x1a000080    //

//MP4_STAT_VLD
#define VLD_Enable  1<<0
#define VLD_Finish  1<<1

#endif
#endif

