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

#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "vds_api.h"
#include "nvram.h"
#include "nvram_local.h"


#define NVRAM_TRACE CSW_TRACE
#define NVRAM_DUMP(buff,size) //DSM_Dump(buff,size,16)

extern NVRAM_GLOBAL_VAL g_nvram_global_var;
INT32 g_iNvramErrcode = 0;
static INT32 nvram_ErrCode(INT32 iLocalErrCode)
{
    INT32 iErrCode = ERR_SUCCESS;
    g_iNvramErrcode = iLocalErrCode;
    switch(iLocalErrCode)
    {
        case _ERR_NVRAM_PARAM_ERROR       :
            iErrCode = ERR_NVRAM_PARAM_ERROR         ;
            break;
        case _ERR_NVRAM_MEDIA_READ_FAILED :
            iErrCode = ERR_NVRAM_DEV_READ_FAILED   ;
            break;
        case _ERR_NVRAM_MEDIA_WRITE_FAILED:
            iErrCode = ERR_NVRAM_DEV_WRITE_FAILED  ;
            break;
        case _ERR_NVRAM_GET_DEV_NR_FAILED :
            iErrCode = ERR_NVRAM_DEV_GET_NR_FAILED   ;
            break;
        case _ERR_NVRAM_NO_MORE_ND        :
            iErrCode = ERR_NVRAM_NO_MORE_ND          ;
            break;
        case _ERR_NVRAM_INVALID_ID        :
            iErrCode = ERR_NVRAM_INVALID_ID          ;
            break;
        case _ERR_NVRAM_INVALID_ND        :
            iErrCode = ERR_NVRAM_INVALID_ND          ;
            break;
        case _ERR_NVRAM_OFFS_OVERFLOW     :
            iErrCode = ERR_NVRAM_OFFS_OVERFLOW       ;
            break;
        case _ERR_NVRAM_STATUS_ERROR      :
            iErrCode = ERR_NVRAM_STATUS_ERROR        ;
            break;
        case _ERR_NVRAM_CHECKOUT_ERROR      :
            iErrCode = ERR_NVRAM_CHECKOUT_ERROR        ;
            break;
        case _ERR_NVRAM_MALLOC_FAILED     :
            iErrCode = ERR_NVRAM_MALLOC_FAILED       ;
            break;
        case _ERR_NVRAM_OPEN_NVE_TOO_MORE          :
            iErrCode = ERR_NVRAM_OPEN_NVE_TOO_MORE       ;
            break;
        case _ERR_NVRAM_INVALID_HD          :
            iErrCode = ERR_NVRAM_INVALID_HD       ;
            break;
        case _ERR_NVRAM_GET_DEV_INFO_FAILED          :
            iErrCode = ERR_NVRAM_GET_DEV_INFO_FAILED       ;
            break;
        case _ERR_NVRAM_UNINIT          :
            iErrCode = ERR_NVRAM_UNINIT       ;
            break;
        case _ERR_NVRAM_ENTRY_IS_OPEN :
            iErrCode = ERR_NVRAM_ENTRY_IS_OPEN       ;
            break;
        case _ERR_NVRAM_RN_ERROR :
            iErrCode = ERR_NVRAM_RN_ERROR       ;
            break;
        case _ERR_NVRAM_CREATE_FAILED :
            iErrCode = ERR_NVRAM_CREATE_FAILED       ;
            break;

        default:
            iErrCode = iLocalErrCode;
            break;
    }
    return iErrCode;
}

#if 0
extern BOOL g_tflashCardExist;
INT32 g_iNvramFp = -1;
VOID nvram_Dump(UINT8 *pBuff, UINT32 iSize)
{
    UINT8 log_name[32] = {'/', 0, 't', 0, '/', 0, 'n', 0, 'v', 0, 'r', 0, 'a', 0, 'm', 0, '.', 0, 't', 0, 'r', 0, 'a', 0, 0, 0,};
    if(g_tflashCardExist)
    {
        if(-1 == g_iNvramFp)
        {
            g_iNvramFp = FS_Create(log_name, 0);
            if(g_iNvramFp < 0)
            {
                return;
            }
        }

        if(g_iNvramFp >= 0)
        {
            FS_Write(g_iNvramFp, pBuff, iSize);
            FS_Flush(g_iNvramFp);
        }
    }
    else
    {
        DSM_Dump(pBuff, iSize, 16);
    }
}

