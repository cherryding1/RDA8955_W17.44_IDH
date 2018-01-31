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

#ifndef _SMS_DM_LOCAL_H_
#define _SMS_DM_LOCAL_H_

#define SMS_DM_VERSION 0x20171106 // the version number

extern UINT16 g_SMS_DM_InitFlag;
extern COS_SEMA g_SMS_DM_Sem;

#ifdef _FS_SIMULATOR_
#define SMS_DM_SemInit()
#define SMS_DM_Down()
#define SMS_DM_Up()
#else
#define SMS_DM_SemInit() COS_SemaInit(&g_SMS_DM_Sem, 1)
#define SMS_DM_Down() COS_SemaTake(&g_SMS_DM_Sem)
#define SMS_DM_Up() COS_SemaRelease(&g_SMS_DM_Sem)
#endif

INT32 SMS_DM_GetRange(CFW_SIM_ID nSimId, UINT16 *nStartIndex, UINT16 *nEndIndex);

#endif
