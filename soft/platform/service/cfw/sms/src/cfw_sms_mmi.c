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






























#include "cfw_sms_prv.h"
#include <csw_mem_prv.h>
#include "csw_debug.h"
//Here, define a golobal variable for longer msg
UINT32 *gpLongerMsgPara = NULL;
UINT32 *gpLongerMsgParaIndex = NULL;
CFW_SMS_LONG_INFO *gpWriteLongerMsgPara = NULL;
UINT8   gnWriteLongerMsgParaCount = 0x0;
//for long sms
PSTR pSMSDir      = "C:/System";
PSTR pSIMFilename = "C:/System/SimLong.txt";
PSTR pMEFilename  = "C:/System/MeLong.txt";

#if (CFW_SMS_DUMP_ENABLE==0) && (CFW_SMS_SRV_ENABLE==1)

///////////////////////////////////////////////////////////////////////////////
//                         Public APIs For SMS MO                            //
///////////////////////////////////////////////////////////////////////////////
BOOL gPendingNewSms[CFW_SIM_COUNT] = {FALSE, FALSE};
PUBLIC VOID CFW_CheckIfAllowedRecvSms(BOOL bIfAllowed, CFW_SIM_ID nSimId)
{
    if(bIfAllowed == TRUE)
    {
        sms_mo_SmsPPSendMMAReq(nSimId);
        gPendingNewSms[nSimId] = FALSE;
    }
    else
    {
        gPendingNewSms[nSimId] = TRUE;
    }
}

PUBLIC UINT32 SRVAPI CFW_SmsSendMessage(CFW_DIALNUMBER *pNumber, UINT8 *pData, UINT16 nDataSize, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT8             nFormat = 0x00;
    UINT32            ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsSendMessage);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SmsSendMessage enter nSimId:%d \n ",0x081010a0), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CheckSimId error ret=0x%x\n ",0x081010a1), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    ret = CFW_IsFreeUTI(nUTI, CFW_SMS_MO_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_IsFreeUTI error ret=0x%x\n ",0x081010a2), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
        return ret;
    }

    //Get SMS format from user setting
    CFW_CfgGetSmsFormat(&nFormat, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SmsSendMessage nFormat=0x%x\n ",0x081010a3), nFormat);
    if(nFormat)//Text
    {
        UINT8 sNumber[2 * TEL_NUMBER_MAX_LEN], uTmpNumberSize; //added by fengwei 20080709;

        SUL_ZeroMemory8(&sNumber, 2 * TEL_NUMBER_MAX_LEN);

        //Check the user input data, do more later
        if((!pNumber) || (!pNumber->pDialNumber))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
            return ERR_CFW_INVALID_PARAMETER ;
        }
        if((pNumber->nDialNumberSize <= 0) || (pNumber->nDialNumberSize > TEL_NUMBER_MAX_LEN ))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
            return ERR_CFW_INVALID_PARAMETER ;
        }
        if((pNumber->nClir != 0) && (pNumber->nClir != 1) && (pNumber->nClir != 2))
        {
            pNumber->nClir = 0;
        }
        if((pNumber->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL) &&  \
                (pNumber->nType != CFW_TELNUMBER_TYPE_NATIONAL) &&      \
                (pNumber->nType != CFW_TELNUMBER_TYPE_UNKNOWN))
        {
            pNumber->nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }

        uTmpNumberSize = SUL_GsmBcdToAscii(pNumber->pDialNumber, pNumber->nDialNumberSize, sNumber);
        //hameina[-] sul function can return number size correctly
#if 0
        if(0xf0 == (*(pNumber->pDialNumber + pNumber->nDialNumberSize - 1) & 0xf0))
            uTmpNumberSize = (UINT8)(pNumber->nDialNumberSize * 2 - 1);
        else
            uTmpNumberSize = (UINT8)(pNumber->nDialNumberSize * 2);