INT32 nvram_Trace(UINT32 id, CONST INT8 *fmt, ...)
{
    INT32 count;

    // static INT32 fp = -1;
    UINT8 log_name[32] = {'/', 0, 't', 0, '/', 0, 'n', 0, 'v', 0, 'r', 0, 'a', 0, 'm', 0, '.', 0, 't', 0, 'r', 0, 'a', 0, 0, 0,};
    char buf[512];
    va_list ap;

    id = id;
    va_start (ap, fmt);

    if((count = DSM_StrVPrint(buf, fmt, ap)) > 510)
    {
        DSM_ASSERT(0, "count %d overflow%x\n", count);
    }

    va_end (ap);

    DSM_StrCat(buf, "\n");
    if(g_tflashCardExist)
    {
        if(-1 == g_iNvramFp)
        {
            g_iNvramFp = FS_Create(log_name, 0);
            if(g_iNvramFp < 0)
            {
                return 0;
            }

        }


        if(g_iNvramFp >= 0)
        {
            FS_Write(g_iNvramFp, buf, DSM_StrLen(buf));
            //FS_Flush(g_iNvramFp);
        }
    }

    CSW_TRACE(BASE_DSM_TS_ID, buf);

    return count;
}
#endif


INT32 NVRAM_Init(VOID)
{
    INT32 iResult = ERR_SUCCESS;
    INT32 iRet = ERR_SUCCESS;


    if(!nvram_Existed())
    {
        iResult = nvram_Format();
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Init: nvram foramt failed,err_code = %d.", iResult));
            iRet = nvram_ErrCode(iResult);
        }
    }

    if(ERR_SUCCESS == iResult)
    {
        if(nvram_Existed())
        {
            iResult = nvram_InitGlobal();
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Init: nvram init global failed,err_code = %d.", iResult));
                iRet = nvram_ErrCode(iResult);
            }
        }
        else
        {
            iResult = _ERR_NVRAM_CREATE_FAILED;
            D((DL_NVRAMERROR, "NVRAM_Init: create nvram failed,err_code = %d.", iResult));
            iRet = nvram_ErrCode(iResult);
        }
    }
    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1), TSTR("nvram init: ret = %d.",0x080000db), iRet);
    return iRet;
}

INT32 NVRAM_Format(void)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Format: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram foramt ret = %d.", iRet);
        return iRet;
    }

    nvram_WaitForSem();
    nvram_CloseNE();
    iResult = nvram_Format();
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "NVRAM_Format: nvram foramt failed,err_code = %d.", iResult));
        iRet = nvram_ErrCode(iResult);
    }
    nvram_ReleaseSem();
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram foramt: ret = %d.", iRet);
    return iRet;
}



INT32 NVRAM_Create(CONST UINT32  iId, UINT32 iSize)
{
    return NVRAM_CreateEx(iId, iSize, NVRAM_DEFAULT_VALUE);
}



