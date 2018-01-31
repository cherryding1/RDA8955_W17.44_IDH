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
#ifndef _SMS_DM_NV_H_
#define _SMS_DM_NV_H_

typedef struct
{
    UINT16 recStatus;
    UINT8 data[SMS_DM_SZ_RECORD_DATA];
}SMS_DM_NV_REC;

typedef struct
{
    UINT32  version;
    SMS_DM_NV_REC smsRec[SMS_DM_MAX_REC_COUNT];
}SMS_DM_NV;

INT32 SMS_DM_GetFreeIndexFromNv(UINT16 *pIndex, CFW_SIM_ID nSimId);
static INT32 SMS_DM_Update(VOID);

#endif
