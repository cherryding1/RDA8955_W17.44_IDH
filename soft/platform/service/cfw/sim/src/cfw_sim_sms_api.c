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


#include <cswtype.h>
#include <errorcode.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include <csw_mem_prv.h>
#include "csw_debug.h"

#if (CFW_SIM_DUMP_ENABLE==0) && (CFW_SIM_SRV_ENABLE==1)

#include "cfw_sim_sms.h"

extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;

UINT32 CFW_SimPatch(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPatch);

    // UINT32 nState = CFW_SM_STATE_IDLE;
    SIM_INFO_READSMS *pReadSMSInfo = NULL;



    pReadSMSInfo = (SIM_INFO_READSMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READSMS));
    if (pReadSMSInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100e9e));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPatch);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pReadSMSInfo, SIZEOF(SIM_INFO_READSMS));

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadSMSInfo, CFW_SimPatchProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pReadSMSInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pReadSMSInfo->nCurrentFile = API_USIM_EF_SMS;

    pReadSMSInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->nTryCount    = 0x01;
    pReadSMSInfo->nIndex       = (UINT16)1;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPatch);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimPatchEXProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimPatchEX(UINT16 nFileID, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPatch);

    // UINT32 nState = CFW_SM_STATE_IDLE;
    SIM_INFO_READSMS *pReadSMSInfo = NULL;



    pReadSMSInfo = (SIM_INFO_READSMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READSMS));
    if (pReadSMSInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100e9f));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPatch);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pReadSMSInfo, SIZEOF(SIM_INFO_READSMS));

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadSMSInfo, CFW_SimPatchEXProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pReadSMSInfo->nCurrentFile = nFileID;
    pReadSMSInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->nTryCount    = 0x01;
    pReadSMSInfo->nIndex       = (UINT16)1;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPatch);
    return (ERR_SUCCESS);
}


UINT32 CFW_SimReadMessage(UINT16 nLocation, UINT32 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadMessage);

    // UINT32 nState = CFW_SM_STATE_IDLE;
    SIM_INFO_READSMS *pReadSMSInfo = NULL;

    if ((nLocation != CFW_SMS_STORAGE_SM) || (0 > nIndex) || (256 < nIndex))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invlaid parameter nLocation 0x%x nIndex 0x%x\n",0x08100ea0), nLocation, nIndex);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadMessage);
        return ERR_INVALID_PARAMETER; // Not support the other type
    }

    pReadSMSInfo = (SIM_INFO_READSMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READSMS));
    if (pReadSMSInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ea1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadMessage);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pReadSMSInfo, SIZEOF(SIM_INFO_READSMS));

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadSMSInfo, CFW_SimReadMessageProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pReadSMSInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pReadSMSInfo->nCurrentFile = API_USIM_EF_SMS;

    pReadSMSInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pReadSMSInfo->nTryCount    = 0x01;
    pReadSMSInfo->nIndex       = (UINT16)nIndex;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadMessage);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimWriteMessage(UINT16 nLocation,
                           UINT32 nIndex, UINT8 *pData, UINT8 nDataSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    //
    // In current version, the value of nDataSize should be filled with 176;
    // So the buffer length of the pDate is 176 bytes;
    //
    // If the value of nIndex is zero, the function will search a blank record
    // to fill the date;
    // else the function will fill the date into the record with the index of
    // nIndex;
    //
    HAO hAo;

    SIM_INFO_WRITESMS *pWriteSMSInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimWriteMessage);
    switch (pData[0])
    {
        case 0x01:
            pData[0] = 0x03;
            break;
        case 0x02:
            pData[0] = 0x01;
            break;
        case 0x04:
            pData[0] = 0x07;
            break;
        case 0x08:
            pData[0] = 0x05;
            break;
        case 0x10:
            pData[0] = 0x0D;
            break;
        case 0x20:
            pData[0] = 0x15;
            break;
        case 0x40:
            pData[0] = 0x1D;
            break;
        case 0x80:
            pData[0] = 0xA0;
            break;
        default:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invlaid parameter pData[0] 0x%x\n",0x08100ea2), pData[0]);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimWriteMessage);
            return ERR_INVALID_PARAMETER;
        }
    }
    if ((nLocation != CFW_SMS_STORAGE_SM) || (0 > nIndex) || (nIndex > 255))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invlaid parameter nLocation 0x%x nIndex 0x%x\n",0x08100ea3), nLocation, nIndex);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWriteMessage);
        return ERR_INVALID_PARAMETER;
    }
    if (nDataSize != SMS_PDU_LEN)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invlaid parameter nDataSize 0x%x\n",0x08100ea4), nDataSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWriteMessage);
        return ERR_INVALID_PARAMETER;
    }
    pWriteSMSInfo = (SIM_INFO_WRITESMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_WRITESMS));
    if (pWriteSMSInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ea5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWriteMessage);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pWriteSMSInfo, SIZEOF(SIM_INFO_WRITESMS));

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pWriteSMSInfo, CFW_SimWriteMessageProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pWriteSMSInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pWriteSMSInfo->nCurrentFile = API_USIM_EF_SMS;

    //
    // Using for search whether the SIM is FULL.
    //
    pWriteSMSInfo->nFlag = 0x00;

    SUL_MemCopy8(pWriteSMSInfo->pData, pData, SMS_PDU_LEN);

    if (nIndex == 0)
    {
        memset(pWriteSMSInfo->SeekPattern, 0xFF, SMS_PDU_LEN);
        pWriteSMSInfo->SeekPattern[0] = 0x00;
        pWriteSMSInfo->nReserveIndex  = 0;
    }
    else
    {
        pWriteSMSInfo->nIndex        = (UINT8)(nIndex);
        pWriteSMSInfo->nReserveIndex = (UINT8)(nIndex);
    }

    pWriteSMSInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pWriteSMSInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pWriteSMSInfo->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimWriteMessage);
    return (ERR_SUCCESS);

}

UINT32 CFW_SimListMessage(UINT16 nLocation, UINT8 nStatus,  // The status of message in SIM,Just as below
                          UINT16 nCount,  // Specify the counter message to list
                          UINT16 nStartIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_LISTSMS *pListSmsInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimListMessage);
    if (nLocation != CFW_SMS_STORAGE_SM)
    {
        //
        // Not support the other type
        //
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invlaid parameter nLocation 0x%x\n",0x08100ea6), nLocation);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListMessage);
        return ERR_INVALID_PARAMETER;
    }

    pListSmsInfo = (SIM_INFO_LISTSMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_LISTSMS));
    if (pListSmsInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ea7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListMessage);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pListSmsInfo, CFW_SimListMessageProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pListSmsInfo, SIZEOF(SIM_INFO_LISTSMS));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pListSmsInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pListSmsInfo->nCurrentFile = API_USIM_EF_SMS;

    pListSmsInfo->nDataSize      = 176;
    pListSmsInfo->SeekPattern[0] = nStatus;
    pListSmsInfo->nStatus        = nStatus;
    pListSmsInfo->nStartIndex    = nStartIndex;
    pListSmsInfo->nCurrCount     = 0;
    pListSmsInfo->nCount         = nCount;
    pListSmsInfo->nLocation      = nLocation;
    pListSmsInfo->nCurrIndex     = (UINT8)nStartIndex;

    pListSmsInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pListSmsInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pListSmsInfo->nTryCount    = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    // CFW_ChangeAoState(hAo);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimListMessage);
    return (ERR_SUCCESS);

}