INT32 NVRAM_CreateEx(CONST UINT32  iId, UINT32 iSize, UINT8 iDefault)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;
    NVRAM_R_INFO sRInfo;
    NVRAM_INFO *psNInfo = NULL;
    UINT32 iFstRN = NVRAM_INVALID_RN;
    UINT32 iRN;
    UINT32 iNextRN;
    UINT32 iNRCount = 0;
    UINT32 iNRGetCount = 0;
    UINT32 iInd;
    UINT32 i;
    INT32  iResult;
    INT32 iRet = ERR_SUCCESS;
    INT32 hd = 0;

    NVRAM_OPT eNDOpt = NVRAM_OPT_DO_NOTHING;
    NVRAM_OPT eRNOpt = NVRAM_OPT_DO_NOTHING;


    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Create: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram create(%d,%d) ret = %d.", iId, iSize, iRet);
        return iRet;
    }

    nvram_WaitForSem();
    // Find a free hd.
    for(i = 0 ; i < NVRAM_OPEN_MAX; i++)
    {
        if(!NVRAM_IS_VALID_ID(g_nvram_global_var.psNInfo[i].iId))
        {
            psNInfo = &g_nvram_global_var.psNInfo[i];
            hd = i;
            break;
        }
    }

    if(NULL == psNInfo)
    {
        D((DL_NVRAMERROR, "NVRAM_Create: open nve too more."));
        iRet = nvram_ErrCode(_ERR_NVRAM_OPEN_NVE_TOO_MORE);
        goto nvram_create;
    }

    // Remove old NE by iId if it is exist.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {
        iNRCount = NVRAM_NR_COUNT(sNDInfo.iSize);
        iResult = nvram_GetRNCount(iId, &iNRGetCount);
        if(ERR_SUCCESS != iResult)
        {
            eNDOpt = NVRAM_OPT_ADD;
            eRNOpt = NVRAM_OPT_ADD;
        }
        else if(iNRGetCount == iNRCount)
        {
            eNDOpt = NVRAM_OPT_DO_NOTHING;
            eRNOpt = NVRAM_OPT_SET_DEFAULT;
        }
        else
        {
            eNDOpt = NVRAM_OPT_RESET;
            eRNOpt = NVRAM_OPT_RESET;
        }
    }
    else  if(NDN_NOT_EXIST == iResult)
    {
        // do nothing.
        eNDOpt = NVRAM_OPT_ADD;
        eRNOpt = NVRAM_OPT_ADD;
    }
    else
    {
        D((DL_NVRAMERROR, "NVRAM_Create: nvram lookupNDN failed,err_code = %d.", iResult));
        iRet = nvram_ErrCode(iResult);
        goto nvram_create;
    }

    DSM_MemSet(sRInfo.pData, iDefault, NVRAM_DATA_SIZE);

    if(NVRAM_OPT_ADD == eRNOpt ||
            NVRAM_OPT_RESET == eRNOpt)
    {
        if(NVRAM_OPT_RESET == eRNOpt)
        {
            iResult = nvram_GetRNCount(iId, &iNRCount);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Create: nvram get rn count failed,iId = 0x%x,err_code = %d.", iId, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_create;
            }

            iRN = NVRAM_INVALID_RN;
            iResult = nvram_GetFstRN(iId, &iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Create: get fst rn failed,iId = 0x%x,err_code = %d.", iId, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_create;
            }
            iInd = 0;
            while(NVRAM_IS_VALID_RN(iRN) && iInd < iNRCount)
            {
                iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
                if(ERR_SUCCESS != iResult)
                {
                    D((DL_NVRAMERROR, "NVRAM_Create: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                    iRet = nvram_ErrCode(iResult);
                    goto nvram_create;
                }

                iResult = nvram_DeleteR(iRN);
                if(ERR_SUCCESS != iResult)
                {
                    D((DL_NVRAMERROR, "NVRAM_Create: nvram delete nr failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                    iRet = nvram_ErrCode(iResult);
                    goto nvram_create;
                }
                iInd++;
                iRN = iNextRN;
            }
        }

        iNRCount = NVRAM_NR_COUNT(iSize);

        sRInfo.sCtr.iInd = 0;
        sRInfo.sCtr.iStatus = NVRAM_STATUS_VALID;
        sRInfo.sCtr.iId = iId;
        sRInfo.sCtr.iCrc = nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE);
        iRN = NVRAM_INVALID_RN;
        iNextRN = NVRAM_INVALID_RN;
        while(sRInfo.sCtr.iInd < iNRCount)
        {
            iResult = nvram_GetFreeRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {
                iFstRN = iFstRN == NVRAM_INVALID_RN ? iNextRN : iFstRN;
                iResult = nvram_WriteR(sRInfo, iNextRN);
                if(ERR_SUCCESS != iResult)
                {
                    D((DL_NVRAMERROR, "NVRAM_Create: nvram write new rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                    iRet = nvram_ErrCode(iResult);
                    goto nvram_create;
                }

                sRInfo.sCtr.iInd++ ;
                iRN = iNextRN;
            }
            else
            {
                D((DL_NVRAMERROR, "NVRAM_Create: get free rn failed,iId = 0x%x,err_code = %d.", iId, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_create;
            }
        }
        if(sRInfo.sCtr.iInd != iNRCount)
        {
            D((DL_NVRAMERROR, "NVRAM_Create: ind != nr_count for id failed,iId = 0x%x,iInd = 0x%x,iNRCount = 0x%x.", iId, sRInfo.sCtr.iInd, iNRCount));
            iResult = _ERR_NVRAM_CREATE_FAILED;
            iRet = nvram_ErrCode(iResult);
            goto nvram_create;
        }

    }
    else if(NVRAM_OPT_SET_DEFAULT == eRNOpt)
    {
        iNRCount = NVRAM_NR_COUNT(iSize);
        iResult = nvram_GetFstRN(iId, &iRN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Create: get fst rn failed,iId = 0x%x,err_code = %d.", iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_create;
        }
        sRInfo.sCtr.iInd = 0;
        while(NVRAM_IS_VALID_RN(iRN) && sRInfo.sCtr.iInd < iNRCount)
        {
            iFstRN = iFstRN == NVRAM_INVALID_RN ? iRN : iFstRN;
            iResult = nvram_WriteR(sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Create: nvram write rn to default failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_create;
            }
            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {
                sRInfo.sCtr.iInd++ ;
                iRN = iNextRN;
            }
            else
            {
                D((DL_NVRAMERROR, "NVRAM_Create: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_create;
            }
        }
        if(sRInfo.sCtr.iInd != iNRCount)
        {
            D((DL_NVRAMERROR, "NVRAM_Create: ind != nr_count for id failed,iId = 0x%x,iInd = 0x%x,iNRCount = 0x%x.", iId, sRInfo.sCtr.iInd, iNRCount));
            iResult = _ERR_NVRAM_CREATE_FAILED;
            iRet = nvram_ErrCode(iResult);
            goto nvram_create;
        }

    }
    else
    {
        // do nothing.
    }

    sNDInfo.iId = iId;
    sNDInfo.iSize = iSize;
    if(NVRAM_OPT_ADD == eNDOpt)
    {
        iResult = nvram_AddND(sNDInfo);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Create: nvram set nd  failed,iNDN = 0x%x,iId = 0x%x,err_code = %d.", iNDN, iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_create;
        }
    }
    else if(NVRAM_OPT_RESET == eNDOpt)
    {
        iResult = nvram_SetNDE(sNDInfo, iNDN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Create: nvram set nd  failed,iNDN = 0x%x,iId = 0x%x,err_code = %d.", iNDN, iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_create;
        }
    }
    else
    {
        // do nothing.
    }
    // lookup NRN,get free record list or invalid record list.

    psNInfo->iId = iId;
    psNInfo->iSize = iSize;
    psNInfo->iFstRN = iFstRN;

nvram_create:
    if(ERR_SUCCESS == iRet)
    {
        iRet = hd;
    }
    else
    {

    }
    nvram_ReleaseSem();
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram create: (%d,%d) ret = %d.", iId, iSize, iRet);
    return iRet;
}


INT32 NVRAM_Open(CONST UINT32  iId, BOOL bCheckout)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;
    NVRAM_INFO *psNInfo = NULL;
    UINT32 iNRCount = 0;
    UINT32 i;
    INT32  iResult;
    INT32 iRet;
    INT32 hd = 0;
    UINT32 iFstRN = NVRAM_INVALID_RN;
    UINT32 iGetRNCount = 0;
    BOOL bResult;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Open: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram open(%d,%d) ret = %d", iId, bCheckout, iRet);
        return iRet;
    }
    nvram_WaitForSem();
    // Find a free hd.
    for(i = 0 ; i < NVRAM_OPEN_MAX; i++)
    {
        if(!NVRAM_IS_VALID_ID(g_nvram_global_var.psNInfo[i].iId))
        {
            psNInfo = &g_nvram_global_var.psNInfo[i];
            hd = i;
            break;
        }
    }

    if(NULL == psNInfo)
    {
        D((DL_NVRAMERROR, "NVRAM_Open: open nve too more."));
        iRet = nvram_ErrCode(_ERR_NVRAM_OPEN_NVE_TOO_MORE);
        goto nvram_open;
    }

    // Find nd and rn.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {
        iNRCount = NVRAM_NR_COUNT(sNDInfo.iSize);

        iResult = nvram_GetRNCount(iId, &iGetRNCount);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Open: nvram GetRNCount failed,iId = 0x%x,err_code = %d.", iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_open;
        }

        if(TRUE == bCheckout)
        {
            bResult = nvram_CheckId(iId, iNRCount);
            if(FALSE == bResult)
            {
                iRet = ERR_NVRAM_CHECKOUT_ERROR;
                D((DL_NVRAMERROR, "NVRAM_Open: nvram_CheckId failed,iId = 0x%x,err_code = %d.", iId, iResult));
                goto nvram_open;
            }
            else
            {
            }
        }

        iResult = nvram_GetFstRN(iId, &iFstRN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Open: nvram GetFstRN failed,iId = 0x%x,err_code = %d.", iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_open;
        }
    }
    else  if(NDN_NOT_EXIST == iResult)
    {
        D((DL_NVRAMERROR, "NVRAM_Open: invalid id,iId = 0x%x,err_code = %d.", iId, iResult));
        iRet  = nvram_ErrCode(ERR_NVRAM_INVALID_ID);
        goto nvram_open;
    }
    else
    {
        D((DL_REGERROR, "NVRAM_Open: nvram lookupNDN failed,err_code = %d.", iResult));
        iRet  = nvram_ErrCode(iResult);
        goto nvram_open;
    }
    psNInfo->iId = iId;
    psNInfo->iSize = sNDInfo.iSize;
    psNInfo->iFstRN = iFstRN;
    iRet = ERR_SUCCESS;
nvram_open:
    if(ERR_SUCCESS == iRet)
    {
        iRet = hd;
    }

    nvram_ReleaseSem();
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram open(%d,%d) ret = %d", iId, bCheckout, hd);
    return iRet;
}


INT32 NVRAM_Close(INT32 hd)
{
    NVRAM_INFO *psNInfo = NULL;
    INT32 iRet;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Close: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram close: (%d) ret = %d.", hd, iRet);
        return iRet;
    }

    nvram_WaitForSem();

    if( hd < 0  || hd >= NVRAM_OPEN_MAX)
    {
        D((DL_NVRAMERROR, "NVRAM_Close: Invalid hd. hd = %d.", hd));
        iRet = nvram_ErrCode(ERR_NVRAM_INVALID_HD);
        goto nvram_close;
    }

    if(!NVRAM_IS_VALID_ID(g_nvram_global_var.psNInfo[hd].iId))
    {
        D((DL_NVRAMERROR, "NVRAM_Close: Invalid hd. hd = %d.", hd));
        iRet = nvram_ErrCode(ERR_NVRAM_NOT_OPEN);
        goto nvram_close;
    }

    psNInfo = &g_nvram_global_var.psNInfo[hd];


    psNInfo->iFstRN = NVRAM_INVALID_RN;
    psNInfo->iId = NVRAM_INVALID_ID;
    psNInfo->iSize = 0;
    iRet = ERR_SUCCESS;

nvram_close:
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram close:(%d) ret = %d.", hd, iRet);
    nvram_ReleaseSem();
    return iRet;
}


BOOL NVRAM_IsExist(CONST UINT32 iId)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;
    INT32  iResult;
    BOOL bRet = FALSE;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_IsExist: nvram uninitialized."));
        nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram isexist:(%d) ret = FALSE.", iId);
        return FALSE;
    }
    nvram_WaitForSem();
    // Look up NDN.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {
        bRet = TRUE;
    }
    else  if(NDN_NOT_EXIST == iResult)
    {
        bRet = FALSE;
    }
    else
    {
        D((DL_NVRAMERROR, "NVRAM_IsExist: nvram lookupNDN failed,id = 0x%x,err_code = %d.", iId, iResult));
        nvram_ErrCode(iResult);
        bRet = FALSE;
    }
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram isexist:(%d) ret = %d.", iId, bRet);
    nvram_ReleaseSem();
    return bRet;
}

