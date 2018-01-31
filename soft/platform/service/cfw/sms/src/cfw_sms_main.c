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
















#include <cos.h>
#include "csw_debug.h"

#include "cfw_sms_prv.h"
//Here, define a golobal variable for longer msg
extern UINT32 *gpLongerMsgPara;
extern UINT32 *gpLongerMsgParaIndex;
extern CFW_SMS_LONG_INFO *gpWriteLongerMsgPara;
extern UINT8   gnWriteLongerMsgParaCount;
//for long sms
extern PSTR pSMSDir;
extern PSTR pSIMFilename;
extern PSTR pMEFilename;
extern BOOL gSMSInitFlag;

#ifdef SMSEX
UINT32 sms_db_InitEX(void);
#endif

extern CFW_SMS_INIT_STATUS gNewSMSInd[CFW_SIM_COUNT];
extern BOOL gPendingNewSms[CFW_SIM_COUNT];

#if (CFW_SMS_DUMP_ENABLE==0) && (CFW_SMS_SRV_ENABLE==1)

/**********************************************************************************/
/**********************************************************************************/
UINT8 gSmsService = 1;
UINT8 CFW_GprsSetSmsSeviceMode(UINT8 nService)
{
    gSmsService = nService;
    return 0;
}
UINT8 CFW_GprsGetSmsSeviceMode(void)
{
    return gSmsService;
}
UINT32 SRVAPI CFW_SmsMoInit(
    UINT16 nUti
    , CFW_SIM_ID nSimID
)
{
    CFW_SMS_INIT_HANDLE  *pSmsMOInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsMoInit);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Start CFW_SmsMoInit, SimID:%d\n",0x08100fbf)), nSimID);
    //Malloc for Handle
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Start CFW_SmsMoInit, SMS_Disp_GetTime : %d Secn  \n",0x08100fc0)), (UINT32)(SMS_Disp_GetTime() / 256));
    pSmsMOInfo = (CFW_SMS_INIT_HANDLE *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_INIT_HANDLE));
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SmsMoInit pSmsMOInfo malloc error!!! \n ",0x08100fc1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsMoInit);
        return ERR_CMS_MEMORY_FULL;
    }
    SUL_ZeroMemory8(pSmsMOInfo, SIZEOF(CFW_SMS_INIT_HANDLE));

    //Register CallBack into system
    pSmsMOInfo->hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pSmsMOInfo, sms_InitAoProc, nSimID);
    CFW_SetUTI(pSmsMOInfo->hAo, nUti, 0);
    CFW_SetAoProcCode(pSmsMOInfo->hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsMoInit);
    return ERR_SUCCESS;
}

UINT32 SRVAPI CFW_SmsMtInit(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsMtInit);
    CFW_RegisterCreateAoProc(API_SMSPP_RECEIVE_IND, (PFN_EV_IND_TRIGGER_PORC)sms_mt_Reg);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsMtInit);
    return ERR_SUCCESS;
}
#ifdef CB_SUPPORT
UINT32 SRVAPI CFW_SmsCbInit(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsCbInit);
    CFW_RegisterCreateAoProc(API_SMSCB_RECEIVE_IND, (PFN_EV_IND_TRIGGER_PORC)sms_cb_Reg);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsCbInit);
    return ERR_SUCCESS;
}
#endif
HAO sms_mt_Reg(CFW_EV *pEvent)
{
    CFW_SMS_MT_INFO    *pSmsMTInfo = NULL; //Point to MT private data.
    CSW_PROFILE_FUNCTION_ENTER(sms_mt_Reg);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    nSimId = pEvent->nFlag;
    pEvent = pEvent;
    pSmsMTInfo = (CFW_SMS_MT_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_MT_INFO));
    if(!pSmsMTInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_mt_Reg pSmsMTInfo malloc error!!! \n ",0x08100fc2));
        CSW_PROFILE_FUNCTION_EXIT(sms_mt_Reg);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsMTInfo, SIZEOF(CFW_SMS_MT_INFO));

    //Register CallBack into system
    pSmsMTInfo->hAo = CFW_RegisterAo(CFW_SMS_MT_SRV_ID, pSmsMTInfo, sms_MTAoProc, nSimId);

    CSW_PROFILE_FUNCTION_EXIT(sms_mt_Reg);
    return pSmsMTInfo->hAo;
}


#ifdef CB_SUPPORT

HAO sms_cb_Reg(CFW_EV *pEvent)
{
    CFW_SMS_CB_INFO    *pSmsCBInfo = NULL; //Point to MT private data.
    CSW_PROFILE_FUNCTION_ENTER(sms_cb_Reg);

    pEvent = pEvent;
    pSmsCBInfo = (CFW_SMS_CB_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_CB_INFO));

    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    nSimId = pEvent->nFlag;
    if(!pSmsCBInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_mt_Reg pSmsCBInfo malloc error!!! \n ",0x08100fc3));
        CSW_PROFILE_FUNCTION_EXIT(sms_cb_Reg);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsCBInfo, SIZEOF(CFW_SMS_CB_INFO));

    //Register CallBack into system
    pSmsCBInfo->hAo = CFW_RegisterAo(CFW_SMS_CB_ID, pSmsCBInfo, sms_CBAoProc,nSimId);

    CSW_PROFILE_FUNCTION_EXIT(sms_cb_Reg);
    return pSmsCBInfo->hAo;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//                         Local Proc For SMS MO APIs                        //
//                          Invoked by CFW_RegisterAo                        //
///////////////////////////////////////////////////////////////////////////////
UINT32 sms_ReadAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           ret = 0x0, nGetUti = 0x0, nState = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL;   //Point to MO private data.
    CFW_EV           sMeEvent;
    UINT8            nSmsStatus = 0x0;
    CFW_SIM_ID  nSimId = CFW_SIM_COUNT;
    CSW_PROFILE_FUNCTION_ENTER(sms_ReadAoProc);
    CFW_GetSimID(hAo, &nSimId);



    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc, 1,get private data fail! \n ",0x08100fc4));
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_MEMORY_FAILURE, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, nState = 0x%x\n",0x08100fc5), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
              )
            {
                ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);

                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc, 3,get private data fail! \n ",0x08100fc6));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                   )
            {
                //Malloc for the read data.
                sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                if(!(UINT8 *)sMeEvent.nParam1)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc sMeEvent.nParam1 malloc error!!! \n ",0x08100fc7));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                ret = CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("sms_ReadAoProc,5, pSmsMOInfo->nIndex is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08100fc8),
                          pSmsMOInfo->nIndex, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                if(ret == ERR_SUCCESS)
                {
                    //Go to CFW_SM_STATE_WAIT directly!
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    pEvent = &sMeEvent;
                    pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                }
                else  //other return value
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 6,CFW_MeReadMessage, ret =0x%x\n ",0x08100fc9), ret);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                    CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            if(((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)) && \
                    (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                UINT32 sOutData = 0x0;
                if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc \n ",0x08100fca));
                    if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM) || (pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME))
                    {
                        CFW_SMS_MULTIPART_INFO sLongerMsg;
                        //Zero memory
                        SUL_ZeroMemory8(&sLongerMsg, SIZEOF(CFW_SMS_MULTIPART_INFO));

                        //Just get SMS status
                        ret = sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 0, NULL, &nSmsStatus, NULL);
                        if(ret != ERR_SUCCESS || nSmsStatus == 0)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 7,sms_tool_DecomposePDU, ret =0x%x\n ",0x08100fcb), ret);
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_INVALID_CHARACTER_IN_PDU, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);

                            if( NULL != (VOID *)(pEvent->nParam1) )
                            {
                                CSW_SMS_FREE(pEvent->nParam1);
                                pEvent->nParam1 = (UINT32)NULL;
                            }
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        //Parse the data
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("befor sms_mo_Parse_PDU, nSmsStatus=0x%x, pEvent->nType=0x%x\n ",0x08100fcc), nSmsStatus, pEvent->nType);
                        ret = sms_mo_Parse_PDU(pSmsMOInfo->nFormat, pSmsMOInfo->nPath, nSmsStatus, pSmsMOInfo->nLocation, (PVOID)pEvent->nParam1, &pEvent->nType, &sOutData, &sLongerMsg);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("after sms_mo_Parse_PDU, nSmsStatus=0x%x, ret=0x%x, sLongerMsg.count=%d\n ",0x08100fcd), nSmsStatus, ret, sLongerMsg.count);

                        //CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("after sms_mo_Parse_PDU, dcs = 0x%x \n ",0x08100fce), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO*)((CFW_SMS_NODE*)sOutData)->pNode)->dcs);

                        if(ret != ERR_SUCCESS)//other data, such as Status report or Command
                        {
                            //[[xueww[mod] 2007.07.30
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_mo_Parse_PDU, error ret =0x%x\n ",0x08100fcf), ret);
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                            {
                                CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                            }
                            else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                            {
                                if( NULL != (VOID *)pEvent->nParam1 )
                                {
                                    CSW_SMS_FREE(pEvent->nParam1);
                                    pEvent->nParam1 = (UINT32)NULL;
                                }
                            }
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);

                            CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        if(sLongerMsg.count > 0) // a longer msg
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 222, dcs = 0x%x \n ",0x08100fd0), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 9,The message is a long message!!!,the count is %d\r\n",0x08100fd1), sLongerMsg.count);
                            UINT16 nConcatSmsNextRec = 0x0;

                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 333, dcs = 0x%x \n ",0x08100fd2), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                            ((CFW_SMS_NODE *)sOutData)->nConcatPrevIndex = sLongerMsg.id;
                            nConcatSmsNextRec = sLongerMsg.current;
                            nConcatSmsNextRec = (nConcatSmsNextRec << 8) | sLongerMsg.count;
                            ((CFW_SMS_NODE *)sOutData)->nConcatNextIndex = nConcatSmsNextRec;
                            //]]xueww[mod] 2007.03.14
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 444, dcs = 0x%x \n ",0x08100fd3), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                        }
                        else // a normal short message
                        {
                            ((CFW_SMS_NODE *)sOutData)->nConcatPrevIndex = 0xffff ;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 555, dcs = 0x%x \n ",0x08100fd4), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                        }

                        //Set value
                        ((CFW_SMS_NODE *)sOutData)->nConcatCurrentIndex   = pSmsMOInfo->nIndex;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("33, nSmsStatus=0x%x, ret=0x%x, sLongerMsg.count=%d\n ",0x08100fd5), nSmsStatus, ret, sLongerMsg.count);
                        ((CFW_SMS_NODE *)sOutData)->nStatus               = nSmsStatus;
                        ((CFW_SMS_NODE *)sOutData)->nType                 = pEvent->nType;
                        ((CFW_SMS_NODE *)sOutData)->nStorage              = pSmsMOInfo->nLocation;

                        //add by qidd for bug 13605 ---begin
                        pSmsMOInfo->sOutData                             = ((CFW_SMS_NODE *)sOutData);
                        //add by qidd for bug 13605 ---end

                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 666, dcs = 0x%x \n ",0x08100fd6), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);

                        //If the unread SMS is read by this function, the SMS message status will be changed to read status by CFW automaticly
                        if(nSmsStatus == CFW_SMS_STORED_STATUS_UNREAD)
                        {
                            CFW_EV  sMeEvent1;
                            CFW_SMS_STORAGE_INFO  sStorageInfo;

                            //Zero memory
                            SUL_ZeroMemory8(&sMeEvent1, SIZEOF(CFW_EV));
                            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));

                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 777, dcs = 0x%x \n ",0x08100fd7), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                            CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 888, dcs = 0x%x \n ",0x08100fd8), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);

                            if(sStorageInfo.unReadRecords > 0)
                                sStorageInfo.unReadRecords--;
                            if(sStorageInfo.readRecords < sStorageInfo.totalSlot)
                                sStorageInfo.readRecords++;
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);

                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 999, dcs = 0x%x \n ",0x08100fd9), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);

                            *((UINT8 *)pEvent->nParam1) = CFW_SMS_STORED_STATUS_READ;

                            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, AAA, dcs = 0x%x \n ",0x08100fda), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                                //Write it with CFW_SMS_STORED_STATUS_READ
                                ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, BBB, dcs = 0x%x \n ",0x08100fdb), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                            }
                            else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                            {
                                UINT32 nTime = 0x0, nAppInt32 = 0x0;
                                TM_FILETIME nFileTime;
                                TM_GetSystemFileTime(&nFileTime);
                                nTime = nFileTime.DateTime;
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, CCC, dcs = 0x%x \n ",0x08100fdc), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                                ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent1);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, DDD, dcs = 0x%x \n ",0x08100fdd), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("sms_ReadAoProc, 10,CFW_MeWriteMessage ,pSmsMOInfo->nIndex is %d, sMeEvent1.nParam1 is (0x)%x , sMeEvent1.nParam2 is (0x)%x , sMeEvent1.nType is (0x)%x\n ",0x08100fde), pSmsMOInfo->nIndex, sMeEvent1.nParam1, sMeEvent1.nParam2, sMeEvent1.nType);
                            }

                            if(ret != ERR_SUCCESS)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 11,save status write me/sim ret=0x%x\n ",0x08100fdf), ret);
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                                if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                                {
                                    CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                                }
                                else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                                {
                                    CSW_SMS_FREE((UINT8 *)pEvent->nParam1);
                                }


                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
                                return ERR_CMS_UNKNOWN_ERROR;
                            }
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, FFF, dcs = 0x%x \n ",0x08100fe0), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);

                            pSmsMOInfo->nReadSmsStatus = CFW_SMS_STORED_STATUS_UNREAD;
                        }
                        if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                        {
                            if(sMeEvent.nParam1)
                            {
                                CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                                sMeEvent.nParam1 = (UINT32)NULL;
                            }
                        }
                        else if( EV_CFW_SIM_READ_MESSAGE_RSP == pEvent->nEventId)
                        {
                            if(pEvent->nParam1)
                            {
                                CSW_SMS_FREE((UINT8 *)(pEvent->nParam1));
                                pEvent->nParam1 = (UINT32)NULL;
                            }
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, GGG, dcs = 0x%x \n ",0x08100fe1), ((CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)((CFW_SMS_NODE *)sOutData)->pNode)->dcs);

                        if(sLongerMsg.count > 0) // a longer msg
                        {
                            //[[xueww[mod] 2007.06.13
                            pEvent->nParam2 = sLongerMsg.current;
                            pEvent->nParam2 = ( (pEvent->nParam2) << 16) | sLongerMsg.count;
                            //]]xueww[mod] 2007.06.13
                        }
                        else
                        {
                            pEvent->nParam2 = 0;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ReadAoProc, 11.5pEvent->nParam2=0x%x\n ",0x08100fe2), pEvent->nParam2);

                        pEvent->nType = 0;
                    }
                }
                //Read a NULL SIM
                else if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1 == 0))
                {
                    sOutData = ERR_CMS_INVALID_MEMORY_INDEX;
                    pEvent->nParam2 = pSmsMOInfo->nIndex;
                    pEvent->nType = 0xf0;
                }
                else if(pEvent->nType == 0xf0)
                {
                    pEvent->nParam2 = pSmsMOInfo->nIndex;
                    if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                    {
                        sOutData = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)//Me Return error
                    {
                        sOutData = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                    {
                        sOutData = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                    {
                        sOutData = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                    {
                        sOutData = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                    {
                        sOutData = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else
                    {
                        sOutData = ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                else
                {
                    pEvent->nType   = 0xf0;
                    pEvent->nParam2 = pSmsMOInfo->nIndex;
                    sOutData        = ERR_CMS_UNKNOWN_ERROR;
                }
                CFW_GetUTI(hAo, &nGetUti);

                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("sms_ReadAoProc, 13,post event to mmi, param1 =0x%x, param2=0x%x, type = 0x%x\n ",0x08100fe3),
                          sOutData, pEvent->nParam2, pEvent->nType);

                //modify by qidd for bug 13605  ---begin
                if(!((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)  &&  (pSmsMOInfo->nReadSmsStatus == CFW_SMS_STORED_STATUS_UNREAD)))
                {
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, sOutData, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                    //modify by qidd for bug 13605  ---end
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc read CFW_UnRegisterAO\n ",0x08100fe4));
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }
            else if((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) && (pSmsMOInfo->nUTI_Internal == pEvent->nTransId))
            {
                if(pEvent->nType == 0)
                {
                    //modify a UTI problem,have a branch without GET UTI from AO.
                    CFW_GetUTI(hAo, &nGetUti);
                    //modify by qidd for bug 13605  ---begin
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, (UINT32)(pSmsMOInfo->sOutData), pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                    //modify by qidd for bug 13605  ---end

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                else if(pEvent->nType == 0xf0)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("sms_ReadAoProc, 14,post event to mmi, param1 =0x%x, param2=0x%x, type = 0x%x\n ",0x08100fe5),
                              ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, pEvent->nType);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_READ_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);

                    //add by qidd for bug 13605 ---begin
                    CSW_SMS_FREE(pSmsMOInfo->sOutData);
                    pSmsMOInfo->sOutData = NULL;

                    //add by qidd for bug 13605 ---end

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ReadAoProc  error\n",0x08100fe6));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_ReadAoProc);
    return ERR_SUCCESS;
}


UINT32 sms_DleteAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           nState = 0x0, nGetUti = 0x0, ret = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL; //Point to MO private data.
    CFW_EV           sMeEvent;
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CSW_PROFILE_FUNCTION_ENTER(sms_DleteAoProc);

    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));

    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
        return ERR_NO_MORE_MEMORY;
    }
    CFW_GetSimID(hAo, &nSimId);

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_DleteAoProc, nState = 0x%x\n",0x08100fe7), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            ret = 0x0;
            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
            {
                if(pSmsMOInfo->nIndex) // delete by index, read it first, got its status
                {
                    ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                    if(ret == ERR_SUCCESS)
                    {
                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    }
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                else //by status, get the number of specified status
                {
                    ret = CFW_SimDeleteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nStatus, pSmsMOInfo->nUTI_Internal, nSimId);
                    if(ret == ERR_SUCCESS)
                    {
                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    }
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                break;
            }
            else if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                   )
            {
                if(pSmsMOInfo->nIndex) // delete by index, read it first, got its status
                {
                    sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                    if(!(UINT8 *)sMeEvent.nParam1)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_mt_Reg pSmsMOInfo malloc error!!! \n ",0x08100fe8));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_NO_MORE_MEMORY, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        return ERR_NO_MORE_MEMORY;
                    }
                    SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                    CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("---sms_DleteAoProc--CFW_MeReadMessage ,pSmsMOInfo->nIndex is %d , sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08100fe9), pSmsMOInfo->nIndex , sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                    pEvent = &sMeEvent;
                    if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                    {
                        //Just get SMS status
                        sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 0, NULL, &(pSmsMOInfo->nStatus), NULL);
                    }
                    //Read a NULL SIM
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                }
                else //by status, get the number of specified status
                {
                    sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_STORAGE_INFO)));
                    if((UINT8 *)sMeEvent.nParam1 == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_DleteAoProc sMeEvent.nParam1 malloc error!!! \n ",0x08100fea));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_NO_MORE_MEMORY, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SIZEOF(CFW_SMS_STORAGE_INFO));
                    //Get ME totalSlot and usedSlot
                    CFW_MeGetStorageInfo(nSimId, pSmsMOInfo->nStatus, &sMeEvent);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("---sms_DleteAoProc--CFW_MeGetStorageInfo ,pSmsMOInfo->nStatus is %d , sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08100feb), pSmsMOInfo->nStatus , sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo usedSlot = %d \n ",0x08100fec), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->usedSlot);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo totalSlot = %d \n ",0x08100fed), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->totalSlot);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo unReadRecords = %d \n ",0x08100fee), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->unReadRecords);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo readRecords = %d \n ",0x08100fef), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->readRecords);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo sentRecords = %d \n ",0x08100ff0), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->sentRecords);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo unsentRecords = %d \n ",0x08100ff1), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->unsentRecords);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo unknownRecords = %d \n ",0x08100ff2), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->unknownRecords);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("After CFW_MeGetStorageInfo storageId = %d \n ",0x08100ff3), ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->storageId);


                    if(sMeEvent.nType == 0) // use nListCount to record the number
                        pSmsMOInfo->nListCount = ((CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1))->usedSlot;
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, 0, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0);
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                        return ERR_SUCCESS;
                    }

                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Delete   ME message by status, pSmsMOInfo->nStatus =(0x) %x  \n ",0x08100ff4), pSmsMOInfo->nStatus);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Delete   ME message by status, pSmsMOInfo->nListCount =(0x) %x  \n ",0x08100ff5), pSmsMOInfo->nListCount );
                    CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                }
                ret = CFW_MeDeleteMessage(nSimId, pSmsMOInfo->nIndex, pSmsMOInfo->nStatus, &sMeEvent);
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("sms_DleteAoProc  CFW_MeDeleteMessage ,pSmsMOInfo->nIndex is %d ,pSmsMOInfo->nStatus is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08100ff6), pSmsMOInfo->nIndex , pSmsMOInfo->nStatus, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                if(ret == ERR_SUCCESS)
                {
                    //Go to CFW_SM_STATE_WAIT directly!
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    pEvent = &sMeEvent;
                    pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                }
                else  //other return value
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            // delete by index in SIM, after reading it, got its status
            if((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP) && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                {
                    //Just get SMS status
                    sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 0, NULL, &(pSmsMOInfo->nStatus), NULL);
                    if(pEvent->nParam1)
                    {
                        CSW_SMS_FREE((UINT8 *)(pEvent->nParam1));
                        pEvent->nParam1 = (UINT32)NULL;
                    }
                }
                //Read a NULL SIM or read fail
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                ret = CFW_SimDeleteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nStatus, pSmsMOInfo->nUTI_Internal, nSimId);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if(((pEvent->nEventId == EV_CFW_SIM_DELETE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_DELETE_MESSAGE_RSP)) && \
                    (pEvent->nTransId  == pSmsMOInfo->nUTI_Internal))
            {
                UINT32 uDelCount = 0;//added by fengwei 20080715 for delete all wrong.
                if(pEvent->nType == 0)
                {
                    uDelCount = pEvent->nParam1;//added by fengwei 20080715 for delete all wrong
                    pEvent->nParam1 = pSmsMOInfo->nIndex;
                    //加上容错处理,SMS最大个数不超过10000
                    if(pEvent->nParam1 > 10000)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        pEvent->nType = 0xf0;
                    }
                    pEvent->nParam2 = pSmsMOInfo->nLocation;
                }
                else if(pEvent->nType == 0xf0)
                {
                    if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else
                    {
                        pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                else
                {
                    pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                }

                CFW_GetUTI(hAo, &nGetUti);
                CFW_PostNotifyEvent(EV_CFW_SMS_DELETE_MESSAGE_RSP, pEvent->nParam1, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);

                /*modified by fengwei 20080709 begin
                we should post EV_CFW_SMS_DELETE_MESSAGE_RSP first,
                then, if needed, post EV_CFW_SMS_INFO_IND*/

#if 1 //remove by wulc to adapt to android ,no need post EV_CFW_SMS_INFO_IND 
                if (0 == pEvent->nType)
                {
                    UINT8 nNewCount = 0x0;
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    CFW_SMS_LONG_INFO_NODE sSmsLongInfoNode;

                    //Delete SIM by status, get the number of specified status sms
                    if((pEvent->nEventId == EV_CFW_SIM_DELETE_MESSAGE_RSP) && (0 == pSmsMOInfo->nIndex))
                        pSmsMOInfo->nListCount = uDelCount;// (UINT16)pEvent->nParam1;modified by fengwei 20080715 for delete all wrong
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Delete a message ,type =0, pSmsMOInfo->nIndex = %d  \n ",0x08100ff7), pSmsMOInfo->nIndex);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Delete a message ,type =0, pSmsMOInfo->nListCount = %d  \n ",0x08100ff8), pSmsMOInfo->nListCount);

                    SUL_ZeroMemory8(&sSmsLongInfoNode, SIZEOF(sSmsLongInfoNode));
                    if((0 == pSmsMOInfo->nIndex) && pSmsMOInfo->nListCount) //delete by status
                    {
                        ret = CFW_CfgGetSmsFullForNew(&nNewCount, nSimId);
                        if(nNewCount >= 1)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("the value of nNewCount is %u\n",0x08100ff9), nNewCount);
                            CFW_GetUTI(hAo, &nGetUti);

                            //deleted by fengwei 20080715 maybe wo don't need notify
                            //CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND,0,pSmsMOInfo->nLocation,nGetUti,0);
                            sms_mo_SmsPPSendMMAReq(nSimId);
                            if(nNewCount >= pSmsMOInfo->nListCount)
                                nNewCount = (UINT8)(nNewCount - pSmsMOInfo->nListCount);
                            else
                                nNewCount = 0;
                            CFW_CfgSetSmsFullForNew(nNewCount, nSimId);
                        }
                        //Delete msg with status, then update CFW_SMS_STORAGE_INFO
                        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                        //当短信存储空间满时, 删除短信后(比如调用了CFW_SmsDeleteMessage函数),并且没有正在等待存储的短信,那么该参数为0.
                        if(sStorageInfo.usedSlot == sStorageInfo.totalSlot)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND,0,pSmsMOInfo->nLocation,nGetUti |(nSimId<<24),0);//hoper
                        }
                        if(sStorageInfo.usedSlot >= pSmsMOInfo->nListCount)
                            sStorageInfo.usedSlot = (UINT16)(sStorageInfo.usedSlot - pSmsMOInfo->nListCount);
                        else
                            sStorageInfo.usedSlot = 0;
                        if((0x7f == pSmsMOInfo->nStatus) || (CFW_SMS_STORED_STATUS_STORED_ALL == pSmsMOInfo->nStatus))
                        {
                            pSmsMOInfo->nStatus = 0x7f;
                            sStorageInfo.usedSlot = 0;
                        }
                        if(CFW_SMS_STORED_STATUS_UNREAD & pSmsMOInfo->nStatus)
                            sStorageInfo.unReadRecords = 0;
                        if(CFW_SMS_STORED_STATUS_READ & pSmsMOInfo->nStatus)
                            sStorageInfo.readRecords = 0;
                        if(CFW_SMS_STORED_STATUS_UNSENT & pSmsMOInfo->nStatus)
                            sStorageInfo.unsentRecords = 0;
                        if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ & pSmsMOInfo->nStatus)         || \
                                (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV & pSmsMOInfo->nStatus)    || \
                                (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE & pSmsMOInfo->nStatus)   || \
                                (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE & pSmsMOInfo->nStatus))
                            sStorageInfo.sentRecords = 0;
                        CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                        //Update longer sms para if it is
                        sSmsLongInfoNode.nConcatCurrentIndex = pSmsMOInfo->nIndex;
                        sSmsLongInfoNode.nStatus = pSmsMOInfo->nStatus;