UINT32 CFW_SimDeleteMessage(UINT16 nLocation, UINT16 nIndex, UINT8 nStatus, // The status of message in SIM,Just as below
                            UINT16 nUTI,  // exclusive UTI number
                            CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_DELESMS *pDeleSmsInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimDeleteMessage);
    if ((nLocation != CFW_SMS_STORAGE_SM) || (nIndex > 0x100))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invlaid parameter nLocation 0x%x nIndex 0x%x\n",0x08100ea8), nLocation, nIndex);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeleteMessage);
        return ERR_INVALID_PARAMETER; // Not support the other type
    }

    pDeleSmsInfo = (SIM_INFO_DELESMS *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_DELESMS));
    if (pDeleSmsInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ea9));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeleteMessage);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pDeleSmsInfo, CFW_SimDeleMessageProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pDeleSmsInfo, SIZEOF(SIM_INFO_DELESMS));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pDeleSmsInfo->nStatus      = nStatus;
    pDeleSmsInfo->nIndex       = nIndex;
    pDeleSmsInfo->nLocation    = nLocation;
    pDeleSmsInfo->nCurrInd     = 1;

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pDeleSmsInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pDeleSmsInfo->nCurrentFile = API_USIM_EF_SMS;

    pDeleSmsInfo->pattern[0]   = 0x00;
    SUL_MemSet8(pDeleSmsInfo->pData, 0xff, 176);
    pDeleSmsInfo->pData[0] = 0x00;

    pDeleSmsInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pDeleSmsInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pDeleSmsInfo->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeleteMessage);
    return ERR_SUCCESS;
}

UINT32 CFW_SimReadBinary(UINT8 nFileId, UINT8 nOffset, UINT8 nBytesToRead, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_READBINARY *pReadBinary = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadBinary);

    if(0xFF == nFileId)
    {
        return ERR_FILE_NOT_FOUND;
    }

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadBinary);
        return Ret;
    }
    pReadBinary = (SIM_INFO_READBINARY *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READBINARY));
    if (pReadBinary == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eaa));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadBinary);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadBinary, CFW_SimReadBinaryProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pReadBinary, SIZEOF(SIM_INFO_READBINARY));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pReadBinary->nFileId      = nFileId;
    pReadBinary->nOffset      = nOffset;
    pReadBinary->nBytesToRead = nBytesToRead;

    pReadBinary->n_CurrStatus = SMS_STATUS_IDLE;
    pReadBinary->n_PrevStatus = SMS_STATUS_IDLE;
    pReadBinary->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadBinary);
    return (ERR_SUCCESS);

}

UINT32 CFW_SimUpdateBinary(UINT8 nFileId,
                           UINT8 nOffset, UINT8 *pData, UINT8 nBytesToWrite, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_UPDATEBINARY *pUpdateBinary = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimUpdateBinary);

    if(0xFF == nFileId)
    {
        return ERR_FILE_NOT_FOUND;
    }


    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateBinary);
        return Ret;
    }
    pUpdateBinary = (SIM_INFO_UPDATEBINARY *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_UPDATEBINARY));
    if (pUpdateBinary == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eab));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateBinary);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pUpdateBinary, CFW_SimUpdateBinaryProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);
    SUL_ZeroMemory32(pUpdateBinary, SIZEOF(SIM_INFO_UPDATEBINARY));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pUpdateBinary->nFileId       = nFileId;
    pUpdateBinary->nOffset       = nOffset;
    pUpdateBinary->nBytesToWrite = nBytesToWrite;

    SUL_MemCopy8(pUpdateBinary->pDate, pData, nBytesToWrite);

    pUpdateBinary->n_CurrStatus = SMS_STATUS_IDLE;
    pUpdateBinary->n_PrevStatus = SMS_STATUS_IDLE;
    pUpdateBinary->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateBinary);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetMR(UINT8 nMR, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_SET_MR *pSetMR = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetMR);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMR);
        return Ret;
    }
    pSetMR = (SIM_INFO_SET_MR *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_SET_MR));
    if (pSetMR == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eac));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMR);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetMR, CFW_SimSetMRProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pSetMR, SIZEOF(SIM_INFO_SET_MR));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pSetMR->nFileId = API_SIM_EF_SMSS;
    else
        pSetMR->nFileId = API_USIM_EF_SMSS;

    pSetMR->nMR     = nMR;

    pSetMR->n_CurrStatus = SMS_STATUS_IDLE;
    pSetMR->n_PrevStatus = SMS_STATUS_IDLE;
    pSetMR->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMR);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetMR(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_GET_MR *pGetMR = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetMR);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetMR);
        return Ret;
    }
    pGetMR = (SIM_INFO_GET_MR *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_GET_MR));
    if (pGetMR == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ead));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetMR);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetMR, CFW_SimGetMRProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pGetMR, SIZEOF(SIM_INFO_GET_MR));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pGetMR->nFileId = API_SIM_EF_SMSS;
    else
        pGetMR->nFileId = API_USIM_EF_SMSS;

    pGetMR->n_CurrStatus = SMS_STATUS_IDLE;
    pGetMR->n_PrevStatus = SMS_STATUS_IDLE;
    pGetMR->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetMR);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetSmsTotalNum(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimGetSmsTotalNum nSimID[%d]\n",0x08100eae), nSimID);
    SIM_INFO_GETSMSINFO *pGetSmsInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetSmsTotalNum);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsTotalNum);
        return Ret;
    }
    pGetSmsInfo = (SIM_INFO_GETSMSINFO *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_GETSMSINFO));
    if (pGetSmsInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eaf));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsStorageInfo);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetSmsInfo, CFW_SimGetSmsTotalNumProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);
    SUL_ZeroMemory32(pGetSmsInfo, SIZEOF(SIM_INFO_GETSMSINFO));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pGetSmsInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pGetSmsInfo->nCurrentFile = API_USIM_EF_SMS;

    pGetSmsInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pGetSmsInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pGetSmsInfo->nTryCount    = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsTotalNum);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetSmsStorageInfo(UINT8 nStatus,  // The status of message in SIM,Just as below
                                UINT16 nUTI, CFW_SIM_ID nSimID)
{
    //
    // If nStatus is 0x80,the result will be the unused number.
    //
    HAO hAo;

    SIM_INFO_GETSMSINFO *pGetSmsInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetSmsStorageInfo);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsStorageInfo);
        return Ret;
    }
    pGetSmsInfo = (SIM_INFO_GETSMSINFO *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_GETSMSINFO));
    if (pGetSmsInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eb0));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsStorageInfo);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetSmsInfo, CFW_SimGetSmsStorageInfoProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);
    SUL_ZeroMemory32(pGetSmsInfo, SIZEOF(SIM_INFO_GETSMSINFO));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pGetSmsInfo->nCurrentFile = API_SIM_EF_SMS;
    else
        pGetSmsInfo->nCurrentFile = API_USIM_EF_SMS;

    switch (nStatus)
    {
        case 0x01:
            nStatus = 0x03;
            break;
        case 0x02:
            nStatus = 0x01;
            break;
        case 0x04:
            nStatus = 0x07;
            break;
        case 0x08:
            nStatus = 0x05;
            break;
        case 0x10:
            nStatus = 0x0D;
            break;
        case 0x20:
            nStatus = 0x15;
            break;
        case 0x40:
            nStatus = 0x1D;
            break;
        case 0x80:
            nStatus = 0xA0;
            break;
        default:
            ;
    }

    pGetSmsInfo->nStatus     = nStatus;
    pGetSmsInfo->nSpecialNum = 0;
    pGetSmsInfo->nCurrInd    = 1;

    pGetSmsInfo->n_CurrStatus = SMS_STATUS_IDLE;
    pGetSmsInfo->n_PrevStatus = SMS_STATUS_IDLE;
    pGetSmsInfo->nTryCount    = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsStorageInfo);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetSmsParameters(UINT8 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    //
    // If nIndex is 0x00,funciton will get all the smsp;
    // If nIndex is 0xff,funciton will get the number smsp;
    // Else the function will get the special smsp;
    //
    HAO hAo;

    SIM_INFO_GetSMSP *pGetSmsParam = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetSmsParameters);

    pGetSmsParam = (SIM_INFO_GetSMSP *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_GetSMSP));
    if (pGetSmsParam == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eb1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsParameters);
        return ERR_NO_MORE_MEMORY;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetSmsParam, CFW_SimGetSmsParametersProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pGetSmsParam, SIZEOF(SIM_INFO_GetSMSP));

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pGetSmsParam->nFileId = API_SIM_EF_SMSP;
    else
        pGetSmsParam->nFileId = API_USIM_EF_SMSP;

    pGetSmsParam->nTotalRecordNumber = 0;
    pGetSmsParam->nCurrRecordNumber  = 1;

    pGetSmsParam->nIndex       = nIndex;
    pGetSmsParam->n_CurrStatus = SMS_STATUS_IDLE;
    pGetSmsParam->n_PrevStatus = SMS_STATUS_IDLE;
    pGetSmsParam->nTryCount    = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetSmsParameters);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetSmsParameters(CFW_SIM_SMS_PARAMETERS *pSmsParamArray,
                               UINT8 nArrayCount, UINT8 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;

    SIM_INFO_SETSMSP *pSetSmsParam = NULL;
    UINT8 Tmp = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetSmsParameters);
    //hal_HstSendEvent(0xfa090306);
    if (nIndex == 0x00)
    {
        //
        // Set the parameters in batch
        //
        pSetSmsParam =
            (SIM_INFO_SETSMSP *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_SETSMSP) + (nArrayCount - 1) * SIZEOF(CFW_SIM_SMS_PARAMETERS));
        if (pSetSmsParam == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eb2));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetSmsParameters);
            return ERR_NO_MORE_MEMORY;
        }
        SUL_ZeroMemory32(pSetSmsParam, (SIZEOF(SIM_INFO_SETSMSP) + (nArrayCount - 1) * SIZEOF(CFW_SIM_SMS_PARAMETERS)));
    }
    else
    {
        pSetSmsParam = (SIM_INFO_SETSMSP *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_SETSMSP));
        if (pSetSmsParam == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eb3));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetSmsParameters);
            return ERR_NO_MORE_MEMORY;
        }
        SUL_ZeroMemory32(pSetSmsParam, SIZEOF(SIM_INFO_SETSMSP));
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetSmsParam, CFW_SimSetSmsParametersProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
        pSetSmsParam->nFileId            = API_SIM_EF_SMSP;
    else
        pSetSmsParam->nFileId            = API_USIM_EF_SMSP;

    pSetSmsParam->nIndex            = nIndex;
    pSetSmsParam->nArrayCount       = nArrayCount;
    pSetSmsParam->nCurrRecordNumber = 1;

    if (nIndex == 0x00)
    {
        //
        // Set the parameters in batch
        //
        while ((Tmp < nArrayCount))
        {
            SUL_MemCopy8(&(pSetSmsParam->Node[Tmp]), &pSmsParamArray[Tmp], SIZEOF(CFW_SIM_SMS_PARAMETERS));
            Tmp++;
        }
    }
    else
    {
        //
        // Set the special record parameter
        //
        SUL_MemCopy8(&(pSetSmsParam->Node[0]), &pSmsParamArray[0], SIZEOF(CFW_SIM_SMS_PARAMETERS));
    }

    pSetSmsParam->n_CurrStatus = SMS_STATUS_IDLE;
    pSetSmsParam->n_PrevStatus = SMS_STATUS_IDLE;
    pSetSmsParam->nTryCount    = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetSmsParameters);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimSmsInitProc(HAO hAO, CFW_EV *pEvent);