#endif
        //Malloc for Handle
        //modified by fengwei 20080709
        //pSmsMOInfo = (CFW_SMS_MO_INFO*)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO) + pNumber->nDialNumberSize + nDataSize);
        pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize + nDataSize);
        if(!pSmsMOInfo)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsSendMessage pSmsMOInfo malloc error!!! \n ",0x081010a4));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
            return ERR_CMS_MEMORY_FULL;
        }
        SUL_ZeroMemory8(pSmsMOInfo, (SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize + nDataSize));//modified by fengwei 20080710

        //Set user input data to handle
        pSmsMOInfo->sNumber.nClir           = pNumber->nClir;
        pSmsMOInfo->sNumber.nType           = pNumber->nType;
        pSmsMOInfo->sNumber.nDialNumberSize = uTmpNumberSize;
        pSmsMOInfo->sNumber.pDialNumber     = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO));
        SUL_MemCopy8((UINT8 *)pSmsMOInfo->sNumber.pDialNumber, &sNumber, uTmpNumberSize);

        pSmsMOInfo->nDataSize = nDataSize;

        //[[hameina[mod]2007-3-22
        if(nDataSize)
            pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize);
        else
            pSmsMOInfo->pData = NULL;
        //]]hameina[mod]2007-3-22

        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SmsSendMessage, send a text sms!nDataSize is %d \n ",0x081010a5), nDataSize);
    }
    else //PDU
    {
        //Malloc for Handle
        pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO) + nDataSize);
        if(!pSmsMOInfo || nDataSize == 0) //hameina[mod] :add the last condition
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsSendMessage pSmsMOInfo malloc error!!! \n ",0x081010a6));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
            return ERR_CMS_MEMORY_FULL;
        }
        SUL_ZeroMemory8(pSmsMOInfo, (SIZEOF(CFW_SMS_MO_INFO) + nDataSize));

        pSmsMOInfo->nDataSize = nDataSize;
        pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO));

        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SmsSendMessage, send a PDU sms! nDataSize is %d\n ",0x081010a7), nDataSize);
    }

    SUL_MemCopy8(pSmsMOInfo->pData, pData, nDataSize);
    pSmsMOInfo->nFormat = nFormat;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(SMS_MO_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_SMS_MO_SRV_ID, pSmsMOInfo, sms_SendProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);

    return ERR_SUCCESS;
}


PUBLIC UINT32 SRVAPI CFW_SmsMoveMessage( UINT16 nIndex, UINT8 nStorage, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO*  pSmsMOInfo = NULL;
    UINT8             nFormat=0x00;
    UINT32            ret=0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsMoveMessage);
    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("CFW_SmsMoveMessage enter \n ",0x081010a8));
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CheckSimId error ret=0x%x\n ",0x081010a9), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    ret = CFW_IsFreeUTI(nUTI,CFW_SMS_MO_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_IsFreeUTI error ret=0x%x\n ",0x081010aa), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
        return ret;
    }

    CFW_SMS_STORAGE_INFO  sStorageInfo;
    UINT8 nDestStorage = 0x00;

    if(CFW_SMS_STORAGE_SM == nStorage)
        nDestStorage = CFW_SMS_STORAGE_ME;
    else
        nDestStorage = CFW_SMS_STORAGE_SM;

    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));

    CFW_CfgGetSmsStorageInfo(&sStorageInfo, nDestStorage, nSimId);
    if(sStorageInfo.totalSlot <= sStorageInfo.usedSlot)
    {
        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("Move failed because storage full\n",0x081010ab));
        return ERR_CMS_MEMORY_FULL;
    }

    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("CFW_SmsMoveMessage enter 1\n ",0x081010ac));

    pSmsMOInfo = (CFW_SMS_MO_INFO*)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SmsSendMessage pSmsMOInfo malloc error!!! \n ",0x081010ad));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));

    pSmsMOInfo->nIndex = nIndex;
    pSmsMOInfo->nLocation = nStorage;

    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("CFW_SmsMoveMessage, nIndex[%d] nStorage[%d]\n ",0x081010ae),nIndex,nStorage);

    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_MoveAoProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo,nUTI,0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo,CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSendMessage);


    return ERR_SUCCESS;
}
PUBLIC UINT32 CFW_SmsDeleteMessage( UINT16 nIndex, UINT8 nStatus,UINT8 nStorage, UINT8 nType, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32 ret = 0x0;
    CFW_SMS_STORAGE_INFO  sStorageInfo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsDeleteMessage);
        return ret;
    }
    //check simid
#ifndef PHONE_SMS_STORAGE_SPACE_COMBINATE
    ret = CFW_CheckSimId(nSimId);