#if USING_FS_SAVE_LONG_PARA
                        sms_tool_SetLongMSGPara((UINT8)pSmsMOInfo->nLocation, (UINT8 *)&sSmsLongInfoNode, 0, LONG_SMS_PARA_DELETE);
#endif
                    }

                    //modified by fengwei 20080704  for bug 8714
                    //else if(pSmsMOInfo->nStatus)
                    else if (0 != pSmsMOInfo->nIndex)//delete by index
                    {
                        ret = CFW_CfgGetSmsFullForNew(&nNewCount, nSimId);
                        if(nNewCount >= 1)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("the value of nNewCount is %u\n",0x08100ffa), nNewCount);
                            CFW_GetUTI(hAo, &nGetUti);

                            //deleted by fengwei 20080715 maybe wo don't need notify
                            //CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND,0,pSmsMOInfo->nLocation,nGetUti,0);
                            sms_mo_SmsPPSendMMAReq(nSimId);
                            nNewCount--;
                            CFW_CfgSetSmsFullForNew(nNewCount, nSimId);
                        }
                        //Delete a msg, then update CFW_SMS_STORAGE_INFO
                        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                        //当短信存储空间满时, 删除短信后(比如调用了CFW_SmsDeleteMessage函数),并且没有正在等待存储的短信,那么该参数为0.
                        if(sStorageInfo.usedSlot == sStorageInfo.totalSlot)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND,0,pSmsMOInfo->nLocation,nGetUti |(nSimId<<24),0);//hoper
                        }
                        if(sStorageInfo.usedSlot > 0)
                            sStorageInfo.usedSlot--;
                        if((CFW_SMS_STORED_STATUS_UNREAD == pSmsMOInfo->nStatus) && (sStorageInfo.unReadRecords > 0))
                            sStorageInfo.unReadRecords--;
                        else if((CFW_SMS_STORED_STATUS_READ == pSmsMOInfo->nStatus) && (sStorageInfo.readRecords > 0))
                            sStorageInfo.readRecords--;
                        else if((CFW_SMS_STORED_STATUS_UNSENT == pSmsMOInfo->nStatus) && (sStorageInfo.unsentRecords > 0))
                            sStorageInfo.unsentRecords--;
                        else if(((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == pSmsMOInfo->nStatus)         || \
                                 (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == pSmsMOInfo->nStatus)    || \
                                 (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == pSmsMOInfo->nStatus)   || \
                                 (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == pSmsMOInfo->nStatus))       && \
                                (sStorageInfo.sentRecords > 0))
                            sStorageInfo.sentRecords--;
                        CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                        //Update longer sms para if it is
                        sSmsLongInfoNode.nConcatCurrentIndex = pSmsMOInfo->nIndex;
                        sSmsLongInfoNode.nStatus = pSmsMOInfo->nStatus;

                    }

                }//
#endif

                //modified by fengwei 20080709 end.
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
            }
            break;
        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_DleteAoProc  error\n",0x08100ffb));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_DleteAoProc);
    return ERR_SUCCESS;
}

UINT32 sms_ListAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           nGetUti = 0x0, nState = 0x0, ret = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL; //Point to MO private data.
    CFW_EV           sMeEvent;
    CFW_SMS_STORAGE_INFO  sStorageInfo;

    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CSW_PROFILE_FUNCTION_ENTER(sms_ListAoProc);

    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    CFW_GetSimID(hAo, &nSimId);
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);

        CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);

    //modify by qidd for bug 13978  ---begin
    CFW_CfgGetSmsStorageInfo(&sStorageInfo, /*CFW_SMS_STORAGE_ME*/pSmsMOInfo->nLocation, nSimId);
    //modify by qidd for bug 13978  ---end

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("sms_ListAoProc, nState = 0x%x, location = 0x%x,usedslot = 0x%x\n",0x08100ffc),
              nState, pSmsMOInfo->nLocation, sStorageInfo.usedSlot);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
            {
                ret = CFW_SimListMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nStatus, pSmsMOInfo->nListCount, pSmsMOInfo->nListStartIndex, pSmsMOInfo->nUTI_Internal, nSimId);

                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }

                else
                {
                    CFW_GetUTI(hAo, &nGetUti);

                    CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                   )
            {
                if(pSmsMOInfo->nListCount != 0)
                {
                    UINT8 *pListOutData = NULL;
                    for(pSmsMOInfo->i = 0; pSmsMOInfo->i < pSmsMOInfo->nListCount; pSmsMOInfo->i++)
                    {
                        UINT8 *pPduDataWithRealSize = NULL;
                        UINT8  nStatus = 0x0;
                        UINT16 nPduRealSize = 0x0;
                        UINT32 nTmp = 0x0;
                        CFW_SMS_MULTIPART_INFO sLongerMsg;

                        //Zero memory
                        SUL_ZeroMemory8(&sLongerMsg, SIZEOF(CFW_SMS_MULTIPART_INFO));
                        if(pSmsMOInfo->i == 0)
                        {
                            //This is a temp memory. just for CFW_MeListMessage.
                            pListOutData = (UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4);
                            if(!pListOutData)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc pListOutData malloc error!!! \n ",0x08100ffd));
                                CFW_GetUTI(hAo, &nGetUti);

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                return ERR_NO_MORE_MEMORY;
                            }
                            SUL_ZeroMemory8(pListOutData, SMS_MO_ONE_PDU_SIZE + 4 + 4);

                            sMeEvent.nParam1 = (UINT32)pListOutData;
                            pEvent = &sMeEvent;
                        }
                        ret = CFW_MeListMessage(nSimId, pSmsMOInfo->nStatus, 1, pSmsMOInfo->nListStartIndex, &sMeEvent);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_MeListMessage ret[0x%x]\r\n",0x08100ffe), ret);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("sms_ListAoProc  CFW_MeListMessage ,pSmsMOInfo->nStatus is %d ,pSmsMOInfo->nListStartIndex is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08100fff),
                                  pSmsMOInfo->nStatus , pSmsMOInfo->nListStartIndex, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);

                        //(ret == ERR_SUCCESS) means that CFW_MeListMessage returns success
                        //pEvent->nParam2 & 0xffff0000) == 0 :  means the sms got is not the last one
                        //pEvent->nParam2  == 0x10001        :  means to get the last(max index) sms in db
                        if((ret == ERR_SUCCESS) &&  \
                                (((pEvent->nParam2 & 0xffff0000) == 0) || (pEvent->nParam2  == 0x10001)))
                        {
                            UINT32 nPduOutData = 0x0;
                            //Just get the size and status of each PDU
                            ret = sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 8, &nPduOutData, &nStatus, &sLongerMsg);
                            if(ret != ERR_SUCCESS)
                            {
                                CFW_GetUTI(hAo, &nGetUti);

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ret, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_SMS_FREE(pListOutData);
                                CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                return ERR_CMS_UNKNOWN_ERROR;
                            }
                            nPduRealSize = ((CFW_SMS_PDU_INFO *)((CFW_SMS_NODE *)nPduOutData)->pNode)->nDataSize;
                            if(nPduRealSize > 200)
                                nPduRealSize = SMS_MO_ONE_PDU_SIZE;
                            CSW_SMS_FREE((UINT8 *)nPduOutData);
                            nPduOutData = 0x0;

                            //Then Get the sLongerMsg
                            if((nStatus == CFW_SMS_STORED_STATUS_UNREAD) || \
                                    (nStatus == CFW_SMS_STORED_STATUS_READ) )
                            {
                                sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 2, &nPduOutData, &nStatus, &sLongerMsg);
                                CSW_SMS_FREE((UINT8 *)nPduOutData);
                            }//Submitted message.
                            else if((nStatus == CFW_SMS_STORED_STATUS_UNSENT)               ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)      ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV) ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV) ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE) )
                            {
                                //Text Mode, Submitted message. when  CFW_CfgSetSmsShowTextModeParam(1)
                                //Parse pEvent->nParam1 to CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO
                                sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 4, &nPduOutData, &nStatus, &sLongerMsg);
                                CSW_SMS_FREE((UINT8 *)nPduOutData);
                                nPduRealSize = 184;
                            }

                            //??? Using sLongerMsg

                            //Malloc the real size for a PDU, 1 means to add status byte
                            pPduDataWithRealSize = (UINT8 *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_NODE) + nPduRealSize + 1);
                            if(!pPduDataWithRealSize)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc pPduDataWithRealSize malloc error!!! \n ",0x08101000));
                                CFW_GetUTI(hAo, &nGetUti);

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                return ERR_NO_MORE_MEMORY;
                            }
                            SUL_ZeroMemory8(pPduDataWithRealSize, (nPduRealSize + SIZEOF(CFW_SMS_NODE) + 1));

                            //Just do one time
                            if(pSmsMOInfo->i == 0)
                            {
                                //     | 4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |...
                                //     |addr count|   addr1   |   addr2   |   addr3   |   addr4   |   addr5   |....
                                //
                                //     ^ pSmsMOInfo->nListAddr = (UINT32)pListAddr
                                UINT32 *pListAddr = NULL;
                                pListAddr = (UINT32 *)CSW_SMS_MALLOC(4 * pSmsMOInfo->nListCount + 4);
                                if(!pListAddr)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc pListAddr malloc error!!! \n ",0x08101001));
                                    CFW_GetUTI(hAo, &nGetUti);

                                    CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                    CSW_SMS_FREE(pListOutData);
                                    CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }
                                SUL_ZeroMemory8(pListAddr, (4 * pSmsMOInfo->nListCount + 4));
                                pSmsMOInfo->nListAddr = (UINT32)pListAddr;
                            }

                            //Set addr(i) each time
                            *((UINT32 *)pSmsMOInfo->nListAddr + pSmsMOInfo->i + 1) = (UINT32)pPduDataWithRealSize;
                        }
                        if(ret != ERR_SUCCESS)
                        {
                            CFW_GetUTI(hAo, &nGetUti);

                            CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_SMS_FREE(pListOutData);
                            CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }

                        //(H16bit)pEvent->nParam2: 0 means continue to list; 1 means list over .
                        //(L16bit)pEvent->nParam2: The index listed.
                        //Continue to list
                        //pEvent->nParam2 & 0xffff0000) == 0 :  means the sms got is not the last one
                        //pEvent->nParam2  == 0x10001        :  means to get the last(max index) sms in db
                        if(((pEvent->nParam2 & 0xffff0000) == 0) || (pEvent->nParam2  == 0x10001))
                        {
                            nTmp = (UINT32)pPduDataWithRealSize;
                            //Set value and copy data
                            if(sLongerMsg.count > 0) // a longer msg
                            {
                                UINT16 nConcatSmsNextRec = 0x0;
                                //[[xueww[mod] 2007.03.14
                                ((CFW_SMS_NODE *)nTmp)->nConcatPrevIndex = sLongerMsg.id;
                                nConcatSmsNextRec = sLongerMsg.current;
                                nConcatSmsNextRec = (nConcatSmsNextRec << 8) | sLongerMsg.count;
                                ((CFW_SMS_NODE *)nTmp)->nConcatNextIndex = nConcatSmsNextRec;
                                //]]xueww[mod] 2007.03.14
                            }
                            else // a normal short message
                            {
                                ((CFW_SMS_NODE *)nTmp)->nConcatPrevIndex  =   0xffff;
                            }

                            //Set value
                            ((CFW_SMS_NODE *)nTmp)->nConcatCurrentIndex   =  (UINT16)(pEvent->nParam2 & 0x0000ffff);
                            if(pEvent->nParam2  == 0x10001)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ListAoProc nListStartIndex=%d \n ",0x08101002), pSmsMOInfo->nListStartIndex);
                                //modify by qidd for bug 13978
                                //((CFW_SMS_NODE *)nTmp)->nConcatCurrentIndex   = pSmsMOInfo->nListStartIndex + (nSimId) * (SMS_ME_MAX_INDEX / (CFW_SIM_COUNT));
                                ((CFW_SMS_NODE *)nTmp)->nConcatCurrentIndex   = (nSimId + 1) * (SMS_ME_MAX_INDEX / (CFW_SIM_COUNT));

                                /*
                                if (1 == sStorageInfo.usedSlot )
                                                  ((CFW_SMS_NODE*)nTmp)->nConcatCurrentIndex   = pSmsMOInfo->nListStartIndex + (nSimId)*(SMS_ME_MAX_INDEX/(CFW_SIM_COUNT));
                                else
                                                  ((CFW_SMS_NODE*)nTmp)->nConcatCurrentIndex   = pSmsMOInfo->nListStartIndex;
                                                  */
                                //modify by qidd for bug 13978  ---end

                            }
                            ((CFW_SMS_NODE *)nTmp)->nStatus  =   nStatus;
                            ((CFW_SMS_NODE *)nTmp)->nStorage =   pSmsMOInfo->nLocation;
                            //Pointer to
                            ((CFW_SMS_NODE *)nTmp)->pNode    =  (UINT8 *)((UINT8 *)nTmp + SIZEOF(CFW_SMS_NODE));
                            //copy the pdu with (real size +1), including status byte
                            SUL_MemCopy8((UINT8 *)((CFW_SMS_NODE *)nTmp)->pNode, (UINT8 *)((UINT8 *)pEvent->nParam1), (nPduRealSize + 1));

                            pSmsMOInfo->nListStartIndex = (UINT16)((pEvent->nParam2 & 0x0000ffff) + 1);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ListAoProc pSmsMOInfo->nListStartIndex=%d \n ",0x08101003), pSmsMOInfo->nListStartIndex);
                            //Get the size of list PDU(add total)
                            pSmsMOInfo->nListSize = (UINT16)(pSmsMOInfo->nListSize + nPduRealSize + SIZEOF(CFW_SMS_NODE) + 1);
                        }

                        //got the last one
                        //pEvent->nParam2 & 0xffff0000) == 0x00010000 : 用户要求的count大于或等于该状态sms实际存在的count, 列到最后一个(非max index的最后的一个)
                        //pEvent->nParam2  == 0x10001                 : 用户要求的count大于或等于该状态sms实际存在的count, 列到最后一个(max index的最后的一个)
                        //(pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount-1))
                        //                                            : 用户要求的count小于该状态sms实际存在的count, 列到需求的最后一个
                        if( ((pEvent->nParam2 & 0xffff0000) == 0x00010000) ||
                                (pEvent->nParam2  == 0x10001)                  ||
                                (((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount - 1))) )
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            //if list over and the comment is null
                            if((pSmsMOInfo->i == 0) && ( *((UINT8 *)pEvent->nParam1) == 0))
                            {

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, 0, nGetUti | (nSimId << 24), 0xf0);
                                CSW_SMS_FREE((UINT32 *)pSmsMOInfo->nListAddr);
                            }
                            else
                            {
                                //Set addr count
                                if((pEvent->nParam2 & 0xffff0000) == 0x00010000)
                                    *((UINT32 *)pSmsMOInfo->nListAddr) = (UINT32)(pSmsMOInfo->i);
                                if(pEvent->nParam2  == 0x10001)
                                {
                                    *((UINT32 *)pSmsMOInfo->nListAddr) = (UINT32)(pSmsMOInfo->i + 1);
                                    pSmsMOInfo->nListStartIndex = SMS_ME_MAX_INDEX + 1;
                                }
                                if(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount - 1)))
                                    *((UINT32 *)pSmsMOInfo->nListAddr) = (UINT32)(pSmsMOInfo->i + 1);
                                pEvent->nParam2 = (UINT32)(*((UINT32 *)pSmsMOInfo->nListAddr) << 16) + (pSmsMOInfo->nListStartIndex - 1);

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, pSmsMOInfo->nListAddr , pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                            }
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            if(pListOutData !=  NULL)
                                CSW_SMS_FREE(pListOutData);
                            CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                            return ERR_SUCCESS;
                        }
                    }//End for(pSmsMOInfo->i=0;pSmsMOInfo->i<pSmsMOInfo->nListCount;pSmsMOInfo->i++)
                }
                else//(pSmsMOInfo->nListCount == 0)
                {
                    ret = CFW_MeListMessage(nSimId, pSmsMOInfo->nStatus, 0, pSmsMOInfo->nListStartIndex, &sMeEvent);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("sms_ListAoProc CFW_MeListMessage ,pSmsMOInfo->nStatus is %d ,pSmsMOInfo->nListStartIndex is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08101004), pSmsMOInfo->nStatus , pSmsMOInfo->nListStartIndex, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);

                    if(ret == ERR_SUCCESS)
                    {
                        CFW_GetUTI(hAo, &nGetUti);

                        CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, 0, sMeEvent.nParam2, nGetUti | (nSimId << 24), sMeEvent.nType);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                        return ERR_SUCCESS;
                    }
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);

                        CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            if((pEvent->nEventId == EV_CFW_SIM_LIST_MESSAGE_RSP) && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if(pEvent->nType == 0)
                {

                    if(pSmsMOInfo->nListCount != 0x0)
                    {
                        //(HOUINT16)pEvent->nParam2 == 0: indicate list operation don’t complete， so there are more SMS to list.
                        //(HOUINT16)pEvent->nParam2 == 1: indicate list operation complete.
                        //if not the last one
                        if((pEvent->nParam2 & 0xffff0000) == 0)
                        {
                            UINT8  *pPduDataWithRealSize = NULL;
                            UINT8   nStatus = 0x0;
                            UINT16  nPduRealSize = 0x0;
                            UINT32  nPduOutData = 0x0;
                            UINT32  nTmp = 0x0;
                            CFW_SMS_MULTIPART_INFO sLongerMsg;

                            //Zero memory
                            SUL_ZeroMemory8(&sLongerMsg, SIZEOF(CFW_SMS_MULTIPART_INFO));
                            //Just get the size ,status of each PDU
                            ret = sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 8, &nPduOutData, &nStatus, &sLongerMsg);
                            if(ret == ERR_SUCCESS)
                            {
                                nPduRealSize = ((CFW_SMS_PDU_INFO *)((CFW_SMS_NODE *)nPduOutData)->pNode)->nDataSize;
                                if(nPduRealSize > 200)
                                    nPduRealSize = SMS_MO_ONE_PDU_SIZE;
                                CSW_SMS_FREE((UINT8 *)nPduOutData);
                            }
                            else// maybe Status report or EMS
                            {
                                //pSmsMOInfo->i++;
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_tool_DecomposePDU Failed Maybe a status report\n ",0x08101005));
                                if(pEvent->nParam1)
                                {
                                    CSW_SMS_FREE((VOID *) pEvent->nParam1);
                                    pEvent->nParam1 = (UINT32)NULL;
                                }

                                break;//xueww[mod] 2007.09.04

                                //nPduRealSize = SMS_MO_ONE_PDU_SIZE;
                                //nStatus      = 0;
                                //                                    CFW_GetUTI(hAo, &nGetUti);
                                //                                    CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP,ERR_CMS_UNKNOWN_ERROR,0,nGetUti,0xf0);
                                //                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE|CFW_AO_PROC_CODE_CONSUMED);
                                //                                    CFW_UnRegisterAO(CFW_APP_SRV_ID,hAo);
                                //                                    return ERR_CMS_UNKNOWN_ERROR;
                            }
                            nPduOutData  = 0x0;

                            //Then Get the sLongerMsg
                            if((nStatus == CFW_SMS_STORED_STATUS_UNREAD) || \
                                    (nStatus == CFW_SMS_STORED_STATUS_READ) )
                            {
                                ret = sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 2, &nPduOutData, &nStatus, &sLongerMsg);
                                if(nPduOutData)
                                    CSW_SMS_FREE((UINT8 *)nPduOutData);
                            }//Submitted message.
                            else if((nStatus == CFW_SMS_STORED_STATUS_UNSENT)               ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)      ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV) ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV) ||
                                    (nStatus == CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE) )
                            {
                                //Text Mode, Submitted message. when  CFW_CfgSetSmsShowTextModeParam(1)
                                //Parse pEvent->nParam1 to CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO
                                ret = sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 4, &nPduOutData, &nStatus, &sLongerMsg);
                                if(nPduOutData)
                                    CSW_SMS_FREE((UINT8 *)nPduOutData);
                            }
                            //[[xueww[-] 2007.09.04
                            /*
                            else// nStatus = 0;
                            {
                                nStatus = CFW_SMS_STORED_STATUS_READ;
                            }
                            */
                            //]]xueww[-] 2007.09.04

                            //Malloc the real size for a PDU, 1 means to add status byte
                            pPduDataWithRealSize = (UINT8 *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_NODE) + nPduRealSize + 1);
                            if(!pPduDataWithRealSize)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc pPduDataWithRealSize malloc error!!! \n ",0x08101006));
                                if(pEvent->nParam1)
                                {
                                    CSW_SMS_FREE((VOID *) pEvent->nParam1);
                                    pEvent->nParam1 = (UINT32)NULL;
                                }

                                CFW_GetUTI(hAo, &nGetUti);

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                return ERR_NO_MORE_MEMORY;
                            }
                            SUL_ZeroMemory8(pPduDataWithRealSize, (nPduRealSize + SIZEOF(CFW_SMS_NODE) + 1));

                            //Just do one time
                            if(pSmsMOInfo->i == 0)
                            {
                                //     | 4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |  4 bytes  |...
                                //     |addr count|   addr1   |   addr2   |   addr3   |   addr4   |   addr5   |....
                                //
                                //     ^ pSmsMOInfo->nListAddr = (UINT32)pListAddr
                                UINT32 *pListAddr = NULL;
                                pListAddr = (UINT32 *)CSW_SMS_MALLOC(4 * pSmsMOInfo->nListCount + 4);
                                if(!pListAddr)
                                {

                                    if(pEvent->nParam1)
                                    {
                                        CSW_SMS_FREE((VOID *) pEvent->nParam1);
                                        pEvent->nParam1 = (UINT32)NULL;
                                    }
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc pListAddr malloc error!!! \n ",0x08101007));
                                    CFW_GetUTI(hAo, &nGetUti);

                                    CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                    CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }
                                SUL_ZeroMemory8(pListAddr, (4 * pSmsMOInfo->nListCount + 4));
                                pSmsMOInfo->nListAddr = (UINT32)pListAddr;
                            }

                            //Set addr(i) each time
                            *((UINT32 *)pSmsMOInfo->nListAddr + pSmsMOInfo->i + 1) = (UINT32)pPduDataWithRealSize;

                            nTmp = (UINT32)pPduDataWithRealSize;
                            if(sLongerMsg.count > 0) // a longer msg
                            {
                                UINT16 nConcatSmsNextRec = 0x0;
                                //[[xueww[mod] 2007.03.14
                                ((CFW_SMS_NODE *)nTmp)->nConcatPrevIndex = sLongerMsg.id;
                                nConcatSmsNextRec = sLongerMsg.current;
                                nConcatSmsNextRec = (nConcatSmsNextRec << 8) | sLongerMsg.count;
                                ((CFW_SMS_NODE *)nTmp)->nConcatNextIndex = nConcatSmsNextRec;
                                //]]xueww[mod] 2007.03.14
                            }
                            else // a normal short message
                            {
                                ((CFW_SMS_NODE *)nTmp)->nConcatPrevIndex  =   0xffff;
                            }
                            //Set value
                            ((CFW_SMS_NODE *)nTmp)->nConcatCurrentIndex   =  (UINT16)(pEvent->nParam2 & 0x0000ffff);
                            ((CFW_SMS_NODE *)nTmp)->nStatus  =   nStatus;
                            ((CFW_SMS_NODE *)nTmp)->nStorage =   pSmsMOInfo->nLocation;
                            //Pointer to
                            ((CFW_SMS_NODE *)nTmp)->pNode    =  (UINT8 *)((UINT8 *)nTmp + SIZEOF(CFW_SMS_NODE));
                            //copy the pdu with (real size +1), including status byte
                            SUL_MemCopy8((UINT8 *)((CFW_SMS_NODE *)nTmp)->pNode, (UINT8 *)((UINT8 *)pEvent->nParam1), (nPduRealSize + 1));
                            pSmsMOInfo->nListStartIndex = (UINT16)((pEvent->nParam2 & 0x0000ffff) + 1);
                            //Get the size of list PDU(add total)
                            pSmsMOInfo->nListSize = (UINT16)(pSmsMOInfo->nListSize + nPduRealSize + SIZEOF(CFW_SMS_NODE) + 1);
                            pSmsMOInfo->i++;
                        }

                        //got the last one
                        if( ((pEvent->nParam2 & 0xffff0000) == 0x00010000) ||
                                (((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount))) )
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            //if list over and the comment is null
                            if((pSmsMOInfo->i == 0) && ( pEvent->nParam1 == 0))
                            {

                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, 0, nGetUti | (nSimId << 24), 0xf0);
                                CSW_SMS_FREE((UINT32 *)pSmsMOInfo->nListAddr);
                            }
                            else
                            {
                                //Set addr count
                                *((UINT32 *)pSmsMOInfo->nListAddr) = (UINT32)pSmsMOInfo->i;
                                //                                    pEvent->nParam2 = (UINT32)((pSmsMOInfo->nListSize)<<16) + (pSmsMOInfo->nListStartIndex-1);
                                pEvent->nParam2 = (UINT32)((pSmsMOInfo->i) << 16) + (pSmsMOInfo->nListStartIndex - 1);
                                CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, pSmsMOInfo->nListAddr , pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                            }
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);

                            if(pEvent->nParam1)
                            {
                                CSW_SMS_FREE((VOID *) pEvent->nParam1);
                                pEvent->nParam1 = (UINT32)NULL;
                            }
                            return ERR_SUCCESS;
                        }
                        else//added by fengwei 20080701, we should free the event when the list not comlete
                        {

                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                        }
                    }
                    else //count is 0
                    {
                        if(pEvent->nParam1)
                        {
                            CSW_SMS_FREE((VOID *) pEvent->nParam1);
                            pEvent->nParam1 = (UINT32)NULL;
                        }

                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, 0, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
                        return ERR_SUCCESS;
                    }//end if(pSmsMOInfo->nListCount)

                    if(pEvent->nParam1)
                    {
                        CSW_SMS_FREE((VOID *) pEvent->nParam1);
                        pEvent->nParam1 = (UINT32)NULL;
                    }


                }
                else if(pEvent->nType == 0xf0)
                {
                    if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CMS_INVALID_STATUS)
                    {
                        pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else
                    {
                        pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                    }
                    CFW_GetUTI(hAo, &nGetUti);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_ListAoProc  pEvent->nParam1=0x%x\n",0x08101008), pEvent->nParam1);
                    CFW_PostNotifyEvent(EV_CFW_SMS_LIST_MESSAGE_RSP, pEvent->nParam1, 0, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }
            break;
        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_ListAoProc  error\n",0x08101009));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_ListAoProc);
    return ERR_SUCCESS;
}