UINT32 CFW_SimSmsInit(CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Initing CFW_SimSmsInit\n",0x08100eb4));

    HAO hMyAO;
    UINT16 nUTI = 0x0;

    SIM_SMS_INIT_INFO *pUserData = NULL;
    UINT32 RET = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSmsInit);

    // SMS_Disp_Debug_Info(33, TSTXT("Start CFW_SIMSmsInit,SMS_Disp_GetTime : %d Sec.\n"),(UINT32)(SMS_Disp_GetTime()/256));
    nUTI = (CFW_APP_UTI_MIN + CFW_APP_UTI_MAX) / 2 + nSimID;

    pUserData = (SIM_SMS_INIT_INFO *)CSW_SIM_MALLOC(SIZEOF(SIM_SMS_INIT_INFO));
    if (pUserData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100eb5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSmsInit);
        return (ERR_NO_MORE_MEMORY);
    }
    SUL_ZeroMemory8(pUserData, SIZEOF(SIM_SMS_INIT_INFO));

    pUserData->nCurrentIndex  = 0x01;
    pUserData->usedSlot       = 0x00;
    pUserData->totalSlot      = 0x00;
    pUserData->readRecords    = 0x00;
    pUserData->unReadRecords  = 0x00;
    pUserData->sentRecords    = 0x00;
    pUserData->unsentRecords  = 0x00;
    pUserData->unknownRecords = 0x00;
    pUserData->storageId      = 0x00;
    pUserData->nLongStruCount = 0x00;
    pUserData->pLongInfo      = NULL;

    hMyAO = CFW_RegisterAo(CFW_SIM_SRV_ID, pUserData, CFW_SimSmsInitProc, nSimID);
    RET = CFW_SetUTI(hMyAO, nUTI, 0);
    if (RET != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SetUTI return 0x%x \n",0x08100eb6), RET);
        CSW_SIM_FREE(pUserData);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSmsInit);
        return (RET);
    }
    RET = CFW_SetAoProcCode(hMyAO, CFW_AO_PROC_CODE_CONTINUE);
    if (RET != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SetAoProcCode return 0x%x \n",0x08100eb7), RET);
        CSW_SIM_FREE(pUserData);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSmsInit);
        return (RET);
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSmsInit);
    return ERR_SUCCESS;
}

//
// Decompose the PDU both Submit and Deliver.
//
UINT32 CFW_DecomposePDU(PVOID pInData,  // 176 length PDU date
                        UINT8 nType, UINT32 *pOutData, UINT8 *pSMSStatus, // SMS Status just as below
                        CFW_SMS_MULTIPART_INFO *pLongerMsg)
{
    // ***************************************************************
    // Change list:
    // 为了满足ME上存储的需求，既将submit短消息保存为deliver
    // 类型，解析PDU的时候，完全按照用户指定参数nType来
    // 实现，而不在根据MTI来判断。
    //
    // ***************************************************************
    UINT8 tmp, * pbuf, nMTI, nScaIndex = 0x00;

    //
    // Output the pSMSStatus.
    //
    CSW_PROFILE_FUNCTION_ENTER(CFW_DecomposePDU);

    pbuf = (UINT8 *)pInData;

    // switch(pbuf[0])
    switch (pbuf[0] & 0x1F)
    {
        case 0x03:
            *pSMSStatus = 0x01;
            break;
        case 0x01:
            *pSMSStatus = 0x02;
            break;
        case 0x07:
            *pSMSStatus = 0x04;
            break;
        case 0x05:
            *pSMSStatus = 0x08;
            break;
        case 0x0D:
            *pSMSStatus = 0x10;
            break;
        case 0x15:
            *pSMSStatus = 0x20;
            break;
        case 0x1D:
            *pSMSStatus = 0x40;
            break;
        case 0xA0:
            *pSMSStatus = 0x80;
            break;
        default:
            *pSMSStatus = 0x00;
    }
    if (nType == 0)
    {
        //
        // Only get the SMS status
        //
        pOutData = NULL;
        CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
        return (ERR_SUCCESS);
    }

    // if(pLongerMsg == NULL)
    // {
    // return(ERR_INVALID_PARAMETER);
    // }
    // else
    if (pLongerMsg)
    {
        pLongerMsg->count   = 0;
        pLongerMsg->current = 0;
        pLongerMsg->id      = 0;
    }

    //
    // Necessary?
    //
    if (pOutData == NULL)
    {
        ; // return 1;
    }

    //
    // Get MTI value
    //
    if (pbuf[1] == 0x00)
    {
        tmp  = 2 + 1;
        nMTI = pbuf[tmp];
    }
#if g_SMS_ValueCheck
    else if (pbuf[1] > TEL_NUMBER_MAX_LEN)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error pbuf[1] Len 0x%x \n",0x08100eb8), pbuf[1]);
        return ERR_INVALID_PARAMETER;
    }
