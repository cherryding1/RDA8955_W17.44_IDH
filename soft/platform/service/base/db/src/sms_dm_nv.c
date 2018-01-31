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
#include "error_id.h"
#include "sms_dm_cd.h"
#include "sms_dm_local.h"
#include "dsm_dbg.h"
#include "sul.h"
#include "sms_dm_nv.h"
#include "fs/sf_api.h"

#define SMS_DM_PATH "sms_dm_nv.bin"

SMS_DM_NV g_sms_dm_nv = {};
// Global Variable
UINT16  g_SMS_DM_InitFlag;                               // 是否初始化
COS_SEMA g_SMS_DM_Sem;                                      // 短信息数据库访问信号量


INT32 SMS_DM_GetFreeIndexFromNv(UINT16 *pIndex, CFW_SIM_ID nSimId)
{
    UINT16 iIndex;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iStartId = 0;
    UINT16 iEndId = 0;
    iRet = SMS_DM_GetRange(nSimId, &iStartId, &iEndId);
    if(ERR_SUCCESS != iRet)
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    for(iIndex = iStartId; iIndex < iEndId; iIndex++)
    {
        if(SMS_DM_STATUS_REC_FREE == g_sms_dm_nv.smsRec[iIndex].recStatus)
        {
            *pIndex = iIndex;
            return ERR_SUCCESS;
        }
    }

    return ERR_SMS_DM_SPACE_FULL;
}

static void SMS_DM_Writeback(void *param)
{
    void *data = COS_MALLOC(sizeof(SMS_DM_NV));
    SMS_DM_Down();
    memcpy(data, &g_sms_dm_nv, sizeof(SMS_DM_NV));
    SMS_DM_Up();

    sf_safefile_write(SMS_DM_PATH, data, sizeof(SMS_DM_NV));
    COS_FREE(data);
}

static INT32 SMS_DM_Update(VOID)
{
    COS_TaskCallbackNotif(sf_async_task_handle(), SMS_DM_Writeback, NULL);
    return 0;
}

/**************************************************************************************/
// Function: This function is initaliazed the sms database.
//           if the sector is unsigned.will been erasured the secors,signed the sectors,and
//           set the global variables,else, will been set the global variable.
// Parameter:
//    None.
// Return value: If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//    ERR_SMS_DM_NOT_ENOUGH_MEDIA_SPACE: check media space failed.
//    ERR_SMS_DM_MEDIA_WRITE_FAILED: write the medium failed.
//    ERR_SMS_DM_NOT_ENOUGH_MEMORY: alloc memory failed.
/***************************************************************************************/
INT32 SMS_DM_Init(VOID)
{
    BOOL bresult;
    INT32 result;
    const UINT8* sms_dm_path = SMS_DM_PATH;

    // Read NV data from file.
    bresult = sf_safefile_init(sms_dm_path);
    if(!bresult)
    {
        SUL_ZeroMemory8(&g_sms_dm_nv, sizeof(SMS_DM_NV));
        result = SMS_DM_Update();
        if(result != 0)
        {
            return -1;
        }
    }
    else
    {
        result = sf_safefile_read(sms_dm_path, (void*)&g_sms_dm_nv, sizeof(SMS_DM_NV));
        if(result != sizeof(SMS_DM_NV) || g_sms_dm_nv.version != SMS_DM_VERSION)
        {
            SUL_ZeroMemory8(&g_sms_dm_nv, sizeof(SMS_DM_NV));
            g_sms_dm_nv.version = SMS_DM_VERSION;
            result = SMS_DM_Update();
            if(result != 0)
            {
                return -1;
            }
        }
    }

    if(!COS_SemaInited(&g_SMS_DM_Sem))
    {
        SMS_DM_SemInit();
    }

    g_SMS_DM_InitFlag = SMS_INITIALIZED;
    return ERR_SUCCESS;
}


/**************************************************************************************/
// Function: This function is add a record to the sms database.
// Parameter:
// pIndex: [out]Output the index in the index table,it's range is from 0 to SMS_DM_MAX_REC_COUNT - 1.
// pData:  [in]The data of the record,the pData size equal to SMS_DM_SZ_RECORD_DATA.
// Return value:
// If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//     ERR_SMS_DM_UNINIT:Uninitialize the sms database.
//     ERR_SMS_DM_INVALID_PARAMETER: parameter is invalid.
//     ERR_SMS_DM_WRITEMEDIA_FAILED: write the medium failed.
//     ERR_SMS_DM_SPACE_FULL: the space is full,maximal record count is SMS_DM_MAX_REC_COUNT.
/***************************************************************************************/
INT32 SMS_DM_Add(UINT16 *pIndex, PVOID pData, CFW_SIM_ID nSimId)
{
    INT32 iRet = ERR_SUCCESS;
    UINT16 iIndex;
    UINT8 iDataStatus;

    if(g_SMS_DM_InitFlag != SMS_INITIALIZED)
    {
        return ERR_SMS_DM_UNINIT;
    }

    if((NULL == pIndex) || (NULL == pData))
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    iDataStatus = *((UINT8 *)pData);
    if (SMS_REC_IVALID_STATUS == iDataStatus)
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    SMS_DM_Down();
    iRet = SMS_DM_GetFreeIndexFromNv(&iIndex, nSimId);
    if (iRet != ERR_SUCCESS)
    {
        goto failed;
    }

    SUL_MemCopy8((UINT8*)g_sms_dm_nv.smsRec[iIndex].data, (UINT8*)pData, SMS_DM_SZ_RECORD_DATA);
    g_sms_dm_nv.smsRec[iIndex].recStatus = SMS_DM_STATUS_REC_VALID;
    SMS_DM_Update();
    *pIndex = iIndex;
    iRet = ERR_SUCCESS;

failed:
    SMS_DM_Up();
    return iRet;

}