UINT32 sms_MoveAoProc(HAO hAo, CFW_EV* pEvent)
{
    UINT32 nGetUti = 0x0, nState = 0x0, ret = 0x0;
    UINT8 nStatus  = 0x0, nTmp = 0x0;

    CFW_SMS_MO_INFO* pSmsMOInfo = NULL;
    CFW_EV sMeEvent;

    CSW_PROFILE_FUNCTION_ENTER(sms_MoveAoProc);

    CFW_SIM_ID nSimId = CFW_SIM_COUNT;

    CFW_GetSimID(hAo, &nSimId);

    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));

    pSmsMOInfo = CFW_GetAoUserData(hAo);
    nStatus    = pSmsMOInfo->nStatus;  // added by fengwei 20080625 for bug 8709
    if (!pSmsMOInfo)
    {
        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_MOVEAoProc, Failed\n",0x0810100a));

        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    if (pEvent == (CFW_EV*)0xffffffff)
        nState = CFW_MOVE_SMS_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(3), TSTR("11111111111111111sms_MoveAoProc, nState = 0x%x pEvent is:%p, pSmsMOInfo->nLocation is:0x%x\n",0x0810100b), nState, pEvent, pSmsMOInfo->nLocation);

    switch (nState)
    {
        case CFW_MOVE_SMS_STATE_IDLE:
        {
            if (pEvent == (CFW_EV*)0xffffffff)
            {
                if (pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                {


                    sMeEvent.nParam1 = (UINT32)((UINT8*)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                    if (!(UINT8*)sMeEvent.nParam1)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(0), TSTR("sms_MoveAoProc sMeEvent.nParam1 malloc error!!! \n ",0x0810100c));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_MEMORY_FULL, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24),
                                            0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                        return ERR_NO_MORE_MEMORY;
                    }
                    SUL_ZeroMemory8((UINT8*)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                    ret = CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(6),
                              TSTR("sms_MoveAoProc,5, pSmsMOInfo->nIndex is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x  pSmsMOInfo->nLocation is:%x , ret is:%x\n ",0x0810100d),
                              pSmsMOInfo->nIndex, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType, pSmsMOInfo->nLocation, ret );
                    if (ret == ERR_SUCCESS)
                    {
                        CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_READ);
                        pEvent           = &sMeEvent;
                        pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(3), TSTR("11111111111111111sms_MoveAoProc, CFW_MeReadMessage nState = 0x%x pEvent is:%p, sMeEvent.nParam1 is:%p\n",0x0810100e), nState, pEvent, sMeEvent.nParam1);
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(1), TSTR("sms_MoveAoProc, 6,CFW_MeReadMessage, ret =0x%x\n ",0x0810100f), ret);
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex,
                                            nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_SMS_FREE((UINT8*)sMeEvent.nParam1);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }

                }
                else
                {

                    ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                    if (ret == ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("222222222222222222sms_MoveAoProc, CFW_MeReadMessage  sucess pSmsMOInfo->nIndex is:%d simId is:%d \n",0x08101010),pSmsMOInfo->nIndex , nSimId);
                        CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_READ);
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("222222222222222222sms_MoveAoProc, CFW_MeReadMessage  Failed  pSmsMOInfo->nIndex is:%d simId is:%d \n",0x08101011),pSmsMOInfo->nIndex , nSimId);
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex,
                                            nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    break;

                }

            }

        }
        case CFW_MOVE_SMS_STATE_READ:
        {

            if (((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP)
                    || (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP))
                    && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {

                if (pEvent->nType == 0)
                {
                    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("sms_Move Read message success.pEvent->nParam1 %d pSmsMOInfo->nLocation is:%d \n ",0x08101012), *(UINT8*)pEvent->nParam1, pSmsMOInfo->nLocation);

                    if (pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                    {

                        switch(((UINT8*)(pEvent->nParam1))[0] & 0x1F  )
                        {
                            case 0x03:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x01;
                                break;
                            case 0x01:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x02;
                                break;
                            case 0x07:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x04;
                                break;
                            case 0x05:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x08;
                                break;
                            case 0x0D:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x10;
                                break;
                            case 0x15:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x20;
                                break;
                            case 0x1D:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x40;
                                break;
                            case 0xA0:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x80;
                                break;
                            default:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x00;
                        }

                        ret =
                            CFW_SimWriteMessage(CFW_SMS_STORAGE_SM, 0x00, (UINT8*)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE,
                                                pSmsMOInfo->nUTI_Internal, nSimId);
                        //Add for memory leak issue bug[32424]
                        CSW_SMS_FREE((UINT8*)pEvent->nParam1);

                        if (ret != ERR_SUCCESS)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex,
                                                nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("sms_Move CFW_SimWriteMessage failed.nSimId %d pSmsMOInfo->nLocation is:%d \n ",0x08101013), nSimId, pSmsMOInfo->nLocation);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        {
                            CFW_SMS_STORAGE_INFO  sStorageInfo;
                            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                            CFW_CfgGetSmsStorageInfo(&sStorageInfo,CFW_SMS_STORAGE_SM, nSimId);
                            if(sStorageInfo.usedSlot >= 0)
                                sStorageInfo.usedSlot++;

                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL), "For SM,sStorageInfo.usedSlot: %d,sStorageInfo.totalSlot:%d\n",sStorageInfo.usedSlot,sStorageInfo.totalSlot);
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo,CFW_SMS_STORAGE_SM, nSimId);

                        }
                        CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_WRITE);
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("sms_Move CFW_SimWriteMessage success.nSimId %d pSmsMOInfo->nLocation is:%d \n ",0x08101014), nSimId, pSmsMOInfo->nLocation);
                        return ERR_SUCCESS;


                    }
                    else
                    {
                        UINT32 nTime = 0x0, nAppInt32 = 0x0, nTmp = 0x0;
                        UINT32 nWriteIndex = 0x00;
                        nTmp = sMeEvent.nParam1;

                        switch(((UINT8*)(pEvent->nParam1))[0] & 0x1F  )
                        {
                            case 0x03:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x01;
                                break;
                            case 0x01:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x02;
                                break;
                            case 0x07:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x04;
                                break;
                            case 0x05:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x08;
                                break;
                            case 0x0D:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x10;
                                break;
                            case 0x15:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x20;
                                break;
                            case 0x1D:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x40;
                                break;
                            case 0xA0:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x80;
                                break;
                            default:
                                ((UINT8*)(pEvent->nParam1))[0] = 0x00;
                        }

                        ret =
                            CFW_MeWriteMessage(nSimId, 0x00, (UINT8*)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime,
                                               nAppInt32, &sMeEvent);
                        //Add for memory leak issue bug[32424]
                        CSW_SMS_FREE(pEvent->nParam1);
                        CSW_SMS_FREE((UINT8*)(nTmp));

                        if ((sMeEvent.nType == 0))
                        {
                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_Move write message success\n ",0x08101015));
                            CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_WRITE);
                            pEvent           = &sMeEvent;
                            pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                            pEvent->nEventId = EV_CFW_SMS_WRITE_MESSAGE_RSP;
                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("sms_Move pEvent write to ME index %d\n ",0x08101016),sMeEvent.nParam1);

                            {
                                CFW_SMS_STORAGE_INFO  sStorageInfo;
                                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo,CFW_SMS_STORAGE_ME, nSimId);
                                if(sStorageInfo.usedSlot >= 0)
                                    sStorageInfo.usedSlot++;

                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL), "For ME,sStorageInfo.usedSlot: %d,sStorageInfo.totalSlot:%d\n",sStorageInfo.usedSlot,sStorageInfo.totalSlot);
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo,CFW_SMS_STORAGE_ME, nSimId);

                            }
                        }
                        else
                        {

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, sMeEvent.nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24),
                                                sMeEvent.nType);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_Move pEvent ERR_CMS_MEMORY_FULL \n ",0x08101017));
                            return ERR_CMS_MEMORY_FULL;
                        }
                        CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_WRITE);
                    }

                }
                else
                {
                    if (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                        CSW_SMS_FREE((UINT8*)(sMeEvent.nParam1));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, pSmsMOInfo->nIndex,
                                        nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_Move pEvent ERR_CMS_OPERATION_NOT_ALLOWED \n ",0x08101018));
                    return ERR_CMS_UNKNOWN_ERROR;
                }

            }

        }
#if 1
        case CFW_MOVE_SMS_STATE_WRITE:
        {
            if (((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP)
                    || (pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP))
                    && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if ((pEvent->nType == 0))
                {
#if 1
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    pSmsMOInfo->nDestIndex = (UINT16)(pEvent->nParam1);
                    U16 destLocation = pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME ?CFW_SMS_STORAGE_SM : CFW_SMS_STORAGE_ME;
                    CFW_GetUTI(hAo, &nGetUti);

                    //get storage info
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, destLocation, nSimId);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL), "destLocation: %d,sStorageInfo.usedSlot: %d,sStorageInfo.totalSlot:%d\n",destLocation,sStorageInfo.usedSlot,sStorageInfo.totalSlot);

                    if (sStorageInfo.totalSlot == sStorageInfo.usedSlot)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL), "SEND EV_CFW_SMS_INFO_IND 0\n");
                        CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND, 1, destLocation, nGetUti | (nSimId << 24), 0);
                    }
                    CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, (destLocation<<16)|pSmsMOInfo->nDestIndex, (pSmsMOInfo->nLocation<<16)|pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0x00);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                    return ERR_SUCCESS;
#else
                    pSmsMOInfo->nDestIndex = (UINT16)(pEvent->nParam1);
                    //pSmsMOInfo->Padding[0] = pEvent->nParam1;
                    if (pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                    {
#ifdef CFW_MULTI_SIM
                        ret = CFW_MeDeleteMessage(nSimId, pSmsMOInfo->nIndex,0, &sMeEvent);
#else

                        ret = CFW_MeDeleteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
#endif
                        CSW_TRACE(_CSW|TLEVEL(0)|TDB|TNB_ARG(5),
                                  TSTR("sms_MoveAoProc,5, pSmsMOInfo->nIndex is %d, sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x, pSmsMOInfo->nLocation is:%d\n ",0x08101019),
                                  pSmsMOInfo->nIndex, sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType, pSmsMOInfo->nLocation);
                        if (ret == ERR_SUCCESS)
                        {
                            // Go to CFW_SM_STATE_WAIT directly!
                            CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_READ);
                            pEvent           = &sMeEvent;
                            pEvent->nTransId = pSmsMOInfo->nUTI_Internal;

                            {
                                CFW_SMS_STORAGE_INFO  sStorageInfo;
                                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
#ifndef CFW_MULTI_SIM
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation);
#else
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation, nSimId);
#endif
                                if(sStorageInfo.usedSlot > 0)
                                    sStorageInfo.usedSlot--;

#ifndef CFW_MULTI_SIM
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation);
#else
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation, nSimId);
#endif

                            }

                        }
                        else  // other return value
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(1), TSTR("sms_MoveAoProc, 6,CFW_MeReadMessage, ret =0x%x\n ",0x0810101a), ret);
                            CFW_GetUTI(hAo, &nGetUti);
#  ifndef CFW_MULTI_SIM
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti, 0xf0);
#  else
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex,
                                                nGetUti | (nSimId << 24), 0xf0);
#  endif
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_SMS_FREE((UINT8*)sMeEvent.nParam1);
                            CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }

                    }
                    else
                    {
#  ifndef CFW_MULTI_SIM
                        ret = CFW_SimDeleteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal);
#  else
                        ret = CFW_SimDeleteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex,0,  pSmsMOInfo->nUTI_Internal, nSimId);
#  endif
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(3), TSTR("sms_MoveAoProc, 6,CFW_SimDeleteMessage , ret =0x%x pSmsMOInfo->nLocation is:%d, pSmsMOInfo->nIndex is:%d\n ",0x0810101b), ret, pSmsMOInfo->nLocation, pSmsMOInfo->nIndex);
                        if (ret == ERR_SUCCESS)
                        {
                            CFW_SetAoState(hAo, CFW_MOVE_SMS_STATE_READ);
                            {
                                CFW_SMS_STORAGE_INFO  sStorageInfo;
                                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
#ifndef CFW_MULTI_SIM
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation);
#else
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation, nSimId);
#endif
                                if(sStorageInfo.usedSlot > 0)
                                    sStorageInfo.usedSlot--;

#ifndef CFW_MULTI_SIM
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation);
#else
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo,pSmsMOInfo->nLocation, nSimId);
#endif

                            }
                        }
                        else
                        {
                            CFW_GetUTI(hAo, &nGetUti);
#  ifndef CFW_MULTI_SIM
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti, 0xf0);
#  else
                            CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex,
                                                nGetUti | (nSimId << 24), 0xf0);
#  endif
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        break;

                    }
#endif
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24),
                                        pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID)|TDB|TNB_ARG(1), TSTR("sms_MoveAoProc, 6,CFW_SimDeleteMessage ERR_CMS_UNKNOWN_ERROR, ret =0x%x\n ",0x0810101c), ret);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }

            case CFW_MOVE_SMS_STATE_DELETE:
            {
                if (((pEvent->nEventId == EV_CFW_SIM_DELETE_MESSAGE_RSP)
                        || (pEvent->nEventId == EV_CFW_SMS_DELETE_MESSAGE_RSP))
                        && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))

                {
                    if ((pEvent->nType == 0))
                    {
                        U16 destLocation = pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME ?CFW_SMS_STORAGE_SM : CFW_SMS_STORAGE_ME;
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(2), TSTR("sms_Move CFW_MOVE_SMS_STATE_DELETE write delete message success src index[%d] dest index[%d]\n ",0x0810101d),pSmsMOInfo->nIndex,pSmsMOInfo->nDestIndex);
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, (destLocation<<16)|pSmsMOInfo->nDestIndex, (pSmsMOInfo->nLocation<<16)|pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0x00);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                        return ERR_SUCCESS;

                    }
                    else
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_Move CFW_MOVE_SMS_STATE_DELETE send EV_CFW_SMS_MOVE_RSP write delete message failed\n ",0x0810101e));
                        return ERR_SUCCESS;

                    }

                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_MOVE_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_MoveAoProc);
                    CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("sms_Move CFW_MOVE_SMS_STATE_DELETE send EV_CFW_SMS_MOVE_RSP write delete message failed\n ",0x0810101f));
                    return ERR_SUCCESS;

                }


            }
        }