#endif
    else
    {
        nScaIndex = 0;
        tmp       = (UINT8)(2 + pbuf[1]);
        nMTI      = pbuf[tmp];
    }
    if (nMTI & 0x02)  // Don't support Commond &status Report
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error SMS is a Command or StatusReport\n",0x08100eb9));
        CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
        return ERR_INVALID_PARAMETER;
    }

    if (nMTI & 0x01)  // For Submit, there will be not read or unread statue
    {
        if ((*pSMSStatus == 0x01) || (*pSMSStatus == 0x02))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Submit status read or unread\n",0x08100eba));

            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
            return ERR_INVALID_PARAMETER;
        }

    }
    else if (nMTI & 0x00) // Fpr DELIVER,  there will be not send or unsend statue
    {
        if ((*pSMSStatus == 0x04) || (*pSMSStatus == 0x08) || (*pSMSStatus == 0x10) ||
                (*pSMSStatus == 0x20) || (*pSMSStatus == 0x40))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Submit status send or unsend\n",0x08100ebb));
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
            return ERR_INVALID_PARAMETER;
        }

    }

    if (nType == 0x01 || nType == 0x02)
    {
        //
        // DELIVER message See 3GPP TP 23040-390 9.2.3.1
        //
        UINT8 oa[TEL_NUMBER_MAX_LEN];
        UINT8 sca[TEL_NUMBER_MAX_LEN];
        UINT8 data[160];
        UINT8 nTime[14];
        UINT8 oa_size  = 0;
        UINT8 sca_size = 0;

        // TM_SMS_TIME_STAMP scts;
        UINT8 tooa  = 0x0;
        UINT8 tosca = 0x0;

        // UINT8 stat;
        // UINT8 fo;
        UINT8 pid     = 0x0;
        UINT8 dcs     = 0x0;
        UINT16 length = 0x0;
        UINT8 nTmp    = 0x0;

        SUL_ZeroMemory8(data, 160);
        SUL_ZeroMemory8(nTime, 14);
        SUL_ZeroMemory8(oa, TEL_NUMBER_MAX_LEN);
        SUL_ZeroMemory8(sca, TEL_NUMBER_MAX_LEN);
#if g_SMS_ValueCheck
        if (pbuf[1] > TEL_NUMBER_MAX_LEN)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error pbuf[1] Len 0x%x \n",0x08100ebc), pbuf[1]);
            return ERR_INVALID_PARAMETER;
        }
#endif
        if (pbuf[1] != 0x00)
        {
            //
            // Get the SCA Address
            //
            // SUL_GSMBCDToAscii(&(pbuf[3]),sca,(UINT8)(pbuf[1] - 1));
            SUL_MemCopy8(sca, &(pbuf[nScaIndex + 3]), (pbuf[1] - 1));
            sca_size = (UINT8)(pbuf[1] - 1);

            //
            // Get the type of SMS Center Address
            //
            tosca = pbuf[2];
            tmp   = (UINT8)(pbuf[1] + 3);
        }
        else
        {
            tmp = (UINT8)(pbuf[1] + 4);
        }

        //
        // Get the Originating telephone number
        //
#if g_SMS_ValueCheck
        if (pbuf[tmp] > 2 * TEL_NUMBER_MAX_LEN)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Originating telephone number Len 0x%x \n",0x08100ebd), pbuf[tmp]);
            return ERR_INVALID_PARAMETER;
        }
#endif
        nTmp = pbuf[tmp];

        if (nTmp != 0x00)
        {
            if (nTmp & 1)
                nTmp += 1;  //

            nTmp = (UINT8)(nTmp / 2);
            if(nTmp > TEL_NUMBER_MAX_LEN)
                nTmp = TEL_NUMBER_MAX_LEN;

            // SUL_GSMBCDToAscii(&(pbuf[tmp + 2]),oa,nTmp);
            SUL_MemCopy8(oa, &(pbuf[tmp + 2]), nTmp);
            oa_size = nTmp;

            //
            // Get the type of the Originating telephone number
            //
            tooa = pbuf[tmp + 1];
            tmp  = (UINT8)(tmp + 2 + nTmp);
        }
        else
        {
            tooa = pbuf[tmp + 1];
            tmp  = (UINT8)(tmp + 2);
        }

        //
        // Get the PID
        //
        pid = pbuf[tmp];

        //
        // Get the DCS
        //
        dcs = pbuf[tmp + 1];

        //
        // Get the SCTS
        //
        SUL_GsmBcdToAscii(&(pbuf[tmp + 2]), 6, nTime);

        INT8 tmpZone = 0;

        tmpZone = (pbuf[tmp + 8] & 0x07) * 10 + (pbuf[tmp + 8] >> 4);
        tmpZone = tmpZone > ((tmpZone / 4) * 4) ? (tmpZone / 4 + 1) : (tmpZone / 4);
        if ((pbuf[tmp + 8] & 0x0f) >= 0x80) // decide the "-" / "+"
        {
            tmpZone = 0 - tmpZone;
        }

        // SUL_GSMBCDToAscii(&(pbuf[tmp + 2]),,7);

        // SUL_MemCopy8(oa,&(pbuf[tmp + 8 + nTmp]),7);

        //
        // Get the User date
        //

        // *********************************************************************
        // TP UDHI bit6  : x0xx xxxx: TP UDHI, for loner SMS
        // *********************************************************************
        if ((nMTI & 0x40) == 0x00)
        {
            //
            // User date里面不包含头信息
            //
            if (dcs == GSM_7BIT)
            {
                //
                // 7-bit解码
                //
                UINT16 nSrcLength = 0x0;
                nSrcLength = (UINT16)(pbuf[tmp + 9] & 7 ? pbuf[tmp + 9] * 7 / 8 + 1 : pbuf[tmp + 9] * 7 / 8);

                length = (UINT16)SUL_Decode7Bit(&(pbuf[tmp + 10]), data, nSrcLength); // 转换到TP-DU
            }
            else if (dcs == GSM_UCS2)
            {
                //
                // UCS2解码
                //
                SUL_MemCopy8(data, &(pbuf[tmp + 10]), pbuf[tmp + 9]);
                length = (UINT16)pbuf[tmp + 9];
            }
            else
            {
                //
                // 8-bit解码
                //
#if g_SMS_ValueCheck
                SUL_MemCopy8(data, &(pbuf[tmp + 10]), pbuf[tmp + 9]);
#else
                SUL_GsmBcdToAscii(&(pbuf[tmp + 10]), pbuf[tmp + 9], data);
#endif
                length = (UINT16)pbuf[tmp + 9];
            }
        }
        else
        {
            //
            // User date里面包含头信息
            //
            if (dcs == GSM_7BIT)
            {
                //
                // 7-bit解码
                //
                UINT8 nBodyLength = 0x00;
                UINT8 nUDHL       = 0x00;
                UINT8 nUDL        = 0x00;
                UINT16 nSrcLength = 0x0000;
                UINT8 nTmpForDate[160];

                SUL_ZeroMemory32(nTmpForDate, 160);

                nBodyLength = (UINT8)(pbuf[tmp + 9] - pbuf[tmp + 10] - 1);

                nUDL  = pbuf[tmp + 9];
                nUDHL = pbuf[tmp + 10];

                if (pbuf[tmp + 10] == 0x00)
                {
                    ;
                }
                else if (pbuf[tmp + 10] > 0x07) // Don't think about EMS, later.........
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100ebe));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[tmp + 11] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[tmp + 10 + 3];
                        pLongerMsg->count   = pbuf[tmp + 10 + 4];
                        pLongerMsg->current = pbuf[tmp + 10 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[tmp + 11] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[tmp + 10 + 3]) << 8) + pbuf[tmp + 10 + 4]);
                        pLongerMsg->count   = pbuf[tmp + 10 + 5];
                        pLongerMsg->current = pbuf[tmp + 10 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[tmp + 10 + 1];

                        // TODO ^.^
                    }
                }

                //
                // Modify for long 7bit sms
                //
                // tmp = (UINT8)(tmp + pbuf[tmp + 10] + 1);

                // nSrcLength = (UINT16)(nBodyLength & 7 ? nBodyLength * 7 / 8 + 1 : nBodyLength * 7 / 8);
                nSrcLength = (UINT16)(nUDL & 7 ? (nUDL * 7 / 8 + 1) : (nUDL * 7 / 8));

                // length = (UINT16)SUL_Decode7Bit(&(pbuf[tmp + 10]), data, nSrcLength);    // 转换到TP-DU

                //
                // 超常7BIT解码，从UDH头开始解码，最后长度去掉UDHL+2，2的意思是UDHL本身和解码后增长的一个字节。
                //

                length = (UINT16)SUL_Decode7Bit(&(pbuf[tmp + 10]), nTmpForDate, nSrcLength);  // 转换到TP-DU