#endif
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((nStorage != CFW_SMS_STORAGE_ME)       && \
            (nStorage != CFW_SMS_STORAGE_SM)       && \
            (nStorage != CFW_SMS_STORAGE_AUTO))
    {
        CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check msg type, currently only support pp msg
    if(nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(!nIndex && !nStatus)//delete by status=0
    {
        CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
    //Get max slot in SIM
    CFW_CfgGetSmsStorageInfo(&sStorageInfo, nStorage, nSimId);

    //Verify Storage
    if(nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID
        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
        {
            nStorage = nSmsStorageId;
        }
        else
        {
            CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
            return ret;
        }
    }

    if(nStorage == CFW_SMS_STORAGE_SM)
    {
        if(nIndex > sStorageInfo.totalSlot )
        {
            CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else if(nStorage == CFW_SMS_STORAGE_ME)
    {
        if(nIndex > SMS_MAX_ENTRY_IN_ME)
        {
            CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }

    if(CFW_SMS_STORED_STATUS_STORED_ALL == nStatus )
    {
        if(nStorage == CFW_SMS_STORAGE_ME)
            nStatus = 0x7f;
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsDeleteMessage pSmsMOInfo malloc error!!! \n ",0x081010af));
        CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));


    //Set value to the handle
    pSmsMOInfo->nLocation = nStorage;
    pSmsMOInfo->nIndex  = nIndex;
    pSmsMOInfo->nStatus = nStatus;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_DleteAoProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SmsDeleteMessage, nIndex is %d, nStatus is %d, nStorage is %d\n ",0x081010b0), nIndex, nStatus, nStorage);

    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsDeleteMessage);
    return ERR_SUCCESS;
}


PUBLIC UINT32 CFW_SmsSetUnSent2Sent(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT8 nSendStatus, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32 ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsSetUnSent2Sent);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
        return ret;
    }
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(nStorage == CFW_SMS_STORAGE_MT)
    {
	nStorage = CFW_SMS_STORAGE_SM;
    }
    if((nStorage != CFW_SMS_STORAGE_ME)     && \
            (nStorage != CFW_SMS_STORAGE_SM)     && \
            (nStorage != CFW_SMS_STORAGE_AUTO))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID
        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
        {
            if((nSmsStorageId != CFW_SMS_STORAGE_ME)  && \
                    (nSmsStorageId != CFW_SMS_STORAGE_SM))
            {
                CSW_SMS_FREE(pSmsMOInfo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
                return ERR_CFW_INVALID_PARAMETER;
            }
            nStorage = nSmsStorageId;
        }
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }

    if(nStorage == CFW_SMS_STORAGE_SM)
    {
        CFW_SMS_STORAGE_INFO  sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
        //Get max slot in SIM

        CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        if((nIndex > sStorageInfo.totalSlot) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else if(nStorage == CFW_SMS_STORAGE_ME)
    {
        if((nIndex > SMS_MAX_ENTRY_IN_ME) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    if((nSendStatus != CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)        && \
            (nSendStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV)   && \
            (nSendStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE)  && \
            (nSendStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsSetUnSent2Sent pSmsMOInfo malloc error!!! \n ",0x081010b1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));


    //Set value to the handle
    pSmsMOInfo->nLocation = nStorage;
    pSmsMOInfo->nIndex         = nIndex;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);
    pSmsMOInfo->nStatus        = nSendStatus;

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_SetUnSent2SentProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SmsSetUnSent2Sent, nStorage is %d, nIndex is %d, nSendStatus is %d\n ",0x081010b2), nStorage, nIndex, nSendStatus);

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnSent2Sent);
    return ERR_SUCCESS;
}


PUBLIC UINT32 CFW_SmsSetUnRead2Read(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32 ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsSetUnRead2Read);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
        return ret;
    }
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((nStorage != CFW_SMS_STORAGE_ME)     && \
            (nStorage != CFW_SMS_STORAGE_SM)     && \
            (nStorage != CFW_SMS_STORAGE_AUTO))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID
        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
        {
            if((nSmsStorageId != CFW_SMS_STORAGE_ME)  && \
                    (nSmsStorageId != CFW_SMS_STORAGE_SM))
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
                return ERR_CFW_INVALID_PARAMETER;
            }
            nStorage = nSmsStorageId;
        }
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }

    if(nStorage == CFW_SMS_STORAGE_SM)
    {
        CFW_SMS_STORAGE_INFO  sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
        //Get max slot in SIM
        CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        if((nIndex > sStorageInfo.totalSlot) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else if(nStorage == CFW_SMS_STORAGE_ME)
    {
        if((nIndex > SMS_MAX_ENTRY_IN_ME) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsSetUnRead2Read pSmsMOInfo malloc error!!! \n ",0x081010b3));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));


    //Set value to the handle
    pSmsMOInfo->nLocation = nStorage;
    pSmsMOInfo->nIndex         = nIndex;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_SetUnRead2ReadProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SmsSetUnRead2Read, nStorage is %d, nIndex is %d\n ",0x081010b4), nStorage, nIndex );

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetUnRead2Read);
    return ERR_SUCCESS;
}

PUBLIC UINT32 CFW_SmsSetRead2UnRead(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32 ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsSetRead2UnRead);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
        return ret;
    }
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if((nStorage != CFW_SMS_STORAGE_ME)     && \
            (nStorage != CFW_SMS_STORAGE_SM)     && \
            (nStorage != CFW_SMS_STORAGE_AUTO))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID
        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
        {
            if((nSmsStorageId != CFW_SMS_STORAGE_ME)  && \
                    (nSmsStorageId != CFW_SMS_STORAGE_SM))
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
                return ERR_CFW_INVALID_PARAMETER;
            }
            nStorage = nSmsStorageId;
        }
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }

    if(nStorage == CFW_SMS_STORAGE_SM)
    {
        CFW_SMS_STORAGE_INFO  sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
        //Get max slot in SIM
        CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        if((nIndex > sStorageInfo.totalSlot) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else if(nStorage == CFW_SMS_STORAGE_ME)
    {
        if((nIndex > SMS_MAX_ENTRY_IN_ME) || (nIndex <= 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsSetRead2UnRead pSmsMOInfo malloc error!!! \n ",0x081010b5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));


    //Set value to the handle
    pSmsMOInfo->nLocation = nStorage;
    pSmsMOInfo->nIndex         = nIndex;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_SetRead2UnReadProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SmsSetRead2UnRead, nStorage is %d, nIndex is %d\n ",0x081010b6), nStorage, nIndex );

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsSetRead2UnRead);
    return ERR_SUCCESS;
}

PUBLIC UINT32 CFW_SmsCopyMessages(CFW_SMS_COPY *pCopyInfo, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32            ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsCopyMessages);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCopyMessages);
        return ret;
    }
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(pCopyInfo->nStartIndex <= 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCopyMessages);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(pCopyInfo->nStatus == CFW_SMS_STORED_STATUS_STORED_ALL)
        pCopyInfo->nStatus = 0x7f;
    if((pCopyInfo->nOption != CFW_SMS_COPY_ME2SM) && (pCopyInfo->nOption != CFW_SMS_COPY_SM2ME))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCopyMessages);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsCopyMessages pSmsMOInfo malloc error!!! \n ",0x081010b7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCopyMessages);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));

    //Set value to the handle
    pSmsMOInfo->i               = 0x00;
    pSmsMOInfo->nListCount      = pCopyInfo->nCount;
    pSmsMOInfo->nStatus         = pCopyInfo->nStatus;
    pSmsMOInfo->nListStartIndex = pCopyInfo->nStartIndex;
    pSmsMOInfo->nListOption     = pCopyInfo->nOption;
    pSmsMOInfo->nUTI_UserInput  = nUTI;
    pSmsMOInfo->nUTI_Internal   = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //
    //add for (listcount == 0x00)
    //
    if( 0x00 == pSmsMOInfo->nListCount )
    {
        CFW_SMS_STORAGE_INFO nStorageInfo;
        if( pCopyInfo->nOption == 0x00 )
        {
            CFW_CfgGetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_ME, nSimId);
        }
        else
        {
            CFW_CfgGetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        }

        if(pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_UNREAD)
        {
            pSmsMOInfo->nListCount +=  nStorageInfo.unReadRecords;
        }
        //[[xueww[mod] 2007.01.30 fix bug 3843, if else-->if
        if(pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_READ)
        {
            pSmsMOInfo->nListCount +=  nStorageInfo.readRecords;
        }
        if(pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_UNSENT)
        {
            pSmsMOInfo->nListCount +=  nStorageInfo.unsentRecords;
        }
        //
        //这里有隐患；因为DOC和storageinfo有冲突
        //
        if((pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)
                || (pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV)
                || (pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE)
                || (pCopyInfo->nStatus & CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE))
        {
            pSmsMOInfo->nListCount +=  nStorageInfo.sentRecords;
        }
        //]]xueww[mod] 2007.01.30
    }
    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_CopyAoProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("CFW_SmsCopyMessages, nStatus is %d,  nCount is %d,  nStartIndex is %d, nOption is %d\n ",0x081010b8),
              pCopyInfo->nStatus, pCopyInfo->nCount, pCopyInfo->nStartIndex, pCopyInfo->nOption );

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCopyMessages);
    return ERR_SUCCESS;
}