#endif
        return ERR_SUCCESS;
    }
}
UINT32 sms_CopyAoProc(HAO hAo, CFW_EV* pEvent)
{
    UINT32           nGetUti = 0x0, nState = 0x0, ret = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL; //Point to MO private data.
    CFW_EV        sMeEvent;
    UINT8            nStatus = 0x0, nTmp = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(sms_CopyAoProc);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);

    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    nStatus = pSmsMOInfo->nStatus;//added by fengwei 20080625 for bug 8709
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_CopyAoProc, nState = 0x%x\n",0x08101020), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if(pSmsMOInfo->nListOption == CFW_SMS_COPY_ME2SM)
            {
                if((pEvent == (CFW_EV *)0xffffffff) || (pEvent->nType == 0))
                {
                    CFW_SMS_MULTIPART_INFO sLongerMsg;

                    //Zero memory
                    SUL_ZeroMemory8(&sLongerMsg, SIZEOF(CFW_SMS_MULTIPART_INFO));
                    if(pSmsMOInfo->i == 0)
                    {
                        //This is a temp memory. just for CFW_MeListMessage.
                        pSmsMOInfo->pData = (UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4);
                        if(!pSmsMOInfo->pData)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_CopyAoProc pSmsMOInfo->pData malloc error!!! \n ",0x08101021));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSmsMOInfo->pData, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                    }
                    sMeEvent.nParam1 = (UINT32)(pSmsMOInfo->pData);
                    pEvent = &sMeEvent;

                    ret = CFW_MeListMessage(nSimId, nStatus, 1, pSmsMOInfo->nListStartIndex, &sMeEvent);
                    if(ret != ERR_SUCCESS)
                    {
                        //Free the memory
                        CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                        CFW_GetUTI(hAo, &nGetUti);
                        if(pSmsMOInfo->i)
                        {
                            //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                            pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                            //]]xueww[mod] 2007.03.02
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                        }
                        else
                        {
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                        }

                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    if(sMeEvent.nType == 0)
                    {
                        //(H16bit)pEvent->nParam2: 0 means continue to list; 1 means list over .
                        //(L16bit)pEvent->nParam2: The index listed.
                        //Continue to list
                        if((ret == ERR_SUCCESS) && ((pEvent->nParam2 & 0xffff0000) == 0))
                        {
                            UINT8 uStatus = 0x0;
                            //Just get SMS status
                            sms_tool_DecomposePDU((UINT8 *)(pEvent->nParam1), 0, NULL, &uStatus, NULL);
                            *((UINT8 *)(pEvent->nParam1)) = uStatus;
                            ret = CFW_SimWriteMessage(CFW_SMS_STORAGE_SM, 0, (UINT8 *)(pEvent->nParam1), SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                            if(ret == ERR_SUCCESS )
                            {
                                //Add for memory leak issue bug[32424]
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                            }
                            else
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                CFW_GetUTI(hAo, &nGetUti);
                                if(pSmsMOInfo->i)
                                {
                                    //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                                    pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                                    //]]xueww[mod] 2007.03.02
                                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                                }
                                else
                                {
                                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                }

                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                                return ERR_CMS_UNKNOWN_ERROR;
                            }
                            pSmsMOInfo->i++;
                            pSmsMOInfo->nListStartIndex = (UINT16)((pEvent->nParam2 & 0x0000ffff) + 1);
                        }
                        //got the last one,post msg
                        if(pSmsMOInfo->nListCount)
                            //deleted by fengwei 20080626 for copy msg 2 less than expected
                            //nTmp = (UINT8)(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount-1)));
                            nTmp = (UINT8)(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount + 1)));
                        else
                            nTmp = 0;
                        if( ((pEvent->nParam2 & 0xffff0000) == 0x00010000) ||  nTmp)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            //if list over and the comment is null
                            if((pSmsMOInfo->i == 0) && ( *((UINT8 *)pEvent->nParam1) == 0))
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, 0, nGetUti | (nSimId << 24), 0xf0);
                            }
                            else
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                if((pEvent->nParam2 & 0xffff0000) == 0x00010000)
                                    pEvent->nParam1 = (UINT32)(pSmsMOInfo->i);
                                if(nTmp)
                                    pEvent->nParam1 = (UINT32)(pSmsMOInfo->i + 1);
                                //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                                pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                                //]]xueww[mod] 2007.03.02
                                CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pEvent->nParam1 , pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                            }
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if(sMeEvent.nType == 0xf0)
                    {
                        //Free the memory
                        CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                        CFW_GetUTI(hAo, &nGetUti);
                        if(pSmsMOInfo->i)
                        {
                            //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                            pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                            //]]xueww[mod] 2007.03.02
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                        }
                        else
                        {
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, 0, nGetUti | (nSimId << 24), 0xf0);
                        }

                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                if(pEvent->nType == 0xf0)
                {
                    //Free the memory
                    CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                    CFW_GetUTI(hAo, &nGetUti);
                    if(pSmsMOInfo->i)
                    {
                        //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                        pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 2);
                        //]]xueww[mod] 2007.03.02
                        CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, (pSmsMOInfo->i - 1), pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                    }
                    else
                    {
                        CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, 0, nGetUti | (nSimId << 24), 0xf0);
                    }

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
            else if(pSmsMOInfo->nListOption == CFW_SMS_COPY_SM2ME)
            {
                if(pSmsMOInfo->nListCount)
                    nStatus = pSmsMOInfo->nStatus;
                else
                {
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    //Get max slot in SIM
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
                    nStatus = 0x7f;
                    pSmsMOInfo->nListCount = sStorageInfo.totalSlot;
                }
                ret = CFW_SimListMessage(CFW_SMS_STORAGE_SM, nStatus, pSmsMOInfo->nListCount, pSmsMOInfo->nListStartIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
            break;
        }
        case CFW_SM_STATE_WAIT:
        {
            if((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if(pEvent->nType == 0)
                {
                    //added by fengwei 20080626 for missing update of StorageInfo begin
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    UINT8 nLastMsgStatus = 0;
                    sms_tool_DecomposePDU(pSmsMOInfo->pData, 0, NULL, &nLastMsgStatus, NULL);
                    //Update storage info, this is rsp of writing msg to sm, so we update sm storage
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);

                    if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                        sStorageInfo.usedSlot++;
                    if(CFW_SMS_STORED_STATUS_UNREAD == nLastMsgStatus)
                        sStorageInfo.unReadRecords++;
                    else if(CFW_SMS_STORED_STATUS_READ == nLastMsgStatus)
                        sStorageInfo.readRecords++;
                    else if(CFW_SMS_STORED_STATUS_UNSENT == nLastMsgStatus)
                        sStorageInfo.unsentRecords++;
                    else if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == nLastMsgStatus)        ||
                            (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == nLastMsgStatus)   ||
                            (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == nLastMsgStatus)  ||
                            (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == nLastMsgStatus))
                        sStorageInfo.sentRecords++;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("usedSlot = %u, unReadRecords = %u, readRecords = %u, unsentRecords = %u, sentRecords = %u\t\n",0x08101022),
                              sStorageInfo.usedSlot, sStorageInfo.unReadRecords, sStorageInfo.readRecords, sStorageInfo.unsentRecords, sStorageInfo.sentRecords);
                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
                    //added by fengwei 20080626 for missing update of StorageInfo end

                    //0: Save to specified memory location and the memory is not full. 1:full
                    pSmsMOInfo->nPath = (UINT8)(pEvent->nParam2);

                    sMeEvent.nParam1 = (UINT32)(pSmsMOInfo->pData);
                    pEvent = &sMeEvent;
                    ret = CFW_MeListMessage(nSimId, nStatus, 1, pSmsMOInfo->nListStartIndex, &sMeEvent);
                    if(ret != ERR_SUCCESS)
                    {
                        //Free the memory
                        CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                        CFW_GetUTI(hAo, &nGetUti);
                        if(pSmsMOInfo->i)
                        {
                            //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                            pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                            //]]xueww[mod] 2007.03.02
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                        }
                        else
                        {
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                        }
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    if(sMeEvent.nType == 0)
                    {
                        //(H16bit)pEvent->nParam2: 0 means continue to list; 1 means list over .
                        //(L16bit)pEvent->nParam2: The index listed.
                        //Continue to list
                        if((ret == ERR_SUCCESS) && ((pEvent->nParam2 & 0xffff0000) == 0))
                        {
                            UINT8 uStatus = 0x0;
                            //Just get SMS status
                            sms_tool_DecomposePDU((UINT8 *)(pSmsMOInfo->pData), 0, NULL, &uStatus, NULL);
                            *((UINT8 *)(pEvent->nParam1)) = uStatus;
                            ret = CFW_SimWriteMessage(CFW_SMS_STORAGE_SM, 0, (UINT8 *)(pEvent->nParam1), SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                            if(ret == ERR_SUCCESS )
                            {
                                //deleted by fengwei 20080627 for copy msg wrong, this moment we still have some msg to copy, and we have send req to sim,
                                //so we should stay in waiting for rsp state.
                                //CFW_SetAoState(hAo, CFW_SM_STATE_IDLE);why should we set Ao state to IDLE?
                            }
                            else
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                CFW_GetUTI(hAo, &nGetUti);
                                if(pSmsMOInfo->i)
                                {
                                    //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                                    pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                                    //]]xueww[mod] 2007.03.02
                                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                                }
                                else
                                {
                                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                }

                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                                return ERR_CMS_UNKNOWN_ERROR;
                            }
                            pSmsMOInfo->i++;
                            pSmsMOInfo->nListStartIndex = (UINT16)((pEvent->nParam2 & 0x0000ffff) + 1);
                        }
                        //got the last one,post msg
                        if(pSmsMOInfo->nListCount)
                            //modify by fengwei 20080626 for copy msg 2 less than expected, this moment we have just receive rsp of last write req,
                            //but i have been counted one more than the count of rsp, so we must compare i with nListCount + 1
                            //nTmp = (UINT8)(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount-1)));
                            nTmp = (UINT8)(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == (pSmsMOInfo->nListCount + 1)));
                        else
                            nTmp = 0;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("after CFW_SimWriteMessage, pEvent->nParam2 = %x\n",0x08101023), pEvent->nParam2);
                        if( ((pEvent->nParam2 & 0xffff0000) == 0x00010000) ||  nTmp)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            //if list over and the comment is null
                            if((pSmsMOInfo->i == 0) && ( *((UINT8 *)pEvent->nParam1) == 0))
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, 0, nGetUti | (nSimId << 24), 0xf0);
                            }
                            else
                            {
                                //Free the memory
                                CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                                if((pEvent->nParam2 & 0xffff0000) == 0x00010000)
                                    pEvent->nParam1 = (UINT32)(pSmsMOInfo->i);
                                if(nTmp)
                                    pEvent->nParam1 = (UINT32)(pSmsMOInfo->i + 1);
                                //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                                pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                                //]]xueww[mod] 2007.03.02
                                CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pEvent->nParam1 , pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                            }
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if(sMeEvent.nType == 0xf0)
                    {
                        //Free the memory
                        CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                        CFW_GetUTI(hAo, &nGetUti);
                        if(pSmsMOInfo->i)
                        {
                            //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                            pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                            //]]xueww[mod] 2007.03.02
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i, pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                        }
                        else
                        {
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, 0, nGetUti | (nSimId << 24), 0xf0);
                        }

                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                }
                else if(pEvent->nType == 0xf0)
                {
                    //Free the memory
                    CSW_SMS_FREE((UINT8 *)(pSmsMOInfo->pData));
                    CFW_GetUTI(hAo, &nGetUti);
                    if(pSmsMOInfo->i)
                    {
                        //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                        pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 2);
                        //]]xueww[mod] 2007.03.02
                        CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, (pSmsMOInfo->i - 1), pEvent->nParam2, nGetUti | (nSimId << 24), 0);
                    }
                    else
                    {
                        CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, 0, nGetUti | (nSimId << 24), 0xf0);
                    }

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
            if((pEvent->nEventId == EV_CFW_SIM_LIST_MESSAGE_RSP) && (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if(pEvent->nType == 0)
                {
                    if((pEvent->nParam2 & 0xffff0000) == 0)
                    {
                        UINT32 nTime = 0x0, nAppInt32 = 0x0;
                        UINT8  uStatus = 0x0;
                        //Just get SMS status
                        sms_tool_DecomposePDU((UINT8 *)(pEvent->nParam1), 0, NULL, &uStatus, NULL);
                        *((UINT8 *)(pEvent->nParam1)) = uStatus;
                        TM_FILETIME nFileTime;
                        TM_GetSystemFileTime(&nFileTime);
                        nTime = nFileTime.DateTime;
                        ret = CFW_MeWriteMessage(nSimId, 0, (UINT8 *)(pEvent->nParam1), SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                        //Add for memory leak issue bug[32424]
                        CSW_SMS_FREE(pEvent->nParam1);
                        pEvent->nParam1 = NULL;
                        if(ret == ERR_SUCCESS)
                        {
                            if(sMeEvent.nType == 0)
                            {
                                //added by fengwei 20080626 for missing update of StorageInfo begin
                                CFW_SMS_STORAGE_INFO  sStorageInfo;
                                //Update storage info, this is rsp of writing msg to ME, so we update sm storage
                                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSimId);

                                if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                                    sStorageInfo.usedSlot++;
                                if(CFW_SMS_STORED_STATUS_UNREAD == nStatus)
                                    sStorageInfo.unReadRecords++;
                                else if(CFW_SMS_STORED_STATUS_READ == nStatus)
                                    sStorageInfo.readRecords++;
                                else if(CFW_SMS_STORED_STATUS_UNSENT == nStatus)
                                    sStorageInfo.unsentRecords++;
                                else if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == nStatus)        ||
                                        (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == nStatus)   ||
                                        (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == nStatus)  ||
                                        (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == nStatus))
                                    sStorageInfo.sentRecords++;
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("usedSlot = %u, unReadRecords = %u, readRecords = %u, unsentRecords = %u, sentRecords = %u\t\n",0x08101024),
                                          sStorageInfo.usedSlot, sStorageInfo.unReadRecords, sStorageInfo.readRecords, sStorageInfo.unsentRecords, sStorageInfo.sentRecords);
                                CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSimId);
                                //added by fengwei 20080626 for missing update of StorageInfo end
                                pSmsMOInfo->nPath = (UINT8)(sMeEvent.nParam2);
                            }
                            else if(sMeEvent.nType == 0xf0)
                            {
                                //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                                sMeEvent.nParam2 = (UINT32)((sMeEvent.nParam2 << 16) + (pSmsMOInfo->nListStartIndex - 1));
                                //]]xueww[mod] 2007.03.02
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pSmsMOInfo->i , sMeEvent.nParam2, nGetUti | (nSimId << 24), sMeEvent.nType);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                                return ERR_CMS_UNKNOWN_ERROR;
                            }
                        }
                        else
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, sMeEvent.nParam2, nGetUti | (nSimId << 24), sMeEvent.nType);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        pSmsMOInfo->i++;
                        pSmsMOInfo->nListStartIndex = (UINT16)((pEvent->nParam2 & 0x0000ffff) + 1);
                    }
                    //got the last one,post msg
                    if(pSmsMOInfo->nListCount)
                        nTmp = (UINT8)(((pEvent->nParam2 & 0xffff0000) == 0x0) && (pSmsMOInfo->i == pSmsMOInfo->nListCount));
                    else
                        nTmp = 0;
                    if(((pEvent->nParam2 & 0xffff0000) == 0x00010000) ||  nTmp)
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        //if list over and the comment is null
                        if((pSmsMOInfo->i == 0) && (pEvent->nParam1 == 0))
                        {
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, 0, nGetUti | (nSimId << 24), 0xf0);
                        }
                        else
                        {
                            //Free the memory
                            CSW_SMS_FREE((UINT8 *)(pEvent->nParam1));
                            pEvent->nParam1 = (UINT32)(pSmsMOInfo->i);
                            //[[xueww[mod] 2007.03.02 //'>>' ====> '<<'
                            pEvent->nParam2 = (UINT32)((pSmsMOInfo->nPath << 16) + pSmsMOInfo->nListStartIndex - 1);
                            //]]xueww[mod] 2007.03.02
                            CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, pEvent->nParam1 , pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                        }
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                        return ERR_SUCCESS;
                    }
                }
                else if(pEvent->nType == 0xf0)
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_COPY_MESSAGE_RSP, ERR_CMS_INVALID_MEMORY_INDEX, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
            break;
        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_CopyAoProc  error\n",0x08101025));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_CopyAoProc);
    return ERR_SUCCESS;
}