#if 0
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>length is 0x%x\r\n",0x08100ebf), length);
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>nUDHL is 0x%x\r\n",0x08100ec0), nUDHL);
#endif
                SUL_MemCopy8(data, &(nTmpForDate[nUDHL + 2]), (length = length - nUDHL - 2));
#if 0
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>data[0] is 0x%x\r\n",0x08100ec1), data[0]);
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>data[1] is 0x%x\r\n",0x08100ec2), data[1]);
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>data[2] is 0x%x\r\n",0x08100ec3), data[2]);
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>data[3] is 0x%x\r\n",0x08100ec4), data[3]);
                CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("=========>length is 0x%x\r\n",0x08100ec5), length);
#endif
            }
            else if (dcs == GSM_UCS2)
            {
                //
                // UCS2解码
                //
                UINT8 nBodyLength = 0x0;

                nBodyLength = (UINT8)(pbuf[tmp + 9] - pbuf[tmp + 10] - 1);

                if (pbuf[tmp + 10] == 0x00)
                {
                    ;
                }
                else if (pbuf[tmp + 10] > 0x07) // Don't think about EMS, later.........
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100ec6));
                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[tmp + 11] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[tmp + 10 + 3];
                        pLongerMsg->count   = pbuf[tmp + 10 + 4];
                        pLongerMsg->current = pbuf[tmp + 10 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[tmp + 11] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[tmp + 10 + 3]) << 8) + pbuf[tmp + 10 + 4]);
                        pLongerMsg->count   = pbuf[tmp + 10 + 5];
                        pLongerMsg->current = pbuf[tmp + 10 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[tmp + 10 + 1];

                        // TODO ^.^
                    }
                }

                tmp = (UINT8)(tmp + pbuf[tmp + 10] + 1);

                SUL_MemCopy8(data, &(pbuf[tmp + 10]), nBodyLength);
                length = (UINT16)nBodyLength;
            }
            else
            {
                //
                // 8-bit解码
                //
                UINT8 nBodyLength;

                nBodyLength = (UINT8)(pbuf[tmp + 9] - pbuf[tmp + 10] - 1);

                if (pbuf[tmp + 10] == 0x00)
                {
                    ;
                }
                else if (pbuf[tmp + 10] > 0x07) // Don't think about EMS, later.........
                {
                    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100ec7));

                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[tmp + 11] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[tmp + 10 + 3];
                        pLongerMsg->count   = pbuf[tmp + 10 + 4];
                        pLongerMsg->current = pbuf[tmp + 10 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[tmp + 11] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[tmp + 10 + 3]) << 8) + pbuf[tmp + 10 + 4]);
                        pLongerMsg->count   = pbuf[tmp + 10 + 5];
                        pLongerMsg->current = pbuf[tmp + 10 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[tmp + 10 + 1];

                        // TODO ^.^
                    }
                }
                tmp = (UINT8)(tmp + pbuf[tmp + 10] + 1);
#if g_SMS_ValueCheck
                SUL_MemCopy8(data, &(pbuf[tmp + 10]), nBodyLength);
#else
                SUL_GsmBcdToAscii(&(pbuf[tmp + 10]), nBodyLength, data);
#endif
                length = (UINT16)nBodyLength;
            }
        }

        if (nType == 0x01)
        {
            //
            // CFW_SMS_TXT_DELIVERED_NO_HRD_INFO
            //
            CFW_SMS_NODE *pDEL_NH = (CFW_SMS_NODE *)CSW_SIM_MALLOC
                                    (sizeof(CFW_SMS_TXT_DELIVERED_NO_HRD_INFO) + sizeof(CFW_SMS_NODE) + length);
            CFW_SMS_TXT_DELIVERED_NO_HRD_INFO *pTmp;
            *pOutData = (UINT32)pDEL_NH;

            SUL_ZeroMemory8(pDEL_NH, (sizeof(CFW_SMS_TXT_DELIVERED_NO_HRD_INFO) + sizeof(CFW_SMS_NODE) + length));

            pDEL_NH->pNode = (PVOID)((UINT8 *)pDEL_NH + sizeof(CFW_SMS_NODE));
            pTmp           = (CFW_SMS_TXT_DELIVERED_NO_HRD_INFO *)(pDEL_NH->pNode);

            pTmp->stat   = *pSMSStatus;
            pTmp->length = length;

            SUL_MemCopy8(pTmp->data, data, length);
            SUL_MemCopy8(pTmp->oa, oa, oa_size);
            pTmp->oa_size      = oa_size;
            pTmp->scts.uYear   = (UINT16)(nTime[1] - 0x30 + (nTime[0] - 0x30) * 10 + 2000);
            pTmp->scts.uMonth  = (UINT8)(nTime[3] - 0x30 + (nTime[2] - 0x30) * 10);
            pTmp->scts.uDay    = (UINT8)(nTime[5] - 0x30 + (nTime[4] - 0x30) * 10);
            pTmp->scts.uHour   = (UINT8)(nTime[7] - 0x30 + (nTime[6] - 0x30) * 10);
            pTmp->scts.uMinute = (UINT8)(nTime[9] - 0x30 + (nTime[8] - 0x30) * 10);
            pTmp->scts.uSecond = (UINT8)(nTime[11] - 0x30 + (nTime[10] - 0x30) * 10);
            pTmp->scts.iZone   = tmpZone; // add by wuys 2008-06-18 for fixing Zone
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);

            return (ERR_SUCCESS);
        }
        else if (nType == 0x02)
        {
            if ((*pSMSStatus == 0x01) || (*pSMSStatus == 0x02))
            {
                //
                // CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO
                //

                CFW_SMS_NODE *pDEL_H = (CFW_SMS_NODE *)CSW_SIM_MALLOC
                                       (sizeof(CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO) + sizeof(CFW_SMS_NODE) + length);
                CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *pTmp;
                *pOutData = (UINT32)pDEL_H;

                SUL_ZeroMemory8(pDEL_H, (sizeof(CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO) + sizeof(CFW_SMS_NODE) + length));

                pDEL_H->pNode = (PVOID)((UINT8 *)pDEL_H + sizeof(CFW_SMS_NODE));

                pTmp = (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)(pDEL_H->pNode);

                pTmp->tooa  = tooa;
                pTmp->tosca = tosca;

                // pTmp->fo     = nMTI;
                pTmp->fo     = 17;
                pTmp->stat   = *pSMSStatus;
                pTmp->length = length;

                SUL_MemCopy8(pTmp->data, data, length);
                SUL_MemCopy8(pTmp->oa, oa, oa_size);
                SUL_MemCopy8(pTmp->sca, sca, sca_size);
                pTmp->oa_size      = oa_size;
                pTmp->sca_size     = sca_size;
                pTmp->scts.uYear   = (UINT16)(nTime[1] - 0x30 + (nTime[0] - 0x30) * 10 + 2000);
                pTmp->scts.uMonth  = (UINT8)(nTime[3] - 0x30 + (nTime[2] - 0x30) * 10);
                pTmp->scts.uDay    = (UINT8)(nTime[5] - 0x30 + (nTime[4] - 0x30) * 10);
                pTmp->scts.uHour   = (UINT8)(nTime[7] - 0x30 + (nTime[6] - 0x30) * 10);
                pTmp->scts.uMinute = (UINT8)(nTime[9] - 0x30 + (nTime[8] - 0x30) * 10);
                pTmp->scts.uSecond = (UINT8)(nTime[11] - 0x30 + (nTime[10] - 0x30) * 10);
                pTmp->scts.iZone   = tmpZone; // add by wuys 2008-06-18 for fixing Zone

                //
                // Modify by lixp at 20060322
                // 为了和文档同步，2.2.6  ^.^
                //
                // dcs = pbuf[tmp + 1];

                if (dcs == GSM_7BIT_UNCOMPRESSED)
                {
                    dcs = 0;
                }
                else if (dcs == GSM_8BIT_UNCOMPRESSED)
                {
                    dcs = 1;
                }
                else if (dcs == GSM_UCS2_UNCOMPRESSED)
                {
                    dcs = 2;
                }
                else if (dcs == GSM_7BIT_COMPRESSED)
                {
                    dcs = 0x10;
                }
                else if (dcs == GSM_8BIT_COMPRESSED)
                {
                    dcs = 0x11;
                }
                else if (dcs == GSM_UCS2_COMPRESSED)
                {
                    dcs = 0x12;
                }

                pTmp->dcs = dcs;
                pTmp->pid = pid;
                CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                return (ERR_SUCCESS);
            }
        }
    }
    else if (nType == 0x03 || nType == 0x04)
    {
        //
        // Decompose SUBMIT message See 3GPP TP 23040-390 9.2.3.1
        //
        UINT8 da[TEL_NUMBER_MAX_LEN];
        UINT8 sca[TEL_NUMBER_MAX_LEN];
        UINT8 data[160];
        UINT8 da_size  = 0;
        UINT8 sca_size = 0;
        UINT8 toda     = 0x0, tosca = 0x0;
        UINT8 pid      = 0x0, dcs = 0x0, nMR = 0x0, VP = 0x0;
        UINT8 nTmp     = 0x0, Tmp = 0x0;
        UINT16 length  = 0x0;

        SUL_ZeroMemory8(data, 160);
        SUL_ZeroMemory8(da, TEL_NUMBER_MAX_LEN);
        SUL_ZeroMemory8(sca, TEL_NUMBER_MAX_LEN);
#if g_SMS_ValueCheck
        if (pbuf[1] > TEL_NUMBER_MAX_LEN)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error pbuf[1] Len 0x%x \n",0x08100ec8), pbuf[1]);
            return ERR_INVALID_PARAMETER;
        }