/**************************************************************************************/
// Function: This function is read a record from the sms database.
// Parameter:
// iIndex: [in] Specified the index of the record,it's range is from 0 to SMS_DM_MAX_REC_COUNT - 1.
// pData:  [out]Output The data of the record,the pData size equal to SMS_DM_SZ_RECORD_DATA.
// Return value:
// If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//     ERR_SMS_DM_UNINIT:Uninitialize the sms database.
//     ERR_SMS_DM_INVALID_PARAMETER: parameter is invalid.
//     ERR_SMS_DM_MEDIA_READ_FAILED: read the medium failed.
//   ERR_SMS_DM_INVALID_INDEX: the index is ivalid.
/***************************************************************************************/
INT32 SMS_DM_Read(UINT16 iIndex, PVOID pData)
{
    UINT16 iStatus;
    INT32 iRet = ERR_SUCCESS;

    if(g_SMS_DM_InitFlag != SMS_INITIALIZED)
    {
        return ERR_SMS_DM_UNINIT;
    }

    if (iIndex >= SMS_DM_MAX_REC_COUNT || NULL == pData)
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    SMS_DM_Down();
    iStatus = g_sms_dm_nv.smsRec[iIndex].recStatus;
    if(iStatus == SMS_DM_STATUS_REC_FREE)
    {
        iRet = ERR_SMS_DM_INVALID_INDEX;
        goto failed;
    }
    else if(iStatus != SMS_DM_STATUS_REC_VALID)
    {
        iRet = ERR_SMS_DM_ERROR;
        goto failed;
    }

    SUL_MemCopy8((UINT8*)pData, (UINT8*)g_sms_dm_nv.smsRec[iIndex].data, SMS_DM_SZ_RECORD_DATA);
failed:
    SMS_DM_Up();
    return iRet;

}


/**************************************************************************************/
// Function: This function is write a specified index record to the sms database.
//           if the specified index record is invalid.will been return ERR_SMS_DM_INVALID_INDEX.
// Parameter:
// iIndex: [in] Specified the index of the record,it's range is from 0 to SMS_DM_MAX_REC_COUNT - 1.
// pData:  [in] The data of the record,the pData size equal to SMS_REC_DATA_SIZE.
// Return value: If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//    ERR_SMS_DM_UNINIT:Uninitialize the sms database.
//    ERR_SMS_DM_INVALID_PARAMETER: parameter is invalid.
//    ERR_SMS_DM_MEDIA_WRITE_FAILED: write the medium failed.
//    ERR_SMS_DM_INVALID_INDEX: the index is invalid.
/***************************************************************************************/
INT32 SMS_DM_OverWrite(UINT16 iIndex, PVOID pData)
{
    UINT16 iStatus;
    INT32 iRet = ERR_SUCCESS;

    if(g_SMS_DM_InitFlag != SMS_INITIALIZED)
    {
        return ERR_SMS_DM_UNINIT;
    }

    if (iIndex >= SMS_DM_MAX_REC_COUNT || NULL == pData)
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    SMS_DM_Down();
    iStatus = g_sms_dm_nv.smsRec[iIndex].recStatus;
    if(iStatus == SMS_DM_STATUS_REC_FREE)
    {
        iRet = ERR_SMS_DM_INVALID_INDEX;
        goto failed;
    }
    else if(iStatus != SMS_DM_STATUS_REC_VALID)
    {
        iRet = ERR_SMS_DM_ERROR;
        goto failed;
    }

    SUL_MemCopy8((UINT8*)g_sms_dm_nv.smsRec[iIndex].data, (UINT8*)pData, SMS_DM_SZ_RECORD_DATA);
    SMS_DM_Update();
failed:
    SMS_DM_Up();
    return iRet;
}