UINT32 sms_WriteAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           nState = 0x0, nGetUti = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL; //Point to MO private data.
    CFW_EV           sMeEvent, sComposeEvent;
    CSW_PROFILE_FUNCTION_ENTER(sms_WriteAoProc);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //Zero memory
    SUL_ZeroMemory8(&sComposeEvent, SIZEOF(CFW_EV));
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));

    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_WriteAoProc, nState = 0x%x\n",0x08101026), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            UINT32 ret = 0x0;
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_WriteAoProc, nFormat = 0x%x\n, nLocation = 0x%x\n",0x08101027),
                      pSmsMOInfo->nFormat, pSmsMOInfo->nLocation);
            if(pSmsMOInfo->nFormat)  //Write Text
            {
                if(pEvent == (CFW_EV *)0xffffffff) //First come
                {
                    UINT8  nConcat = 0x0;
                    //should be deliver msg
                    if((pSmsMOInfo->nStatus & CFW_SMS_STORED_STATUS_UNREAD) ||
                            (pSmsMOInfo->nStatus & CFW_SMS_STORED_STATUS_READ))
                    {
                        nConcat = 0x08; // 0000 1000   bit3=1:for write, bit0=0,bit1=0:Deliver
                    } //should be submit msg
                    else
                    {
                        nConcat = 0x09; // 0000 1001  bit3=1:for write, bit0=1,bit1=0:Submit
                    }
                    ret = sms_tool_ComposePDU(nConcat, pSmsMOInfo->nStatus, pSmsMOInfo->pData, pSmsMOInfo->nDataSize,
                                              &pSmsMOInfo->sNumber, nSimId, &sComposeEvent);
                    if(ret == ERR_SUCCESS )
                    {
                        pSmsMOInfo->pTmpData_Addr = (UINT8 *)(sComposeEvent.nParam1);
                        pSmsMOInfo->pTmpPara_Addr = (UINT8 *)(sComposeEvent.nParam2);
                        if(sComposeEvent.nType == 0x0) //for a normal SMS
                        {
                            pSmsMOInfo->isLongerMsg = 0;  // a normal msg or the last part of a longer msg
                            //To SIM
                            if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                              )
                            {
                                ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, (UINT8 *)(sComposeEvent.nParam1),
                                                          SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);

                                if(ret == ERR_SUCCESS )
                                    CFW_SetAoState(pSmsMOInfo->hAo, CFW_SM_STATE_WAIT);
                                else
                                    goto flag_fail;
                                break;
                            }
                            //To ME
                            else if((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                                   )
                            {
                                UINT32 nTime = 0x0, nAppInt32 = 0x0;
                                TM_FILETIME nFileTime;
                                TM_GetSystemFileTime(&nFileTime);
                                nTime = nFileTime.DateTime;
                                ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, (UINT8 *)(sComposeEvent.nParam1), SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("sms_WriteAoProc  CFW_MeWriteMessage ,pSmsMOInfo->nIndex is %d , sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08101028),
                                          pSmsMOInfo->nIndex  , sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                                if(ret == ERR_SUCCESS)
                                {
                                    if(0 == sMeEvent.nType)
                                    {
                                        //Free the memory located by CFW_ComposePDU_ASYN
                                        //CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                                        //Go to CFW_SM_STATE_WAIT directly!
                                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                                        pEvent = &sMeEvent;
                                        pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                                    }
                                    else if(0xf0 == sMeEvent.nType)
                                    {
                                        goto flag_fail;
                                    }
                                }
                                else
                                {
                                    goto flag_fail;
                                }
                            }
                        }
                        else if(CFW_SIM_SMS_SUPPORT_LONG_MSG && (sComposeEvent.nType == 0x01)) //for a longer SMS
                        {
                            ret = 0x0;
                            CFW_SMS_STORAGE_INFO  sStorageInfo;

                            //sComposeEvent
                            //pEvent->nParam1 指向一个176byte固定结构
                            //pEvent->nParam2 为一个32位数组,each part is 8 bit,as following
                            //   _____________________________________
                            //  |           |           |          |            |
                            //  | part A | part B | part C | part D  |
                            //  |______|______|______|_______|
                            // A : Current index
                            // B : Total count
                            // C : MR
                            // D : 结构 Size
                            //Point pEvent->nParam1/2 to MT handle
                            pSmsMOInfo->pTmpData      = (UINT8 *)(sComposeEvent.nParam1);
                            pSmsMOInfo->pTmpPara      = (UINT8 *)(sComposeEvent.nParam2);
                            pSmsMOInfo->pTmpData_Addr = (UINT8 *)(sComposeEvent.nParam1);
                            pSmsMOInfo->pTmpPara_Addr = (UINT8 *)(sComposeEvent.nParam2);
                            //Set value
                            pSmsMOInfo->isLongerMsg         = 1;  // a longer msg
                            pSmsMOInfo->nLongerMsgCurrent   = *(pSmsMOInfo->pTmpPara + 3);  //part A
                            pSmsMOInfo->nLongerMsgCount     = *(pSmsMOInfo->pTmpPara + 2);  //part B
                            pSmsMOInfo->nMR                 = *(pSmsMOInfo->pTmpPara + 1);  //part C
                            pSmsMOInfo->nLongerEachPartSize = *(pSmsMOInfo->pTmpPara);      //part D

                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc Write StorageID is %d ! \n",0x08101029)), pSmsMOInfo->nLocation);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("sms_WriteAoProc Write a Longer sms, Total(Curr):%d (%d)! \n",0x0810102a)), pSmsMOInfo->nLongerMsgCount, pSmsMOInfo->nLongerMsgCurrent);
                            pSmsMOInfo->isLongerMsg = 1; //a longer msg
                            //if add new, verify there is free slot or not
                            if(pSmsMOInfo->nIndex == 0) //Add new
                            {
                                //dual sim mode, we only have storage ME and SM, we don't support any other storage type.
                                //Verify the free slot
                                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc has free slot: %d ! \n",0x0810102b)), (sStorageInfo.totalSlot - sStorageInfo.usedSlot));
                                if(pSmsMOInfo->nLongerMsgCount > (sStorageInfo.totalSlot - sStorageInfo.usedSlot))
                                {
                                    if( CFW_SMS_STORAGE_ME == pSmsMOInfo->nLocation )
                                    {
                                        pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;
                                        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc has free slot: %d ! \n",0x0810102c)), (sStorageInfo.totalSlot - sStorageInfo.usedSlot));
                                        if(pSmsMOInfo->nLongerMsgCount > (sStorageInfo.totalSlot - sStorageInfo.usedSlot))
                                        {
                                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                                            CFW_GetUTI(hAo, &nGetUti);
                                            CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_D0_SIM_SMS_STORAGE_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                            CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                            return ERR_CMS_UNKNOWN_ERROR;
                                        }
                                    }
                                    else
                                    {
                                        pSmsMOInfo->nLocation = CFW_SMS_STORAGE_ME;
                                        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc has free slot: %d ! \n",0x0810102d)), (sStorageInfo.totalSlot - sStorageInfo.usedSlot));
                                        if(pSmsMOInfo->nLongerMsgCount > (sStorageInfo.totalSlot - sStorageInfo.usedSlot))
                                        {
                                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                                            CFW_GetUTI(hAo, &nGetUti);
                                            CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_D0_SIM_SMS_STORAGE_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                            CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                            return ERR_CMS_UNKNOWN_ERROR;
                                        }
                                    }

                                }
                            }// end of if(pSmsMOInfo->nIndex == 0)

                            //Write long msg to ME
                            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)//To ME
                            {
                                UINT32 nTime = 0x0, nAppInt32 = 0x0;
                                UINT8 i = 0x0;
                                TM_FILETIME nFileTime;
                                TM_GetSystemFileTime(&nFileTime);
                                nTime = nFileTime.DateTime;
                                for(i = 0; i < pSmsMOInfo->nLongerMsgCount; i++)
                                {
                                    ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, pSmsMOInfo->pTmpData,
                                                             SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR(" sms_WriteAoProc  CFW_MeWriteMessage long,pSmsMOInfo->nIndex is %d , sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x0810102e), pSmsMOInfo->nIndex  , sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                                    if(0 == sMeEvent.nType) //Write OK
                                    {
                                        if(pSmsMOInfo->nIndex == 0)//Write a new long sms
                                        {
                                            //CFW_SMS_STORAGE_INFO  sStorageInfo;
                                            CFW_SMS_LONG_INFO    *pTmpSmsLongInfo = NULL;
                                            //use nListStartIndex to save the first index
                                            if(1 == pSmsMOInfo->nLongerMsgCurrent)
                                                pSmsMOInfo->nListStartIndex = (UINT16)sMeEvent.nParam1;
                                            //Update storage info
                                            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                                            CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                                            if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                                                sStorageInfo.usedSlot++;
                                            if(CFW_SMS_STORED_STATUS_UNREAD == pSmsMOInfo->nStatus)
                                                sStorageInfo.unReadRecords++;
                                            else if(CFW_SMS_STORED_STATUS_READ == pSmsMOInfo->nStatus)
                                                sStorageInfo.readRecords++;
                                            else if(CFW_SMS_STORED_STATUS_UNSENT == pSmsMOInfo->nStatus)
                                                sStorageInfo.unsentRecords++;
                                            else if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == pSmsMOInfo->nStatus)        ||
                                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == pSmsMOInfo->nStatus)   ||
                                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == pSmsMOInfo->nStatus)  ||
                                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == pSmsMOInfo->nStatus))
                                                sStorageInfo.sentRecords++;
                                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                                            //Malloc CFW_SMS_LONG_INFO for pStru_SmsLongInfo
                                            pTmpSmsLongInfo = (CFW_SMS_LONG_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_LONG_INFO));
                                            if(!pTmpSmsLongInfo)
                                            {
                                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_WriteAoProc  pTmpSmsLongInfo malloc error!!! \n ",0x0810102f));
                                                goto flag_fail;
                                            }
                                            SUL_ZeroMemory8(pTmpSmsLongInfo, (SIZEOF(CFW_SMS_LONG_INFO)));
                                            pTmpSmsLongInfo->count   = pSmsMOInfo->nLongerMsgCount;
                                            pTmpSmsLongInfo->current = pSmsMOInfo->nLongerMsgCurrent;
                                            pTmpSmsLongInfo->id      = pSmsMOInfo->nMR;
                                            pTmpSmsLongInfo->nStatus = pSmsMOInfo->nStatus;
                                            pTmpSmsLongInfo->index   = (UINT16)(sMeEvent.nParam1);
                                            pTmpSmsLongInfo->pNext   = NULL;
                                            //Conncet the struct to the link
                                            if(pSmsMOInfo->pStru_SmsLongInfo)
                                            {
                                                CFW_SMS_LONG_INFO *pLink = NULL;
                                                pLink = pSmsMOInfo->pStru_SmsLongInfo;
                                                while(pLink->pNext)
                                                {
                                                    pLink = (CFW_SMS_LONG_INFO *)(pLink->pNext);
                                                }
                                                pLink->pNext = (UINT32 *)pTmpSmsLongInfo;
                                            }
                                            else // for writing the first one
                                                pSmsMOInfo->pStru_SmsLongInfo = pTmpSmsLongInfo;

                                            if(pSmsMOInfo->nLongerMsgCurrent < pSmsMOInfo->nLongerMsgCount)
                                            {
                                                //offset
                                                pSmsMOInfo->pTmpPara += 4;
                                                pSmsMOInfo->pTmpData += pSmsMOInfo->nLongerEachPartSize;
                                                //Set value
                                                pSmsMOInfo->nLongerMsgCurrent   = *(pSmsMOInfo->pTmpPara + 3);  //part A
                                                pSmsMOInfo->nLongerEachPartSize = *(pSmsMOInfo->pTmpPara );     //part D
                                            }
                                            //[[xueww[+] 2007.03.02 //can turn back the index of every msg
                                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The pSmsMOInfo->nLongerMsgCurrent is %d\n",0x08101030), pSmsMOInfo->nLongerMsgCurrent);
                                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The pSmsMOInfo->nLongerMsgCount is %d\n",0x08101031), pSmsMOInfo->nLongerMsgCount);
                                            if((i + 1) < pSmsMOInfo->nLongerMsgCount)
                                            {
                                                //[[xueww[mod] 2007.06.19
                                                UINT16 nParam = 0x0;
                                                UINT8  nSign = 0X0;

                                                CFW_GetUTI(hAo, &nGetUti);
                                                sMeEvent.nParam1 = (UINT32)(sMeEvent.nParam1 + (pSmsMOInfo->nLocation << 16));

                                                //if it is a long msg,the HIUINT8(LOUINT16 of param2) will be the MR of the msg,
                                                //HIUINT4(LOUINT8(LOUINT16 of param2)) will be the current count of multi-part message
                                                //LOUINT4(LOUINT8(LOUINT16 of param2)) will be the total count of multi-part message
                                                nParam = pSmsMOInfo->nMR;
                                                nSign = (i + 1);
                                                nSign = (nSign << 4) | pSmsMOInfo->nLongerMsgCount;
                                                nParam = (nParam << 8) | nSign;

                                                if(sMeEvent.nParam2 == 1)
                                                {
                                                    sMeEvent.nParam2 = 0x10000 + nParam;//(i+1);
                                                }
                                                else
                                                {
                                                    sMeEvent.nParam2 = nParam;//i+1;
                                                }
                                                //]]xueww[mod] 2007.06.19
                                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, sMeEvent.nParam1, sMeEvent.nParam2, nGetUti | (nSimId << 24), sMeEvent.nType);
                                            }
                                            //]]xueww[+] 2007.03.02
                                        }
                                        else //Update an existed long msg,
                                        {
                                            //1.将原来的超长短信删掉
                                            //2.按pSmsMOInfo->nIndex保存超长第一条
                                            //3.保存其他
                                            //4.更新pMEFilename
                                            goto flag_fail;
                                        }
                                    }
                                    else //Write Fail
                                    {
                                        goto flag_fail;
                                    }
                                }//emd of for(i=0;i<pSmsMOInfo->nLongerMsgCount;i++)
                                if(pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)
                                {
                                    //Go to CFW_SM_STATE_WAIT directly!
                                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                                    pEvent = &sMeEvent;
                                    pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                                }//end of if(pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)
                            } //end of if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)

                            //Write long msg to SIM ,Save the first part of a longer msg
                            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM) //To SIM
                            {
#if CFW_SIM_SMS_SUPPORT_LONG_MSG
                                if(pSmsMOInfo->nIndex == 0)//Write a new long sms
                                {
                                    ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex,
                                                              pSmsMOInfo->pTmpData, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc,After CFW_SimWriteMessage, ret = %d \n",0x08101032)), ret);
                                    if(ret == ERR_SUCCESS)
                                    {
                                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                                        break;
                                    }
                                    else
                                        goto flag_fail;
                                }
                                else//Update an existed long msg,
                                {
                                    //1.将原来的超长短信删掉
                                    //2.按pSmsMOInfo->nIndex保存超长第一条
                                    //3.保存其他
                                    //4.更新pMEFilename
                                    goto flag_fail;
                                }
#else
                                goto flag_fail;
#endif
                            }//end of if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM) //To SIM


                        }
                        else if(sComposeEvent.nType == 0xf0) //compose error
                        {
                            goto flag_fail;
                        }
                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    }
                    else
                    {
                        goto flag_fail;
                    }
                }
            }
            else //Write PDU
            {
                //added by fengwei 20080624 for bug 8697 write msg with length =200, formate = pdu begin
                if (pSmsMOInfo->nDataSize > SMS_MO_ONE_PDU_SIZE)
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_INVALID_LENGTH, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                    return ERR_SUCCESS;
                }
                //added by fengwei 20080624 for bug 8697 write msg with length =200, formate = pdu end
                if(pEvent == (CFW_EV *)0xffffffff) //First come
                {
                    UINT8 *pWritePDU = NULL;
                    UINT8  nTP_MTI = 0x0, nSCA_Length = 0x0;
                    if(!pSmsMOInfo->isLongerMsg)//a normal PDU sms
                    {
                        nSCA_Length = *pSmsMOInfo->pData;
                        nTP_MTI =  *(pSmsMOInfo->pData + nSCA_Length + 1);
                        if(((nTP_MTI) & 0x03) == 0x00) //SMS DELIVER REPORT or DELIVER
                        {
                            pSmsMOInfo->nMR = 0x88;// in fact , there is no MR for deliver

                            if((pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_UNREAD)  &&
                                    (pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_READ))
                            {
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                return ERR_SUCCESS;
                            }
                        }
                        else if(((nTP_MTI) & 0x03) == 0x01)//SMS SUBMIT or SUBMIT REPORT
                        {
                            //Get MR
                            pSmsMOInfo->nMR  = *(pSmsMOInfo->pData + nSCA_Length + 2); // Get MR from the PDU data

                            if((pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_UNSENT)                &&
                                    (pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ)       &&
                                    (pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV)  &&
                                    (pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE) &&
                                    (pSmsMOInfo->nStatus != CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE))
                            {
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                return ERR_SUCCESS;
                            }
                        }
                        else if(((nTP_MTI) & 0x03) == 0x10)//STATUS REPORT or SMS COMMAND
                        {
                            pSmsMOInfo->nMR = 0x88;// in fact , there is no MR for STATUS REPORT or SMS COMMAND
                        }
                        //Malloc
                        pWritePDU = (UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE);
                        if(!pWritePDU)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_WriteAoProc  pWritePDU malloc error!!! \n ",0x08101033));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pWritePDU, SMS_MO_ONE_PDU_SIZE);
                        //Set the status
                        *pWritePDU = pSmsMOInfo->nStatus;
                        //Copy data
                        SUL_MemCopy8((UINT8 *)(pWritePDU + 1), (UINT8 *)pSmsMOInfo->pData, pSmsMOInfo->nDataSize);
                        //To SIM
                        if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                        {
                            ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pWritePDU, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                            if(ret == ERR_SUCCESS)
                            {
                                CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                            }
                            else
                            {
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_SMS_FREE(pWritePDU);
                                CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                return ERR_SUCCESS;
                            }
                            break;
                        }
                        //To ME
                        else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                        {
                            UINT32 nTime = 0x0, nAppInt32 = 0x0;
                            TM_FILETIME nFileTime;
                            TM_GetSystemFileTime(&nFileTime);
                            nTime = nFileTime.DateTime;
                            ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, pWritePDU, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("sms_WriteAoProc  CFW_MeWriteMessage PDU,pSmsMOInfo->nIndex is %d , sMeEvent.nParam1 is (0x)%x , sMeEvent.nParam2 is (0x)%x , sMeEvent.nType is (0x)%x\n ",0x08101034), pSmsMOInfo->nIndex  , sMeEvent.nParam1, sMeEvent.nParam2, sMeEvent.nType);
                            if(ret == ERR_SUCCESS)
                            {
                                //Go to CFW_SM_STATE_WAIT directly!
                                CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                                pEvent = &sMeEvent;
                                pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                            }
                            else
                            {
                                CFW_GetUTI(hAo, &nGetUti);
                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                CSW_SMS_FREE(pWritePDU);
                                CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                                return ERR_SUCCESS;
                            }
                        }
                        //Free pWritePDUSize
                        CSW_SMS_FREE(pWritePDU);
                    }
                    else // Write a longer PDU msg
                    {
                        //??? later
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
                        return ERR_SUCCESS;
                    }
                }
            }//end  Write PDU
        }
        case CFW_SM_STATE_WAIT:
        {
            if(((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP)) &&
                    (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_WriteAoProc  pEvent->nType = 0x%x\n ",0x08101035), pEvent->nType);
                if(pEvent->nType == 0)
                {
                    if(pSmsMOInfo->isLongerMsg)// a long sms
                    {
                        if(CFW_SMS_STORAGE_ME == pSmsMOInfo->nLocation)
                        {
                            // the last part of a long msg
                            CFW_SMS_PARAMETER  sInfo;
                            SUL_ZeroMemory8(&sInfo, SIZEOF(CFW_SMS_PARAMETER));
                            //Verify MR
                            if(pSmsMOInfo->nMR == 0xff)
                                pSmsMOInfo->nMR = 0x01;
                            else
                                pSmsMOInfo->nMR++;
                            //Get MR
                            CFW_CfgGetDefaultSmsParam(&sInfo, nSimId);
                            sInfo.mr = pSmsMOInfo->nMR;
                            CFW_CfgSetDefaultSmsParam(&sInfo, nSimId);
                            //[[xueww[mod] 2007.03.02
                            pEvent->nParam1 = (UINT32)(pEvent->nParam1 + (pSmsMOInfo->nLocation << 16));
                            //]]xueww[mod] 2007.03.02
                        }
                        else if(CFW_SMS_STORAGE_SM == pSmsMOInfo->nLocation)
                        {
                            CFW_SMS_STORAGE_INFO  sStorageInfo;
                            CFW_SMS_LONG_INFO    *pTmpSmsLongInfo = NULL;
                            UINT32                ret = 0x0;
                            //use nListStartIndex to save the first index
                            if(1 == pSmsMOInfo->nLongerMsgCurrent)
                                pSmsMOInfo->nListStartIndex = (UINT16)pEvent->nParam1;
                            //Update storage info
                            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                            CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                            if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                                sStorageInfo.usedSlot++;
                            if(CFW_SMS_STORED_STATUS_UNREAD == pSmsMOInfo->nStatus)
                                sStorageInfo.unReadRecords++;
                            else if(CFW_SMS_STORED_STATUS_READ == pSmsMOInfo->nStatus)
                                sStorageInfo.readRecords++;
                            else if(CFW_SMS_STORED_STATUS_UNSENT == pSmsMOInfo->nStatus)
                                sStorageInfo.unsentRecords++;
                            else if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == pSmsMOInfo->nStatus)        ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == pSmsMOInfo->nStatus)   ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == pSmsMOInfo->nStatus)  ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == pSmsMOInfo->nStatus))
                                sStorageInfo.sentRecords++;
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                            //Malloc CFW_SMS_LONG_INFO for pStru_SmsLongInfo
                            pTmpSmsLongInfo = (CFW_SMS_LONG_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_LONG_INFO));
                            if(!pTmpSmsLongInfo)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_WriteAoProc  pTmpSmsLongInfo malloc error!!! \n ",0x08101036));
                                goto flag_fail;
                            }
                            SUL_ZeroMemory8((UINT8 *)pTmpSmsLongInfo, (SIZEOF(CFW_SMS_LONG_INFO)));
                            pTmpSmsLongInfo->count   = pSmsMOInfo->nLongerMsgCount;
                            pTmpSmsLongInfo->current = pSmsMOInfo->nLongerMsgCurrent;
                            pTmpSmsLongInfo->id      = pSmsMOInfo->nMR;
                            pTmpSmsLongInfo->nStatus = pSmsMOInfo->nStatus;
                            pTmpSmsLongInfo->index   = (UINT16)(pEvent->nParam1);
                            pTmpSmsLongInfo->pNext   = NULL;
                            //Conncet the struct to the link
                            if(pSmsMOInfo->pStru_SmsLongInfo)
                            {
                                CFW_SMS_LONG_INFO *pLink = NULL;
                                pLink = pSmsMOInfo->pStru_SmsLongInfo;
                                while(pLink->pNext)
                                {
                                    pLink = (CFW_SMS_LONG_INFO *)(pLink->pNext);
                                }
                                pLink->pNext = (UINT32 *)pTmpSmsLongInfo;
                            }
                            else // for writing the first one
                                pSmsMOInfo->pStru_SmsLongInfo = pTmpSmsLongInfo;
                            //the last part of a long msg
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc,pSmsMOInfo->nLongerMsgCurrent =%d \n",0x08101037)), pSmsMOInfo->nLongerMsgCurrent);
                            if(pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)
                            {
                                CFW_SMS_PARAMETER  sInfo;
                                //Verify MR
                                if(pSmsMOInfo->nMR == 0xff)
                                    pSmsMOInfo->nMR = 0x01;
                                else
                                    pSmsMOInfo->nMR++;
                                //Get MR
                                CFW_CfgGetDefaultSmsParam(&sInfo, nSimId);
                                sInfo.mr = pSmsMOInfo->nMR;
                                CFW_CfgSetDefaultSmsParam(&sInfo, nSimId);

                                //[[xueww[mod] 2007.03.02
                                pEvent->nParam1 = (UINT32)(pEvent->nParam1 + (pSmsMOInfo->nLocation << 16));
                                //]]xueww[mod] 2007.03.02
                            }
                            else//continue to write
                            {
                                //[[xueww[+] 2007.03.02 //can turn back the index of every msg
                                pEvent->nParam1 = (UINT32)(pEvent->nParam1 + (pSmsMOInfo->nLocation << 16));

                                //[[xueww[mod] 2007.06.19
                                UINT16 nParam = 0x0;
                                UINT8  nSign = 0X0;

                                CFW_GetUTI(hAo, &nGetUti);

                                //if it is a long msg,the HIUINT8(LOUINT16 of param2) will be the MR of the msg,
                                //HIUINT4(LOUINT8(LOUINT16 of param2)) will be the current count of multi-part message
                                //LOUINT4(LOUINT8(LOUINT16 of param2)) will be the total count of multi-part message
                                nParam = pSmsMOInfo->nMR;
                                nSign = pSmsMOInfo->nLongerMsgCurrent;
                                nSign = (nSign << 4) | pSmsMOInfo->nLongerMsgCount;
                                nParam = (nParam << 8) | nSign;

                                if(pEvent->nParam2 == 1)
                                {
                                    pEvent->nParam2 = 0x10000 + nParam;
                                    //pEvent->nParam2 = 0x10000 + pSmsMOInfo->nLongerMsgCurrent;
                                }
                                else
                                {
                                    pEvent->nParam2 = nParam;
                                    //pEvent->nParam2 = pSmsMOInfo->nLongerMsgCurrent;
                                }
                                //]]xueww[mod] 2007.06.19
                                CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, pEvent->nParam1, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);
                                //]]xueww[+] 2007.03.02
                                //offset
                                pSmsMOInfo->pTmpPara += 4;
                                pSmsMOInfo->pTmpData += pSmsMOInfo->nLongerEachPartSize;
                                //Set value
                                pSmsMOInfo->nLongerMsgCurrent   = *(pSmsMOInfo->pTmpPara + 3);  //part A
                                pSmsMOInfo->nLongerEachPartSize = *(pSmsMOInfo->pTmpPara );     //part D

                                if(pSmsMOInfo->nIndex == 0)//Write a new long sms
                                {
                                    ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex,
                                                              (UINT8 *)pSmsMOInfo->pTmpData, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_WriteAoProc Write a Longer sms,After CFW_SimWriteMessage ,ret = %d! \n",0x08101038)), ret);
                                    if(ret == ERR_SUCCESS)
                                    {
                                        CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                                        break;
                                    }
                                    else
                                        goto flag_fail;
                                }
                                else
                                {
                                }
                            }//end of if(pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)
                        }
                    }
                    else //a normal sms
                    {
                        pEvent->nParam1 = pEvent->nParam1 + (pSmsMOInfo->nLocation << 16);
                        //Update CFW_SMS_STORAGE_INFO,  add a new msg
                        if((pSmsMOInfo->nIndex == 0) &&
                                ((pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM) || (pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)))
                        {
                            CFW_SMS_STORAGE_INFO  sStorageInfo;
                            //Update storage info
                            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                            CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                            if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                                sStorageInfo.usedSlot++;
                            if(CFW_SMS_STORED_STATUS_UNREAD == pSmsMOInfo->nStatus)
                                sStorageInfo.unReadRecords++;
                            else if(CFW_SMS_STORED_STATUS_READ == pSmsMOInfo->nStatus)
                                sStorageInfo.readRecords++;
                            else if(CFW_SMS_STORED_STATUS_UNSENT == pSmsMOInfo->nStatus)
                                sStorageInfo.unsentRecords++;
                            else if((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ == pSmsMOInfo->nStatus)        ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV == pSmsMOInfo->nStatus)   ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE == pSmsMOInfo->nStatus)  ||
                                    (CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE == pSmsMOInfo->nStatus))
                                sStorageInfo.sentRecords++;
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                        }
                    }
                }
                else if(pEvent->nType == 0xf0)
                {
                    if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_ME_SPACE_FULL)  //Me Return error
                    {
                        pEvent->nParam1 = ERR_CMS_D0_SIM_SMS_STORAGE_FULL;
                    }
                    else if(pEvent->nParam1 == ERR_CME_MEMORY_FULL)
                    {
                        pEvent->nParam1 = ERR_CMS_D0_SIM_SMS_STORAGE_FULL;
                    }
                    else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                    {
                        pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                    }
                    else
                    {
                        pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                    }
                    pEvent->nParam2 = pSmsMOInfo->nIndex;
                }
                else
                {
                    pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                    pEvent->nParam2 = pSmsMOInfo->nIndex;
                }
                //CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE|CFW_AO_PROC_CODE_CONSUMED);
                //if a normal msg or the last part of a longer msg;
                if((!pSmsMOInfo->isLongerMsg) || (pSmsMOInfo->isLongerMsg && (pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)) )
                {
                    //added by fengwei 20080627 begin. when the storage is full after writing, we should notify AT.
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    //get storage info
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    //added by fengwei 20080627 end. when the storage is full after writing, we should notify AT.

                    CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                    CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                    CFW_GetUTI(hAo, &nGetUti);
                    //[[xueww[mod] 2007.03.02 //if it is a long msg,the LOUINT16 of param2 will be the current count of multi-part message
                    UINT16 nParam = 0x0;
                    UINT8  nSign = 0X0;

                    if(pSmsMOInfo->isLongerMsg) //the last part of a longer msg
                    {
                        //if it is a long msg,the HIUINT8(LOUINT16 of param2) will be the MR of the msg,
                        //HIUINT4(LOUINT8(LOUINT16 of param2)) will be the current count of multi-part message
                        //LOUINT4(LOUINT8(LOUINT16 of param2)) will be the total count of multi-part message
                        nParam = pSmsMOInfo->nMR - 1;
                        nSign = pSmsMOInfo->nLongerMsgCurrent;
                        nSign = (nSign << 4) | pSmsMOInfo->nLongerMsgCount;
                        nParam = (nParam << 8) | nSign;

                        if(pEvent->nParam2 == 1)
                        {
                            pEvent->nParam2 = 0x10000 + nParam;
                            //pEvent->nParam2 = 0x10000 + pSmsMOInfo->nLongerMsgCurrent;
                        }
                        else
                        {
                            pEvent->nParam2 = nParam;
                            //pEvent->nParam2 = pSmsMOInfo->nLongerMsgCurrent;
                        }
                    }
                    else //a normal msg
                    {
                        if(pEvent->nParam2 == 1)
                        {
                            pEvent->nParam2 = 0x10000;
                        }
                        else
                        {
                            pEvent->nParam2 = 0x0;
                        }
                    }

                    //]]xueww[mod] 2007.03.02
                    CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, pEvent->nParam1, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);

                    //added by fengwei 20080627 begin. when the storage is full after writing, we should notify AT.
                    if (sStorageInfo.totalSlot == sStorageInfo.usedSlot)
                    {

                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SEND EV_CFW_SMS_INFO_IND 3\n",0x08101039));
                        CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND, 1, pSmsMOInfo->nLocation, nGetUti | (nSimId << 24), 0);
                    }
                    //added by fengwei 20080627 end. when the storage is full after writing, we should notify AT.

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                //[[xueww[+] 2007.09.13
                else
                {
                    //added by fengwei 20080627 begin. when the storage is full after writing, we should notify AT.
                    CFW_SMS_STORAGE_INFO  sStorageInfo;
                    //get storage info
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    //added by fengwei 20080627 end. when the storage is full after writing, we should notify AT.

                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, pEvent->nParam1, pEvent->nParam2, nGetUti | (nSimId << 24), pEvent->nType);

                    //added by fengwei 20080627 begin. when the storage is full after writing, we should notify AT.
                    if (sStorageInfo.totalSlot == sStorageInfo.usedSlot)
                    {

                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SEND EV_CFW_SMS_INFO_IND 1\n",0x0810103a));
                        CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND, 1, pSmsMOInfo->nLocation, nGetUti | (nSimId << 24), 0);
                    }
                    //added by fengwei 20080627 end. when the storage is full after writing, we should notify AT.

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                //]]xueww[+] 2007.09.13
            }
        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_WriteAoProc  error\n",0x0810103b));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
    return ERR_SUCCESS;

flag_fail:
    CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
    CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
    CFW_GetUTI(hAo, &nGetUti);
    CFW_PostNotifyEvent(EV_CFW_SMS_WRITE_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
    CSW_PROFILE_FUNCTION_EXIT(sms_WriteAoProc);
    return ERR_CMS_UNKNOWN_ERROR;
}

extern bool g_SMSConcat[CFW_SIM_COUNT];

#define CONCATENATE_8SMS            0x00
#define SPECIAL_SMS_IND             0x01
#define CONCATENATE_16SMS           0x08
#define SIM_TOOLKIT_SECURITY_HEADER 0x70        //MO should be 0x70 and MT should be 0x71

#define LONG_SHORT_MESSAGE              0x80
#define STK_SECURITY_HEADER_MESSAGE 0x81
#define OTHER_TYPE_MESSAGE              0x8F

UINT8 CFW_TypeOfSMS(CFW_SMS_MO_INFO *pSmsMOInfo)
{
    UINT8 MaxCount = 0;
    UINT8 CurrentCount = 0;
    UINT8 nSCALength, nDALength = 0x0, nVPF = 0x0, nVPLength = 0;
    nSCALength = *pSmsMOInfo->pData;
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TypeOfSMS: nSCALength = %x",0x0810103c),nSCALength);
    //Point to the length of TP-DA
    //one byte SCALength, nSCALength, MTI, MR
    UINT16 nOffset =  1 + nSCALength + 1 + 1;
    nDALength   = pSmsMOInfo->pData[nOffset];
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TypeOfSMS: nDALength = %x",0x0810103d),nDALength);
    nDALength = ((nDALength + 1) >> 1) + 2; //all numbers of address occupied,including length, type and itself
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_TypeOfSMS: nOffset = %x, nDALength= %x\n",0x0810103e), nOffset,nDALength);
    nVPF = (pSmsMOInfo->pData[nSCALength + 1] >> 3) & 0x03;

    if(nVPF == 2)  //TP-VP filed present-relative format
    {
        nVPLength = 1;
    }
    else if(nVPF == 1)//TP-VP filed present-enhanced format
    {
        nVPLength = 7;
    }
    else if(nVPF == 3)//TP-VP filed present-absolute format
    {
        nVPLength = 7;
    }
    else if(nVPF == 0)//TP-VP filed no present
    {
        nVPLength = 0;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nVPF = %x, nVPLength= %x\n",0x0810103f), nVPF,nVPLength);
    //first byte of SCALength, nSCALength,MTI, MR, nDALength(length byte of DA, type of DA,DA
    // length),PID,DCS,VP,UDL
    nOffset = 1 + nSCALength + 1 + 1 + nDALength + 1 + 1 + nVPLength + 1;
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_TypeOfSMS:    nOffset = %x, nDALength= %x\n",0x08101040), nOffset,nDALength);

    UINT8 nUDHL = pSmsMOInfo->pData[nOffset];
    //nIEI is the next byte of nUDHL, so plus one byte.
    UINT8 nIEI = pSmsMOInfo->pData[nOffset + 1];
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nUDHL = %x, nIEI= %x\n",0x08101041), nUDHL, nIEI);
    while(nUDHL > 0 && nOffset < pSmsMOInfo->nDataSize)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nIEI= %x\n",0x08101042), nIEI);
        if(nIEI == SIM_TOOLKIT_SECURITY_HEADER)
        {
            // 2 = IEI + one byte IEIDL
            //3 = IEI + IEIDL + first byte of two length
            if((pSmsMOInfo->pData[nOffset + 2] == 0)&&(pSmsMOInfo->pData[nOffset + 3] == 0))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("STK_SECURITY_HEADER_MESSAGE",0x08101043));
                return STK_SECURITY_HEADER_MESSAGE;
            }
        }
        else if((nIEI ==CONCATENATE_8SMS) ||(nIEI == CONCATENATE_16SMS))
        {
            nOffset += 1/*IEI*/ + 1 /*LIE*/+ 1 /*concatenated referece number*/ +
                       1/*Max number of concatenated sms*/;
            MaxCount = pSmsMOInfo->pData[nOffset];
            CurrentCount = pSmsMOInfo->pData[nOffset + 1];
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("It's a longer sms,Max count is %d\n",0x08101044), MaxCount);
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("It's a longer sms,current number is %d\n",0x08101045), CurrentCount);
            return LONG_SHORT_MESSAGE;
        }
        else
            nUDHL -= 2 + pSmsMOInfo->pData[nOffset + 2];
        nOffset += 2 + pSmsMOInfo->pData[nOffset + 2];
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nUDHL = %x, nOffset= %x\n",0x08101046), nUDHL, nOffset);
    }
    return OTHER_TYPE_MESSAGE;
}

#if 0
UINT8 CFW_GetPduMaxCount(CFW_SMS_MO_INFO *pSmsMOInfo)
{
    UINT8 MaxCount = 0x0, nSCALength = 0x0, nDALength = 0x0, nVPF = 0x0, nVPLength = 0x0;
    nSCALength = *pSmsMOInfo->pData;
    UINT8 *pTmp = NULL;

    pTmp   = (UINT8 *)(pSmsMOInfo->pData + nSCALength + 3); //Point to the length of TP-DA
    nDALength = (UINT8)(*pTmp / 2 + *pTmp % 2 + 2);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pTmp = %x, nDALength= %x\n",0x08101047), pTmp,nDALength);
    nVPF = (UINT8)((*(pSmsMOInfo->pData + nSCALength + 1) & 0x18) >> 3);

    if(nVPF == 2)  //TP-VP filed present-relative format
    {
        nVPLength = 1;
    }
    else if(nVPF == 1)//TP-VP filed present-enhanced format
    {
        nVPLength = 7;
    }
    else if(nVPF == 3)//TP-VP filed present-absolute format
    {
        nVPLength = 7;
    }
    else if(nVPF == 0)//TP-VP filed no present
    {
        nVPLength = 0;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nVPF = %x, nVPLength= %x\n",0x08101048), nVPF,nVPLength);
    //MaxCount =  (UINT8*)(pSmsMOInfo->pData + nSCALength + 1 + 1 + 1 + 1 + nDALength + 1 + 1 + nVPLength + 1 + 1 + 1 + 1);
    //  +first byte of SCALength, MTI, MR,
    // length byte of DA, type of DA.
    UINT8 ntemp = pSmsMOInfo->pData[1 + nSCALength + 1 + 1 + nDALength + 1 + 1 + nVPLength + 1 + 1 + 1 + 1];
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ntemp= %x\n",0x08101049), ntemp);

    MaxCount =  pSmsMOInfo->pData[nSCALength + 1 + 1 + 1 + 1 + nDALength + 1 + 1 + nVPLength + 1 + 1 + 1 + 1];
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("It's a longer sms,Max count is %d\n",0x0810104a), MaxCount);
    return MaxCount;
}
UINT8 CFW_GetPduCurrentCount(CFW_SMS_MO_INFO *pSmsMOInfo)
{
    UINT8 CurrentCount = 0x0, nSCALength = 0x0, nDALength = 0x0, nVPF = 0x0, nVPLength = 0x0;
    UINT8 *pTmp = NULL;
    nSCALength = *pSmsMOInfo->pData;
    pTmp      = (UINT8 *)(pSmsMOInfo->pData + nSCALength + 3); //Point to the length of TP-DA
    nDALength = (UINT8)(*pTmp / 2 + *pTmp % 2 + 2);

    nVPF = (UINT8)((*(pSmsMOInfo->pData + nSCALength + 1) & 0x18) >> 3);
    if(nVPF == 2)  //TP-VP filed present-relative format
    {
        nVPLength = 1;
    }
    else if(nVPF == 1)//TP-VP filed present-enhanced format
    {
        nVPLength = 7;
    }
    else if(nVPF == 3)//TP-VP filed present-absolute format
    {
        nVPLength = 7;
    }
    else if(nVPF == 0)//TP-VP filed no present
    {
        nVPLength = 0;
    }

    CurrentCount = pSmsMOInfo->pData[nSCALength + 1 + 1 + 1 + 1 + nDALength + 1 + 1 + nVPLength + 1 + 1 + 1 + 1 + 1];
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("It's a longer sms,current number is %d\n",0x0810104b), CurrentCount);
    return CurrentCount;

}
#endif

