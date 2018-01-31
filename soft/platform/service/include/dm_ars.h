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

#if !defined(__DM_ARS_H__)
#define __DM_ARS_H__

#include "ars_m.h"


BOOL DM_ArsSetup(CONST ARS_AUDIO_CFG_T *cfg);
BOOL DM_ArsRecord(CONST ARS_ENC_STREAM_T *stream, CONST ARS_AUDIO_CFG_T *cfg, BOOL loop);
BOOL DM_ArsPause(BOOL pause);
BOOL DM_ArsStop(VOID);

UINT32 *DM_ArsGetBufPosition(VOID);
BOOL DM_ArsReadData(UINT32 addedDataBytes);
UINT32 DM_ArsAvailableData(VOID);

#endif // __DM_ARS_H__