#endif
        if (pbuf[1] != 0x00)
        {
            //
            // Get the SCA Address
            //
            // SUL_GSMBCDToAscii(&(pbuf[3]),sca,(UINT8)(pbuf[1] - 1));
            SUL_MemCopy8(sca, &(pbuf[3]), (pbuf[1] - 1));
            sca_size = (UINT8)(pbuf[1] - 1);

            //
            // Get the type of SMS Center Address just as 0x91
            //
            tosca = pbuf[2];
        }
        else
        {
            // tooa = pbuf[tmp +1];
            // tmp = tmp + 2;
            tosca = pbuf[2];  //
        }

        //
        // Get the MTI/VPF
        //
        // *********************************************************************
        // TP-MTI: bit1-0: xxxx xx01: SMS-SUBMIT(MS->SC)
        // 0 0   : SMS DELIVER (in the direction SC to MS)
        // 0 0   : SMS DELIVER REPORT (in the direction MS to SC)
        // 1 0   : SMS STATUS REPORT (in the direction SC to MS)
        // 1 0   : SMS COMMAND (in the direction MS to SC)
        // 0 1   : SMS SUBMIT (in the direction MS to SC)
        // 0 1   : SMS SUBMIT REPORT (in the direction SC to MS)
        // 1 1   : Reserved
        // *********************************************************************

        // *********************************************************************
        // TP-RD:  bit2  : xxxx x0xx:
        // 0   : Instruct the SC to accept an SMS SUBMIT for an SM still held in the
        // SC which has the same TP MR and the same TP DA as a
        // previously  submitted SM from   the same OA.
        // 1   : Instruct the SC to reject an SMS SUBMIT for an SM still held in the
        // SC which has the same TP MR and the same TP DA as the
        // previously submitted SM   from the same OA. In this case
        // the response returned by the SC is as specified in 9.2.3.6.
        // *********************************************************************

        // *********************************************************************
        // TP-VPF  bit4-3: xxx1 0xxx:
        // 1 0   : TP-VP filed present-relative format
        // 0 1   : TP-VP filed present-enhanced format
        // 1 1   : TP-VP filed present-absolute format
        // 0 0   : TP-VP filed no present
        // *********************************************************************

        // *********************************************************************
        // TP SRR  bit5  : xx1x xxxx: TP SRR
        // 0   : A status report is not requested
        // 1   : A status report is requested
        // *********************************************************************

        // *********************************************************************
        // TP UDHI bit6  : x0xx xxxx: TP UDHI, for loner SMS
        // *********************************************************************

        // *********************************************************************
        // TP-RP   bit7  : 0xxx xxxx: TP-RP
        // 0 : TP Reply Path parameter is not set in this SMS SUBMIT/DELIVER
        // 1   : TP Reply Path parameter is set in this SMS SUBMIT/DELIVER
        // *********************************************************************
        if (pbuf[1] != 0x00)
        {
            Tmp = (UINT8)(pbuf[1] + 2);
        }
        else
        {
            //
            // 考虑SCA为空TON保留
            //
            Tmp = 3;
        }

        nMTI = pbuf[Tmp];
        Tmp++;

        //
        // Get the MR
        //
        nMR = pbuf[Tmp];
        Tmp++;

        //
        // Get DA length
        //
        nTmp = pbuf[Tmp];
#if g_SMS_ValueCheck
        if (nTmp > 2 * TEL_NUMBER_MAX_LEN)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error DA Len 0x%x \n",0x08100ec9), nTmp);
            return ERR_INVALID_PARAMETER;
        }