BOOL NVRAM_Check(CONST UINT32 iId, BOOL bCheckout)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;
    UINT32 iNRCount = 0;
    INT32  iResult;
    BOOL  bRet;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Check: nvram uninitialized."));
        nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram check:(%d,%d) ret = FALSE.", iId, bCheckout);
        return FALSE;
    }
    nvram_WaitForSem();
    // Look up NDE.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {
        iNRCount = NVRAM_NR_COUNT(sNDInfo.iSize);

        if(TRUE == bCheckout)
        {
            bRet = nvram_CheckId(iId, iNRCount);
        }
        else
        {
            bRet = TRUE;
        }

    }
    else  if(NDN_NOT_EXIST == iResult)
    {
        D((DL_NVRAMERROR, "NVRAM_Check: invalid id,iId = 0x%x,err_code = %d.", iId, iResult));
        nvram_ErrCode(ERR_NVRAM_INVALID_ID);
        bRet = FALSE;
    }
    else
    {
        D((DL_REGERROR, "NVRAM_Check: nvram lookupNDN failed,err_code = %d.", iResult));
        nvram_ErrCode(iResult);
        bRet =  FALSE;
    }

    nvram_ReleaseSem();
    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram check:(%d,%d) ret = %d.", iId, bCheckout, bRet);
    return bRet;
}