UINT8 CFW_GetPduUDHI(CFW_SMS_MO_INFO *pSmsMOInfo, UINT8 nSimID)

{
    UINT8 PDUType;
    UINT8  UDHI, nSCALength;

    nSCALength = *pSmsMOInfo->pData;
    CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSmsMOInfo->pData, 200, 16);
    //PDUType = (UINT8*)(pSmsMOInfo->pData + nSCALength + 1); //for android divided long sms
    PDUType = pSmsMOInfo->pData[nSCALength + 1]; //for android divided long sms

    UDHI = (PDUType >> 6) & 0x01;
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("PDUType = %x, UDHI = %x",0x0810104c), PDUType,UDHI);
    if(UDHI == 1)
    {
        if(LONG_SHORT_MESSAGE == CFW_TypeOfSMS(pSmsMOInfo))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_GetPduUDHI: Long short Message!",0x0810104d));
            CFW_SetSMSConcat (TRUE, nSimID);
        }
#if 0
        MaxCount = CFW_GetPduMaxCount(pSmsMOInfo);
        CurrentCount = CFW_GetPduCurrentCount(pSmsMOInfo);
        if(MaxCount - CurrentCount != 0)
        {
            CFW_SetSMSConcat (TRUE, nSimID);
        }
        else
        {
            CFW_SetSMSConcat (FALSE, nSimID);
        }
#endif

    }

    return 0;
}


UINT32 sms_SendProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32  nState = 0x0, ret = 0x0, nGetUti = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL; //Point to MO private data.
    CFW_EV        sComposeEvent;

    CSW_PROFILE_FUNCTION_ENTER(sms_SendProc);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);

    SUL_ZeroMemory8(&sComposeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_SendProc, nState = 0x%x pSmsMOInfo->nFormat %d\n",0x0810104e), nState, pSmsMOInfo->nFormat);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            ret = 0x0;
            if(pSmsMOInfo->nFormat)  //Compose PDU Synchronous
            {
                UINT8 nConcat = 0x0;
                nConcat = 0x00; // 0000 0000   bit3=0:for Send,bit2=0:no other sms(bit2=1: keep connection), don't care other bit.
                ret = sms_tool_ComposePDU(nConcat, CFW_SMS_STORED_STATUS_UNSENT, pSmsMOInfo->pData,
                                          pSmsMOInfo->nDataSize, &pSmsMOInfo->sNumber, nSimId, &sComposeEvent);
                pEvent = &sComposeEvent;
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_SendProc pEvent->nType = 0x%x\n",0x0810104f)), pEvent->nType);
                if(ret == ERR_SUCCESS )
                {
                    if(pEvent->nType == 0x0) //Send normal Text sms
                    {
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;
                        ret = 0x0;
                        UINT8 nStruSize = 0x0;
                        UINT8 nPath = 0x0;

                        nStruSize = (UINT8)(SIZEOF(api_SmsPPSendReq_t) + ((api_SmsPPSendReq_t *)pEvent->nParam1)->DataLen);
                        pSmsMOInfo->isLongerMsg = 0;  // a normal msg
                        //Get MR
                        pSmsMOInfo->nMR = (UINT8)pEvent->nParam2;
                        //Systen will Free pSendSmsReq
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc  pSendSmsReq malloc error!!! \n ",0x08101050));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        SUL_MemCopy8((UINT8 *)pSendSmsReq, (UINT8 *)pEvent->nParam1, nStruSize);
                        CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSendSmsReq, nStruSize, 16);
                        pSendSmsReq->Concat = g_SMSConcat[nSimId];
                        nPath = CFW_GprsGetSmsSeviceMode();
                        if((nPath == 0x0)||(nPath == 0x2))
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GPRS;
                        }
                        else
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GSM;
                        }
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq, nSimId);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x08101051), pSendSmsReq->Concat);
                        //hameina[mod] save for send fail.
                        //CSW_SMS_FREE((UINT8*)(pEvent->nParam1));
                        pSmsMOInfo->pSendSmsReq = (api_SmsPPSendReq_t *)(pEvent->nParam1);
                        if(ret == ERR_SUCCESS )
                        {
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc nret: %d\r\n",0x08101052), ret);
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
                    //
                    //Modify by lixp for the support of long sms
                    //at 20060721
#if CFW_SIM_SMS_SUPPORT_LONG_MSG
                    else if((pEvent->nType == 0x01)) //for a longer SMS
                    {
                        //sComposeEvent
                        //pEvent->nParam1 为一个api_SmsPPSendReq_t结构体数组
                        //pEvent->nParam2 为一个32位数组,each part is 8 bit,as following
                        //   _____________________________________
                        //  |        |        |        |         |
                        //  | part A | part B | part C | part D  |
                        //  |________|________|________|_________|
                        // A : Current index
                        // B : Total count
                        // C : MR
                        // D : Current api_SmsPPSendReq_t Size
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;
                        ret = 0x0;
                        UINT8  nStruSize = 0x0;
                        UINT8 nPath =0X0;

                        //Point pEvent->nParam1/2 to MT handle
                        pSmsMOInfo->pTmpData      = (UINT8 *)(pEvent->nParam1);
                        pSmsMOInfo->pTmpPara      = (UINT8 *)(pEvent->nParam2);
                        pSmsMOInfo->pTmpData_Addr = (UINT8 *)(pEvent->nParam1);
                        pSmsMOInfo->pTmpPara_Addr = (UINT8 *)(pEvent->nParam2);


                        //Set value
                        pSmsMOInfo->isLongerMsg       = 1;  // a longer msg
                        pSmsMOInfo->nLongerMsgCurrent = *(pSmsMOInfo->pTmpPara + 3);  //part A
                        pSmsMOInfo->nLongerMsgCount   = *(pSmsMOInfo->pTmpPara + 2);  //part B
                        pSmsMOInfo->nMR               = *(pSmsMOInfo->pTmpPara + 1);  //part C
                        nStruSize                     = *(pSmsMOInfo->pTmpPara);      //part D
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc,1, nStruSize = %d\n",0x08101053), nStruSize);
                        //CSW_TC_MEMBLOCK(CFW_SMS_TS_ID,pSmsMOInfo->pTmpData_Addr, pSmsMOInfo->nLongerMsgCount*156, 16);
                        //CSW_TC_MEMBLOCK(CFW_SMS_TS_ID,pSmsMOInfo->pTmpPara_Addr, pSmsMOInfo->nLongerMsgCount*4, 16);
                        //Send the first part of a longer msg,System will Free pSendSmsReq
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc 2  pSendSmsReq malloc error!!! \n ",0x08101054));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        SUL_MemCopy8((UINT8 *)pSendSmsReq, pSmsMOInfo->pTmpData, nStruSize);
                        if((FALSE == g_SMSConcat[nSimId]) &&
                                ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        else if ((FALSE == g_SMSConcat[nSimId]) &&
                                 ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent != pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = TRUE;
                        }
                        else
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        nPath = CFW_GprsGetSmsSeviceMode();
                        if((nPath == 0x0)||(nPath == 0x2))
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GPRS;
                        }
                        else
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GSM;
                        }
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq, nSimId);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x08101055), pSendSmsReq->Concat);
                        if(ret == ERR_SUCCESS )
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc,2,call CFW_SendSclMessage success! \n",0x08101056));
                            pSmsMOInfo->pTmpPara += 4;
                            pSmsMOInfo->pTmpData += nStruSize;
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            //pSmsMOInfo->pTmpPara_Addr = NULL;
                            //pSmsMOInfo->pTmpData_Addr = NULL;

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
#else
                    else if((pEvent->nType == 0x01)) //Doesn't support longer SMS
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                        return ERR_SUCCESS;
                    }
#endif
                    else if(pEvent->nType == 0xf0)   //compose error
                    {
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                        return ERR_SUCCESS;
                    }
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
            else //Send PDU directly
            {
                CFW_GetPduUDHI(pSmsMOInfo, nSimId);
                ret = sms_mo_ComposeLongPDU(pSmsMOInfo, nSimId);
                if(ret != ERR_SUCCESS)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc ret =%d\n ",0x08101057), ret);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc: Send PDU directly\n ",0x08101058));
            }

        }
        break;
        case CFW_SM_STATE_WAIT:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_SendProc: pEvent->nEventId = %x, API_SMSPP_ACK_IND=%x\n",0x08101059),pEvent->nEventId, API_SMSPP_ACK_IND);
            if(pEvent->nEventId == API_SMSPP_ACK_IND)
            {
#ifdef CFW_CUSTOM_IMEI_COUNT
                CFW_CustomSendIMEIProc(TRUE, nSimId);
#endif
                CFW_SMS_PARAMETER  sInfo;
                UINT32             nP2 = 0x0;

                SUL_ZeroMemory8(&sInfo, SIZEOF(CFW_SMS_PARAMETER));
                //Verify MR
                if(pSmsMOInfo->nMR == 0xff)
                    pSmsMOInfo->nMR = 0x0;
                else
                    pSmsMOInfo->nMR++;
                //Get MR
                CFW_CfgGetDefaultSmsParam(&sInfo, nSimId);
                sInfo.mr = pSmsMOInfo->nMR;
                CFW_CfgSetDefaultSmsParam(&sInfo, nSimId);
                //Post MR
                if(pSmsMOInfo->nMR == 0)
                    pSmsMOInfo->nMR = 0x1;
                CFW_GetUTI(pSmsMOInfo->hAo, &nGetUti);
                nP2 = (pSmsMOInfo->nLongerMsgCount << 16) + pSmsMOInfo->nLongerMsgCurrent;
                CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, (pSmsMOInfo->nMR - 1), nP2, nGetUti | (nSimId << 24), 0);
                //if a normal msg or the last part of a longer msg
                if((!pSmsMOInfo->isLongerMsg) ||
                        ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc: a normal msg",0x0810105a));
                    CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                    CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                    CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                    pSmsMOInfo->pTmpPara_Addr = NULL;
                    pSmsMOInfo->pTmpData_Addr = NULL;
                    pSmsMOInfo->pSendSmsReq = NULL;

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                    return ERR_SUCCESS;
                }
                else  // a longer msg
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc: a longer msg",0x0810105b));
                    if(pSmsMOInfo->nFormat) //Text longer msg
                    {
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;
                        ret = 0x0;
                        UINT8  nStruSize = 0x0;
                        UINT8 nPath = 0x0;
                        //Set value
                        pSmsMOInfo->nLongerMsgCurrent = *(pSmsMOInfo->pTmpPara + 3);  //part A
                        pSmsMOInfo->nMR               = *(pSmsMOInfo->pTmpPara + 1);  //part C
                        nStruSize                     = *(pSmsMOInfo->pTmpPara );     //part D
                        //Send the first part of a longer msg,System will Free pSendSmsReq
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc 3  pSendSmsReq malloc error!!! \n ",0x0810105c));
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            pSmsMOInfo->pTmpPara_Addr = NULL;
                            pSmsMOInfo->pTmpData_Addr = NULL;

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        SUL_MemCopy8((UINT8 *)pSendSmsReq, pSmsMOInfo->pTmpData, nStruSize);
                        //CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSendSmsReq, nStruSize, 16);
                        if((FALSE == g_SMSConcat[nSimId]) &&
                                ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        else if ((FALSE == g_SMSConcat[nSimId]) &&
                                 ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent != pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = TRUE;
                        }
                        else
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        nPath = CFW_GprsGetSmsSeviceMode();
                        if((nPath == 0x0)||(nPath == 0x2))
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GPRS;
                        }
                        else
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GSM;
                        }
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq, nSimId);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x0810105d), pSendSmsReq->Concat);
                        if(ret == ERR_SUCCESS)
                        {
                            pSmsMOInfo->pTmpPara += 4;
                            pSmsMOInfo->pTmpData += nStruSize;
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc,5,call CFW_SendSclMessage failed! \n",0x0810105e));
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            pSmsMOInfo->pTmpPara_Addr = NULL;
                            pSmsMOInfo->pTmpData_Addr = NULL;

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc:  Text Message!",0x0810105f));
                    }
                    else  //PDU longer msg
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc:  PDU longer msg",0x08101060));
                        ret = 0x0;
                        UINT8 nPath = 0x0;
                        UINT16 nStruSize = 0x0;
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;

                        nStruSize = (UINT16)(SIZEOF(api_SmsPPSendReq_t) + pSmsMOInfo->nLongerEachPartSize);
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc 4  pSendSmsReq malloc error!!! \n ",0x08101061));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc: PDU longer msg",0x08101062));
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        //Copy all the data to compose a Send data
                        SUL_MemCopy8((UINT8 *)(pSendSmsReq), (UINT8 *)pSmsMOInfo->pSendSmsReq, nStruSize);

                        pSmsMOInfo->nLongerMsgCurrent++;
                        //Set Longer Current
                        pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 5] = pSmsMOInfo->nLongerMsgCurrent;
                        if(pSmsMOInfo->nLongerMsgCurrent != pSmsMOInfo->nLongerMsgCount)
                        {
                            //pSendSmsReq->Concat  = 1;
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                            //copy user data content
                            SUL_MemCopy8((UINT8 *)(pSendSmsReq->Data + pSmsMOInfo->nLongerIndexOffset + pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 0] + 1), pSmsMOInfo->pData, (0x8C - pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 0] - 1));
                            //pData offset
                            pSmsMOInfo->pData = pSmsMOInfo->pData + 0x8C - pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 0] - 1;
                        }
                        else//the last part of longer msg
                        {
                            //pSendSmsReq->Concat  = 0;
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                            pSendSmsReq->DataLen = (UINT8)(pSmsMOInfo->nLongerIndexOffset + pSmsMOInfo->nLongerLastUserDataSize);
                            //Set TP UDL
                            if(pSmsMOInfo->nDCS == 0) //GSM 7 bit, Total number os septets
                                pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset - 1] = (UINT8)((8 * pSmsMOInfo->nLongerLastUserDataSize) / 7 + 1);
                            else   //UCS2 or 8-bit
                                pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset - 1] = pSmsMOInfo->nLongerLastUserDataSize;
                            //copy user data content after head
                            SUL_MemCopy8((UINT8 *)(pSendSmsReq->Data + pSmsMOInfo->nLongerIndexOffset + pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 0] + 1), pSmsMOInfo->pData, (pSmsMOInfo->nLongerLastUserDataSize - pSendSmsReq->Data[pSmsMOInfo->nLongerIndexOffset + 0] - 1));
                        }
                        //Set MR
                        pSendSmsReq->Data[1] = pSmsMOInfo->nMR;
                        nPath = CFW_GprsGetSmsSeviceMode();
                        if((nPath == 0x0)||(nPath == 0x2))
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GPRS;
                        }
                        else
                        {
                            pSendSmsReq->Path = CFW_SMS_PATH_GSM;
                        }
                        CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSendSmsReq, nStruSize, 16);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x08101063), pSendSmsReq->Concat);
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq,nSimId);
                        if(ret == ERR_SUCCESS )
                        {
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                            pSmsMOInfo->pSendSmsReq = NULL;

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc: End",0x08101064));
                    }
//#endif
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONTINUE);
                }
            }
            else if(pEvent->nEventId == API_SMSPP_ERROR_IND)
            {
                //Parse the error code later
                ret = sms_mo_Parse_SmsPPErrorInd(hAo, (api_SmsPPErrorInd_t *)pEvent->nParam1);
                CFW_GetUTI(hAo, &nGetUti);
                if(ret != ERR_SUCCESS && ERR_CMS_RESOURCES_UNAVAILABLE != ret)
                {
                    pSmsMOInfo->nResendCnt = 0;
                    if(NULL != pSmsMOInfo->pTmpPara_Addr)
                    {
                        CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                        pSmsMOInfo->pTmpPara_Addr = NULL;

                    }
                    if(NULL != pSmsMOInfo->pTmpData_Addr)
                    {

                        CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                        pSmsMOInfo->pTmpData_Addr = NULL;
                    }
                    if(NULL != pSmsMOInfo->pSendSmsReq)
                    {
                        CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                        pSmsMOInfo->pSendSmsReq = NULL;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                }
                else if(ERR_CMS_RESOURCES_UNAVAILABLE == ret)
                {

                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSmsMOInfo->isLongerMsg is %d, pSmsMOInfo->nFormat=%d\n ",0x08101065), pSmsMOInfo->isLongerMsg, pSmsMOInfo->nFormat);
                    //here we need to resend the PP SMS
                    //Text longer msg
                    if(pSmsMOInfo->isLongerMsg && pSmsMOInfo->nFormat
                            && pSmsMOInfo->nResendCnt < CFW_SMS_MAX_RESEND_COUNT) //hadn't resent yet
                    {
                        pSmsMOInfo->nResendCnt++;
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;
                        ret = 0x0;
                        UINT8  nStruSize = 0x0;

                        //restore to the param which had been send
                        pSmsMOInfo->pTmpPara -= 4;
                        nStruSize                     = *(pSmsMOInfo->pTmpPara );     //part D
                        pSmsMOInfo->pTmpData -= nStruSize;
                        //Send the first part of a longer msg,System will Free pSendSmsReq
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc 5  pSendSmsReq malloc error!!! \n ",0x08101066));
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            //pSmsMOInfo->pTmpPara_Addr = NULL;
                            //pSmsMOInfo->pTmpData_Addr = NULL;

                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);

                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        SUL_MemCopy8((UINT8 *)pSendSmsReq, pSmsMOInfo->pTmpData, nStruSize);
                        CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSendSmsReq, nStruSize, 16);
                        if((FALSE == g_SMSConcat[nSimId]) &&
                                ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent == pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        else if ((FALSE == g_SMSConcat[nSimId]) &&
                                 ((pSmsMOInfo->isLongerMsg) && (pSmsMOInfo->nLongerMsgCurrent != pSmsMOInfo->nLongerMsgCount)))
                        {
                            pSendSmsReq->Concat     = TRUE;
                        }
                        else
                        {
                            pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        }
                        if((pSendSmsReq->Path != 0)&&(pSendSmsReq->Path != 1))
                        {
                            CSW_ASSERT(0);
                        }
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq, nSimId);

                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x08101067), pSendSmsReq->Concat);
                        if(ret == ERR_SUCCESS)
                        {
                            pSmsMOInfo->pTmpPara += 4;
                            pSmsMOInfo->pTmpData += nStruSize;
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc,5,call CFW_SendSclMessage error ret=0x%x \n",0x08101068) , ret);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            //pSmsMOInfo->pTmpPara_Addr = NULL;
                            //pSmsMOInfo->pTmpData_Addr = NULL;
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        return ERR_SUCCESS;
                    }
                    //hadn't resent yet)//send normal msg fail
                    else if(!pSmsMOInfo->isLongerMsg && pSmsMOInfo->nFormat
                            && pSmsMOInfo->nResendCnt < CFW_SMS_MAX_RESEND_COUNT) //hadn't resend yet
                    {
                        api_SmsPPSendReq_t *pSendSmsReq = NULL;
                        UINT8 nStruSize = 0;

                        pSmsMOInfo->nResendCnt++;

                        nStruSize = (UINT8)(SIZEOF(api_SmsPPSendReq_t) + pSmsMOInfo->pSendSmsReq->DataLen);
                        pSendSmsReq = (api_SmsPPSendReq_t *)CFW_MemAllocMessageStructure(nStruSize);
                        if(!pSendSmsReq)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc 6  pSendSmsReq malloc error!!! \n ",0x08101069));
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_NO_MORE_MEMORY;
                        }
                        SUL_ZeroMemory8(pSendSmsReq, nStruSize);
                        SUL_MemCopy8((UINT8 *)pSendSmsReq, (UINT8 *)( pSmsMOInfo->pSendSmsReq), nStruSize);
                        CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pSmsMOInfo->pSendSmsReq, nStruSize, 16);
                        pSendSmsReq->Concat     = g_SMSConcat[nSimId];
                        if((pSendSmsReq->Path != 0)&&(pSendSmsReq->Path != 1))
                        {
                            CSW_ASSERT(0);
                        }
                        ret = CFW_SendSclMessage(API_SMSPP_SEND_REQ, pSendSmsReq, nSimId);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc, Concat = 0x%x\n",0x0810106a), pSendSmsReq->Concat);
                        if(ret == ERR_SUCCESS)
                        {
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                            CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc,5,call CFW_SendSclMessage error ret=0x%x \n",0x0810106b), ret);
                            CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ERR_CMS_UNKNOWN_ERROR, 0, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
                    else
                    {
                        pSmsMOInfo->nResendCnt = 0;
                        if(NULL != pSmsMOInfo->pTmpPara_Addr)
                        {
                            CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                            pSmsMOInfo->pTmpPara_Addr = NULL;

                        }
                        if(NULL != pSmsMOInfo->pTmpData_Addr)
                        {

                            CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                            pSmsMOInfo->pTmpData_Addr = NULL;
                        }


                        if(NULL != pSmsMOInfo->pSendSmsReq)
                        {
                            CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                            pSmsMOInfo->pSendSmsReq = NULL;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc,6666,send EV_CFW_SMS_SEND_MESSAGE_RSP to MMI, P1=0X%X\n",0x0810106c), ret);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ret, 0, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                    }
                }

                else
                {
                    pSmsMOInfo->nResendCnt = 0;
                    if(NULL != pSmsMOInfo->pTmpPara_Addr)
                    {
                        CSW_SMS_FREE(pSmsMOInfo->pTmpPara_Addr);
                        pSmsMOInfo->pTmpPara_Addr = NULL;

                    }
                    if(NULL != pSmsMOInfo->pTmpData_Addr)
                    {

                        CSW_SMS_FREE(pSmsMOInfo->pTmpData_Addr);
                        pSmsMOInfo->pTmpData_Addr = NULL;
                    }


                    if(NULL != pSmsMOInfo->pSendSmsReq)
                    {
                        CSW_SMS_FREE(pSmsMOInfo->pSendSmsReq);
                        pSmsMOInfo->pSendSmsReq = NULL;
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SendProc,777,send EV_CFW_SMS_SEND_MESSAGE_RSP to MMI, P1=0X%X\n",0x0810106d), ret);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SEND_MESSAGE_RSP, ret, 0, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_SMS_MO_SRV_ID, hAo);
                }
                CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);

                return ERR_SUCCESS;
            }
        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc  error\n",0x0810106e));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_SendProc);
    return ERR_SUCCESS;
}