UINT32 CFW_SmsReadMessage(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32            ret = 0x0;
    UINT8             nFormat = 0x0, nShow = 0x0;
    UINT8 nStorage1 = 0;
    CFW_SMS_STORAGE_INFO  sStorageInfo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsReadMessage);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ret;
    }
    //check storageid
    if((nStorage != CFW_SMS_STORAGE_ME)     && \
            (nStorage != CFW_SMS_STORAGE_SM)     && \
            (nStorage != CFW_SMS_STORAGE_AUTO) )
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check msg type, currently only support pp msg
    if(nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //check simid
#ifndef PHONE_SMS_STORAGE_SPACE_COMBINATE
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
#endif
    //check index
    if(CFW_SMS_STORAGE_AUTO == nStorage)
    {
        ret = CFW_CfgGetSmsStorage(&nStorage1, nSimId);
        nStorage = nStorage1;
    }

    //Get max slot in SIM
    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
    CFW_CfgGetSmsStorageInfo(&sStorageInfo, nStorage, nSimId);
    if(nStorage == CFW_SMS_STORAGE_ME)
    {
        if((nIndex > SMS_MAX_ENTRY_IN_ME) || (nIndex == 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else//sim
    {
        if((nIndex > sStorageInfo.totalSlot ) || (nIndex == 0))
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsReadMessage pSmsMOInfo malloc error!!! \n ",0x081010b9));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));


    //Set value to the handle
    pSmsMOInfo->nLocation = nStorage;
    pSmsMOInfo->nIndex         = nIndex;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    CFW_CfgGetSmsFormat(&nFormat, nSimId);
    CFW_CfgGetSmsShowTextModeParam(&nShow, nSimId);
    pSmsMOInfo->nFormat  = nFormat;
    pSmsMOInfo->nPath    = nShow;    //use nPath to save nShow,hh

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_ReadAoProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SmsReadMessage, nStorage is %d,  nIndex is %d \n ",0x081010ba), nStorage, nIndex);

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
    return ERR_SUCCESS;
}


PUBLIC UINT32 CFW_SmsWriteMessage(CFW_SMS_WRITE *pSMSWrite, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO   *pSmsMOInfo = NULL;
    CFW_DIALNUMBER     sNumber;
    UINT8              nFormat = 0x00;
    UINT32             ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsWriteMessage);

    SUL_ZeroMemory8(&sNumber, SIZEOF(CFW_DIALNUMBER));
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);

    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
        return ret;
    }

    //Verify Storage
    if(pSMSWrite->nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID

        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
            pSMSWrite->nStorage = nSmsStorageId;
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }

    if(pSMSWrite->nStorage == CFW_SMS_STORAGE_SM)
    {
        CFW_SMS_STORAGE_INFO  sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
        //Get max slot in SIM
        CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        if(pSMSWrite->nIndex > sStorageInfo.totalSlot )
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else if(pSMSWrite->nStorage == CFW_SMS_STORAGE_ME)
    {
        if(pSMSWrite->nIndex > SMS_MAX_ENTRY_IN_ME)
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CMS_INVALID_MEMORY_INDEX;
        }
    }
    else
    {

        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check msg type, currently only support pp msg
    if(pSMSWrite->nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if((pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_UNREAD)                && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_READ)                  && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_UNSENT)                && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)       && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV)  && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE) && \
            (pSMSWrite->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //Get SMS format from user setting
    CFW_CfgGetSmsFormat(&nFormat, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SmsWriteMessage nFormat=0x%x\n ",0x081010bb), nFormat);
    if(nFormat)//Text
    {
        UINT8 sTmpNumber[2 * TEL_NUMBER_MAX_LEN], uTmpNumberSize; //added by fengwei 20080709

        SUL_ZeroMemory8(&sTmpNumber, 2 * TEL_NUMBER_MAX_LEN);
        //Check the user input data, do more later
        if(!pSMSWrite->pData)
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CFW_INVALID_PARAMETER ;
        }

        if(!pSMSWrite->sNumber.pDialNumber)
            pSMSWrite->sNumber = sNumber;

        if((pSMSWrite->sNumber.nClir != 0) && (pSMSWrite->sNumber.nClir != 1) && (pSMSWrite->sNumber.nClir != 2))
        {
            //default value
            pSMSWrite->sNumber.nClir = 0;
        }
        if((pSMSWrite->sNumber.nType != CFW_TELNUMBER_TYPE_INTERNATIONAL) &&  \
                (pSMSWrite->sNumber.nType != CFW_TELNUMBER_TYPE_NATIONAL) &&      \
                (pSMSWrite->sNumber.nType != CFW_TELNUMBER_TYPE_UNKNOWN))
        {
            //default value
            pSMSWrite->sNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }
        if(pSMSWrite->sNumber.nDialNumberSize > TEL_NUMBER_MAX_LEN )
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CFW_INVALID_PARAMETER ;
        }

        uTmpNumberSize = SUL_GsmBcdToAscii(pSMSWrite->sNumber.pDialNumber, pSMSWrite->sNumber.nDialNumberSize, sTmpNumber);
        //hameina delete the codes below, I think the sul_** can return the size correctly
#if 0
        if(0xf0 == (*(pSMSWrite->sNumber.pDialNumber + pSMSWrite->sNumber.nDialNumberSize - 1) & 0xf0))
            uTmpNumberSize = (UINT8)(pSMSWrite->sNumber.nDialNumberSize * 2 - 1);
        else
            uTmpNumberSize = (UINT8)(pSMSWrite->sNumber.nDialNumberSize * 2);
#endif
        //Malloc for Handle
        pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize + pSMSWrite->nDataSize);
        if(!pSmsMOInfo)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsWriteMessage pSmsMOInfo malloc error!!! \n ",0x081010bc));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CMS_MEMORY_FULL;
        }
        SUL_ZeroMemory8(pSmsMOInfo, (SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize + pSMSWrite->nDataSize)); //modified by fengwei 20080710 for bug 8698

        //Set user input data to handle
        pSmsMOInfo->sNumber.nClir           = pSMSWrite->sNumber.nClir;
        pSmsMOInfo->sNumber.nType           = pSMSWrite->sNumber.nType;
        pSmsMOInfo->sNumber.nDialNumberSize = uTmpNumberSize;
        pSmsMOInfo->sNumber.pDialNumber     = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO));
        SUL_MemCopy8((UINT8 *)pSmsMOInfo->sNumber.pDialNumber, &sTmpNumber, uTmpNumberSize);

        pSmsMOInfo->nDataSize = pSMSWrite->nDataSize;

        if(pSMSWrite->nDataSize)
            pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO) + uTmpNumberSize);
        else
            pSmsMOInfo->pData = NULL;

        //CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID|C_DETAIL)|TDB|TNB_ARG(4), TSTR("---CFW_SmsWriteMessage, write a text sms,nDataSize is %d, nStorage is %d , nIndex is %d , nStatus is %d\n ",0x081010bd),nDataSize,nStorage,nIndex,nStatus);
    }
    else// Write a PDU
    {
        //Malloc for Handle
        pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO) + pSMSWrite->nDataSize);
        if(!pSmsMOInfo)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsWriteMessage pSmsMOInfo malloc error!!! \n ",0x081010be));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ERR_CMS_MEMORY_FULL;
        }
        SUL_ZeroMemory8(pSmsMOInfo, (SIZEOF(CFW_SMS_MO_INFO) + pSMSWrite->nDataSize));

        pSmsMOInfo->nDataSize = pSMSWrite->nDataSize;
        pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + SIZEOF(CFW_SMS_MO_INFO));;

        //CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID|C_DETAIL)|TDB|TNB_ARG(4), TSTR("---CFW_SmsWriteMessage, write a PDU sms,nDataSize is %d, nStorage is %d , nIndex is %d , nStatus is %d\n ",0x081010bf),nDataSize,nStorage,nIndex,nStatus);
    }

    SUL_MemCopy8(pSmsMOInfo->pData, pSMSWrite->pData, pSMSWrite->nDataSize);

    pSmsMOInfo->nLocation = pSMSWrite->nStorage;
    pSmsMOInfo->nFormat = nFormat;
    pSmsMOInfo->nIndex  = pSMSWrite->nIndex;
    pSmsMOInfo->nStatus = pSMSWrite->nStatus;
    pSmsMOInfo->nUTI_UserInput = nUTI;
    pSmsMOInfo->nUTI_Internal  = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_WriteAoProc, nSimId);

    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
    return ERR_SUCCESS;
}