INT32 NVRAM_Remove(CONST UINT32 iId)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;

    UINT32 iRNCount = 0;
    UINT32 iRN = NVRAM_INVALID_RN;
    UINT32 iNextRN = NVRAM_INVALID_RN;
    UINT32 iInd = 0;

    INT32  iResult;
    INT32 iRet;
    UINT32 i;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_Remove: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram remove: 1(%d) ret = %d.", iId, iRet);
        return iRet;
    }



    nvram_WaitForSem();

    if(!NVRAM_IS_VALID_ID(iId))
    {
        D((DL_NVRAMERROR, "NVRAM_Remove: invalid id iId = 0x%x.", iId));
        iRet = nvram_ErrCode(_ERR_NVRAM_INVALID_ID);
        goto nvram_remove;
    }

    // check this entry if is opened.
    for(i = 0 ; i < NVRAM_OPEN_MAX; i++)
    {
        if(iId == g_nvram_global_var.psNInfo[i].iId)
        {
            D((DL_NVRAMERROR, "NVRAM_Remove: nvram  entry is opened,iId = 0x%x.", iId));
            iRet = nvram_ErrCode(_ERR_NVRAM_ENTRY_IS_OPEN);
            goto nvram_remove;
        }
    }



    // Remove old NE by iId if it is exist.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {

        iResult = nvram_GetRNCount(iId, &iRNCount);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Remove: get rn count failed,iId = 0x%x,err_code = %d.", iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_remove;
        }

        iResult = nvram_GetFstRN(iId, &iRN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Remove: get fst rn failed,iId = 0x%x,err_code = %d.", iId, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_remove;
        }

        // Set Record status to invalid.
        iInd = 0;
        while(iInd < iRNCount)
        {
            if(!NVRAM_IS_VALID_RN(iRN))
            {
                D((DL_NVRAMERROR, "NVRAM_Remove: invalid iRN,iId = 0x%x,iRN = 0x%x,iInd = %d.", iId, iRN, iInd));
                iResult = _ERR_NVRAM_RN_ERROR;
                iRet = nvram_ErrCode(iResult);
                goto nvram_remove;
            }
            iNextRN = NVRAM_INVALID_RN;
            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Remove: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_remove;
            }

            iResult = nvram_DeleteR(iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "NVRAM_Remove: nvram delete RN failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                iRet = nvram_ErrCode(iResult);
                goto nvram_remove;
            }

            iInd++ ;
            iRN = iNextRN;
        }
        if(iRNCount == iInd)
        {
            // donoting.
        }
        else
        {
            D((DL_NVRAMERROR, "NVRAM_Remove: iInd error,iId = 0x%x,iInd = 0x%x,iRNCount = 0x%x.", iId, iInd, iRNCount));
            iResult = _ERR_NVRAM_RN_ERROR;
            iRet = nvram_ErrCode(iResult);
            goto nvram_remove;
        }

        // Delete NED
        iResult = nvram_DelNDE(iNDN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "NVRAM_Remove: nvram delete NDE failed,iId = 0x%x,err_code = %d.", iNDN, iResult));
            iRet = nvram_ErrCode(iResult);
            goto nvram_remove;
        }
    }
    else  if(NDN_NOT_EXIST == iResult)
    {
        // do nothing.
    }
    else
    {
        D((DL_NVRAMERROR, "NVRAM_Remove: nvram lookupNDN failed,err_code = %d.", iResult));
        iRet = nvram_ErrCode(iResult);
        goto nvram_remove;
    }
    iRet = ERR_SUCCESS;