UINT32 sms_SetUnRead2ReadProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           ret = 0x0, nGetUti = 0x0, nState = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL;   //Point to MO private data.
    CFW_EV           sMeEvent;
    UINT8            nSmsStatus = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(sms_SetUnRead2ReadProc);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SetUnRead2ReadProc, nState = 0x%x\n",0x0810106f), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
            {
                ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
            {
                //Malloc for the read data.
                sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                if(!(UINT8 *)sMeEvent.nParam1)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetUnRead2ReadProc   sMeEvent.nParam1  malloc error!!! \n ",0x08101070));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId < 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                ret = CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                if(ret == ERR_SUCCESS)
                {
                    //Go to CFW_SM_STATE_WAIT directly!
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    pEvent = &sMeEvent;
                    pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                }
                else  //other return value
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            if(((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)) &&
                    (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                {
                    //Just get SMS status
                    ret =  sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 1, NULL, &nSmsStatus, NULL);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("call SMS_TOOL_DECOMPOSEPDU END, ret=0x%x\n",0x08101071), ret);
                    if(nSmsStatus == CFW_SMS_STORED_STATUS_UNREAD)
                    {
                        *((UINT8 *)pEvent->nParam1) = CFW_SMS_STORED_STATUS_READ;
                        if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                        {
                            //Write it with CFW_SMS_STORED_STATUS_READ
                            ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                        }
                        else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                        {
                            UINT32 nTime = 0x0, nAppInt32 = 0x0, nTmp = 0x0;
                            nTmp = sMeEvent.nParam1;
                            TM_FILETIME nFileTime;
                            TM_GetSystemFileTime(&nFileTime);
                            nTime = nFileTime.DateTime;
                            ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                            CSW_SMS_FREE((UINT8 *)(nTmp));
                        }
                        if(ret != ERR_SUCCESS)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        pEvent = &sMeEvent;
                        pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                        pSmsMOInfo->nReadSmsStatus = CFW_SMS_STORED_STATUS_UNREAD;
                    }
                    else
                    {
                        if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                            CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    pEvent->nParam2 = 0;
                }
                else //read false
                {
                    if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                        CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                    //Read a NULL SIM
                    if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1 == 0))
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        pEvent->nType = 0xf0;
                    }
                    else if(pEvent->nType == 0xf0)
                    {
                        if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)//Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else
                        {
                            pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

            if(((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP)) &&
                    (pSmsMOInfo->nUTI_Internal == pEvent->nTransId) )
            {
                if(pEvent->nType == 0)
                {
                    CFW_SMS_STORAGE_INFO  sStorageInfo;

                    //Zero memory
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    if(sStorageInfo.unReadRecords > 0)
                        sStorageInfo.unReadRecords--;
                    if(sStorageInfo.readRecords < sStorageInfo.totalSlot)
                        sStorageInfo.readRecords++;
                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    pEvent->nParam1 = pSmsMOInfo->nIndex + (pSmsMOInfo->nLocation << 16);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, pEvent->nParam1, 0, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNREAD2READ_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc  error\n",0x08101072));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnRead2ReadProc);
    return ERR_SUCCESS;
}



UINT32 sms_SetRead2UnReadProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           ret = 0x0, nGetUti = 0x0, nState = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL;   //Point to MO private data.
    CFW_EV           sMeEvent;
    UINT8            nSmsStatus = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(sms_SetRead2UnReadProc);
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SetRead2UnReadProc, nState = 0x%x\n",0x08101073), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
            {
                ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
            {
                //Malloc for the read data.
                sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                if(!(UINT8 *)sMeEvent.nParam1)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetRead2UnReadProc   sMeEvent.nParam1  malloc error!!! \n ",0x08101074));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                ret = CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                if(ret == ERR_SUCCESS)
                {
                    //Go to CFW_SM_STATE_WAIT directly!
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    pEvent = &sMeEvent;
                    pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                }
                else  //other return value
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            if(((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)) &&
                    (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                {
                    //Just get SMS status
                    sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 1, NULL, &nSmsStatus, NULL);
                    if(nSmsStatus == CFW_SMS_STORED_STATUS_READ)
                    {
                        *((UINT8 *)pEvent->nParam1) = CFW_SMS_STORED_STATUS_UNREAD;
                        if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                        {
                            //Write it with CFW_SMS_STORED_STATUS_READ
                            ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                        }
                        else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                        {
                            UINT32 nTime = 0x0, nAppInt32 = 0x0, nTmp = 0x0;
                            nTmp = sMeEvent.nParam1;
                            TM_FILETIME nFileTime;
                            TM_GetSystemFileTime(&nFileTime);
                            nTime = nFileTime.DateTime;
                            ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                            CSW_SMS_FREE((UINT8 *)(nTmp));
                        }
                        if(ret != ERR_SUCCESS)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        pEvent = &sMeEvent;
                        pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                        pSmsMOInfo->nReadSmsStatus = CFW_SMS_STORED_STATUS_UNREAD;
                    }
                    else
                    {
                        if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                            CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    pEvent->nParam2 = 0;
                }
                else //read false
                {
                    if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                        CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                    //Read a NULL SIM
                    if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1 == 0))
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        pEvent->nType = 0xf0;
                    }
                    else if(pEvent->nType == 0xf0)
                    {
                        if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)//Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else
                        {
                            pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

            if(((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP)) &&
                    (pSmsMOInfo->nUTI_Internal == pEvent->nTransId) )
            {
                if(pEvent->nType == 0)
                {
                    CFW_SMS_STORAGE_INFO  sStorageInfo;

                    //Zero memory
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    if(sStorageInfo.unReadRecords > 0)
                        sStorageInfo.unReadRecords--;
                    if(sStorageInfo.readRecords < sStorageInfo.totalSlot)
                        sStorageInfo.readRecords++;
                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    pEvent->nParam1 = pSmsMOInfo->nIndex + (pSmsMOInfo->nLocation << 16);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, pEvent->nParam1, 0, nGetUti | (nSimId << 24), pEvent->nType);

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_READ2UNREAD_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetRead2UnReadProc  error\n",0x08101075));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_SetRead2UnReadProc);
    return ERR_SUCCESS;
}



UINT32 sms_SetUnSent2SentProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32           ret = 0x0, nGetUti = 0x0, nState = 0x0; //MO State
    CFW_SMS_MO_INFO *pSmsMOInfo = NULL;   //Point to MO private data.
    CFW_EV           sMeEvent;
    UINT8            nSmsStatus = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(sms_SetUnSent2SentProc);
    CFW_SIM_ID nSimId =  CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //Zero memory
    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SetUnSent2SentProc, nState = 0x%x\n",0x08101076), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
            {
                ret = CFW_SimReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, pSmsMOInfo->nUTI_Internal, nSimId);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
            {
                //Malloc for the read data.
                sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SMS_MO_ONE_PDU_SIZE + 4 + 4));
                if(!(UINT8 *)sMeEvent.nParam1)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetUnSent2SentProc   sMeEvent.nParam1  malloc error!!! \n ",0x08101077));
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SMS_MO_ONE_PDU_SIZE + 4 + 4);
                ret = CFW_MeReadMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, &sMeEvent);
                if(ret == ERR_SUCCESS)
                {
                    //Go to CFW_SM_STATE_WAIT directly!
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                    pEvent = &sMeEvent;
                    pEvent->nTransId = pSmsMOInfo->nUTI_Internal;
                }
                else  //other return value
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        case CFW_SM_STATE_WAIT:
        {
            if(((pEvent->nEventId == EV_CFW_SIM_READ_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)) &&
                    (pEvent->nTransId == pSmsMOInfo->nUTI_Internal))
            {
                if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1))
                {
                    //Just get SMS status
                    sms_tool_DecomposePDU((PVOID)pEvent->nParam1, 1, NULL, &nSmsStatus, NULL);
                    if(nSmsStatus == CFW_SMS_STORED_STATUS_UNSENT)
                    {
                        *((UINT8 *)pEvent->nParam1) = pSmsMOInfo->nStatus;
                        if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_SM)
                        {
                            //Write it with CFW_SMS_STORED_STATUS_SENT
                            ret = CFW_SimWriteMessage(pSmsMOInfo->nLocation, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, pSmsMOInfo->nUTI_Internal, nSimId);
                        }
                        else if(pSmsMOInfo->nLocation == CFW_SMS_STORAGE_ME)
                        {
                            UINT32 nTime = 0x0, nAppInt32 = 0x0, nTmp = 0x0;
                            nTmp = sMeEvent.nParam1;
                            TM_FILETIME nFileTime;
                            TM_GetSystemFileTime(&nFileTime);
                            nTime = nFileTime.DateTime;
                            ret = CFW_MeWriteMessage(nSimId, pSmsMOInfo->nIndex, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sMeEvent);
                            CSW_SMS_FREE((UINT8 *)(nTmp));
                        }
                        if(ret != ERR_SUCCESS)
                        {
                            CFW_GetUTI(hAo, &nGetUti);
                            CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
                            return ERR_CMS_UNKNOWN_ERROR;
                        }
                        pEvent = &sMeEvent;
                        pEvent->nTransId  = pSmsMOInfo->nUTI_Internal;
                        pSmsMOInfo->nReadSmsStatus = CFW_SMS_STORED_STATUS_UNREAD;
                    }
                    else
                    {
                        if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                            CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                        CFW_GetUTI(hAo, &nGetUti);
                        CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_OPERATION_NOT_ALLOWED, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
                        return ERR_CMS_UNKNOWN_ERROR;
                    }
                    pEvent->nParam2 = 0;
                }
                else //read false
                {
                    if(pEvent->nEventId == EV_CFW_SMS_READ_MESSAGE_RSP)
                        CSW_SMS_FREE((UINT8 *)(sMeEvent.nParam1));
                    //Read a NULL SIM
                    if((pEvent->nType == 0) && (*(UINT8 *)pEvent->nParam1 == 0))
                    {
                        pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        pEvent->nType = 0xf0;
                    }
                    else if(pEvent->nType == 0xf0)
                    {
                        if(pEvent->nParam1 == ERR_CME_ME_FAILURE)  //Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_INVALID_INDEX)//Me Return error
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CMS_INVALID_MEMORY_INDEX)
                        {
                            pEvent->nParam1 = ERR_CMS_INVALID_MEMORY_INDEX;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_PIN_REQUIRED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_FAILURE)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else if(pEvent->nParam1 == ERR_CME_SIM_NOT_INSERTED)
                        {
                            pEvent->nParam1 = ERR_CMS_TEMPORARY_FAILURE;
                        }
                        else
                        {
                            pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
                        }
                    }
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, pEvent->nParam1, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), pEvent->nType);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }
            if(((pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP)) &&
                    (pSmsMOInfo->nUTI_Internal == pEvent->nTransId) )
            {
                if(pEvent->nType == 0)
                {
                    CFW_SMS_STORAGE_INFO  sStorageInfo;

                    //Zero memory
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    if(sStorageInfo.unsentRecords > 0)
                        sStorageInfo.unsentRecords--;
                    if(sStorageInfo.sentRecords < sStorageInfo.totalSlot)
                        sStorageInfo.sentRecords++;
                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMOInfo->nLocation, nSimId);
                    pEvent->nParam1 = pSmsMOInfo->nIndex + (pSmsMOInfo->nLocation << 16);
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, pEvent->nParam1, 0, nGetUti | (nSimId << 24), pEvent->nType);

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_UNSENT2SENT_RSP, ERR_CMS_UNKNOWN_ERROR, pSmsMOInfo->nIndex, nGetUti | (nSimId << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }

        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetUnSent2SentProc  error\n",0x08101078));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_SetUnSent2SentProc);
    return ERR_SUCCESS;
}


//
// SMS MT AO process entry.
//
//for SMS MT, there are the following 2 states,define in cfw.h:
//CFW_SM_STATE_IDLE
//CFW_SM_STATE_WAIT

UINT32 sms_MTAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32              nState     = 0x0, ret = 0x0; //MT State
    UINT8               nMode      = 0x0;    //the overflow presentation mode; 0 disable (default), 1 enable
    CFW_IND_EVENT_INFO  sEventInd;
    CFW_SMS_MT_INFO    *pSmsMTInfo = NULL; //Point to MT private data.
    CSW_PROFILE_FUNCTION_ENTER(sms_MTAoProc);

    CFW_SIM_ID nSimId =  CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //Get private date
    pSmsMTInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMTInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_MTAoProc, 1,get private data fail! \n ",0x08101079));
        CSW_PROFILE_FUNCTION_EXIT(sms_MTAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory8(&sEventInd, SIZEOF(CFW_IND_EVENT_INFO));
    //Get current State
    nState = CFW_GetAoState(hAo);

    //SMS MT State machine process
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, nState=0x%x \n ",0x0810107a), nState);
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, pEvent->nEventId =0x%x \n ",0x0810107b), pEvent->nEventId );
            if(pEvent->nEventId == API_SMSPP_RECEIVE_IND)
            {
                CFW_EV sMeEvent;

                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Start sms_MTAoProc SMS_Disp_GetTime : %d Sec., pSmsMTInfo->nSmsStorageID=%d\n",0x0810107c)),
                          (UINT32)(SMS_Disp_GetTime() / 256), pSmsMTInfo->nSmsStorageID);
                if (CFW_SMS_INITED != gNewSMSInd[nSimId])
                {
                    sms_mt_SmsPPErrorReq(0xd3, nSimId);
                    gNewSMSInd[nSimId] = CFW_SMS_WAITING_MSG;
                }
                else if(TRUE == gPendingNewSms[nSimId])
                {
                    sms_mt_SmsPPErrorReq(0xd3, nSimId);
                }
                else
                {
                    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                    //Store received SMS
                    ret = sms_mt_Store_SmsPPReceiveInd(hAo, (api_SmsPPReceiveInd_t *)pEvent->nParam1, &sMeEvent);
                    if( ERR_NO_NEED_SAVE == ret )
                    {
                        return ERR_SUCCESS;
                    }
                    else if(ret != ERR_SUCCESS)
                    {
                        //Unregister
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, 2,call sms_mt_Store_SmsPPReceiveInd = 0x%x \n ",0x0810107d), ret);
                        CFW_UnRegisterAO(CFW_SMS_MT_SRV_ID, hAo);
                        CSW_PROFILE_FUNCTION_EXIT(sms_MTAoProc);
                        return ERR_CFW_INVALID_PARAMETER;
                    }

                    if(pSmsMTInfo->nSmsStorageID == CFW_SMS_STORAGE_ME)
                    {
                        pEvent = &sMeEvent;
                        pEvent->nTransId = SMS_MT_UTI_MIN;
                    }
                    else
                        break;
                }
            }
        case CFW_SM_STATE_WAIT:
        {
            //Get the overflow presentation mode
            CFW_CfgGetSmsOverflowInd(&nMode, nSimId);
            //Wait nEventId after calling CFW_SmsWriteMessage()
            if(((pEvent->nEventId == EV_CFW_SMS_WRITE_MESSAGE_RSP) || (pEvent->nEventId == EV_CFW_SIM_WRITE_MESSAGE_RSP)) &&
                    (pEvent->nTransId == SMS_MT_UTI_MIN))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, pEvent->nType =0x%x \n ",0x0810107e), pEvent->nType );
                if(pEvent->nType == 0) //Write OK
                {
                    CFW_IND_EVENT_INFO *pEventInd = NULL;
                    pSmsMTInfo->nIndex = (UINT16)pEvent->nParam1;
                    //Update SmsStorageInfo
                    if((pSmsMTInfo->nSmsStorageID == CFW_SMS_STORAGE_ME) || (pSmsMTInfo->nSmsStorageID == CFW_SMS_STORAGE_SM))
                    {
                        CFW_SMS_STORAGE_INFO  sStorageInfo;

                        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMTInfo->nSmsStorageID, nSimId);
                        if(sStorageInfo.usedSlot < sStorageInfo.totalSlot)
                            sStorageInfo.usedSlot++;
                        if(sStorageInfo.unReadRecords < sStorageInfo.totalSlot)
                            sStorageInfo.unReadRecords++;
                        CFW_CfgSetSmsStorageInfo(&sStorageInfo, pSmsMTInfo->nSmsStorageID, nSimId);


                        //when SIM has no enough memory to save a cohcat SMS, we save the whole concat SMS to ME,
                        //then switch the default memory to SIM.
                        if((pSmsMTInfo->nSmsStorageID == CFW_SMS_STORAGE_ME) &&
                                pSmsMTInfo->nLongerMsgCount &&
                                (pSmsMTInfo->nLongerMsgCount == pSmsMTInfo->nLongerMsgCurrent))
                        {
                            CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);
                            if(sStorageInfo.usedSlot <= sStorageInfo.totalSlot)
                            {
                                UINT8 uOption = 0;
                                UINT8 uNewSmsStorage = 0;
                                CFW_CfgGetNewSmsOption(&uOption, &uNewSmsStorage, nSimId);
                                CFW_CfgSetNewSmsOption(uOption, CFW_SMS_STORAGE_SM, nSimId);
                            }

                        }
                    }
                    //when a new short message arrives, after saving,setting the following
                    pEventInd = &sEventInd;
                    CFW_CfgGetIndicatorEventInfo(pEventInd, nSimId);
                    sEventInd.sms_receive = 1;
                    CFW_CfgSetIndicatorEventInfo(&sEventInd, nSimId);
                    //Parse received SMS
                    if(pSmsMTInfo->isRoutDetail)
                        sms_mt_Parse_SmsPPReceiveInd(hAo);
                    //if first longer msg, get its index after saving
                    if((pSmsMTInfo->isLongerMsg) && (pSmsMTInfo->nLongerMsgCurrent == 1) && gpLongerMsgParaIndex)
                        ((CFW_SMS_MT_LONGER_MSG_INDEX *)gpLongerMsgParaIndex)->gLongerMsgFirstIndex = pSmsMTInfo->nIndex;
                    //if a longer msg, 更新pMEFilename /pSMFilename
                    if(pSmsMTInfo->isLongerMsg)
                    {
                        if(USING_FS_SAVE_LONG_PARA)
                            sms_mt_SetLongPara(hAo);
                    }
                    //a normal sms, or a long sms and user reassembles
                    if((!pSmsMTInfo->isLongerMsg)                                ||
                            (pSmsMTInfo->isLongerMsg && !pSmsMTInfo->isListLongFirst) ||
                            (pSmsMTInfo->isLongerMsg && pSmsMTInfo->isListLongFirst && (pSmsMTInfo->nLongerMsgCurrent == pSmsMTInfo->nLongerMsgCount)))
                    {
                        if((pSmsMTInfo->isLongerMsg) && (pSmsMTInfo->nLongerMsgCurrent == pSmsMTInfo->nLongerMsgCount) && gpLongerMsgParaIndex)
                        {
                            if(pSmsMTInfo->isListLongFirst)
                            {
                                //if last longer msg, CFW reassembles it, then post their first index
                                pSmsMTInfo->nIndex = ((CFW_SMS_MT_LONGER_MSG_INDEX *)gpLongerMsgParaIndex)->gLongerMsgFirstIndex;
                                CSW_SMS_FREE(gpLongerMsgParaIndex);
                                if(pSmsMTInfo->nIndex == 0)
                                {
                                    //
                                }
                            }
                        }
                        if((pSmsMTInfo->isLongerMsg)  && pSmsMTInfo->isListLongFirst && (pSmsMTInfo->nLongerMsgCurrent == pSmsMTInfo->nLongerMsgCount) && !gpLongerMsgPara)
                        {
                            //???
                            //如果系统只收到长短信的一部分,然后关机,再次开机后,接收到的长短信的其他部分,此时不知道其第一条的index
                            //当然可以用遍沥方法得到,目前还没有这样实现
                            //pSmsMTInfo->nIndex = 0;
                        }
                        if((pSmsMTInfo->nSmsType == 1) && (!(pSmsMTInfo->isRoutDetail)))// 1: DELIVER SMS : Storage-Index
                        {
                            //Send a MSG to AT , Type = 1
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_MTAoProc, 4,param1=0x%x,param2=0x%x\n ",0x0810107f),
                                      pSmsMTInfo->nSmsStorageID, pSmsMTInfo->nIndex);
                            CFW_PostNotifyEvent(EV_CFW_NEW_SMS_IND, (UINT32)pSmsMTInfo->nSmsStorageID, pSmsMTInfo->nIndex, SMS_MT_UTI | (nSimId << 24), 1);
                        }
                    }

                    if((pSmsMTInfo->nSmsType == 0) && (!(pSmsMTInfo->isRoutDetail))) // 3:STATUS REPORT : Storage-Index
                    {
                        //Send a MSG to AT , Type = 3
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_MTAoProc, 5, param1=0x%x,param2=0x%x\n ",0x08101080),
                                  pSmsMTInfo->nSmsStorageID, pSmsMTInfo->nIndex);
                        CFW_PostNotifyEvent(EV_CFW_NEW_SMS_IND, (UINT32)pSmsMTInfo->nSmsStorageID, pSmsMTInfo->nIndex, SMS_MT_UTI | (nSimId << 24), 3);
                    }
                    //Save to specified memory location successfully and the memory has been full.
                    if((nMode == 1) && (pEvent->nParam2 == 1))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, 6, param1=1,param2=0x%x\n ",0x08101081),
                                  pSmsMTInfo->nSmsStorageID);
                        CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND, 1, pSmsMTInfo->nSmsStorageID, SMS_MT_UTI | (nSimId << 24), 0);
                    }
                    //Post Ack Event to SCL
                    // if(pSmsMTInfo->nSmsType) //if receive a SMS
                    //Both SMS and SR need SmsPPAckReq???
#if 0
                    sms_mt_SmsPPAckReq(nSimId);
#endif
                }
                else if(pEvent->nType == 0xf0) //Write error, We think the Storage has been FUll
                {
                    UINT8 nNewCount = 0x0;
                    if(nMode == 1)
                    {
                        //Post EV_CFW_SMS_INFO_IND Event to AT
                        //Buffer full and new message waiting in Memory Storage(SC, ME) for delivery to phone.
                        CFW_SMS_STORAGE_INFO sStorageInfo;
                        CFW_CfgGetSmsStorageInfo(&sStorageInfo, pSmsMTInfo->nSmsStorageID, nSimId);
                        //Be sure whether the storage is full or not
                        if(sStorageInfo.usedSlot == sStorageInfo.totalSlot)
                        {
                            UINT32 uParam2 = 0;
                            uParam2 = pSmsMTInfo->nSmsStorageID | (pSmsMTInfo->nLongerMsgCount << 8);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_MTAoProc, 5, param1=2,param2=0x%x\n ",0x08101082),
                                      uParam2);
                            CFW_PostNotifyEvent(EV_CFW_SMS_INFO_IND, 2, uParam2, SMS_MT_UTI | (nSimId << 24), 0);
                        }
                    }
                    ret = CFW_CfgGetSmsFullForNew(&nNewCount, nSimId);
                    nNewCount++;;
                    ret = CFW_CfgSetSmsFullForNew(nNewCount, nSimId);
                    //Post Error Event to SCL
                    sms_mt_SmsPPErrorReq(0xd3, nSimId); //0xd3: Memory capacity exceeded
                }
            }
            else if(( EV_CFW_SAT_RESPONSE_RSP == pEvent->nEventId ))
            {
                if( 0x90 == pEvent->nParam2 )
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0x90 == pEvent->nParam1\n ",0x08101083));
//                    sms_mt_SmsPPAckReq(nSimId);
                }
                else if( 0x91 == pEvent->nParam2)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0x91 == pEvent->nParam1\n ",0x08101084));
                    return ERR_SUCCESS;
                }
                else
                {
                    sms_mt_SmsPPErrorReq(0xD5, nSimId);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEvent->nParam1 %x\n ",0x08101085), pEvent->nParam1);
                }
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else if(( EV_CFW_SAT_CMDTYPE_IND == pEvent->nEventId ))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("EV_CFW_SAT_CMDTYPE_IND nType %x  pEvent->nParam1 %d\n",0x08101086), pEvent->nType,pEvent->nParam1);
                if(CFW_SAT_IND_PDU == CFW_GetSATIndFormate())
                {
                    UINT8 *pStr = CSW_SMS_MALLOC(pEvent->nParam2);
                    SUL_ZeroMemory8(pStr, pEvent->nParam2);
                    SUL_MemCopy8(pStr, (CONST VOID *)pEvent->nParam1, pEvent->nParam2);
#ifdef AT_MMI_SUPPORT
                    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, sxr_Link((UINT32)pStr), pEvent->nParam2, 1 | (nSimId << 24), pEvent->nType);
#else
                    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, (UINT32)pStr, pEvent->nParam2, 1 | (nSimId << 24), pEvent->nType);
#endif
                }
                else
                {
#ifdef AT_MMI_SUPPORT
                    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pEvent->nParam1, 0, 1 | (nSimId << 24), pEvent->nType);
#else
                    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pEvent->nParam1, 0, 1 | (nSimId << 24), pEvent->nType);
#endif
                }
//                sms_mt_SmsPPAckReq(nSimId);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            CSW_SMS_FREE(pSmsMTInfo->pInMsg);
            CFW_UnRegisterAO(CFW_SMS_MT_SRV_ID, hAo);
            break;
        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_MTAoProc  error\n",0x08101087));
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_MTAoProc);
    return ERR_SUCCESS;
}

