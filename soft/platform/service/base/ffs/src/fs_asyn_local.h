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


#ifndef _FS_ASYN_LOCAL_H_
#define _FS_ASYN_LOCAL_H_

typedef struct  _FS_ASYN_PARAM
{
    UINT32 Fd;
    UINT32 pBuff;
    UINT32 uSize;
    UINT32 pCallback;
} FS_ASYN_PARAM;

typedef struct _FS_ASYN_EVENT
{
    UINT32 nEventId;
    UINT32 nParam1;
    UINT32 nParam2;
    UINT32 nParam3;
} FS_ASYN_EVENT;

VOID FS_AsynReadPrc(UINT32 pParam1);
VOID FS_AsynWritePrc(UINT32 pParam1);
VOID FS_AsynGetFsInfoPrc(UINT32 pParam1);


#endif






