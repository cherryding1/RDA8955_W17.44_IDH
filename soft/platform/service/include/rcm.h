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

#ifndef _RCM_H_
#define _RCM_H_

BOOL SRVAPI RCM_FlashGetSpace(UINT8 nModule, UINT32 *pStart, UINT32 *pSize);
BOOL SRVAPI RCM_GetRamSpace(UINT32 *pRamAddr, UINT32 *pRamSize);
BOOL SRVAPI RCM_ReleaseRamSpace(VOID);
BOOL SRVAPI RCM_IsValidHeap(PVOID p); // Check the addres is valid heap.

//
//add by lixp
//

typedef struct _RCM_PBK_CAP
{
    UINT32  m_PbkMinNum;      // size in byte
    UINT32  m_PbkMcMaxNumber; // size in byte
    UINT32  m_PbkRcMaxNum;    // size in byte
    UINT32  m_PbkLdNum;       // size in byte.
} RCM_PBK_CAP;

typedef struct _RCM_SMS_CAP
{
    UINT32  m_SmsMaxNum;    // size in byte.
    UINT32  m_SmsCbMaxNum;  // size in byte.
    UINT32  m_SmsSrMaxNum;  // size in byte.
} RCM_SMS_CAP;

UINT32 SRVAPI RCM_GetPbkCap(RCM_PBK_CAP *pPbkCap ) ;
UINT32 SRVAPI RCM_GetSmsCap(RCM_SMS_CAP *pSmsCap ) ;


#endif // _H_