/**************************************************************************************/
// Function: This function is delete a specified index record from the sms database.
//           if the specified index record is invalid.will been return ERR_SMS_DM_INVALID_INDEX.
// Parameter:
// iIndex: [in] Specified the index of the record,it's range is from 0 to SMS_DM_MAX_REC_COUNT - 1.
// Return value: If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//    ERR_SMS_DM_UNINIT:Uninitialize the sms database.
//    ERR_SMS_DM_INVALID_PARAMETER: parameter is invalid.
//    ERR_SMS_DM_MEDIA_WRITE_FAILED: write the medium failed.
//    ERR_SMS_DM_INVALID_INDEX: the index is ivalid.
/***************************************************************************************/
INT32 SMS_DM_Delete(UINT16 iIndex)
{
    UINT16 iStatus;
    INT32 iRet = ERR_SUCCESS;

    if(g_SMS_DM_InitFlag != SMS_INITIALIZED)
    {
        return ERR_SMS_DM_UNINIT;
    }

    if (iIndex >= SMS_DM_MAX_REC_COUNT)
    {
        return ERR_SMS_DM_INVALID_PARAMETER;
    }

    SMS_DM_Down();
    iStatus = g_sms_dm_nv.smsRec[iIndex].recStatus;
    if(iStatus == SMS_DM_STATUS_REC_FREE)
    {
        iRet = ERR_SMS_DM_INVALID_INDEX;
        goto failed;
    }
    else if(iStatus != SMS_DM_STATUS_REC_VALID)
    {
        iRet = ERR_SMS_DM_ERROR;
        goto failed;
    }

    g_sms_dm_nv.smsRec[iIndex].recStatus = SMS_DM_STATUS_REC_FREE;
    SMS_DM_Update();
failed:
    SMS_DM_Up();
    return iRet;

}


/**************************************************************************************/
// Function: This function is rebuied the sms database.
// Parameter:
//    None.
// Return value: If the function succeeds, the return value is ERR_SUCCESS.
// If the function fails, the return value as follows:
//    ERR_SMS_DM_NOT_ENOUGH_MEDIA_SPACE: check media space failed.
//    ERR_SMS_DM_MEDIA_WRITE_FAILED: write the medium failed.
/***************************************************************************************/
INT32 SMS_DM_Clear(VOID)
{
    INT32 iRet = ERR_SUCCESS;

    if (SMS_INITIALIZED == g_SMS_DM_InitFlag)
    {
        g_SMS_DM_InitFlag = SMS_UNINIT;
    }

    SUL_ZeroMemory8(&g_sms_dm_nv, sizeof(SMS_DM_NV));
    iRet = SMS_DM_Update();
    if(iRet != 0)
    {
        return -1;
    }

    if(!COS_SemaInited(&g_SMS_DM_Sem))
    {
        SMS_DM_SemInit();
    }

    g_SMS_DM_InitFlag = SMS_INITIALIZED;
    return ERR_SUCCESS;

}


/**************************************************************************************/
// Function: This function is list the record of specified status from the sms database.
// Parameter:
// pIndex: [out] output the list of index.
// iStatus: [in]Specified the status for the list of record.
// Return value: return the count of the index list.
/***************************************************************************************/
UINT16 SMS_DM_GetIndexByStatus(UINT16 *pIndex, UINT8 iStatus, CFW_SIM_ID nSimId)
{
    UINT16 i = 0;
    UINT16 iRecordIndex;
    UINT16 iRecordStatus;
    UINT8   iDataStatus;
    INT32   iRet = ERR_SUCCESS;
    UINT16 nStartIndex = 0,  nEndIndex = 0;
    if((NULL == pIndex) || (SMS_REC_IVALID_STATUS == iStatus))
    {
        return 0;
    }

    if(g_SMS_DM_InitFlag != SMS_INITIALIZED)
    {
        return 0;
    }

    SMS_DM_Down();
    iRet = SMS_DM_GetRange(nSimId, &nStartIndex, &nEndIndex);
    if(ERR_SUCCESS != iRet)
    {
        return CFW_SMS_MAX_REC_COUNT;
    }

    for(iRecordIndex = nStartIndex; iRecordIndex < nEndIndex; iRecordIndex++)
    {
        iRecordStatus = g_sms_dm_nv.smsRec[iRecordIndex].recStatus;
        if (iRecordStatus != SMS_DM_STATUS_REC_VALID)
        {
            continue;
        }

        iDataStatus = g_sms_dm_nv.smsRec[iRecordIndex].data[0];
        if(0 != (iStatus & iDataStatus))
        {
            *(pIndex + i) = iRecordIndex;
            i ++;
        }
    }

    SMS_DM_Up();
    return i;
}


/**************************************************************************************/
// Function: This function get the  version of sms_dm module.
// Parameter:
// None.
// Return value: return the version of sms_dm module..
/***************************************************************************************/
UINT32 SMS_DM_GetVersion(VOID)
{
    return SMS_DM_VERSION;
}

UINT32 SMS_DM_GetUsedSize(VOID)
{
    return SMS_DM_SZ_MEDIA_USE;
}
