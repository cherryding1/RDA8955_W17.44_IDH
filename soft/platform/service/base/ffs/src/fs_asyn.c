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

#ifndef _FS_SIMULATOR_



#include "dsm_cf.h"
#include "fs.h"
#include "fs_asyn.h"
#include "fs_asyn_local.h"
#include "ts.h"
#include "csw_mem_prv.h"

HANDLE g_hAsynFsTask = HNULL;
INT32 FS_AsynReadReq(INT32 iFd, UINT8 *pBuff, UINT32 uSize, FS_ASNY_FUNCPTR pCallback)
{
    FS_ASYN_EVENT sEv = {0,};
    FS_ASYN_PARAM  *pParam = NULL;

    if(iFd < 0 || NULL == pBuff || NULL == pCallback)
    {
        return ERR_FS_ASYN_PARA_INVALID;
    }

    pParam = (FS_ASYN_PARAM *)CSW_FFS_MALLOC(SIZEOF(FS_ASYN_PARAM));
    if(pParam == NULL)
    {
        return ERR_FS_ASYN_MALLOC_FAILED;
    }
    pParam ->Fd = iFd;
    pParam ->pBuff = (UINT32)pBuff;
    pParam ->uSize = uSize;
    pParam ->pCallback = (UINT32)pCallback;

    sEv.nEventId = FS_ASYN_EVENT_ID_READ_REQ;
    sEv.nParam1 = (UINT32)pParam;
    sEv.nParam2 = 0;
    sEv.nParam3 = 0;

    TASK_HANDLE *pHTask = (TASK_HANDLE *)g_hAsynFsTask;
    if(pHTask)
    {
        COS_SendEvent(pHTask, &sEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }
    else
    {
        CSW_FFS_FREE(pParam);
        return ERR_FS_ASYN_TASK_NOT_START;
    }
    return ERR_FS_ASYN_SUCCESS;
}

INT32 FS_AsynWriteReq(INT32 iFd, UINT8 *pBuff, UINT32 uSize, FS_ASNY_FUNCPTR pCallback)
{
    FS_ASYN_EVENT sEv = {0,};
    FS_ASYN_PARAM  *pParam = NULL;

    if(iFd < 0 || NULL == pBuff || NULL == pCallback)
    {
        return ERR_FS_ASYN_PARA_INVALID;
    }

    pParam = (FS_ASYN_PARAM *)CSW_FFS_MALLOC(SIZEOF(FS_ASYN_PARAM));
    if(NULL == pParam)
    {
        return ERR_FS_ASYN_MALLOC_FAILED;
    }
    pParam ->Fd = iFd;
    pParam ->pBuff = (UINT32)pBuff;
    pParam ->uSize = uSize;
    pParam ->pCallback = (UINT32)pCallback;

    sEv.nEventId = FS_ASYN_EVENT_ID_WRITE_REQ;
    sEv.nParam1 = (UINT32)pParam;
    sEv.nParam2 = 0;
    sEv.nParam3 = 0;

    TASK_HANDLE *pHTask = (TASK_HANDLE *)g_hAsynFsTask;
    if(pHTask)
    {
        COS_SendEvent(pHTask, &sEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }
    else
    {
        CSW_FFS_FREE(pParam);
        return ERR_FS_ASYN_TASK_NOT_START;
    }
    return ERR_FS_ASYN_SUCCESS;
}


INT32 FS_AsynGetFsInfoReq(UINT8 *pDevName, FS_ASNY_FUNCPTR pCallback)
{
    FS_ASYN_EVENT sEv = {0,};
    FS_ASYN_GETFSINFO_REQ  *pParam = NULL;

    if(NULL == pDevName || NULL == pCallback)
    {
        return ERR_FS_ASYN_PARA_INVALID;
    }

    pParam = (FS_ASYN_GETFSINFO_REQ *)CSW_FFS_MALLOC(SIZEOF(FS_ASYN_GETFSINFO_REQ));
    if(NULL == pParam)
    {
        return ERR_FS_ASYN_MALLOC_FAILED;
    }
    DSM_StrCpy(pParam->pDevName,pDevName);
    pParam ->pCBFunc = (FS_ASNY_FUNCPTR)pCallback;

    sEv.nEventId = FS_ASYN_EVENT_ID_GETFSINFO_REQ;
    sEv.nParam1 = (UINT32)pParam;
    sEv.nParam2 = 0;
    sEv.nParam3 = 0;

    TASK_HANDLE *pHTask = (TASK_HANDLE *)g_hAsynFsTask;
    if(pHTask)
    {
        COS_SendEvent(pHTask, &sEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }
    else
    {
        CSW_FFS_FREE(pParam);
        return ERR_FS_ASYN_TASK_NOT_START;
    }
    return ERR_FS_ASYN_SUCCESS;
}


VOID BAL_AsynFsTask(void *pData)
{
    FS_ASYN_EVENT ev = {0,};
    TASK_HANDLE *pHTask = NULL;

    pHTask = (TASK_HANDLE *)g_hAsynFsTask;
    SUL_ZeroMemory32(&ev, SIZEOF(FS_ASYN_EVENT) );

    while (1)
    {
        COS_WaitEvent(pHTask, &ev, COS_WAIT_FOREVER);
        switch(ev.nEventId)
        {
            case FS_ASYN_EVENT_ID_READ_REQ:
                FS_AsynReadPrc(ev.nParam1);
                break;
            case FS_ASYN_EVENT_ID_WRITE_REQ:
                FS_AsynWritePrc(ev.nParam1);
                break;
            case FS_ASYN_EVENT_ID_GETFSINFO_REQ:
                FS_AsynGetFsInfoPrc(ev.nParam1);
                break;
            default:
                break;
        }
        CSW_FFS_FREE((void *)ev.nParam1);
        //sxr_Sleep(10);
    }
    return;
}

VOID FS_AsynReadPrc(UINT32 pParam1)
{
    FS_ASYN_READ_REQ *pReq = NULL;
    FS_ASYN_READ_RESULT  sRsp;
    INT32 iResult = 0;

    if(0 == pParam1)
    {
        return;
    }

    pReq = (FS_ASYN_READ_REQ *)pParam1;
    iResult = FS_Read(pReq->iFd, pReq->pBuff, pReq->uSize);
    if(iResult >= 0)
    {
        sRsp.iResult = ERR_SUCCESS;
        sRsp.pBuff = pReq->pBuff;
        sRsp.uSize = iResult;
    }
    else
    {
        // mmi_trace(TRUE,"FS_Read() Filed!iResult = %d.",iResult);
        sRsp.iResult = iResult;
        sRsp.pBuff = pReq->pBuff;
        sRsp.uSize = 0;
    }

    pReq->pCBFunc(&sRsp);
    return;
}

VOID FS_AsynWritePrc(UINT32 pParam1)
{
    FS_ASYN_WRITE_REQ *pReq = NULL;
    FS_ASYN_READ_RESULT  sRsp = {0,};
    INT32 iResult;

    if(0 == pParam1)
    {
        return;
    }

    pReq = (FS_ASYN_WRITE_REQ *)pParam1;

    iResult = FS_Write(pReq->iFd, pReq->pBuff, pReq->uSize);
    if(iResult >= 0)
    {
        sRsp.iResult = ERR_SUCCESS;
        sRsp.pBuff = pReq->pBuff;
        sRsp.uSize = iResult;
    }
    else
    {
        // mmi_trace(TRUE,"FS_Write() Filed!iResult = %d.", iResult);
        sRsp.iResult = iResult;
        sRsp.pBuff = pReq->pBuff;
        sRsp.uSize = 0;
    }

    pReq->pCBFunc(&sRsp);
    return;
}

VOID FS_AsynGetFsInfoPrc(UINT32 pParam1)
{
    FS_ASYN_GETFSINFO_REQ *pReq = NULL;
    FS_ASYN_GETFSINFO_RESULT sRsp = {0,};
    FS_INFO sFsInfo;
    INT32 iResult;

    if(0 == pParam1)
    {
        return;
    }

    pReq = (FS_ASYN_GETFSINFO_REQ *)pParam1;

    iResult = FS_GetFSInfo(pReq->pDevName,&sFsInfo);
    if(iResult == 0)
    {
        sRsp.iResult = ERR_SUCCESS;
        sRsp.iTotalSize = sFsInfo.iTotalSize;
        sRsp.iUsedSize = sFsInfo.iUsedSize;
    }
    else
    {
        // mmi_trace(TRUE,"FS_GetFSInfo() Filed!iResult = %d.", iResult);
        sRsp.iResult = iResult;
        sRsp.iTotalSize = 0;
        sRsp.iUsedSize = 0;
    }

    pReq->pCBFunc(&sRsp);
    return;
}

#endif