nvram_remove:
    nvram_ReleaseSem();
    return iRet;
}


UINT32 NVRAM_SetValue(INT32 hd, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff)
{
    NVRAM_INFO *psNInfo = NULL;
    INT32 iResult;
    INT32 iRet;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_SetValue: nvram uninitialized."));
        nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram setvalue: 1(%d,0x%x,0x%x).", hd, iOffs, iSize);
        return 0;
    }

    nvram_WaitForSem();
    if( hd < 0  || hd >= NVRAM_OPEN_MAX)
    {
        D((DL_NVRAMERROR, "NVRAM_SetValue: Invalid hd. hd = %d.", hd));
        nvram_ErrCode(ERR_NVRAM_INVALID_HD);
        iRet = 0;
        goto nvram_setvalue;
    }

    if(!NVRAM_IS_VALID_ID(g_nvram_global_var.psNInfo[hd].iId))
    {

        D((DL_NVRAMERROR, "NVRAM_SetValue: Invalid hd. hd = %d.", hd));
        nvram_ErrCode(ERR_NVRAM_NOT_OPEN);
        iRet = 0;
        goto nvram_setvalue;

    }

    psNInfo = &g_nvram_global_var.psNInfo[hd];

    iResult = nvram_WriteNV(psNInfo, psNInfo->iId, iOffs, iSize, pBuff);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "NVRAM_SetValue: nvram write nv failed,iId = 0x%x,err_code = %d.", psNInfo->iId, iResult));
        nvram_ErrCode(iResult);
        iRet = 0;
        goto nvram_setvalue;
    }
    else
    {
        iRet = iSize;
    }

