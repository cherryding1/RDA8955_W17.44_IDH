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

#ifndef __RDAMTV_FM_H__
#define __RDAMTV_FM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rda5888drv.h"

#ifdef RDA5888_FM_ENABLE
void RDA5888FM_Init(void);
uint16 RDA5888FM_GetRSSI(void);
void RDA5888FM_SetFreq(int16 curf);
uint8 RDA5888FM_GetSigLevel(int16 curf);
uint8 RDA5888FM_IsValidFreq(int16 freq);
void RDA5888FM_SetMute(uint8 mute);
bool RDA5888FM_IsValidChip(void);
void RDA5888FM_Exit(void);
#endif

#ifdef __cplusplus
}
#endif
#endif