#endif
        if (nTmp != 0x00)
        {
            if (nTmp & 1)
                nTmp += 1;
            nTmp = (UINT8)(nTmp / 2);

            // SUL_GSMBCDToAscii(&(pbuf[Tmp + 2]),da,nTmp);
            SUL_MemCopy8(da, &(pbuf[Tmp + 2]), nTmp);
            da_size = nTmp;

            //
            // Get the type of the DA
            //
            toda = pbuf[Tmp + 1];
            Tmp  = (UINT8)(Tmp + 2 + nTmp);
        }
        else
        {
            // toda = pbuf[Tmp +1];
            toda = pbuf[Tmp];
            Tmp  = (UINT8)(Tmp + 2);
        }

        //
        // Get the PID
        //
        pid = pbuf[Tmp];

        //
        // Get the DCS
        //
        dcs = pbuf[Tmp + 1];

        // Verify there is VP or not, xxxY Yxxx bit3 and bit 4
        if ((nMTI & 0x18) == 0) // xxx0 0xxx TP-VP filed no present
        {
            // no VP
            Tmp--;
        }
        else
        {
            //
            // Get the VP
            //
            VP = pbuf[Tmp + 2];

        }

        //
        // Get the User date
        //

        // *********************************************************************
        // TP UDHI bit6  : x0xx xxxx: TP UDHI, for loner SMS
        // *********************************************************************
        if ((nMTI & 0x40) == 0x00)
        {
            //
            // User date里面不包含头信息
            //
            if (dcs == GSM_7BIT)
            {
                //
                // 7-bit解码
                //
                UINT16 nSrcLength = 0x0;

                nSrcLength = (UINT16)(pbuf[Tmp + 3] & 7 ? pbuf[Tmp + 3] * 7 / 8 + 1 : pbuf[Tmp + 3] * 7 / 8);

                length = (UINT16)SUL_Decode7Bit(&(pbuf[Tmp + 4]), data, nSrcLength);  // 转换到TP-DU
            }
            else if (dcs == GSM_UCS2)
            {
                //
                // UCS2解码
                //
                SUL_MemCopy8(data, &(pbuf[Tmp + 4]), pbuf[Tmp + 3]);
                length = (UINT16)pbuf[Tmp + 3];
            }
            else
            {
                //
                // 8-bit解码
                //
#if g_SMS_ValueCheck
                SUL_MemCopy8(data, &(pbuf[Tmp + 4]), pbuf[Tmp + 3]);
                length = pbuf[Tmp + 3];
#else
                SUL_GsmBcdToAscii(&(pbuf[Tmp + 4]), pbuf[Tmp + 3], data);
                length = (UINT16)pbuf[tmp + 9];
#endif
            }
        }
        else
        {
            //
            // User date里面包含头信息
            //
            if (dcs == GSM_7BIT)
            {
                //
                // 7-bit解码
                //
                UINT8 nBodyLength = 0x0;
                UINT16 nSrcLength = 0x0;
                UINT8 nUDHL       = 0x00;
                UINT8 nUDL        = 0x00;
                UINT8 nTmpForDate[160];

                SUL_ZeroMemory32(nTmpForDate, 160);
                nBodyLength = (UINT8)(pbuf[Tmp + 3] - pbuf[Tmp + 4] - 1);

                nUDHL = pbuf[Tmp + 4];
                nUDL  = pbuf[Tmp + 3];

                if (pbuf[Tmp + 4] == 0x00)
                {
                    ;
                }
                else if (pbuf[Tmp + 4] > 0x07)  // Don't think about EMS, later.........
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100eca));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[Tmp + 5] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[Tmp + 4 + 3];
                        pLongerMsg->count   = pbuf[Tmp + 4 + 4];
                        pLongerMsg->current = pbuf[Tmp + 4 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[Tmp + 5] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[Tmp + 4 + 3]) << 8) + pbuf[Tmp + 4 + 4]);
                        pLongerMsg->count   = pbuf[Tmp + 4 + 5];
                        pLongerMsg->current = pbuf[Tmp + 4 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[Tmp + 4 + 1];

                        // TODO ^.^
                    }
                }

                // Tmp = (UINT8)(Tmp + pbuf[Tmp + 4]  + 1);

                // nSrcLength = (UINT16)(nBodyLength & 7 ? nBodyLength * 7 / 8 + 1 : nBodyLength * 7 / 8);
                nSrcLength = (UINT16)(nUDL & 7 ? nUDL * 7 / 8 + 1 : nUDL * 7 / 8);

                // length = (UINT16)SUL_Decode7Bit(&(pbuf[Tmp + 4]), data, nSrcLength);    // 转换到TP-DU
                length = (UINT16)SUL_Decode7Bit(&(pbuf[Tmp + 5]), nTmpForDate, nSrcLength); // 转换到TP-DU
                SUL_MemCopy8(data, &(nTmpForDate[nUDHL + 2]), (length = length - nUDHL - 2));

                // length = length - nUDHL - 1;//-1? not
            }
            else if (dcs == GSM_UCS2)
            {
                //
                // UCS2解码
                //
                UINT8 nBodyLength;

                nBodyLength = (UINT8)(pbuf[Tmp + 3] - pbuf[Tmp + 4] - 1);
                if (pbuf[Tmp + 4] == 0x00)
                {
                    ;
                }
                else if (pbuf[Tmp + 4] > 0x07)  // Don't think about EMS, later.........
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100ecb));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[Tmp + 5] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[Tmp + 4 + 3];
                        pLongerMsg->count   = pbuf[Tmp + 4 + 4];
                        pLongerMsg->current = pbuf[Tmp + 4 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[Tmp + 5] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[Tmp + 4 + 3]) << 8) + pbuf[Tmp + 4 + 4]);
                        pLongerMsg->count   = pbuf[Tmp + 4 + 5];
                        pLongerMsg->current = pbuf[Tmp + 4 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[Tmp + 4 + 1];

                        // TODO ^.^
                    }
                }

                Tmp = (UINT8)(Tmp + pbuf[Tmp + 4] + 1);

                SUL_MemCopy8(data, &(pbuf[Tmp + 4]), nBodyLength);
                length = (UINT16)nBodyLength;
            }
            else
            {
                //
                // 8-bit解码
                //
                UINT8 nBodyLength;

                nBodyLength = (UINT8)(pbuf[Tmp + 3] - pbuf[Tmp + 4] - 1);
                if (pbuf[Tmp + 4] == 0x00)
                {
                    ;
                }
                else if (pbuf[Tmp + 4] > 0x07)  // Don't think about EMS, later.........
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error it is a EMS\n",0x08100ecc));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                    return ERR_INVALID_PARAMETER;
                }
                else
                {
                    if (pbuf[Tmp + 5] == 0x00)
                    {
                        //
                        // 例如UDH的实际例子格式为05 00 03 81 04 03
                        //
                        pLongerMsg->id      = pbuf[Tmp + 4 + 3];
                        pLongerMsg->count   = pbuf[Tmp + 4 + 4];
                        pLongerMsg->current = pbuf[Tmp + 4 + 5];
                    }

                    //
                    // Modify by lxp for decompose 16 reference SMS
                    //
                    else if (pbuf[Tmp + 5] == 0x08)
                    {
                        //
                        // 例如UDH的实际例子格式为06 08 04 81 81 04 03
                        //
                        pLongerMsg->id      = (UINT8)(((pbuf[Tmp + 4 + 3]) << 8) + pbuf[Tmp + 4 + 4]);
                        pLongerMsg->count   = pbuf[Tmp + 4 + 5];
                        pLongerMsg->current = pbuf[Tmp + 4 + 6];
                    }
                    else
                    {
                        pLongerMsg->id = pbuf[Tmp + 4 + 1];

                        // TODO ^.^
                    }
                }
                Tmp = (UINT8)(Tmp + pbuf[Tmp + 4] + 1);
#if g_SMS_ValueCheck
                SUL_MemCopy8(data, &(pbuf[Tmp + 4]), nBodyLength);
#else
                SUL_GsmBcdToAscii(&(pbuf[Tmp + 4]), nBodyLength, data);