nvram_setvalue:
    nvram_ReleaseSem();
    return  iRet;
}


UINT32 NVRAM_GetValue(INT32 hd, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff)
{
    NVRAM_INFO *psNInfo = NULL;
    INT32 iResult;
    INT32 iRet;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_GetValue: nvram uninitialized."));
        nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getvalue: 1(%d,0x%x,0x%x).", hd, iOffs, iSize);
        return 0;
    }

    nvram_WaitForSem();

    if( hd < 0  || hd >= NVRAM_OPEN_MAX)
    {
        D((DL_NVRAMERROR, "NVRAM_GetValue: Invalid hd. hd = %d.", hd));
        nvram_ErrCode(ERR_NVRAM_INVALID_HD);
        iRet = 0;
        goto nvram_getvalue;
    }

    if(!NVRAM_IS_VALID_ID(g_nvram_global_var.psNInfo[hd].iId))
    {
        D((DL_NVRAMERROR, "NVRAM_GetValue: Invalid hd. hd = %d.", hd));
        nvram_ErrCode(ERR_NVRAM_NOT_OPEN);
        iRet = 0;
        goto nvram_getvalue;
    }

    psNInfo = &g_nvram_global_var.psNInfo[hd];

    iResult = nvram_ReadNV(psNInfo, psNInfo->iId, iOffs, iSize, pBuff);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "NVRAM_GetValue: nvram read nv failed.,iId = 0x%x,err_code = %d.", psNInfo->iId, iResult));
        nvram_ErrCode(iResult);
        iRet = 0;
        goto nvram_getvalue;
    }
    else
    {
        iRet = iSize;
    }