PUBLIC UINT32 CFW_SmsListMessages(CFW_SMS_LIST *pListInfo, UINT16 nUTI, CFW_SIM_ID nSimId)
{
    CFW_SMS_MO_INFO  *pSmsMOInfo = NULL;
    UINT32            ret = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsListMessages);

    //Verify input parameters
    ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
        return ret;
    }
    //check simid
    ret = CFW_CheckSimId(nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check msg type, currently only support pp msg
    if(pListInfo->nType != CFW_SMS_TYPE_PP)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsReadMessage);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if((pListInfo->nStorage != CFW_SMS_STORAGE_ME)     && \
            (pListInfo->nStorage != CFW_SMS_STORAGE_SM)     && \
            (pListInfo->nStorage != CFW_SMS_STORAGE_AUTO))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(pListInfo->nStartIndex <= 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //Verify Storage
    if(pListInfo->nStorage == CFW_SMS_STORAGE_AUTO)
    {
        UINT8  nSmsStorageId = 0x0;
        //Get SMS StorageID
        ret = CFW_CfgGetSmsStorage(&nSmsStorageId, nSimId);
        if (ret == ERR_SUCCESS)
        {
            if((nSmsStorageId != CFW_SMS_STORAGE_ME)  && \
                    (nSmsStorageId != CFW_SMS_STORAGE_SM))
            {
                CSW_SMS_FREE(pSmsMOInfo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
                return ERR_CFW_INVALID_PARAMETER;
            }
            pListInfo->nStorage = nSmsStorageId;
        }
        else
        {
            CSW_SMS_FREE(pSmsMOInfo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }

    if(pListInfo->nStorage == CFW_SMS_STORAGE_SM)
    {
        CFW_SMS_STORAGE_INFO  sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
        //Get max slot in SIM
        CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
        if(pListInfo->nStartIndex > sStorageInfo.totalSlot )
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
            return ERR_CFW_INVALID_PARAMETER ;
        }
    }
    else if(pListInfo->nStorage == CFW_SMS_STORAGE_ME)
    {
        if(pListInfo->nStartIndex > SMS_MAX_ENTRY_IN_ME)
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The SMS_MAX_ENTRY_IN_ME is %d \n",0x081010c0), SMS_MAX_ENTRY_IN_ME);
            return ERR_CFW_INVALID_PARAMETER ;
        }
    }
    if(pListInfo->nStatus == 0xff)
        pListInfo->nStatus = CFW_SMS_STORED_STATUS_UNREAD;
    if(pListInfo->nStatus == CFW_SMS_STORED_STATUS_STORED_ALL)
        pListInfo->nStatus = 0x7f;
    if(!(pListInfo->nOption & CFW_SMS_LIST_OPTION__TIME_DESCEND_ORDER))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(!(pListInfo->nOption & CFW_SMS_MULTIPART_OPTION_ALL) && !(pListInfo->nOption & CFW_SMS_MULTIPART_OPTION_FIRST))
        pListInfo->nOption = (UINT8)(pListInfo->nOption & CFW_SMS_MULTIPART_OPTION_ALL);

    //Malloc for Handle
    pSmsMOInfo = (CFW_SMS_MO_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MO_INFO));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsListMessages pSmsMOInfo malloc error!!! \n ",0x081010c1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_MO_INFO));

    //Set value to the handle
    pSmsMOInfo->nLocation = pListInfo->nStorage;
    pSmsMOInfo->i               = 0x00;
    pSmsMOInfo->nListCount      = pListInfo->nCount;
    pSmsMOInfo->nStatus         = pListInfo->nStatus;
    pSmsMOInfo->nListStartIndex = pListInfo->nStartIndex;
    pSmsMOInfo->nListOption     = pListInfo->nOption;
    pSmsMOInfo->nUTI_UserInput  = nUTI;
    pSmsMOInfo->nUTI_Internal   = (UINT16)(CFW_APP_UTI_MIN + nUTI);

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_ListAoProc, nSimId);
    CFW_SetUTI(pSmsMOInfo->hAo, nUTI, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);

    //CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID|C_DETAIL)|TDB|TNB_ARG(4), TSTR("---CFW_SmsListMessages ,nStorage is %d, nStatus is %d , nCount is %d , nStartIndex is, nOption is %d\n ",0x081010c2),nStorage,nStatus,nCount,nStartIndex,nOption);

    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsListMessages);
    return ERR_SUCCESS;
}