#endif
                // SUL_GSMBCDToAscii(&(pbuf[Tmp + 4]), data, nBodyLength);
                length = (UINT16)nBodyLength;

            }
        }

        if (nType == 0x03)
        {

            //
            // CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO
            //
            CFW_SMS_NODE *pSUB_NH = (CFW_SMS_NODE *)CSW_SIM_MALLOC
                                    (sizeof(CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO) + sizeof(CFW_SMS_NODE) + length);
            CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO *pTmp;

            if (pSUB_NH == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ecd));
                CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                return (ERR_NO_MORE_MEMORY);
            }

            *pOutData = (UINT32)pSUB_NH;

            SUL_ZeroMemory8(pSUB_NH, (sizeof(CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO) + sizeof(CFW_SMS_NODE) + length));

            // a = sizeof(CFW_SMS_NODE);

            pSUB_NH->pNode = (PVOID)((UINT8 *)pSUB_NH + sizeof(CFW_SMS_NODE));
            pTmp           = (CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO *)(pSUB_NH->pNode);

            pTmp->stat   = *pSMSStatus;
            pTmp->length = length;

            SUL_MemCopy8(pTmp->data, data, length);
            SUL_MemCopy8(pTmp->da, da, da_size);
            pTmp->da_size = da_size;
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);

            return (ERR_SUCCESS);

        }
        else if (nType == 0x04)
        {
            //
            // CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO
            //
            CFW_SMS_NODE *pSUB_H = (CFW_SMS_NODE *)CSW_SIM_MALLOC
                                   (sizeof(CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO) + sizeof(CFW_SMS_NODE) + length);
            CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *pTmp;
            *pOutData = (UINT32)pSUB_H;
            if (pSUB_H == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ece));
                CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                return (ERR_NO_MORE_MEMORY);
            }
            SUL_ZeroMemory8(pSUB_H, (sizeof(CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO) + sizeof(CFW_SMS_NODE) + length));

            pSUB_H->pNode = (PVOID)((UINT8 *)pSUB_H + sizeof(CFW_SMS_NODE));

            pTmp        = (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *)(pSUB_H->pNode);
            pTmp->toda  = toda;
            pTmp->tosca = tosca;

            // pTmp->fo = nMTI;
            pTmp->fo     = 17;
            pTmp->stat   = *pSMSStatus;
            pTmp->length = length;

            SUL_MemCopy8(pTmp->data, data, length);
            SUL_MemCopy8(pTmp->da, da, da_size);
            pTmp->da_size = da_size;
            SUL_MemCopy8(pTmp->sca, sca, sca_size);
            pTmp->sca_size = sca_size;
            if (dcs == GSM_7BIT_UNCOMPRESSED)
            {
                dcs = 0;
            }
            else if (dcs == GSM_8BIT_UNCOMPRESSED)
            {
                dcs = 1;
            }
            else if (dcs == GSM_UCS2_UNCOMPRESSED)
            {
                dcs = 2;
            }
            else if (dcs == GSM_7BIT_COMPRESSED)
            {
                dcs = 0x10;
            }
            else if (dcs == GSM_8BIT_COMPRESSED)
            {
                dcs = 0x11;
            }
            else if (dcs == GSM_UCS2_COMPRESSED)
            {
                dcs = 0x12;
            }
            pTmp->dcs = dcs;
            pTmp->pid = pid;
            pTmp->vp  = VP;
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);

            return (ERR_SUCCESS);
        }
    }
    else if (nType == 0x08)
    {
        //
        // CFW_SMS_PDU_INFO
        //

        UINT16 length = 0x0;
        UINT16 nTpUdl = 0x0;
        UINT8 nDCS    = 0x00;

        // ***********************************************************
        //
        // 根据MTI来确定是哪种类型的PDU
        //
        // ***********************************************************

        if ((nMTI & 0x03) == 0x01)
        {
            //
            // Submit type PDU
            //
            CFW_SMS_NODE *pPDU;
            CFW_SMS_PDU_INFO *pTmp;

            UINT8 nTmp, Tmp;

            Tmp  = tmp;
            Tmp  = (UINT8)(Tmp + 2);
            nTmp = pbuf[Tmp];
#if g_SMS_ValueCheck
            if (nTmp > 2 * TEL_NUMBER_MAX_LEN)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error DA Len 0x%x \n",0x08100ecf), nTmp);
                return ERR_INVALID_PARAMETER;
            }
#endif
            if (nTmp != 0x00)
            {
                if (nTmp & 1)
                {
                    nTmp += 1;
                }
                nTmp = (UINT8)(nTmp / 2);
                Tmp  = (UINT8)(Tmp + 2 + nTmp);
            }
            else
            {
                Tmp = (UINT8)(Tmp + 2);
            }

            nDCS   = pbuf[Tmp + 1];
            nTpUdl = (UINT16)pbuf[Tmp + 3];
            length = (UINT16)(nTpUdl + Tmp + 3);

            pPDU = (CFW_SMS_NODE *)CSW_SIM_MALLOC(sizeof(CFW_SMS_PDU_INFO) + sizeof(CFW_SMS_NODE) + length);

            if (pPDU == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ed0));
                CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                return ERR_NO_MORE_MEMORY;
            }

            SUL_ZeroMemory8(pPDU, (sizeof(CFW_SMS_PDU_INFO) + sizeof(CFW_SMS_NODE) + length));

            *pOutData = (UINT32)pPDU;

            pPDU->pNode = (PVOID)((UINT8 *)pPDU + sizeof(CFW_SMS_NODE));
            pTmp        = (CFW_SMS_PDU_INFO *)(pPDU->pNode);
#if 1
            if (nDCS == GSM_7BIT_UNCOMPRESSED)
            {
                nTpUdl = nTpUdl * 7 / 8 + nTpUdl * 7 % 8;
            }
#endif

#if g_SMS_ValueCheck
            if (nTpUdl > 140)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nTpUdl 0x%x \n",0x08100ed1), nTpUdl);
                return ERR_INVALID_PARAMETER;
            }
#endif
            pTmp->nTpUdl    = nTpUdl;
            pTmp->nDataSize = length;
            pTmp->nStatus   = *pSMSStatus;
            SUL_MemCopy8(pTmp->pData, &(pbuf[1]), length);
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
            return (ERR_SUCCESS);
        }

        else if ((nMTI & 0x03) == 0x00)
        {
            //
            // For Deliver
            //

            CFW_SMS_NODE *pPDU;
            CFW_SMS_PDU_INFO *pTmp;

            UINT8 nTmp, Tmp;

            Tmp  = tmp;
            nTmp = pbuf[Tmp + 1];
#if g_SMS_ValueCheck
            if (nTmp > 2 * TEL_NUMBER_MAX_LEN)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error DA Len 0x%x \n",0x08100ed2), nTmp);
                return ERR_INVALID_PARAMETER;
            }
#endif
            if (nTmp != 0x00)
            {
                if (nTmp & 1)
                    nTmp += 1;  //
                nTmp = (UINT8)(nTmp / 2);
                Tmp  = (UINT8)(Tmp + 2 + nTmp);
            }
            else
            {
                Tmp = (UINT8)(Tmp + 2);
            }

            nTpUdl = pbuf[Tmp + 3 + 7];
#if g_SMS_ValueCheck
            if (nTpUdl > 160)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nTpUdl Len 0x%x \n",0x08100ed3), nTpUdl);
                return ERR_INVALID_PARAMETER;
            }
#endif
            length = (UINT16)(nTpUdl + Tmp + 3 + 7);

            pPDU = (CFW_SMS_NODE *)CSW_SIM_MALLOC(sizeof(CFW_SMS_PDU_INFO) + sizeof(CFW_SMS_NODE) + length);

            if (pPDU == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ed4));
                CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
                return ERR_NO_MORE_MEMORY;
            }

            SUL_ZeroMemory8(pPDU, (sizeof(CFW_SMS_PDU_INFO) + sizeof(CFW_SMS_NODE) + length));

            *pOutData = (UINT32)pPDU;

            pPDU->pNode = (PVOID)((UINT8 *)pPDU + sizeof(CFW_SMS_NODE));
            pTmp        = (CFW_SMS_PDU_INFO *)(pPDU->pNode);
#if 1
            if (nDCS == GSM_7BIT_UNCOMPRESSED)
            {
                nTpUdl = nTpUdl * 7 / 8 + nTpUdl * 7 % 8;
            }
#endif
            pTmp->nTpUdl    = nTpUdl;
            pTmp->nDataSize = length;
            pTmp->nStatus   = *pSMSStatus;
            SUL_MemCopy8(pTmp->pData, &(pbuf[1]), length);
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
            return (ERR_SUCCESS);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error MTI Value 0x%x\n",0x08100ed5), nMTI);
            CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);
            return (ERR_INVALID_PARAMETER);
        }

    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nType Value 0x%x\n",0x08100ed6), nType);
        CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);

        return (ERR_INVALID_PARAMETER);
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_DecomposePDU);

    return (ERR_SUCCESS);
}

#endif