nvram_getvalue:
    nvram_ReleaseSem();
    return iRet;
}


INT32 NVRAM_GetInfo(UINT32 *piTotalSize, UINT32 *piUsedSize)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    UINT32 iTotalSize = 0;
    UINT32 iUsedSize = 0;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_GetInfo: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "NVRAM_GetInfo: 1 uninitialized ret = %d .", iRet);
        return iRet;
    }

    iTotalSize = (NVRAM_R_COUNT * NVRAM_DATA_SIZE);
    iResult = nvram_GetUsedSize(&iUsedSize);
    if(ERR_SUCCESS == iResult)
    {
        *piTotalSize = iTotalSize;
        *piUsedSize = iUsedSize;
        iRet = ERR_SUCCESS;
    }
    else
    {
        *piTotalSize = 0;
        *piUsedSize = 0;
        D((DL_NVRAMERROR, "NVRAM_GetInfo: nvram_GetUsedSize falied. iResult = %d .", iResult));
        iRet = nvram_ErrCode(iResult);
    }
    //NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getinfo: 2 *piTotalSize = 0x%x,*piUsedSize = 0x%x, ret = %d .",*piTotalSize,*piUsedSize,iRet);
    return iRet;
}


UINT32 NVRAM_GetNESize(CONST UINT32 iId)
{
    UINT32 iNDN = NVRAM_INVALID_NDN;
    NVRAM_ND_INFO sNDInfo;
    INT32  iResult;
    UINT32 iSize;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_GetNESize: nvram uninitialized."));
        nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getnesize:(%d) ret = FALSE.", iId);
        return FALSE;
    }

    nvram_WaitForSem();
    // Look up NDN.
    iResult  = nvram_GetNDE(iId, &sNDInfo, &iNDN);
    if(NDN_IS_EXIST == iResult)
    {
        iSize = sNDInfo.iSize;
    }
    else
    {
        iSize = 0;
    }

    NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getnesize:(%d) size = %d.", iSize);
    nvram_ReleaseSem();
    return iSize;
}


INT32 NVRAM_GetEntries(NVRAM_ENTRY *psEntries, UINT32 *piCount)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    NVRAM_ND_INFO *psNd;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_GetInfo: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getentires: uninitialized, ret = %d .", iRet);
        return iRet;
    }

    psNd = (NVRAM_ND_INFO *)psEntries;

    iResult = nvram_GetEntries(psNd, piCount);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }
    else
    {
        iRet = nvram_ErrCode(iResult);
    }
    //NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getentires: psEntries = 0x%x,*piCount = 0x%x, ret = %d .",psEntries,*piCount,iRet);
    return iRet;
}


INT32 NVRAM_GetEntriesCount(UINT32 *piCount)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;

    if(0 == g_nvram_global_var.iHasInited)
    {
        D((DL_NVRAMERROR, "NVRAM_GetInfo: nvram uninitialized."));
        iRet = nvram_ErrCode(_ERR_NVRAM_UNINIT);
        NVRAM_TRACE(BASE_DSM_TS_ID, "nvram getentires: uninitialized, ret = %d .", iRet);
        return iRet;
    }



    iResult = nvram_GetEntriesCount(piCount);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }
    else
    {
        iRet = nvram_ErrCode(iResult);
    }
    return iRet;
}