PUBLIC UINT32 SRVAPI CFW_SmsGetMessageNode(PVOID pListResult, UINT16 nIndex, CFW_SMS_NODE **pNode, CFW_SIM_ID nSimId)
{
    UINT8   nFormat = 0x0, nShow = 0x0;
    UINT32  sDecomposeOutData = 0x0, ret = 0x0;
    UINT16  nListAddrCount = 0x0;
    CFW_SMS_NODE *pListAddr = NULL;
    CFW_SMS_MULTIPART_INFO sLongerMsg;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsGetMessageNode);

    CFW_CfgGetSmsFormat(&nFormat, nSimId);
    CFW_CfgGetSmsShowTextModeParam(&nShow, nSimId);

    //Zero memory
    SUL_ZeroMemory8(&sLongerMsg, SIZEOF(CFW_SMS_MULTIPART_INFO));

    if(!pListResult)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
        return ERR_CFW_INVALID_PARAMETER ;
    }
    if(!pNode)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
        return ERR_CFW_INVALID_PARAMETER ;
    }
    //Set value
    nListAddrCount = *((UINT16 *)pListResult);
    if(nListAddrCount == 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
        return ERR_CFW_INVALID_PARAMETER ;
    }
    if((nIndex + 1) > nListAddrCount)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
        return ERR_CFW_INVALID_PARAMETER ;
    }

    pListAddr = (CFW_SMS_NODE *)(*((UINT32 *)pListResult + 1 + nIndex));
    if(!pListAddr)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
        return ERR_CFW_INVALID_PARAMETER ;
    }

    ret = sms_mo_Parse_PDU(nFormat, nShow, (UINT8)pListAddr->nStatus, pListAddr->nStorage, pListAddr->pNode, &pListAddr->nType, &sDecomposeOutData, &sLongerMsg);
    if((ret != ERR_SUCCESS) || (sDecomposeOutData == 0))
    {
        CFW_SMS_NODE *pTmpNode = NULL;
        pTmpNode   = (CFW_SMS_NODE *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO) + SIZEOF(CFW_SMS_NODE));
        if(!pTmpNode)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsGetMessageNode pSmsMOInfo malloc error!!! \n ",0x081010c3));
            CSW_SMS_FREE(pListAddr);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
            return ERR_NO_MORE_MEMORY ;
        }
        SUL_ZeroMemory8(pTmpNode, (SIZEOF(CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO) + SIZEOF(CFW_SMS_NODE)));
        pTmpNode->pNode = (PVOID)((UINT8 *)pTmpNode + sizeof(CFW_SMS_NODE));
        sDecomposeOutData = (UINT32)pTmpNode;
    }
    //Set value
    ((CFW_SMS_NODE *)sDecomposeOutData)->nConcatPrevIndex     = pListAddr->nConcatPrevIndex;
    ((CFW_SMS_NODE *)sDecomposeOutData)->nConcatCurrentIndex  = pListAddr->nConcatCurrentIndex;
    ((CFW_SMS_NODE *)sDecomposeOutData)->nConcatNextIndex     = pListAddr->nConcatNextIndex;
    ((CFW_SMS_NODE *)sDecomposeOutData)->nStatus  = pListAddr->nStatus;
    ((CFW_SMS_NODE *)sDecomposeOutData)->nType    = pListAddr->nType;
    ((CFW_SMS_NODE *)sDecomposeOutData)->nStorage = pListAddr->nStorage;
    CSW_SMS_FREE(pListAddr);
    *pNode = (CFW_SMS_NODE *)sDecomposeOutData;
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsGetMessageNode);
    return ERR_SUCCESS;
}