#ifdef CB_SUPPORT
UINT32 sms_CBAoProc(HAO hAo, CFW_EV *pEvent)
{
    CFW_SMS_CB_INFO    *pSmsCBInfo = NULL; //Point to CB private data.
    UINT32              ret = 0x0;  //CB State
    UINT8               nFormat = 0x0, nSmsStorageId = 0x0, nOption = 0x0, nMode = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(sms_CBAoProc);

    CFW_SIM_ID nSimId =  CFW_SIM_END;
    CFW_GetSimID(hAo, &nSimId);


    //Get private date
    pSmsCBInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsCBInfo)
    {
        CSW_PROFILE_FUNCTION_EXIT(sms_CBAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    CFW_CfgGetSmsCB(&nMode, NULL, NULL);
    //nMode ==0 :Accepts CB
    if(nMode == 0)
    {
        //Get SMS StorageID

        ret = CFW_CfgGetNewSmsOption(&nOption, &nSmsStorageId,nSimId);

        if (ret == ERR_SUCCESS)
        {
            pSmsCBInfo->nCBStorageID = (UINT8)(nSmsStorageId & 0x4);
            pSmsCBInfo->isRoutDetail = (UINT8)(nOption & CFW_SMS_ROUT_DETAIL_INFO);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetNewSmsOption error ret=0x%x\n",0x08101088), ret);
            CSW_PROFILE_FUNCTION_EXIT(sms_CBAoProc);
            return ERR_CFW_INVALID_PARAMETER;
        }

        if(pSmsCBInfo->nCBStorageID == 0x4)
        {
            UINT32 nTime = 0x0, nAppInt32 = 0x0;
            CFW_EV sEvent;

            SUL_ZeroMemory8(&sEvent, SIZEOF(CFW_EV));


            ret = CFW_MeWriteMessage(nSimId, 0, (UINT8 *)pEvent->nParam1, SMS_MO_ONE_PDU_SIZE, nTime, nAppInt32, &sEvent);

            if(ret == ERR_SUCCESS)
            {
            }
            else if(ret == ERR_CFW_INVALID_PARAMETER )
            {
                CSW_PROFILE_FUNCTION_EXIT(sms_CBAoProc);
                return ERR_CFW_INVALID_PARAMETER;
            }
        }

        //Parse received CB
        if(pSmsCBInfo->isRoutDetail)
        {
            UINT32  nStructureSize = 0x0;
            CFW_NEW_SMS_NODE    *pSmsNode    = NULL;
            //Get SMS format from user setting


            CFW_CfgGetSmsFormat(&nFormat,nSimId);

            if(pEvent->nEventId == API_SMSCB_RECEIVE_IND)
            {
                api_SmsCBReceiveInd_t *pInMsg = NULL;
                pInMsg = (api_SmsCBReceiveInd_t *)pEvent->nParam1;
                if(nFormat) //Text
                {
                    //todo
                }
                else  //Pdu
                {
                    CFW_SMS_PDU_INFO    *pSmsPDU     = NULL;  //PDU Format

                    nStructureSize = (SIZEOF(CFW_NEW_SMS_NODE) + SIZEOF(CFW_SMS_PDU_INFO) + pInMsg->DataLen);
                    pSmsNode = (CFW_NEW_SMS_NODE *)CSW_SMS_MALLOC(nStructureSize);
                    if(!pSmsNode)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_CBAoProc  pSmsNode  malloc error!!! \n ",0x08101089));
                        CSW_PROFILE_FUNCTION_EXIT(sms_CBAoProc);
                        return ERR_NO_MORE_MEMORY;
                    }
                    SUL_ZeroMemory8(pSmsNode, nStructureSize);
                    pSmsNode->pNode = (CFW_SMS_PDU_INFO *)((UINT8 *)pSmsNode + SIZEOF(CFW_NEW_SMS_NODE));
                    //Set pSmsPDU Value
                    pSmsPDU = (CFW_SMS_PDU_INFO *)(pSmsNode->pNode);
                    pSmsPDU->nDataSize = pInMsg->DataLen  ;
                    pSmsPDU->nStatus  = 0;
                    SUL_MemCopy8(pSmsPDU->pData, pInMsg->Data, (UINT32)pInMsg->DataLen);
                    pSmsNode->nConcatPrevIndex = 0xffff;
                    pSmsNode->nConcatCurrentIndex = pSmsCBInfo->nIndex;
                    pSmsNode->nStorage = pSmsCBInfo->nCBStorageID;
                    pSmsNode->nType = 0x30;

                    CFW_PostNotifyEvent(EV_CFW_NEW_SMS_IND, (UINT32)pSmsNode, 0, SMS_MT_UTI|(nSimId << 24), 0);

                }
            }
            else if(pEvent->nEventId == API_SMSCB_RECEIVE_IND)
            {
                //api_SmsCBPageInd_t
            }
        }
        else
        {
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_CBAoProc);
    return ERR_SUCCESS;
}
#endif

static BOOL bMEFinish = FALSE;
//Task
// 1.Initialize ME DB, Create a new one if NULL                : sms_db_Init()
// 2.Get Storage information of SIM, including longer sms info : CFW_SIMSmsInit()
UINT32 sms_InitAoProc(HAO hAo, CFW_EV *pEvent)
{
    CFW_SMS_INIT_HANDLE   *pSmsMOInfo = NULL;
    UINT32                 nGetUti = 0x0, nState = 0x0, ret = 0x0;
    CFW_SIM_ID nSimId =  CFW_SIM_END;
    CFW_GetSimID(hAo, &nSimId);
    CSW_PROFILE_FUNCTION_ENTER(sms_InitAoProc);
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    nState = CFW_GetAoState(hAo);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sms_InitAoProc, init start \n",0x0810108a)));
    //Get private date
    pSmsMOInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsMOInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_InitAoProc, nState=0x%x \n ",0x0810108b), nState);
    //SMS MO State machine process
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {

            CFW_SMS_STORAGE_INFO *pMEStorageInfo = NULL;
            CFW_SMS_STORAGE_INFO  sStorageInfo;
            CFW_EV                sMeEvent;
            UINT32                nTmpEventP1 = 0x0;
            UINT8                 nIsFail = 0x0;

            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_InitAoProc, bMEFinish=%d \n",0x0810108c)), bMEFinish);
            if( TRUE == bMEFinish )
            {
                ret = CFW_SimSmsInit(nSimId);
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_InitAoProc, don't need to init ME 1 ret=0x%x\n",0x0810108d)), ret);
                if(ret == ERR_SUCCESS)
                {
                    CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
                break;
            }
            //Zero memory
            SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
            SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));


            //Get ME sms info, then Set them
            //??? maybe just do it one time, think about it later
            sMeEvent.nParam1 = (UINT32)((UINT8 *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_STORAGE_INFO)));
            if((UINT8 *)sMeEvent.nParam1 == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_InitAoProc  sMeEvent.nParam1  malloc error!!! \n ",0x0810108e));
                CFW_GetUTI(hAo, &nGetUti);
                CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                return ERR_CMS_UNKNOWN_ERROR;
            }

            for(nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
            {

                SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SIZEOF(CFW_SMS_STORAGE_INFO));
                pMEStorageInfo = (CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1);
                //Get ME totalSlot and usedSlot
                CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_STORED_ALL, &sMeEvent);
                SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                if(sMeEvent.nType == 0)
                {
                    sStorageInfo.storageId = CFW_SMS_STORAGE_ME;
                    sStorageInfo.totalSlot = pMEStorageInfo->totalSlot;
                    sStorageInfo.usedSlot  = pMEStorageInfo->usedSlot;
                    //Get ME unReadRecords
                    nTmpEventP1 = sMeEvent.nParam1;
                    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                    SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));
                    sMeEvent.nParam1 = nTmpEventP1;
                    CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNREAD, &sMeEvent);
                    if(sMeEvent.nType == 0)
                    {
                        sStorageInfo.unReadRecords = pMEStorageInfo->usedSlot;
                        //Get ME readRecords
                        nTmpEventP1 = sMeEvent.nParam1;
                        SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                        SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));
                        sMeEvent.nParam1 = nTmpEventP1;
                        CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_READ, &sMeEvent);
                        if(sMeEvent.nType == 0)
                        {
                            UINT8 nSentStatus = 0x0;

                            sStorageInfo.readRecords = pMEStorageInfo->usedSlot;
                            //Get ME sentRecords
                            nTmpEventP1 = sMeEvent.nParam1;
                            SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                            SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));
                            sMeEvent.nParam1 = nTmpEventP1;
                            nSentStatus = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
                                          CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE | CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
                            CFW_MeGetStorageInfo(nSimId, nSentStatus, &sMeEvent);
                            if(sMeEvent.nType == 0)
                            {
                                sStorageInfo.sentRecords = pMEStorageInfo->usedSlot;
                                //Get ME unsentRecords
                                nTmpEventP1 = sMeEvent.nParam1;
                                SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                                SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));
                                sMeEvent.nParam1 = nTmpEventP1;
                                CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNSENT, &sMeEvent);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MeGetSI 0\n",0x0810108f)));

                                if(sMeEvent.nType == 0)
                                {
                                    sStorageInfo.unsentRecords = pMEStorageInfo->usedSlot;
                                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSimId);

                                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MeGetSI 1 \n",0x08101090)));

                                    //Add by lixp at 20090213 for free crash issue
                                    if( nSimId == ( CFW_SIM_COUNT - 1 ))
                                    {
                                        CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                                    }
                                }//unsentRecords
                                else
                                {
                                    nIsFail = 1;
                                }
                            }//sentRecords
                            else
                            {
                                nIsFail = 1;
                            }
                        }//readRecords
                        else
                        {
                            nIsFail = 1;
                        }
                    }//unReadRecords
                    else
                    {
                        nIsFail = 1;
                    }
                }//totalSlot and usedSlot
                else
                {
                    nIsFail = 1;
                }

                if(nIsFail)
                    break;
            }

            if(nIsFail)
            {
                CSW_SMS_FREE(pMEStorageInfo);
                CFW_GetUTI(hAo, &nGetUti);
                CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                return ERR_CMS_UNKNOWN_ERROR;
            }
            //Init SIM long sms
            bMEFinish = TRUE;
            CFW_GetSimID(hAo, &nSimId);
            ret = CFW_SimSmsInit(nSimId);
            if(ret == ERR_SUCCESS)
            {
                CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
            }
            else
            {
                CFW_GetUTI(hAo, &nGetUti);
                CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                return ERR_CMS_UNKNOWN_ERROR;
            }
            break;
        }//case CFW_SM_STATE_IDLE:
        case CFW_SM_STATE_WAIT:
        {
            if(pEvent->nEventId == EV_CFW_SIM_INIT_SMS_RSP)
            {
                if(pEvent->nType == 0)
                {
                    CFW_SMS_INIT_INFO     *pSIMInitInfo = NULL;
                    CFW_SMS_STORAGE_INFO   sStorageInfo;
                    nSimId = pEvent->nFlag;
                    //Zero memory
                    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
                    pSIMInitInfo = (CFW_SMS_INIT_INFO *)(pEvent->nParam1);
                    //Get SIM sms Storage info, then Set them
                    sStorageInfo.storageId      = CFW_SMS_STORAGE_SM;
                    sStorageInfo.totalSlot      = pSIMInitInfo->totalSlot;
                    sStorageInfo.usedSlot       = pSIMInitInfo->usedSlot;
                    sStorageInfo.readRecords    = pSIMInitInfo->readRecords;
                    sStorageInfo.unReadRecords  = pSIMInitInfo->unReadRecords;
                    sStorageInfo.sentRecords    = pSIMInitInfo->sentRecords;
                    sStorageInfo.unsentRecords  = pSIMInitInfo->unsentRecords;
                    sStorageInfo.unknownRecords = pSIMInitInfo->unknownRecords;
                    CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSimId);

#if 0
                    UINT32 nRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, (nSimId == CFW_SIM_0 ? CFW_SIM_1 : CFW_SIM_0));

                    if(!sStorageInfo.totalSlot)//the other card not been init
                    {
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_MULTIPLE | CFW_AO_PROC_CODE_CONSUMED);
                        return ERR_SUCCESS;
                    }
#endif
                    //SMS INIT OK
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 0, nGetUti | (nSimId << 24), SMS_INIT_EV_OK_TYPE);
                    if(pEvent->nParam1)
                    {
                        //CSW_SMS_FREE(pSIMInitInfo->pLongInfo);
                        //pSIMInitInfo->pLongInfo = (UINT32)NULL;
                        CSW_SMS_FREE(pEvent->nParam1);
                        pEvent->nParam1 = (UINT32)NULL;

                    }

                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SRV_STATUS_IND, 0, 1, nGetUti | (nSimId << 24), SMS_INIT_EV_ERR_TYPE);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
                    return ERR_CMS_UNKNOWN_ERROR;
                }

            }//if(pEvent->nEventId == EV_CFW_SIM_INIT_SMS_RSP)
            break;
        }// case CFW_SM_STATE_WAIT:
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_MTAoProc  error\n",0x08101091));
            break;
    }//switch(nState)
    CSW_PROFILE_FUNCTION_EXIT(sms_InitAoProc);
    return ERR_SUCCESS;
}


UINT32 CFW_SmsInitCompleteProc(HAO hAo, CFW_EV *pEvent)
{
    CFW_SMS_INIT *pSmsInit = NULL;
    UINT32 nRet = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SmsInitCompleteProc \n",0x08101092)));
    CSW_PROFILE_FUNCTION_ENTER(CFW_SmsInitCompleteProc);

    if( (hAo == 0) || (pEvent == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SmsInitCompleteProc  ao[0x%x] pEvent[0x%x] ERROR\n",0x08101093)), hAo, pEvent);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SmsInitCompleteProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hAo);

    pSmsInit = CFW_GetAoUserData(hAo);

    switch (pSmsInit->n_CurrStatus)
    {
        case CFW_SM_STATE_IDLE:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SmsInitCompleteProc CFW_SM_STATE_IDLE gNewSMSInd[0x%x]= 0x%x\n",0x08101094)), nSimID, gNewSMSInd[nSimID]);
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if( CFW_SMS_NOINITED == gNewSMSInd[nSimID] )
                {
                    gNewSMSInd[nSimID] = CFW_SMS_INITED;
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    return ERR_SUCCESS;

                }
                else if( CFW_SMS_INITED == gNewSMSInd[nSimID] )
                {
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                else if( CFW_SMS_WAITING_MSG == gNewSMSInd[nSimID] )
                {
                    gNewSMSInd[nSimID] = CFW_SMS_INITED;
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Invalid status gNewSMSInd %x\n",0x08101095)), gNewSMSInd[nSimID]);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }


                if (ERR_SUCCESS == ( nRet = sms_mo_SmsPPSendMMAReq(nSimID) ))
                {
                    pSmsInit->n_PrevStatus = pSmsInit->n_CurrStatus;
                    pSmsInit->n_CurrStatus = CFW_SM_STATE_WAIT;
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_mo_SmsPPSendMMAReq Error 0x%x\n",0x08101096)), nRet);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }
        }
        break;

        case CFW_SM_STATE_WAIT:
        {
            if (pEvent->nEventId == API_SMSPP_ACK_IND)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Receive API_SMSPP_ACK_IND\n",0x08101097)));
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
            }
            else if (pEvent->nEventId == API_SMSPP_ERROR_IND)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Receive API_SMSPP_ERROR_IND and We will retry\n",0x08101098)));
                if (ERR_SUCCESS != ( nRet = sms_mo_SmsPPSendMMAReq(nSimID) ))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("sms_mo_SmsPPSendMMAReq Error 0x%x\n",0x08101099)), nRet);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                }
            }
            else
            {
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SmsInitCompleteProc ERROR!!! \n",0x0810109a)));
            }
        }
        break;
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SmsInitCompleteProc state ERROR 0x%x \n",0x0810109b)), pSmsInit->n_CurrStatus);
            break;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SmsInitCompleteProc end \n",0x0810109c)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SmsInitCompleteProc);

    return ERR_SUCCESS;
}

#ifdef CFW_CUSTOM_IMEI_COUNT

PRIVATE UINT8 gCustomSendIMEINum[42] = {0x00,};
PRIVATE UINT8 gCustomSendIMEINumLen = 0x00;


PRIVATE UINT8 gCustomSendIMEINumFlag[CFW_SIM_COUNT] = {0x00,};



UINT32 CFW_CfgSetCustomSendIMEI(UINT8 nFlag , CFW_SIM_ID nSimID)
{
    return ERR_SUCCESS;
}
UINT32 CFW_CfgGetCustomSendIMEI(UINT8 *pFlag , CFW_SIM_ID nSimID)
{
    *pFlag = 0;
    return ERR_SUCCESS;
}


UINT32 CFW_RegisterCustomSendIMEI( UINT8 *pNum, UINT8 nNumLen )
{
    SUL_MemCopy8(gCustomSendIMEINum, pNum, gCustomSendIMEINumLen = nNumLen);
}

VOID CFW_CustomSendIMEI(CFW_SIM_ID nSimID)
{
    //Example: UINT8 pNumber[]   = "13811189836";//{0x68, 0x31, 0x76, 0x21, 0x42, 0x81, 0xF1 };
    //UINT8 nNumberSize = 11;

    CFW_DIALNUMBER      sNumber;
    UINT8 pBcd[21]    = {0x00,};
    UINT8 nIMEI[30]   = {0x00,};
    UINT8 nIMEILen    = 0x00;
    UINT8 nFormat     = 0x00;
    UINT8 nFlag       = 0x00;


    CFW_CfgGetCustomSendIMEI(&nFlag , nSimID);


    if( 0x01 == nFlag )
    {
        return;
    }

    if( 0x00 == gCustomSendIMEINumLen )
    {
        return ;
    }


    if( 0x00 == gCustomSendIMEINumFlag[nSimID] )
    {
        gCustomSendIMEINumFlag[nSimID] = 1;
    }
    else
    {
        return;
    }

    sNumber.nDialNumberSize = SUL_AsciiToGsmBcd(gCustomSendIMEINum, gCustomSendIMEINumLen, pBcd);

    sNumber.pDialNumber     = pBcd;
    SUL_MemCopy8(nIMEI, "IMEI:", 5);
    //Send MSG

    CFW_EmodGetIMEI(nIMEI + 5, &nIMEILen, nSimID);

    CFW_CfgGetSmsFormat(&nFormat , nSimID);

    CFW_CfgSetSmsFormat(1, nSimID); //0 PDU mode; 1 text mode
    if( CFW_SmsSendMessage(&sNumber, nIMEI, 30, 0x10, nSimID) != 0 )  // Text Mode with GSM 7bit is OK.
    {
        CFW_CfgSetSmsFormat( nFormat , nSimID );
    }

}
VOID CFW_CustomSendIMEIProc(UINT8 nCSFlag, CFW_SIM_ID nSimID )
{
    UINT8 nFlag       = 0x00;

    CFW_CfgGetCustomSendIMEI(&nFlag  , nSimID );

    if(( 0x00 == nFlag ) && (TRUE == nCSFlag))
    {
        CFW_CfgSetCustomSendIMEI(1  , nSimID );
    }
    else
    {
        return;
    }

    if( 0x01 == gCustomSendIMEINumFlag[nSimID] )
    {
        gCustomSendIMEINumFlag[nSimID] = 0;
    }
    else
    {
        return;
    }

}

#endif
// Made by LXP
#endif // ENABLE 

VOID NewSMS_PushCommandEX(CFW_DIALNUMBER *pNumber, UINT8 nData[], UINT16 nDatalen, CFW_SIM_ID nSimId)
{

    UINT8 nConcat   = 0x08;
    UINT32 nRet     = ERR_SUCCESS;
    CFW_EV  sComposeEvent;
    CFW_EV *pEvent = NULL;

    SUL_ZeroMemory8(&sComposeEvent, SIZEOF(CFW_EV));


    pNumber->nClir = 0x35; // magic number for time


    nRet = sms_tool_ComposePDU(nConcat, CFW_SMS_STORED_STATUS_UNREAD, nData,

                               nDatalen, pNumber, nSimId, &sComposeEvent);


    pEvent = &sComposeEvent;

    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("NewSMS_TestCommandEX pEvent->nType = 0x%x\n",0x0810109d)), pEvent->nType);

    if(nRet == ERR_SUCCESS )
    {
        if(pEvent->nType == 0x0) //Send normal Text sms
        {
            api_SmsPPReceiveInd_t *pDelieveSmsInd = NULL;

            UINT16 nStruSize = 0x0;
            nStruSize = (UINT16)(SIZEOF(api_SmsPPReceiveInd_t) + 176);

            //Systen will Free pSendSmsReq

            pDelieveSmsInd = (api_SmsPPReceiveInd_t *)CFW_MemAllocMessageStructure(nStruSize);//api_SmsPPSendReq_t

            if(!pDelieveSmsInd)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc  pSendSmsReq malloc error!!! \n ",0x0810109e));
                return ;
            }

            SUL_ZeroMemory8(pDelieveSmsInd, nStruSize);

            pDelieveSmsInd->AddressLen = *((UINT8 *)pEvent->nParam1 + 1);
            SUL_MemCopy8(pDelieveSmsInd->Address, (UINT8 *)pEvent->nParam1 + 2, pDelieveSmsInd->AddressLen);

            pDelieveSmsInd->DataLen = 176;

            SUL_MemCopy8((UINT8 *)pDelieveSmsInd->Data, (UINT8 *)pEvent->nParam1 + 2 + pDelieveSmsInd->AddressLen, pDelieveSmsInd->DataLen);
            CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pDelieveSmsInd, nStruSize, 16);

            CFW_BalSendMessage(CFW_MBX, API_SMSPP_RECEIVE_IND, pDelieveSmsInd, nStruSize, CFW_SIM_0);

        }
        else if(pEvent->nType == 0x01)
        {
            // Long SMS
            UINT8 *pTmpData = NULL;
            UINT8 *pTmpPara = NULL;
            UINT8 *pTmpData_Addr = NULL;
            UINT8 *pTmpPara_Addr = NULL;

            UINT8 isLongerMsg = 0x00;
            UINT8 nLongerMsgCurrent = 0x00;
            UINT8 nLongerMsgCount = 0x00;
            UINT8 nMR = 0x00;
            UINT8 nLongerEachPartSize = 0x00;
            UINT8 i = 0x0;

            pTmpData      = (UINT8 *)(sComposeEvent.nParam1);
            pTmpPara      = (UINT8 *)(sComposeEvent.nParam2);
            pTmpData_Addr = (UINT8 *)(sComposeEvent.nParam1);
            pTmpPara_Addr = (UINT8 *)(sComposeEvent.nParam2);
            //Set value
            isLongerMsg         = 1;  // a longer msg
            nLongerMsgCurrent   = *(pTmpPara + 3);  //part A
            nLongerMsgCount     = *(pTmpPara + 2);  //part B
            nMR                 = *(pTmpPara + 1);  //part C
            nLongerEachPartSize = *(pTmpPara);      //part D
            for(i = 0; i < nLongerMsgCount; i++)
            {
                api_SmsPPReceiveInd_t *pDelieveSmsInd = NULL;

                UINT16 nStruSize = 0x0;
//                nStruSize = (UINT16)(SIZEOF(api_SmsPPReceiveInd_t) + 176);

                //Systen will Free pSendSmsReq

                pDelieveSmsInd = (api_SmsPPReceiveInd_t *)CFW_MemAllocMessageStructure(nStruSize);

                if(!pDelieveSmsInd)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SendProc  pSendSmsReq malloc error!!! \n ",0x0810109f));
                    return ;
                }

                SUL_ZeroMemory8(pDelieveSmsInd, nStruSize);

                pDelieveSmsInd->AddressLen = *((UINT8 *)pTmpData + 1);
                SUL_MemCopy8(pDelieveSmsInd->Address, (UINT8 *)pTmpData + 2, pDelieveSmsInd->AddressLen);

                pDelieveSmsInd->DataLen = 176;

                SUL_MemCopy8((UINT8 *)pDelieveSmsInd->Data, (UINT8 *)pTmpData + 2 + pDelieveSmsInd->AddressLen, pDelieveSmsInd->DataLen);
                CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pDelieveSmsInd, nStruSize, 16);

                CFW_BalSendMessage(CFW_MBX, API_SMSPP_RECEIVE_IND, pDelieveSmsInd, nStruSize, CFW_SIM_0);

                if(nLongerMsgCurrent < nLongerMsgCount)
                {

                    pTmpPara += 4;
                    pTmpData += nLongerEachPartSize;

                    nLongerMsgCurrent   = *(pTmpPara + 3);  //part A
                    nLongerEachPartSize = *(pTmpPara );     //part D
                }

            }
        }

    }

}