// This function is used by the MMI to send a notification to stack that
// the user has canceled to send SMS.
UINT32 CFW_SmsAbortSendMessage(CFW_SIM_ID nSimID)
{
    api_SmsPPErrorInd_t *pSmsPPErrorInd = NULL;
    pSmsPPErrorInd = (api_SmsPPErrorInd_t *)CSW_SMS_MALLOC(SIZEOF(api_SmsPPErrorInd_t));
    if(!pSmsPPErrorInd)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsAbortMessage malloc error!!! \n ",0x081010c4));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsPPErrorInd, SIZEOF(api_SmsPPErrorInd_t));
    pSmsPPErrorInd->CauseLen = 0x01;
    pSmsPPErrorInd->Cause[0] = SMS_MM_OTHER_FALIURE;
    pSmsPPErrorInd->DataLen = 0x00;
    pSmsPPErrorInd->ErrorType = 0x00;

    //pSmsPPErrorInd->Data = NULL;

    CFW_BalSendMessage(CFW_MBX, API_SMSPP_ERROR_IND, pSmsPPErrorInd, SIZEOF(api_SmsPPErrorInd_t), nSimID);
    CSW_SMS_FREE(pSmsPPErrorInd);
    pSmsPPErrorInd = NULL;

    api_SmsPPAbortReq_t *pSmsPPAbortReq = NULL;
    pSmsPPAbortReq = (api_SmsPPAbortReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SmsPPAbortReq_t));
    if(!pSmsPPAbortReq)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsAbortMessage malloc error!!! \n ",0x081010c5));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsPPAbortReq, SIZEOF(api_SmsPPSendMMAReq_t));

    //Set Value
    pSmsPPAbortReq->Path = 0; //0:GSM, 1:GPRS

    //Send MSG to SCL
    CFW_SendSclMessage(API_SMSPP_ABORT_REQ, pSmsPPAbortReq, nSimID);


    return ERR_SUCCESS;

}


CFW_SMS_INIT_STATUS gNewSMSInd[CFW_SIM_COUNT] = {CFW_SMS_NOINITED, CFW_SMS_NOINITED};
UINT32 CFW_SmsInitComplete(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    CFW_SMS_INIT *pSmsInit = NULL;
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SmsInitComplete() start\r\n",0x081010c6)));
    if( nSimID >= CFW_SIM_COUNT )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SmsInitComplete()\r\n",0x081010c7)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsInitComplete);
        return ERR_CFW_INVALID_PARAMETER;
    }
    pSmsInit = (CFW_SMS_INIT *)CSW_SIM_MALLOC(SIZEOF(CFW_SMS_INIT));
    if(pSmsInit == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_SmsInitComplete() data error\r\n",0x081010c8)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsInitComplete);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(pSmsInit, SIZEOF(CFW_SMS_INIT));
    pSmsInit->n_CurrStatus      = CFW_SM_STATE_IDLE;
    pSmsInit->n_PrevStatus      = CFW_SM_STATE_IDLE;

    hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsInit, CFW_SmsInitCompleteProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 1);
    CFW_SetServiceId(CFW_APP_SRV_ID);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SmsInitComplete() end\r\n",0x081010c9)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsInitComplete);
    return ret;
}


BOOL g_SMSConcat[CFW_SIM_COUNT] = {FALSE,};
UINT32 CFW_SetSMSConcat (BOOL bConcat, CFW_SIM_ID nSimID)
{
    if( nSimID >= CFW_SIM_COUNT )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SendMultSMS()\r\n",0x081010ca)));
        return ERR_CFW_INVALID_PARAMETER;
    }
    g_SMSConcat [nSimID] = bConcat;
    return g_SMSConcat [nSimID];
}

BOOL CFW_GetSMSConcat (CFW_SIM_ID nSimID)
{
    return g_SMSConcat [nSimID];
}
/**********************************************************************************/
/**********************************************************************************/

#endif // ENABLE 

