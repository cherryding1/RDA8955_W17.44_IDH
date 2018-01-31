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


#include "api_msg.h"
#include <cfw.h>
#include <cfw_prv.h>
#include <sul.h>
#include "csw_debug.h"

#if (CFW_SS_DUMP_ENABLE==0) && (CFW_SS_SRV_ENABLE==1)

#include "cfw_ss_data.h"
#include "cfw_ss_tools.h"
#define USSD_ENDABLE
#define SS_NO_RETURNRESULT_HANDLE

#define CHECKUTI(nUTI) if(CFW_IsFreeUTI(nUTI, CFW_SS_SRV_ID) == ERR_CFW_UTI_IS_BUSY) \
{ \
    return ERR_CFW_UTI_IS_BUSY; \
}

PRIVATE T_COMM_sSsTeleBSCodeList const sSsTeleBSCodeLists[] =
{
    // Teleservices
    {COMM_SS_ALL_TELESERVICES, LOCAL_SS_ALL_TELESERVICES},
    {COMM_SS_TELEPHONY, LOCAL_SS_ALL_SPEECH_TELESERVICES},
    {COMM_SS_TELEPHONY, LOCAL_SS_TELEPHONY},
    {COMM_SS_TELEPHONY, LOCAL_SS_EMERGENCY_CALLS},
    {COMM_SS_ALL_DATA_TELESERVICES, LOCAL_SS_ALL_DATA_TELESERVICES},
    {COMM_SS_ALL_FAX_SERVICES, LOCAL_SS_ALL_FAX_SERVICES},
    {COMM_SS_ALL_FAX_SERVICES, LOCAL_SS_FAX_GROUP3_ALTER_SPEECH},
    {COMM_SS_ALL_FAX_SERVICES, LOCAL_SS_AUTO_FAX_GROUP3},
    {COMM_SS_ALL_FAX_SERVICES, LOCAL_SS_FAX_GROUP4},
    {COMM_SS_ALL_SMS, LOCAL_SS_ALL_SMS},
    {COMM_SS_ALL_SMS, LOCAL_SS_SM_MT_PP},
    {COMM_SS_ALL_SMS, LOCAL_SS_SM_MO_PP},

    // { COMM_SS_ALL_TELESERVICES_XCPT_SMS, LOCAL_SS_ALL_TELESERVICES_XCPT_SMS},
    // { COMM_SS_VOICE_GROUP_CALL,          LOCAL_SS_ALL_VOICE_GRP_CALL_SERVICES  },
    // { COMM_SS_VOICE_GROUP_CALL,          LOCAL_SS_VOICE_GROUP_CALL         },
    // { COMM_SS_VOICE_BROADCAST_CALL,      LOCAL_SS_VOICE_BROADCAST_CALL     },
    // { COMM_SS_allPLMN_specificTS,        LOCAL_SS_allPLMN_specificTS       },
    // { COMM_SS_plmn_specificTS_1,         LOCAL_SS_plmn_specificTS_1},
    // { COMM_SS_plmn_specificTS_2,         LOCAL_SS_plmn_specificTS_2},
    // { COMM_SS_plmn_specificTS_3,         LOCAL_SS_plmn_specificTS_3},
    // { COMM_SS_plmn_specificTS_4,         LOCAL_SS_plmn_specificTS_4},
    // { COMM_SS_plmn_specificTS_5,         LOCAL_SS_plmn_specificTS_5},
    // { COMM_SS_plmn_specificTS_6,         LOCAL_SS_plmn_specificTS_6},
    // { COMM_SS_plmn_specificTS_7,         LOCAL_SS_plmn_specificTS_7},
    // { COMM_SS_plmn_specificTS_8,         LOCAL_SS_plmn_specificTS_8},
    // { COMM_SS_plmn_specificTS_9,         LOCAL_SS_plmn_specificTS_9},
    // { COMM_SS_plmn_specificTS_A,         LOCAL_SS_plmn_specificTS_A},
    // { COMM_SS_plmn_specificTS_B,         LOCAL_SS_plmn_specificTS_B},
    // { COMM_SS_plmn_specificTS_C,         LOCAL_SS_plmn_specificTS_C},
    // { COMM_SS_plmn_specificTS_D,         LOCAL_SS_plmn_specificTS_D},
    // { COMM_SS_plmn_specificTS_E,         LOCAL_SS_plmn_specificTS_E},
    // { COMM_SS_plmn_specificTS_F,         LOCAL_SS_plmn_specificTS_F},
};

PRIVATE T_COMM_sSsBearBSCodeList const sSsBearBSCodeLists[] =
{
    // Bearer services
    {COMM_SS_AllBearerServices, LOCAL_SS_AllBearerServices},
    {COMM_SS_allSynchronousServices, 0x68},
    {COMM_SS_allAsynchronousServices, 0x60},


    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_allAsynchronousServices   },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_AllPadAccessCAServices    },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_300bps        },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_1200bps       },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_1200_75bps    },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_2400bps       },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_4800bps       },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_PadAccessCA_9600bps       },
    // { COMM_SS_allAsynchronousServices,         LOCAL_SS_general_padAccessCA       },
    //
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_allSynchronousServices    },
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_ALLDataPDSServices        },
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_DataPDS_2400bps           },
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_DataPDS_4800bps           },
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_DataPDS_9600bps           },
    // { COMM_SS_allSynchronousServices,         LOCAL_SS_general_dataPDS           },
    //
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_allDataCircuitSynchronous },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_AllDataCDSServices        },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_DataCDS_1200bps           },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_DataCDS_2400bps           },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_DataCDS_4800bps           },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_DataCDS_9600bps           },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_general_dataCDS           },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_allAlternateSpeech_DataCDS  },
    // { COMM_SS_allDataCircuitSynchronous,  LOCAL_SS_allSpeechFollowedByDataCDS  },
    //
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_allDataCircuitAsynchronous},
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_AllDataCDAServices        },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_300bps            },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_1200bps           },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_1200_75bps        },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_2400bps           },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_4800bps           },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_DataCDA_9600bps           },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_general_dataCDA           },
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_allAlternateSpeech_DataCDA},
    // { COMM_SS_allDataCircuitAsynchronous, LOCAL_SS_allSpeechFollowedByDataCDA},
    //
    // { COMM_SS_allPLMN_specificBS,         LOCAL_SS_allPLMN_specificBS},
    // { COMM_SS_plmn_specificBS_1,          LOCAL_SS_plmn_specificBS_1},
    // { COMM_SS_plmn_specificBS_2,          LOCAL_SS_plmn_specificBS_2},
    // { COMM_SS_plmn_specificBS_3,          LOCAL_SS_plmn_specificBS_3},
    // { COMM_SS_plmn_specificBS_4,          LOCAL_SS_plmn_specificBS_4},
    // { COMM_SS_plmn_specificBS_5,          LOCAL_SS_plmn_specificBS_5},
    // { COMM_SS_plmn_specificBS_6,          LOCAL_SS_plmn_specificBS_6},
    // { COMM_SS_plmn_specificBS_7,          LOCAL_SS_plmn_specificBS_7},
    // { COMM_SS_plmn_specificBS_8,          LOCAL_SS_plmn_specificBS_8},
    // { COMM_SS_plmn_specificBS_9,          LOCAL_SS_plmn_specificBS_9},
    // { COMM_SS_plmn_specificBS_A,          LOCAL_SS_plmn_specificBS_A},
    // { COMM_SS_plmn_specificBS_B,          LOCAL_SS_plmn_specificBS_B},
    // { COMM_SS_plmn_specificBS_C,          LOCAL_SS_plmn_specificBS_C},
    // { COMM_SS_plmn_specificBS_D,          LOCAL_SS_plmn_specificBS_D},
    // { COMM_SS_plmn_specificBS_E,          LOCAL_SS_plmn_specificBS_E},
    // { COMM_SS_plmn_specificBS_F,          LOCAL_SS_plmn_specificBS_F},
    //
    // { COMM_SS_allGPRS_BearerServices,     LOCAL_SS_INVALID_BearerServices},
};
PRIVATE T_COMM_sSsServCodeList const sSsServCodeLists[] =
{
    {COMM_SS_CLIP, ss_CLIP},
    {COMM_SS_CLIR, ss_CLIR},
    {COMM_SS_COLP, ss_COLP},
    {COMM_SS_COLR, ss_COLR},
    {COMM_SS_CNAP, ss_CNAP},
    {COMM_SS_CFA, ss_CFA},
    {COMM_SS_CFU, ss_CFU},
    {COMM_SS_CD, ss_CD},
    {COMM_SS_CFC, ss_CFC},
    {COMM_SS_CFB, ss_CFB},
    {COMM_SS_CFNRY, ss_CFNRY},
    {COMM_SS_CFNRC, ss_CFNRC},
    {COMM_SS_USSD_V2, ss_USSD_V2},
    {COMM_SS_USSD_V1, ss_USSD_V1},
    {COMM_SS_ECT, ss_ECT},
    {COMM_SS_CW, ss_CW},
    {COMM_SS_HOLD, ss_HOLD},
    {COMM_SS_MPTY, ss_MPTY},
    {COMM_SS_AOCI, ss_AOCI},
    {COMM_SS_AOCC, ss_AOCC},
    {COMM_SS_UUS1, ss_UUS1},
    {COMM_SS_UUS2, ss_UUS2},
    {COMM_SS_UUS3, ss_UUS3},
    {COMM_SS_AllBarringSS, ss_AllBarringSS},
    {COMM_SS_BarringOfOutgoingCalls, ss_BarringOfOutgoingCalls},
    {COMM_SS_BAOC, ss_BAOC},
    {COMM_SS_BOIC, ss_BOIC},
    {COMM_SS_BOIC_ExHC, ss_BOIC_ExHC},
    {COMM_SS_BarringOfIncomingCalls, ss_BarringOfIncomingCalls},
    {COMM_SS_BAIC, ss_BAIC},
    {COMM_SS_BIC_ROAM, ss_BIC_ROAM}
};

//
// When System start, this function will be called one time.
// Typically, used this function to register AO proc,
// don't take a long time to process in this function.
// If you don't care this function, return ERR_SUCCESS.
//
#define HAVE_SS_STACK_AOM 0

PRIVATE T_COMM_eSsCode ss_GetMmiServCode(api_SsCode_t iSSProCode);
PRIVATE UINT32 CFW_SsAoProc(HAO hAo, CFW_EV *pEvent);

#if HAVE_SS_STACK_AOM
PRIVATE UINT32 CFW_SsStaticAoProc(HAO hAo, CFW_EV *pEvent);
#endif
PRIVATE UINT8 GetBSProtocolCode(UINT8 iBSMmiCode, T_COMM_eSsBasicServiceCodeCategory iBSCategory);
PRIVATE UINT8 GetBSMmiCode(UINT8 iBSProctolCode, api_SsBasicServiceCodeCategory_t iBSCategory);
PRIVATE UINT8 GetBSCategory(UINT8 iBSMmiCode);
PRIVATE UINT32 ss_GetMMiClass(UINT8 nClass);

// [[hameina [modify] 2006.10.23 :the declaration of this function is changed.
PRIVATE VOID ss_SetMmiBitStatus(T_COMM_eSsBasicServiceCodeCategory iBSCategory, UINT8 nBSCode, UINT8 status, UINT32 *p1,
                                UINT32 nClass);

// ]]hameina [modify]2006.10.23
PRIVATE VOID ss_InterCFList_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_QUERY_CALLFORWARDING_RSP *pForwardingInd,
                                 SS_SM_INFO *proc);
PRIVATE VOID ss_SetCF_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCF,
                           SS_SM_INFO *proc);
PRIVATE VOID ss_Api_Err_Ind(HAO hAo, api_SsErrorInd_t *pInMsg, SS_SM_INFO *proc);

#ifdef SS_NO_RETURNRESULT_HANDLE
PRIVATE VOID ss_SetCF_NoRet_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCF,
                                 SS_SM_INFO *proc);
PRIVATE VOID ss_Api_Cnf_No_Retresult(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc);
#endif
PRIVATE VOID ss_Inter_BSGroupList_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc);
PRIVATE VOID ss_SetCBInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc);
PRIVATE UINT8 ss_Inter_StatusInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc);
PRIVATE VOID ss_Inter_GSInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc);
VOID Ucs2ToHexAscii(UINT8 *pSrc, UINT8 *pDest, UINT16 uSrcLen, UINT16 uDestLen);

#ifdef USSD_ENDABLE
PRIVATE UINT32 CFW_SsMtAoProc(HAO hAo, CFW_EV *pEvent); // hameina[+]
#endif

// This function is for USSD, it can converte the net string to ascii

VOID Ucs2ToHexAscii(UINT8 *pSrc, UINT8 *pDest, UINT16 uSrcLen, UINT16 uDestLen)
{
    UINT8 low   = 0;
    UINT8 high  = 0;
    UINT8 *pStr = NULL;
    UINT16 i    = 0;

    if (pSrc == NULL || pDest == NULL || uSrcLen == 0 || uDestLen == 0)
    {
        return;
    }

    if (uSrcLen * 2 > uDestLen)
    {
        return;
    }

    pStr = pSrc;

    while ((*pStr != 0) && (i < uSrcLen))
    {
        low  = (*pStr) & 0x0F;
        high = ((*pStr) >> 4) & 0x0F;

        if (low < 10)
        {
            low += '0';
        }
        else
        {
            low = ('A' + low - 10);
        }

        if (high < 10)
        {
            high += '0';
        }
        else
        {
            high = ('A' + high - 10);
        }

        *(pDest + i * 2) = high;
        *(pDest + i * 2 + 1) = low;

        i++;
        pStr++;

    }

}

// [[hameina[+]2007-5-30 :when recieve MT msg from stack, registor ao in this function
// [[Qiaoguangjun[mod] 2007.6.6
#ifdef USSD_ENDABLE

UINT32 CFW_SendFacilityMsg(CFW_SIM_ID nSimID, UINT8 nTI)
{
    api_SsActionReq_t *sOutMsg = NULL;
    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    if (sOutMsg == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SendFacilityMsg,msg Malloc return NULL\n",0x081004bf)));
        return ERR_NO_MORE_MEMORY;
    }
    sOutMsg->ss_Code      = ss_USSD_V2;
    sOutMsg->ss_Operation = ss_UnstructuredSSNotify;
    SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
    sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_FacilityMessageType;
    sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDReturnResult;
    sOutMsg->ss_Params.ss_USSDParams.TI                   = nTI;
    CFW_SendSclMessage(API_SS_ACTION_REQ, sOutMsg, nSimID);
    return ERR_SUCCESS;
}

UINT32 CFW_SendRelComMsg(CFW_SIM_ID nSimID, UINT8 nTI)
{
    api_SsActionReq_t *sOutMsg = NULL;
    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    if (sOutMsg == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SendFacilityMsg,msg Malloc return NULL\n",0x081004c0)));
        return ERR_NO_MORE_MEMORY;
    }
    sOutMsg->ss_Code      = ss_USSD_V2;
    sOutMsg->ss_Operation = ss_UnstructuredSSNotify;
    SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
    sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_ReleaseCompleteMessageType;
    sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDNoComponentPresent;
    sOutMsg->ss_Params.ss_USSDParams.TI                   = nTI;

    CFW_SendSclMessage(API_SS_ACTION_REQ, sOutMsg, nSimID);

    return ERR_SUCCESS;

}

HAO CFW_SsMtReg(CFW_EV *pEvent)
{
    SS_SM_INFO *proc = NULL;
    HAO s_hAo = 0;
    UINT32 nAoCount;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsMtReg);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtReg  begin\n",0x081004c1)));

    UINT8 ret;
    CFW_SIM_ID nSimID;

    nSimID = (CFW_SIM_ID) pEvent->nFlag;
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsMtReg  nSimID =%d\n",0x081004c2)), nSimID);

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtReg  check nSimID failed\n",0x081004c3)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClip);
        return ret;
    }
    nAoCount = CFW_GetAoCount(CFW_SS_SRV_ID, nSimID);

    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));

    if (NULL == proc)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtReg  get proc data failed\n",0x081004c4)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtReg);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(proc, SIZEOF(SS_SM_INFO));
    proc->nTI               = 0;
    proc->nApi_SS_Code      = ss_InvalidSSCode;
    proc->nApi_SS_Operation = ss_InvalidSSOperation;
    proc->nClass            = 0xFF;
    proc->nUSSDFlag         = TRUE;
    proc->pMsg              = NULL;
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SsMtReg  pSSProc = 0x%X , aocount, %d\n",0x081004c5)), proc, nAoCount);

    s_hAo = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsMtAoProc, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtReg  end\n",0x081004c6)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtReg);
    return s_hAo;

}

PRIVATE UINT32 CFW_SsMtAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nEvtId;
    UINT32 nUTI, nState;

    // VOID* pEvParam = NULL;
    SS_SM_INFO *pSSProc = NULL;
    VOID *nEvParam = NULL;
    CFW_EV ev;

    api_SsActionReq_t *sOutMsg = NULL;  // hameina[+]
    CSW_PROFILE_FUNCTION_ENTER(CFW_SsMtAoProc);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc begin !\n",0x081004c7)));
    if ((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc Send MESSAGE !\n",0x081004c8)));

        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent     = &ev;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsMtAoProc MESSAGE  ***EvtID %x\n",0x081004c9)), pEvent->nEventId);
    }

    if (pEvent == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc INPUT pEvent=NULL !\n",0x081004ca)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    // Get private date

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    pSSProc = CFW_GetAoUserData(hAo);

    // Verify memory
    if (!pSSProc)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SsMtAoProc, 1,get private data fail! \n ",0x081004cb));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
        return ERR_NO_MORE_MEMORY;
    }
    sOutMsg = (api_SsActionReq_t *)(pSSProc->pMsg); // hameina[+]

    // Get current State
    nState   = CFW_GetAoState(hAo);
    nEvtId   = pEvent->nEventId;
    nEvParam = (VOID *)pEvent->nParam1;

    // ss MT State machine process
    if (CFW_SM_STATE_IDLE == nState)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc idle state\n",0x081004cc)));
        if (nEvtId == API_SS_ACTION_IND)  // to be API_SS_ACTION_IND
        {
            api_SsActionCnf_t *pInMsg = (api_SsActionCnf_t *)(pEvent->nParam1);

            if (pInMsg->ss_Code != ss_USSD_V1 && pInMsg->ss_Code != ss_USSD_V2)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc,2,ti=0x%x, ss_code=0x%x, operation=0x%x \n",0x081004cd)), pInMsg->TI,
                          pInMsg->ss_Code, pInMsg->ss_Operation);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo); // here need a new service id
                ret = ERR_CFW_INVALID_COMMAND;  // ???
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                return ret;
            }
            else  // ussd operation
            {
                // UINT16 nNotifyID=0;
                UINT32 nParam2  = 0;
                UINT32 nDataLen = 0;
                UINT16 nUTItemp = 0;

                CFW_SS_USSD_IND_INFO *pUssdInfo = NULL;

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("ti=0x%x, ss_code=0x%x, operation=0x%x \n",0x081004ce)),
                          pInMsg->TI, pInMsg->ss_Code, pInMsg->ss_Operation);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RxMsgType(Rels:2A,F:3A;R:3B;No:3C)=0x%x\n",0x081004cf)),
                          pInMsg->ss_ReturnResult.ss_RxMessageType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RtnRstInfoPre(V1=6; v2=5)=%d\n",0x081004d0)),
                          pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ComponentType(nwInvoke=0; nwRtn=1; no=2)=%d\n",0x081004d1)),
                          pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ussdpre(0/1)=%d\n",0x081004d2)),
                          pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pInMsg->TI %d\n",0x081004d3)), pInMsg->TI);

                // save proc info/////////////////////
                pSSProc->nTI = pInMsg->TI;

                // if(ERR_SUCCESS==CFW_GetFreeUTI(CFW_SS_SRV_ID,(UINT8 *)&nUTItemp))
                if (0)
                    pSSProc->nUTI_Mmi = nUTItemp;
                else
                {
                    pSSProc->nUTI_Mmi = pInMsg->TI;
                    nUTItemp          = pInMsg->TI;
                }

                CFW_SetUTI(hAo, nUTItemp, 0);
                CFW_SetUTI(hAo, pInMsg->TI, 2);
                pSSProc->nApi_SS_Code      = pInMsg->ss_Code;
                pSSProc->nApi_SS_Operation = pInMsg->ss_Operation;

                // if(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent!=FALSE)
                {
                    if (pInMsg->ss_Code == ss_USSD_V1)
                    {
                        nDataLen =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                    }
                    else
                    {
                        nDataLen = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                    }
                }
                pUssdInfo = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nDataLen + 1);
                if (NULL == pUssdInfo)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, Malloc Mem ERR\n",0x081004d4)));

                    // here need a operation to deal with err
                    // err to NW to be + ??
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory32(pUssdInfo, SIZEOF(CFW_SS_USSD_IND_INFO) + nDataLen + 1);
                pUssdInfo->pUsdString = (UINT8 *)((UINT8 *)pUssdInfo + SIZEOF(CFW_SS_USSD_IND_INFO));

                pUssdInfo->nStingSize = nDataLen;

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CSW_TC_MEMBLOCK nDataLen %d\n",0x081004d5)), nDataLen);
                CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                nDataLen, 0x16);

                if (ss_USSD_V1 == pInMsg->ss_Code)
                {
                    SUL_MemCopy8(pUssdInfo->pUsdString,
                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5String,
                                 nDataLen);
                    pUssdInfo->nOption = 1; // further user action request
                    pUssdInfo->nDcs    = 0;
                }
                else  // v2
                {
                    SUL_MemCopy8(pUssdInfo->pUsdString,
                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                 nDataLen);
                    if (ss_UnstructuredSSRequest == pInMsg->ss_Operation)
                        pUssdInfo->nOption = 1; // further user action request
                    else if (ss_UnstructuredSSNotify == pInMsg->ss_Operation)
                    {
                        pUssdInfo->nOption = 0; // request user return a empty result
                        // Add Facility msg to NW
                        CFW_SendFacilityMsg(nSimID, pInMsg->TI);
                    }
                    pUssdInfo->nDcs =
                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;
                }
                nParam2 = (pInMsg->TI << 16) | (pInMsg->ss_Code);

                CFW_GetUTI(hAo, &nUTI);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Static SS receive USSD Messages pSSProc->nUTI_Mmi = %d nUTI %d \n",0x081004d6)),
                          pSSProc->nUTI_Mmi, nUTI);

                CFW_PostNotifyEvent(EV_CFW_SS_USSD_IND, (UINT32)pUssdInfo, nParam2, nUTI | (nSimID << 24), 0);
                if (ss_USSD_V2 == pInMsg->ss_Code && ss_UnstructuredSSNotify == pInMsg->ss_Operation)
                {

                    // Add Release Complete msg to NW
                    CFW_SendRelComMsg(nSimID, pInMsg->TI);

                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                }
                else
                    CFW_SetAoState(hAo, CFW_SM_SS_STATE_CONNECT);

                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                return ret;

            }

            // end of ussd operation //end of nEvtId==API_SS_ACTION_CNF && ss_code=ussd

        }

        else  // other EventIDs
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, Unknown event, not consumed\n",0x081004d7)));
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
            return ret;
        }
    }
    else if (CFW_SM_SS_STATE_CONNECT == nState) // mmi response to nw
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc CFW_SM_SS_STATE_CONNECT\n",0x081004d8)));
        if (0xff == pEvent->nParam1)  // send from internal function
        {
#ifdef USSD_TEST
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Code	 = %d\n ",0x081004d9), sOutMsg->ss_Code);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Operation %d\n ",0x081004da), sOutMsg->ss_Operation);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x081004db),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x081004dc),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDDataCodingScheme = %d\n ",0x081004dd),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(".ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = %d\n ",0x081004de),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x081004df),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[0]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x081004e0),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[1]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x081004e1),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[2]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x081004e2),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[3]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x081004e3),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[4]);

#else
            CFW_SendSclMessage(API_SS_ACTION_REQ, pSSProc->pMsg, nSimID);
#endif
            if (pSSProc->nApi_SS_Code == ss_USSD_V2 || pSSProc->nApi_SS_Code == ss_USSD_V1)
            {
                // MS release transation, no cnf come back from stack, so, release AO
                if (ss_ReleaseCompleteMessageType == sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType)
                {
                    CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, 0, 2, nUTI | (nSimID << 24), 0);

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Send release msg unregisterao!!,3, nUTI=%d",0x081004e4)), nUTI);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                    return ret;
                }
            }
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc Send api_ss_action_req \n",0x081004e5)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
            return ret;
        }

        switch (nEvtId)
        {

            case API_SS_ACTION_IND:  // to be API_SS_ACTION_IND
            {
                // it is a unexpect case, csw can not deal with it
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR] CFW_SsMtAoProc,API_SS_ACTION_IND wrong place \n",0x081004e6)));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                return ret;

            }
            break;

            case API_SS_ACTION_CNF:
            {
                api_SsActionCnf_t *pInMsg = (api_SsActionCnf_t *)nEvParam;

                if (pSSProc->nTI != pInMsg->TI)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc,not same ti, exit\n",0x081004e7)));
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                    return ret;
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("10,In API_SS_ACTION_CNF\n",0x081004e8)));
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_RetRsltInfoPresence %d\n",0x081004e9)), pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pInMsg->ss_Operation %d\n",0x081004ea)), pInMsg->ss_Operation);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_RxMessageType %d\n",0x081004eb)), pInMsg->ss_ReturnResult.ss_RxMessageType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_USSDNetworkComponentType %d\n",0x081004ec)),
                          pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType);
                switch (pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence)
                {

                    case ss_USSDV2ResultsPresent:
                    {
                        pSSProc->nApi_SS_Operation = pInMsg->ss_Operation;

                        switch (pInMsg->ss_Operation)
                        {
                            // return result for the MS invoke, it should not be here, here is MT proc
                            // according 24090(3GPP), in network initiate USSD Operation, the MS will not invoke another in the same transaciton
                            // while  NW can do that in the situation that MS initiate USSD Operation.
                            // it is not necessary the follow code to exsit, but for same extremely case, we keep it temporarily

                            case ss_ProcessUnstructuredSSRequest:
                            {

                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                param2 = pInMsg->ss_Code;

                                CFW_GetUTI(hAo, &nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x081004ed), nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[Warning]! CFW_SsMtAoProc,msg belong MS iniated, here is MT proc!\n",0x081004ee)));
                                nNum            = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, Malloc Mem ERR\n",0x081004ef)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                pUSSD_info_temp->nDcs =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                pUSSD_info_temp->nStingSize =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                // SUL_UCS2ToLocallanguage()输入输出字符串的长度一致?会不会越界?
                                // if(pUSSD_info_temp->nStingSize>0)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);
                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                             pUSSD_info_temp->nStingSize);

                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsMtAoProc pInMsg->ss_ReturnResult.ss_RxMessageType=%d\n",0x081004f0)),
                                          pInMsg->ss_ReturnResult.ss_RxMessageType);
                                if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {

                                    pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x081004f1)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                                else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {
                                    pUSSD_info_temp->nOption = 1;

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                            }
                            break;

                            // t
                            case ss_UnstructuredSSNotify:
                            case ss_UnstructuredSSRequest:
                            {

                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                CFW_GetUTI(hAo, &nUTI);
                                param2          = pInMsg->ss_Code;
                                nNum            = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);

                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, 2,Malloc Mem ERR\n",0x081004f2)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                pUSSD_info_temp->nDcs =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                pUSSD_info_temp->nStingSize =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                // if(pUSSD_info_temp->nStingSize)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);

                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                             pUSSD_info_temp->nStingSize);
                                if (ss_USSDNetworkNoComponentPresent ==
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x081004f3)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;

                                }
                                else if (ss_USSDNetworkInvoke == pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    if (ss_FacilityMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 1; // further information is needed

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else if (ss_RegisterMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        // it should not be here
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong place \n",0x081004f4)));
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;
                                    }
                                    else if (ss_ReleaseCompleteMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 6; // USSD terminated by network without display

                                        CFW_PostNotifyEvent(EV_CFW_SS_USSD_IND, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24), 0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x081004f5)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else
                                    {
                                        // err, need debug
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong parameter,msg type \n",0x081004f6)));
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }

                                }
                                else if (ss_USSDNetworkReturnResult ==
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  release comleate !component ss_USSDReturnResult \n",0x081004f7)));

                                        pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MT ussd CFW_UnRegisterAO \n",0x081004f8)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  msg type API_SS_FACILITY_MESSAGE_TYPE\n",0x081004f9)));
                                        pUSSD_info_temp->nOption = 1;

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        // 需要确认，是否要consumed?
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;
                                    }
                                    else if (ss_ReleaseCompleteMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 6; // USSD terminated by network without display

                                        CFW_PostNotifyEvent(EV_CFW_SS_USSD_IND, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24), 0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x081004fa)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else
                                    {
                                        // err, need debug
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong parameter,msg type \n",0x081004fb)));
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                }
                                else
                                {
                                    // err, need debug
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd ,wrong component type \n",0x081004fc)));
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;

                                }

                            }
                            break;

                            default:
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong parameter ss_operation\n",0x081004fd)));
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                return ret;
                                break;
                        }

                    }

                    break;
                    case ss_USSDV1ResultsPresent:
                    {
                        switch (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                        {

                            case ss_USSDNetworkNoComponentPresent: // nw release
                            {
                                // UINT32 nNum=0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                param2 = pInMsg->ss_Code;
                                CFW_GetUTI(hAo, &nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x081004fe), nUTI);
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO));
                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x081004ff)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }
                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO));
                                pUSSD_info_temp->nOption    = 2;
                                pUSSD_info_temp->pUsdString = NULL;

                                CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24), 0);

                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_UnRegisterAO\n",0x08100500)));
                                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                return ret;

                            }
                            break;

                            case ss_USSDNetworkReturnResult:
                            {

                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                param2 = pInMsg->ss_Code;
                                CFW_GetUTI(hAo, &nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x08100501), nUTI);

                                nNum =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100502)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                // pUSSD_info_temp->nDcs=pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                pUSSD_info_temp->nStingSize = nNum;

                                // if(nNum>0)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5String,nNum, pUSSD_info_temp->pUsdString);

                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.
                                             ss_IA5String, pUSSD_info_temp->nStingSize);

                                if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {

                                    pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_UnRegisterAO\n",0x08100503)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                                else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {
                                    pUSSD_info_temp->nOption = 1;

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                            }
                            break;
                            case ss_USSDNetworkInvoke:
                            {
                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                param2 = pInMsg->ss_Code;
                                CFW_GetUTI(hAo, &nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x08100504), nUTI);

                                nNum =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100505)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));
                                pUSSD_info_temp->nStingSize = nNum;

                                // if(nNum>0)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5String,nNum, pUSSD_info_temp->pUsdString);
                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.
                                             ss_IA5String, pUSSD_info_temp->nStingSize);
                                pUSSD_info_temp->nOption = 1;

                                CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24), 0);

                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                return ret;

                            }
                            break;

                            default:

                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_UnRegisterAO\n",0x08100506)));
                                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err,ussd wrong parameter, component!\n",0x08100507)));
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                return ret;
                                break;
                        }
                    }
                    case ss_NoRetRsltInfoPresent:
                    {
                        pSSProc->nApi_SS_Operation = pInMsg->ss_Operation;

                        switch (pInMsg->ss_Operation)
                        {
                            // return result for the MS invoke, it should not be here, here is MT proc
                            // according 24090(3GPP), in network initiate USSD Operation, the MS will not invoke another in the same transaciton
                            // while  NW can do that in the situation that MS initiate USSD Operation.
                            // it is not necessary the follow code to exsit, but for same extremely case, we keep it temporarily

                            case ss_ProcessUnstructuredSSRequest:
                            {

                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                param2 = pInMsg->ss_Code;

                                CFW_GetUTI(hAo, &nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x08100508), nUTI);
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[Warning]! CFW_SsMtAoProc,msg belong MS iniated, here is MT proc!\n",0x08100509)));
                                nNum            = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, Malloc Mem ERR\n",0x0810050a)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                pUSSD_info_temp->nDcs =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                pUSSD_info_temp->nStingSize =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                // SUL_UCS2ToLocallanguage()输入输出字符串的长度一致?会不会越界?
                                // if(pUSSD_info_temp->nStingSize>0)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);
                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                             pUSSD_info_temp->nStingSize);

                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsMtAoProc pInMsg->ss_ReturnResult.ss_RxMessageType=%d\n",0x0810050b)),
                                          pInMsg->ss_ReturnResult.ss_RxMessageType);
                                if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {

                                    pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x0810050c)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                                else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                {
                                    pUSSD_info_temp->nOption = 1;

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;
                                }
                            }
                            break;

                            // t
                            case ss_UnstructuredSSNotify:
                            case ss_UnstructuredSSRequest:
                            {

                                UINT32 nNum   = 0;
                                UINT32 param2 = 0;

                                CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                CFW_GetUTI(hAo, &nUTI);
                                param2          = pInMsg->ss_Code;
                                nNum            = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);

                                if (NULL == pUSSD_info_temp)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, 2,Malloc Mem ERR\n",0x0810050d)));
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ERR_NO_MORE_MEMORY;
                                }

                                SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                pUSSD_info_temp->nDcs =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                pUSSD_info_temp->nStingSize =
                                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                // if(pUSSD_info_temp->nStingSize)
                                // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);

                                SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                             pUSSD_info_temp->nStingSize);
                                if (ss_USSDNetworkNoComponentPresent ==
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x0810050e)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;

                                }
                                else if (ss_USSDNetworkInvoke == pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    if (ss_FacilityMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 1; // further information is needed

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else if (ss_RegisterMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        // it should not be here
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong place \n",0x0810050f)));
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;
                                    }
                                    else if (ss_ReleaseCompleteMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 6; // USSD terminated by network without display anything

                                        CFW_PostNotifyEvent(EV_CFW_SS_USSD_IND, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24), 0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsMtAoProc UnRegisterAO\n",0x08100510)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;
                                    }
                                    else
                                    {
                                        // err, need debug
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong parameter,msg type \n",0x08100511)));
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }

                                }
                                else if (ss_USSDNetworkReturnResult ==
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                {
                                    if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  release comleate !component ss_USSDReturnResult \n",0x08100512)));

                                        pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MT ussd CFW_UnRegisterAO \n",0x08100513)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;

                                    }
                                    else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  msg type API_SS_FACILITY_MESSAGE_TYPE\n",0x08100514)));
                                        pUSSD_info_temp->nOption = 1;

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        // 需要确认，是否要consumed?
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                        return ret;
                                    }
                                }
                                else
                                {
                                    // err, need debug
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd ,wrong component type \n",0x08100515)));
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                    return ret;

                                }

                            }
                            break;

                            default:
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err ussd wrong parameter ss_operation\n",0x08100516)));
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                                return ret;
                                break;
                        }

                    }
                    break;

                    default:
                    {
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("not be here!USSD ss_RetRsltInfoPresence %d\n",0x08100517)),
                                  pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                        return ret;
                    }

                    break;
                }

            }
            break;
            case API_SS_ERROR_IND:
            {
                api_SsErrorInd_t *pInMsg = (api_SsErrorInd_t *)nEvParam;

                // pSSProc->nTI=pInMsg->TI; //???? error ti what to do
                if (pInMsg->ss_Code == ss_USSD_V2 || pInMsg->ss_Code == ss_USSD_V1)
                {
                    // add Qiaogj
                    // to avoid unregister others AO when err, for there are two AO(MO/MT)
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD err msg pInMsg->ss_Code %d\n",0x08100518), pInMsg->ss_Code);
                    if (pSSProc->nTI != pInMsg->TI)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("not same TI!pInMsg->TI %d\n",0x08100519)), pInMsg->TI);
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                        return ret;

                    }
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("28,In API_SS_ERROR_IND!\n",0x0810051a)));
                ss_Api_Err_Ind(hAo, pInMsg, pSSProc);

                // CFW_SetAoState(hAo,CFW_SM_STATE_IDLE);

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("29,Unregistered AO\n",0x0810051b));
                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                break;
            }
            break;
            default:
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("31,should not be here 4, need debug!% x\n",0x0810051c)), nEvtId);

                CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                return ret;
            }

            break;

        }
    }
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
    return ret;
}

#endif
UINT32 CFW_SsInit(VOID)
{
    UINT32 ret = ERR_SUCCESS;

#ifdef USSD_ENDABLE

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsInit);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SsInit begin\n",0x0810051d));

    CFW_RegisterCreateAoProc(API_SS_ACTION_IND, (PFN_EV_IND_TRIGGER_PORC)CFW_SsMtReg);

    CSW_PROFILE_FUNCTION_EXIT(CFW_SsInit);
#endif
    return ret;
}

// ]][+]end
// ]]Qiaoguangjun[mod] for mt msg 2007.6.6

// [[Qiaoguangjun[-] for mt msg 2007.6.6
#if 0
UINT32 CFW_SsInit(VOID)
{
    UINT32 ret = ERR_SUCCESS;

#if HAVE_SS_STACK_AOM
    SS_SM_INFO *proc = NULL;
    HAO s_hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsInit);

    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));

    proc->nTI               = 0;
    proc->nApi_SS_Code      = ss_InvalidSSCode;
    proc->nApi_SS_Operation = ss_InvalidSSOperation;
    proc->nClass            = 0xFF;
    proc->nUSSDFlag         = ss_USSDNetworkNoComponentPresent;
    proc->pMsg              = NULL;
    s_hAo                   = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsStaticAoProc);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsInit);
#endif
    return ret;
}

#if HAVE_SS_STACK_AOM
PRIVATE UINT32 CFW_SsStaticAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId;
    VOID *nEvParam;

    SS_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;
    CFW_EV ev;
    UINT32 nUTI;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsStaticAoProc);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In SS_Static_AO_Proc\n",0x0810051e)));

    if ((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SS Send MESSAGE !\n",0x0810051f)));

        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent     = &ev;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Static_SS RECEIVED MESSAGE  ***EvtID %x\n",0x08100520)), pEvent->nEventId);
    }

    if (pEvent == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        proc     = CFW_GetAoUserData(hAo);
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    if (pEvent->nParam1 == 0xff)  // send from internal function
    {
        CFW_SendSclMessage(API_SS_ACTION_REQ, proc->pMsg);
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Static SS Send api_ss_action_req \n",0x08100521)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
        return ret;
    }
    else if (nEvtId == API_SS_TI_IND)
    {
        api_SsTIInd_t *p = (api_SsTIInd_t *)nEvParam;
        if (p->ss_Code != ss_USSD_V1 && p->ss_Code != ss_USSD_V2)
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
            return ret;
        }
        proc->nTI = p->TI;
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In Static API_SS_TI_IND===> TI is: %d\n",0x08100522)), p->TI);
        CFW_SetUTI(hAo, p->TI, 2);
    } // end of  if(nEvtId==API_SS_TI_IND)
    else if (nEvtId == API_SS_ACTION_CNF)
    {
        api_SsActionCnf_t *pInMsg = (api_SsActionCnf_t *)nEvParam;

        if (pInMsg->ss_Code != ss_USSD_V1 && pInMsg->ss_Code != ss_USSD_V2)
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
            return ret;
        }
        else  // ussd operation
        {
            UINT16 nNotifyID = 0;
            UINT32 nParam2   = 0;
            UINT32 nNum      = 0;

            CFW_SS_USSD_IND_INFO *pUssdInfo = NULL;

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("ti=0x%x, ss_code=0x%x, operation=0x%x \n",0x08100523)),
                      pInMsg->TI, pInMsg->ss_Code, pInMsg->ss_Operation);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RxMsgType(Rels:2A,F:3A;R:3B;No:3C)=0x%x\n",0x08100524)),
                      pInMsg->ss_ReturnResult.ss_RxMessageType);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RtnRstInfoPre(V1=6; v2=5)=%d\n",0x08100525)), pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ComponentType(nwInvoke=0; nwRtn=1; no=2)=%d\n",0x08100526)),
                      pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ussdpre(0/1)=%d\n",0x08100527)),
                      pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent);

            if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent != FALSE)
            {
                if (pInMsg->ss_Code == ss_USSD_V1)
                {
                    nNum = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                }
                else
                {
                    nNum = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                }
            }
            pUssdInfo = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
            if (pUssdInfo == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100528)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
                return ERR_NO_MORE_MEMORY;
            }
            SUL_ZeroMemory32(pUssdInfo, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
            pUssdInfo->pUsdString = (UINT8 *)((UINT8 *)pUssdInfo + SIZEOF(CFW_SS_USSD_IND_INFO));
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Static SS receive USSD Messages\n",0x08100529)));

            // /////////////////////
            proc->nTI               = pInMsg->TI;
            proc->nApi_SS_Code      = pInMsg->ss_Code;
            proc->nApi_SS_Operation = pInMsg->ss_Operation;
            proc->nUSSDFlag         = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType;

            // /////////////////////
            switch (pInMsg->ss_Operation)
            {
                case ss_UnstructuredSSRequest:
                    nNotifyID = (UINT8)EV_CFW_SS_USSD_IND;
                    if (pInMsg->ss_ReturnResult.ss_RxMessageType == ss_ReleaseCompleteMessageType)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("received USSD_Request_Release Msg\n",0x0810052a));
                        proc->nApi_SS_Code      = ss_InvalidSSCode;
                        proc->nApi_SS_Operation = ss_InvalidSSOperation;
                        proc->nUSSDFlag         = ss_USSDNetworkNoComponentPresent;

                        pUssdInfo->nOption = 2; // ussd terminated by nw
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received USSD_Request_register(Facility) Msg\n",0x0810052b)));
                        pUssdInfo->nOption = 1; // futher action required
                    }

                    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent != FALSE)
                    {
                        pUssdInfo->nStingSize =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                        pUssdInfo->nDcs =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;
                        if (pUssdInfo->nStingSize > 0)
                        {
                            SUL_MemCopy8(pUssdInfo->pUsdString,
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                         pUssdInfo->nStingSize);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("MMI USSD str:%d,%d,%d--%d,%d,%d\n",0x0810052c)),
                                      pUssdInfo->pUsdString[0],
                                      pUssdInfo->pUsdString[1],
                                      pUssdInfo->pUsdString[2],
                                      pUssdInfo->pUsdString[3], pUssdInfo->pUsdString[4], pUssdInfo->pUsdString[5]);
                        }

                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("nUSSDFlag=%d, nApi_SS_Code=%d, nApi_SS_Operation=%d\n",0x0810052d)),
                              proc->nUSSDFlag, proc->nApi_SS_Code, proc->nApi_SS_Operation);
                    break;
                case ss_UnstructuredSSNotify:
                    nNotifyID = (UINT8)EV_CFW_SS_USSD_IND;
                    if (pInMsg->ss_ReturnResult.ss_RxMessageType == ss_ReleaseCompleteMessageType)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received USSD_Notify_Release Msg\n",0x0810052e)));
                        proc->nApi_SS_Code      = ss_InvalidSSCode;
                        proc->nApi_SS_Operation = ss_InvalidSSOperation;
                        proc->nUSSDFlag         = ss_USSDNetworkNoComponentPresent;
                        pUssdInfo->nOption      = 2; // ussd terminated by network
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received USSD_Notify_Register(Facility) Msg\n",0x0810052f)));
                        pUssdInfo->nOption = 0; // No further action required
                    }

                    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent != FALSE)
                    {
                        pUssdInfo->nStingSize =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                        pUssdInfo->nDcs =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;
                        if (pUssdInfo->nStingSize > 0)
                        {
                            SUL_MemCopy8(pUssdInfo->pUsdString,
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                         pUssdInfo->nStingSize);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("MMI USSD str:%d,%d,%d--%d,%d,%d\n",0x08100530)),
                                      pUssdInfo->pUsdString[0],
                                      pUssdInfo->pUsdString[1],
                                      pUssdInfo->pUsdString[2],
                                      pUssdInfo->pUsdString[3], pUssdInfo->pUsdString[4], pUssdInfo->pUsdString[5]);
                        }

                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("nUSSDFlag=%d, nApi_SS_Code=%d, nApi_SS_Operation=%d\n",0x08100531)),
                              proc->nUSSDFlag, proc->nApi_SS_Code, proc->nApi_SS_Operation);
                    break;
                case ss_ProcessUnstructuredSSRequest:
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received ProUSSD_Request Msg\n",0x08100532)));
                    nNotifyID               = EV_CFW_SS_SEND_USSD_RSP;
                    proc->nApi_SS_Code      = ss_InvalidSSCode;
                    proc->nApi_SS_Operation = ss_InvalidSSOperation;
                    proc->nUSSDFlag         = ss_USSDNetworkNoComponentPresent;
                    pUssdInfo->nOption      = 2; // ussd terminated by network

                    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent != FALSE)
                    {
                        pUssdInfo->nStingSize =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                        pUssdInfo->nDcs =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;
                        if (pUssdInfo->nStingSize > 0)
                        {
                            SUL_MemCopy8(pUssdInfo->pUsdString,
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                         pUssdInfo->nStingSize);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("MMI USSD str:%d,%d,%d--%d,%d,%d\n",0x08100533)),
                                      pUssdInfo->pUsdString[0],
                                      pUssdInfo->pUsdString[1],
                                      pUssdInfo->pUsdString[2],
                                      pUssdInfo->pUsdString[3], pUssdInfo->pUsdString[4], pUssdInfo->pUsdString[5]);
                        }

                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("nUSSDFlag=%d, nApi_SS_Code=%d, nApi_SS_Operation=%d\n",0x08100534)),
                              proc->nUSSDFlag, proc->nApi_SS_Code, proc->nApi_SS_Operation);
                    break;
                case ss_ProcessUnstructuredSSData:
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received ProUSSD_Data Msg\n",0x08100535)));
                    nNotifyID               = EV_CFW_SS_SEND_USSD_RSP;
                    proc->nApi_SS_Code      = ss_InvalidSSCode;
                    proc->nApi_SS_Operation = ss_InvalidSSOperation;
                    proc->nUSSDFlag         = ss_USSDNetworkNoComponentPresent;
                    pUssdInfo->nOption      = 2; // ussd terminated by network
                    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.bUSSDResultsPresent != FALSE)
                    {
                        pUssdInfo->nStingSize =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                        pUssdInfo->nDcs =
                            pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;
                        if (pUssdInfo->nStingSize > 0)
                        {
                            SUL_MemCopy8(pUssdInfo->pUsdString,
                                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                         pUssdInfo->nStingSize);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("MMI USSD str:%d,%d,%d--%d,%d,%d",0x08100536)),
                                      pUssdInfo->pUsdString[0],
                                      pUssdInfo->pUsdString[1],
                                      pUssdInfo->pUsdString[2],
                                      pUssdInfo->pUsdString[3], pUssdInfo->pUsdString[4], pUssdInfo->pUsdString[5]);
                        }

                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("nUSSDFlag=%d, nApi_SS_Code=%d, nApi_SS_Operation=%d",0x08100537)),
                              proc->nUSSDFlag, proc->nApi_SS_Code, proc->nApi_SS_Operation);
                    break;
                default:
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
                    return ret;
            }
            CFW_GetUTI(hAo, &nUTI);
            CFW_PostNotifyEvent(nNotifyID, (UINT32)pUssdInfo, nParam2, nUTI, 0);

        } // end of ussd operation //end of nEvtId==API_SS_ACTION_CNF && ss_code=ussd

    } // end of if(nEvtId==API_SS_ACTION_CNF)
    else if (nEvtId == API_SS_ERROR_IND)
    {
        api_SsErrorInd_t *pInMsg = (api_SsErrorInd_t *)nEvParam;
        if (pInMsg->ss_Code != ss_USSD_V1 && pInMsg->ss_Code != ss_USSD_V2)
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
            return ret;

        }
        else
        {
            api_SsErrorInd_t *pInMsg = (api_SsErrorInd_t *)nEvParam;
            proc->nTI = pInMsg->TI; // ???? error ti what to do
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In API_SS_ERROR_IND!\n",0x08100538)));
            ss_Api_Err_Ind(hAo, pInMsg, proc);
        }
    } // end if  nEvtId==API_SS_ERROR_IND
    else  // other EventIDs
    {
        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
        return ret;
    }
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsStaticAoProc);
    return ret;
}
#endif
#endif
// ]]Qiaoguangjun[-] for mt msg

PRIVATE UINT32 CFW_SsAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId = 0;
    UINT32 nUTI   = 0;
    UINT32 nState = 0;
    UINT32 ret    = ERR_SUCCESS;
    CFW_EV ev;

    // UINT8 pBuf[160] = {0};

    UINT8 nUnregAOFlag = 0;

    CFW_SS_QUERY_CALLFORWARDING_RSP *pForwardingInd = NULL;
    CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCFInd = NULL;
    VOID *nEvParam = NULL;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;  // hameina[+] 2007.6.7

    CFW_SIM_ID nSimID = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsAoProc);
    if (pEvent == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]0,In CFW_SsAoProc,pEvent==NULL\n",0x08100539)));
        return ERR_CFW_INVALID_PARAMETER;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("1,In CFW_SsAoProc\n",0x0810053a)));
    if ((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("2,SS Send MESSAGE !\n",0x0810053b)));

        SUL_MemSet32(&ev, 0xff, SIZEOF(CFW_EV));  // hameina [mod] 2006.9.28 SUL_MemSet8->SUL_MemSet32
        ev.nParam1 = 0xff;
        pEvent     = &ev;
    }
    else
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("3,SS RECEIVED MESSAGE  ***EvtID 0x%x\n",0x0810053c)), pEvent->nEventId);

    nSimID = CFW_GetSimCardID(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("3,SS RECEIVED MESSAGE  nSimID0x%x\n",0x0810053d)), nSimID);

    proc = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);
    sOutMsg = (api_SsActionReq_t *)proc->pMsg; // hameina[+]

    nState   = CFW_GetAoState(hAo);
    nEvtId   = pEvent->nEventId;
    nEvParam = (VOID *)pEvent->nParam1;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SsAoProc, nState=0x%x, pEvent->nParam1=0x%x\n",0x0810053e)), nState, nEvParam);
    switch (nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            if (pEvent->nParam1 == 0xff)  // send from internal function
            {
#ifdef USSD_TEST
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Code	 = %d\n ",0x0810053f), sOutMsg->ss_Code);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Operation %d\n ",0x08100540), sOutMsg->ss_Operation);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x08100541),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x08100542),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDDataCodingScheme = %d\n ",0x08100543),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(".ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = %d\n ",0x08100544),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100545),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[0]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100546),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[1]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100547),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[2]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100548),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[3]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100549),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[4]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("v1.strinLength = %d\n ",0x0810054a),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5StringLength);

#else
                CFW_SendSclMessage(API_SS_ACTION_REQ, proc->pMsg, nSimID);
#endif
                // [[Qiaoguangjun[+]2007.6.5
                if (ss_USSD_V2 == proc->nApi_SS_Code || ss_USSD_V1 == proc->nApi_SS_Code)
                {
                    if (ss_ReleaseCompleteMessageType == sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("2,SS release USSD MESSAGE !\n",0x0810054b)));

                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, 0, 2, nUTI | (nSimID << 24), 0);

                        nUnregAOFlag = 1;
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Send release msg unregisterao!!,2, nUTI=%d",0x0810054c)), nUTI);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                        return ret;
                    }
                }

                // ]]Qiaoguangjun[+]2007.6.5
                CFW_SetAoState(hAo, CFW_SM_SS_STATE_CONNECT);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("5,SS====>>>send api_ss_action_req \n",0x0810054d)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                return ret;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("6,Idle State! !!! SHOULD not be here!!!!",0x0810054e)));
                nUnregAOFlag = 1;
                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
            }
            break;
        } // end of case CFW_SM_STATE_IDLE
        case CFW_SM_SS_STATE_CONNECT:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("7,In connect state\n",0x0810054f)));
            if (pEvent->nParam1 == 0xff)  // send from internal function
            {

#ifdef  USSD_TEST
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->TI = %d\n ",0x08100550), proc->nTI);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Code	 = %d\n ",0x08100551), sOutMsg->ss_Code);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Operation %d\n ",0x08100552), sOutMsg->ss_Operation);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x08100553),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("reqMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x08100554),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDDataCodingScheme = %d\n ",0x08100555),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(".ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = %d\n ",0x08100556),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100557),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[0]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100558),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[1]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x08100559),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[2]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810055a),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[3]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810055b),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[4]);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("v1.strinLength = %d\n ",0x0810055c),
                          sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5StringLength);

                // CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("v1.strin = %s\n ",0x0810055d),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5String);

#else
                CFW_SendSclMessage(API_SS_ACTION_REQ, proc->pMsg, nSimID);

#endif
                // [[Qiaoguangjun[+]2007.6.5

                if (proc->nApi_SS_Code == ss_USSD_V2 || proc->nApi_SS_Code == ss_USSD_V1)
                {
                    if (ss_ReleaseCompleteMessageType == sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, 0, proc->nApi_SS_Code, nUTI | (nSimID << 24), 0);

                        nUnregAOFlag = 1;
                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Send release msg unregisterao!!,111, nUTI=%d",0x0810055e)), nUTI);

                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                        return ret;
                    }
                }

                // ]]Qiaoguangjun[+]2007.6.5
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("8,SS send api_ss_action_req In Connect State\n",0x0810055f)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                return ret;
            }
            switch (nEvtId)
            {
                case API_SS_TI_IND:
                {
                    api_SsTIInd_t *p = (api_SsTIInd_t *)nEvParam;
                    proc->nTI = p->TI;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("9,In API_SS_TI_IND===> TI is: %d\n",0x08100560)), p->TI);
                    CFW_SetUTI(hAo, p->TI, 2);
                    break;
                }
                case API_SS_ACTION_CNF:
                {
                    api_SsActionCnf_t *pInMsg = (api_SsActionCnf_t *)nEvParam;

                    // [[Qiaoguangjun[-] 2007.6.6 for USSD
                    // proc->nTI=pInMsg->TI;
                    // ]]Qiaoguangjun[-] 2007.6.6 for USSD
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("10,In API_SS_ACTION_CNF\n",0x08100561)));

                    // ===========================Test begin==================================//
                    // Only for test the clir at 20070406
                    // pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence = ss_NoRetRsltInfoPresent;
                    // ============================Test end=================================//
                    switch (pInMsg->ss_ReturnResult.ss_RetRsltInfoPresence)
                    {
                        case ss_InterrogateResultsPresent:
                            // [[Qiaoguangjun[+] 2007.6.6 for USSD
                            proc->nTI = pInMsg->TI;

                            // ]]Qiaoguangjun[+] 2007.6.6 for USSD
                            switch (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltInfoPresent)
                            {
                                case ss_BasicServiceGroupListPresent:  // inter cw , cb(active)
                                    ss_Inter_BSGroupList_Conf(hAo, pInMsg, proc);
                                    nUnregAOFlag = 1;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("11,unregisterAO!\n",0x08100562)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                    break;
                                case ss_GenericServiceInfoPresent: // Query CLir(provided)
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("12,Action_Inter_GenericInfo\n",0x08100563));
                                    ss_Inter_GSInfo_Conf(hAo, pInMsg, proc);  // 4 here
                                    nUnregAOFlag = 1;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("13,unregisterAO!\n",0x08100564)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                    break;
                                case ss_ForwardingFeatureListPresent:  // inter_CF_active
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("14,ActionCnf_Query_CF(active)!\n",0x08100565));
                                    pForwardingInd = (CFW_SS_QUERY_CALLFORWARDING_RSP *)CSW_SS_MALLOC(SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP));
                                    if (pForwardingInd == NULL)
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x08100566)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CALL_FORWARDING_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        nUnregAOFlag = 1;
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("16,unregisterAO!\n",0x08100567)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;
                                    }
                                    SUL_ZeroMemory32(pForwardingInd, SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP));
                                    ss_InterCFList_Conf(hAo, pInMsg, pForwardingInd, proc);
                                    nUnregAOFlag = 1;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("16,unregisterAO!\n",0x08100568)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                    break;
                                case ss_StatusInfoPresent:

                                    // clip , clir ,  cw, cb, CF (not active) Q P R A
                                    ss_Inter_StatusInfo_Conf(hAo, pInMsg, proc);
                                    nUnregAOFlag = 1;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("17,unregisterAO!\n",0x08100569)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo); // item1
                                    break;
                                case ss_NoIntRsltChoicesPresent:
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("18,should not be here 1!\n",0x0810056a)));
#ifdef SS_NO_RETURNRESULT_HANDLE
                                    ss_Api_Cnf_No_Retresult(hAo, pInMsg, proc);
#endif
                                    nUnregAOFlag = 1;
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                    return ret;
                            }
                            break;
                        case ss_ForwardingInfoPresent: // end of inter result,forwarding operation(exp inter)start

                            // [[Qiaoguangjun[+] 2007.6.6 for USSD  ti compare
                            proc->nTI = pInMsg->TI;

                            // ]]Qiaoguangjun[+] 2007.6.6 for USSD   ti compare

                            // set CF
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("19,ActionCnf_Set_CF! api_operation=%x\n",0x0810056b), pInMsg->ss_Operation);
                            pSetCFInd =
                                (CFW_SS_SET_CALLFORWARDING_RSP_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO) + 24);
                            if (pSetCFInd == NULL)
                            {
                                UINT32 param1 = 0;

                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x0810056c)));
                                param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                CFW_PostNotifyEvent(EV_CFW_SS_SET_CALL_FORWARDING_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                nUnregAOFlag = 1;
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("16,unregisterAO!\n",0x0810056d)));
                                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                return ERR_NO_MORE_MEMORY;
                            }

                            /*
                            {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("20,Malloc Mem ERR\n",0x0810056e)));
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                            return ERR_NO_MORE_MEMORY;
                            }
                            */
                            SUL_ZeroMemory8(pSetCFInd, SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO) + 24);
                            pSetCFInd->nNumber.pDialNumber = (UINT8 *)((UINT8 *)pSetCFInd + SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO));
                            ss_SetCF_Conf(hAo, pInMsg, pSetCFInd, proc);
                            nUnregAOFlag = 1;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("21,unregisterAO!\n",0x0810056f)));
                            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                            break;
                        case ss_CallBarringInfoPresent:  // start of call barring option(exp inter)
                            // [[Qiaoguangjun[+] 2007.6.6 for USSD  ti compare
                            proc->nTI = pInMsg->TI;

                            // ]]Qiaoguangjun[+] 2007.6.6 for USSD   ti compare
                            ss_SetCBInfo_Conf(hAo, pInMsg, proc);
                            nUnregAOFlag = 1;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("22,unregisterAO!\n",0x08100570)));
                            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                            break;
                        case ss_SSDataPresent:

                            // set cw
                        {
                            UINT32 param1 = 0;

                            // [[Qiaoguangjun[+] 2007.6.6 for USSD  ti compare
                            proc->nTI = pInMsg->TI;

                            // ]]Qiaoguangjun[+] 2007.6.6 for USSD   ti compare

                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("23,uti=%d\n",0x08100571), nUTI);
                            if (pInMsg->ss_Operation == ss_Activate)
                                param1 = 1;
                            else if (pInMsg->ss_Operation == ss_Deactivate)
                                param1 = 0;

                            // CFW_SetAoState(hAo,CFW_SM_STATE_IDLE);

                            CFW_PostNotifyEvent(EV_CFW_SS_SET_CALL_WAITING_RSP, param1, 0, nUTI | (nSimID << 24), 0);

                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,ActionCnf_Set_CW: operation=%x\n",0x08100572), pInMsg->ss_Operation);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("25,unregisterAO!\n",0x08100573)));
                            nUnregAOFlag = 1;
                            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                        }
                        break;
#ifdef USSD_ENDABLE
                        case ss_USSDV2ResultsPresent:
                            // [[Qiaoguangjun[+]2007.6.5
                        {
                            // is the msg belog to this AO?
                            if (proc->nTI != pInMsg->TI)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Warnning: it maybe a bug proc->nTI %d, msg->TI %d\n",0x08100574), proc->nTI, pInMsg->TI);
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                return ret;
                            }

                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD, pInMsg->ss_Operation=%d\n",0x08100575), pInMsg->ss_Operation);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD, ss_USSDNetworkComponentType=%d\n",0x08100576),
                                      pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD, ss_RxMessageType=%d\n",0x08100577), pInMsg->ss_ReturnResult.ss_RxMessageType);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD, ss_USSDDataCodingScheme=%d\n",0x08100578),
                                      pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.
                                      ss_USSDDataCodingScheme);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD, ss_USSDStringLength=%d\n",0x08100579),
                                      pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength);
                            CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString, pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength, 16);

                            switch (pInMsg->ss_Operation)
                            {

                                // ss_ProcessUnstructuredSSRequest indicate this USSD Operation is initated by MS
                                // it contain the return result from NW, facility or release.
                                case ss_ProcessUnstructuredSSRequest:
                                {

                                    UINT32 nNum   = 0;
                                    UINT32 param2 = 0;

                                    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                    param2 = pInMsg->ss_Code;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  CNF ss_ProcessUnstructuredSSRequest\n",0x0810057a)));
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,CFW_SsAoProc nUTI=%d\n",0x0810057b), nUTI);

                                    proc->nApi_SS_Operation = pInMsg->ss_Operation;
                                    nNum                    = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                    pUSSD_info_temp         = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    if (NULL == pUSSD_info_temp)
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x0810057c)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;
                                    }

                                    /*
                                       {
                                       CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x0810057d)));
                                       CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                       return ERR_NO_MORE_MEMORY;
                                       }
                                     */

                                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                    pUSSD_info_temp->nDcs =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                    pUSSD_info_temp->nStingSize =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                    // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);
                                    SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                                 pUSSD_info_temp->nStingSize);

                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("USSD ss_RxMessageType=0x%x \n",0x0810057e)),
                                              pInMsg->ss_ReturnResult.ss_RxMessageType);
                                    if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        nUnregAOFlag = 1;
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD CFW_UnRegisterAO \n",0x0810057f)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;

                                    }
                                    else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 1;

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        // 需要确认，是否要consumed?
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;
                                    }

                                }
                                break;

                                // the follow case indicate the Operation is initiated by NW(not indicate the transation is established by NW),
                                // it maybe invoke,or release component, it should not be return result from NW
                                case ss_UnstructuredSSNotify:

                                    CFW_SendFacilityMsg(nSimID, pInMsg->TI);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  CNF CFW_SendFacilityMsg \n",0x08100580)));
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  CNF ss_UnstructuredSSNotify \n",0x08100581)));
                                case ss_UnstructuredSSRequest:
                                {
                                    UINT32 nNum   = 0;
                                    UINT32 param2 = 0;

                                    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                    param2                  = pInMsg->ss_Code;
                                    proc->nApi_SS_Operation = pInMsg->ss_Operation;

                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  CNF  ss_UnstructuredSSRequest\n",0x08100582)));
                                    nNum            = pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;
                                    pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    if (NULL == pUSSD_info_temp)
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x08100583)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;

                                    }

                                    /*
                                       {
                                       CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100584)));
                                       CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                       return ERR_NO_MORE_MEMORY;
                                       }
                                     */

                                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                    pUSSD_info_temp->nDcs =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                    pUSSD_info_temp->nStingSize =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDStringLength;

                                    // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,pUSSD_info_temp->nStingSize, pUSSD_info_temp->pUsdString);

                                    SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDString,
                                                 pUSSD_info_temp->nStingSize);
                                    if (ss_USSDNetworkInvoke == pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                    {

                                        if (ss_FacilityMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                        {
                                            if(pInMsg->ss_Operation == ss_UnstructuredSSRequest)
                                                pUSSD_info_temp->nOption = 1;
                                            else
                                                pUSSD_info_temp->nOption = 0;
                                            CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2,
                                                                nUTI | (nSimID << 24), 0);

                                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                            return ret;
                                        }
                                        else if (ss_RegisterMessageType == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                        {
                                            // maybe something wrong, it shoud not be here
                                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD, debug needed! it's register msg!\n",0x08100585)));

                                            // 需要确认，是否要not_consumed?
                                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

                                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                            return ret;
                                        }
                                    }
                                    else if (ss_USSDNetworkNoComponentPresent ==
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                    {

                                        // according to api1.05 18.8.5.2, and 24090. Figure 6.2 . this msg should be used for NW initeated transation 's release
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0),
                                                  TSTXT
                                                  (TSTR("USSD Warnning, release compleate! component ss_USSDNoComponentPresent!should be MT!\n",0x081013d1)));

                                        pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        nUnregAOFlag = 1;
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD CFW_UnRegisterAO \n",0x08100586)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;
                                    }
                                    else if (ss_USSDNetworkReturnResult ==
                                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                                    {
                                        if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                        {

                                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  release comleate !component ss_USSDReturnResult \n",0x08100587)));

                                            pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                            CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2,
                                                                nUTI | (nSimID << 24), 0);

                                            nUnregAOFlag = 1;
                                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD CFW_UnRegisterAO \n",0x08100588)));
                                            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                            return ret;

                                        }
                                        else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                        {

                                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  msg type API_SS_FACILITY_MESSAGE_TYPE\n",0x08100589)));
                                            pUSSD_info_temp->nOption = 1;

                                            CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2,
                                                                nUTI | (nSimID << 24), 0);

                                            // 需要确认，是否要consumed?
                                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                            return ret;
                                        }
                                    }
                                    else
                                    {
                                        // need debug
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD err, debug needed!unknow msg componet!\n",0x0810058a)));

                                        // 需要确认，是否要not_consumed?
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;
                                    }
                                }
                                break;

                                default:
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("err, debug needed! this is MO proc!\n",0x0810058b)));

                                    // 需要确认，是否要not_consumed?
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                    return ret;
                                    break;
                            }

                        }
                            // ]]Qiaoguangjun[+]2007.6.5
                        break;
                        case ss_USSDV1ResultsPresent:
                            // [[Qiaoguangjun[+]2007.6.5
                        {

                            // according to wangfei(stack), there is no NW register in v1.
                            // so component may be : (NW) release, return result, invoke(not register but facility)

                            if (proc->nTI != pInMsg->TI)
                            {
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                return ret;
                            }

                            switch (pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkComponentType)
                            {

                                case ss_USSDNetworkNoComponentPresent: // nw release
                                {
                                    UINT32 nNum   = 0;
                                    UINT32 param2 = 0;

                                    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                    param2 = pInMsg->ss_Code;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x0810058c), nUTI);
                                    nNum =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                                    pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    if (pUSSD_info_temp == NULL)
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x0810058d)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;
                                    }

                                    /*
                                               {
                                               CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x0810058e)));
                                               CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                               return ERR_NO_MORE_MEMORY;
                                               }
                                             */
                                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO));
                                    pUSSD_info_temp->nOption = 2;

                                    // pUSSD_info_temp->pUsdString=NULL;
                                    pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                    pUSSD_info_temp->nStingSize = nNum;
                                    SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.
                                                 ss_IA5String, pUSSD_info_temp->nStingSize);

                                    CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                        0);

                                    nUnregAOFlag = 1;
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD CFW_UnRegisterAO V1 \n",0x0810058f)));
                                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                    return ret;

                                }
                                break;

                                case ss_USSDNetworkReturnResult: // NW return result . facility or release
                                {

                                    UINT32 nNum   = 0;
                                    UINT32 param2 = 0;

                                    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                    param2 = pInMsg->ss_Code;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x08100590), nUTI);

                                    nNum =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                                    pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    if (NULL == pUSSD_info_temp)

                                        /*
                                                   {
                                                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100591)));
                                                   CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                                   return ERR_NO_MORE_MEMORY;
                                                   }
                                                 */
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x08100592)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;
                                    }

                                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                    // pUSSD_info_temp->nDcs=pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                    pUSSD_info_temp->nStingSize = nNum;

                                    // if(nNum>0)
                                    // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5String,nNum, pUSSD_info_temp->pUsdString);

                                    SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.
                                                 ss_IA5String, pUSSD_info_temp->nStingSize);

                                    if (API_SS_RELEASE_COMPLETE_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {

                                        pUSSD_info_temp->nOption = 2; // USSD terminated by network

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        nUnregAOFlag = 1;
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD CFW_UnRegisterAO V1 \n",0x08100593)));
                                        CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;

                                    }
                                    else if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 1;

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;
                                    }
                                }
                                break;
                                case ss_USSDNetworkInvoke:
                                {
                                    UINT32 nNum   = 0;
                                    UINT32 param2 = 0;

                                    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
                                    param2 = pInMsg->ss_Code;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("24,uti=%d\n",0x08100594), nUTI);

                                    nNum =
                                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5StringLength;
                                    pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    if (NULL == pUSSD_info_temp)

                                        /*
                                           {
                                           CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x08100595)));
                                           CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                           return ERR_NO_MORE_MEMORY;
                                           }
                                         */
                                    {
                                        UINT32 param1 = 0;

                                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("15,Malloc Mem ERR\n",0x08100596)));
                                        param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, param1, 14, nUTI | (nSimID << 24), 0xFA);

                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ERR_NO_MORE_MEMORY;
                                    }

                                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO) + nNum);
                                    pUSSD_info_temp->pUsdString = (UINT8 *)((UINT8 *)pUSSD_info_temp + SIZEOF(CFW_SS_USSD_IND_INFO));

                                    // pUSSD_info_temp->nDcs=pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDRes.ss_USSDDataCodingScheme;

                                    pUSSD_info_temp->nStingSize = nNum;

                                    // if(nNum>0)
                                    // SUL_UCS2ToLocallanguage(pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.ss_IA5String,nNum, pUSSD_info_temp->pUsdString);

                                    SUL_MemCopy8(pUSSD_info_temp->pUsdString,
                                                 pInMsg->ss_ReturnResult.ss_RetRslt.ss_USSD.ss_USSDNetworkResult.ss_USSDUserData.
                                                 ss_IA5String, pUSSD_info_temp->nStingSize);

                                    if (API_SS_FACILITY_MESSAGE_TYPE == pInMsg->ss_ReturnResult.ss_RxMessageType)
                                    {
                                        pUSSD_info_temp->nOption = 1;

                                        CFW_PostNotifyEvent(EV_CFW_SS_SEND_USSD_RSP, (UINT32)pUSSD_info_temp, param2, nUTI | (nSimID << 24),
                                                            0);

                                        // 需要确认，是否consumed?
                                        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                        return ret;
                                    }
                                }
                                break;

                                default:

                                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("DEBUG needed, v1 msg,should not be here!\n",0x08100597)));

                                    // 需要确认，是否not_consumed?
                                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                                    return ret;
                                    break;
                            }
                        }

                            // ]]Qiaoguangjun[+]2007.6.5
                        break;
#else

                        case ss_USSDV1ResultsPresent:
                        case ss_USSDV2ResultsPresent:
                            break;

#endif
                        case ss_NoRetRsltInfoPresent:

                            // 1 因为没有返回结果的事件不是现有任何分类的一种,所以需要增加一个事件
                            // EV_CFW_SS_NO_RESULT_RSP

                            // 2 参数param1 =0xff, param2=0xff
                            // 3 需要修改CSW文档，增加这个事件

                            // CFW_PostNotifyEvent (EV_CFW_SS_NO_RESULT_RSP, 0xff, 0xff, nUTI, 0);
                            // Because of customers need to change their codes of MMI when we use new event ->EV_CFW_SS_NO_RESULT_RSP, we can
                            // handle this status ->ss_NoRetRsltInfoPresent like as a error.
                            // ss_Api_Err_Ind(hAo, pInMsg, proc);

                            // We ,Both CSW and STACK,are not sure about this case, In the real network, it is successful case.
                            // So we take  operations that need no data back as successful, and that need data back as fail.
                            // And it may be not precision
#ifdef SS_NO_RETURNRESULT_HANDLE
                            ss_Api_Cnf_No_Retresult(hAo, pInMsg, proc);
#endif

                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("26,==========should not be here 3!=============\n",0x08100598)));
                            nUnregAOFlag = 1;
                            CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                            return ret;
                    }

                    // UnRegister AO
                    switch (pInMsg->ss_ReturnResult.ss_RxMessageType)
                    {
                        case ss_ReleaseCompleteMessageType:
                        case ss_InvalidSSMessageType:
                            if (nUnregAOFlag == 0)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("27,warning here!!!!!ActionCnf UnRegister AO!\n",0x08100599)));
                                nUnregAOFlag = 1;
                                CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                            }
                            break;
                        default:
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                            return ret;
                    }
                    break;
                }
                case API_SS_ERROR_IND:
                {
                    api_SsErrorInd_t *pInMsg = (api_SsErrorInd_t *)nEvParam;

                    // [[Qiaoguangjun [mod] 2007.6.6 for ussd compare ti
                    // proc->nTI=pInMsg->TI; //???? error ti what to do

                    if (pInMsg->ss_Code == ss_USSD_V2 || pInMsg->ss_Code == ss_USSD_V1)
                    {
                        // add Qiaogj
                        // to avoid unregister others AO when err, for there are two AO(MO/MT)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD err msg pInMsg->ss_Code %d\n",0x0810059a), pInMsg->ss_Code);
                        if (proc->nTI != pInMsg->TI)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("err ind proc->nTI %d , pInMsg->TI %d!\n",0x0810059b)), proc->nTI, pInMsg->TI);
                            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                            return ret;

                        }
                    }

                    // ]]Qiaoguangjun [mod] 2007.6.6 for ussd compare ti

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("28,In API_SS_ERROR_IND!\n",0x0810059c)));
                    ss_Api_Err_Ind(hAo, pInMsg, proc);

                    // CFW_SetAoState(hAo,CFW_SM_STATE_IDLE);
                    nUnregAOFlag = 1;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("29,Unregistered AO\n",0x0810059d));
                    CFW_UnRegisterAO(CFW_SS_SRV_ID, hAo);
                    break;
                }
                default:
                {
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("31,should not be here 4!\n",0x0810059e)));
                    nUnregAOFlag = 1;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("32,unregisterAO!\n",0x0810059f)));
                    CFW_UnRegisterAO(CFW_SM_STATE_IDLE, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
                    return ret;
                }
            }
            break;
        } // end of case CFW_SM_SS_STATE_CONNECT
        case CFW_SM_SS_STATE_SIM:
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("33,should not be sim state!\n",0x081005a0)));
            nUnregAOFlag = 1;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("34,unregisterAO!\n",0x081005a1)));
            CFW_UnRegisterAO(CFW_SM_STATE_IDLE, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
            return ret;
        }
        default:
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("35,should not be other state!\n",0x081005a2)));
            nUnregAOFlag = 1;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("36,unregisterAO!\n",0x081005a3)));
            CFW_UnRegisterAO(CFW_SM_STATE_IDLE, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
            return ret;
        }
    }

    // the event are processed one time, and don't need to process again in this state.
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("37,nunregAOFlag=%x\n",0x081005a4)), nUnregAOFlag);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsAoProc);
    return (ret);
}

UINT32 CFW_SsQueryClip(UINT16 nUTI
                       , CFW_SIM_ID nSimID
                      )
{
    HAO hSm;
    UINT32 ret = ERR_SUCCESS;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryClip);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In CFW_SSQueryClip\n",0x081005a5));

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SSQueryClip,nSimID=%d \n",0x081005a6), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SSQueryClip,check nSimID failed\n",0x081005a7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClip);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SSQueryClip,get AO COUNT !=0\n",0x081005a8));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClip);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SSQueryClip Malloc Mem ERR\n",0x081005a9)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClip);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg               = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    sOutMsg->ss_Code      = ss_CLIP;
    sOutMsg->ss_Operation = ss_Interrogate;
    SUL_ZeroMemory32(&(sOutMsg->ss_Params), SIZEOF(api_SsActionReqParams_t));

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In CFW_SSQueryClip, END\n",0x081005aa));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClip);
    return ret;

}

UINT32 CFW_SsQueryClir(UINT16 nUTI
                       , CFW_SIM_ID nSimID
                      )
{
    HAO hSm;

    SS_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;

    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryClir);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In CFW_SSQueryClir!\n",0x081005ab));

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SSQueryClir,nSimID=%d \n",0x081005ac), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SSQueryClir, CHECK nSimID FAILED\n",0x081005ad));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClir);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SSQueryClir, AO COUNT!=0\n",0x081005ae));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClir);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SSQueryClir,Malloc Mem ERR\n",0x081005af)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClir);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg               = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    sOutMsg->ss_Code      = ss_CLIR;
    sOutMsg->ss_Operation = ss_Interrogate;
    SUL_ZeroMemory32(&(sOutMsg->ss_Params), SIZEOF(api_SsActionReqParams_t));

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);  // to add  here  every function
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SSQueryClir,END\n",0x081005b0)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryClir);
    return ret;

}

UINT32 CFW_SsQueryColp(UINT16 nUTI
                       , CFW_SIM_ID nSimID
                      )
{
    HAO hSm;

    SS_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;

    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryColp);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In CFW_SSQueryColp!\n",0x081005b1));

    UINT8 nAOCount = 0;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColp);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColp);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x081005b2)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColp);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg               = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    sOutMsg->ss_Code      = ss_COLP;
    sOutMsg->ss_Operation = ss_Interrogate;
    SUL_ZeroMemory32(&(sOutMsg->ss_Params), SIZEOF(api_SsActionReqParams_t));

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColp);
    return ret;
}

UINT32 CFW_SsQueryColr(UINT16 nUTI
                       , CFW_SIM_ID nSimID
                      )
{
    HAO hSm;
    UINT32 ret = ERR_SUCCESS;

    api_SsActionReq_t *sOutMsg = NULL;
    SS_SM_INFO *proc = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryColr);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_CmQueryColr\n",0x081005b3)));

    UINT8 nAOCount = 0;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColr);
        return ret;
    }

    // check if other SIM operate on SS module

    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColr);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x081005b4)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColr);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg               = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    sOutMsg->ss_Code      = ss_COLR;
    sOutMsg->ss_Operation = ss_Interrogate;
    SUL_ZeroMemory32(&(sOutMsg->ss_Params), SIZEOF(api_SsActionReqParams_t));

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryColr);
    return ret;
}

UINT32 CFW_SsQueryCallWaiting(UINT8 nClass, UINT16 nUTI
                              , CFW_SIM_ID nSimID
                             )
{
    HAO hSm;
    UINT32 ret = ERR_SUCCESS;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("In CFW_CmQueryCallWaiting! nclass=%d\n",0x081005b5), nClass);

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("In CFW_CmQueryCallWaiting! nSimID=%d\n",0x081005b6), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]In CFW_CmQueryCallWaiting! check SIM ID  failed\n",0x081005b7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallWaiting);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]In CFW_CmQueryCallWaiting! Other SIM busy, call this API later\n",0x081005b8));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallWaiting);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);

    if(0 != nClass) //add by wulc  for gcf-ss test
    {
        nCategory = GetBSCategory(nClass);
        nProcCode = GetBSProtocolCode(nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]In CFW_CmQueryCallWaiting! invalid parameter nClass\n",0x081005b9));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallWaiting);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        nClass = 11;

    }
#if 0 // remove by wulc ,for ss string *#43# when in calling

    // Frank add for adjust CC status start.
    if (CFW_GetAoCount(CFW_CC_SRV_ID, nSimID) > 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]the CC is working now!\n",0x081005ba));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
#endif
    // Frank add for adjust CC status end.
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallWaiting!Malloc return NULL\n",0x081005bb)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallWaiting);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg                                                        = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    sOutMsg->ss_Code                                               = ss_CW;
    sOutMsg->ss_Operation                                          = ss_Interrogate;
    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCode         = nProcCode;
    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCodeCategory = nCategory;

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nClass            = nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_CmQueryCallWaiting!end\n",0x081005bc)));
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallWaiting);
    return ret;
}

// hameina [Modify] the interface, the original one has no nUTI. 2006.10.23
UINT32 CFW_SsSetCallWaiting(UINT8 nMode, UINT8 nClass, UINT16 nUTI
                            , CFW_SIM_ID nSimID
                           )
{
    HAO hsm;
    UINT16 ret      = ERR_SUCCESS;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsSetCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("In CFW_SsSetCallWaiting!begin, nMode=%d, nClass=0x%x\n",0x081005bd)), nMode, nClass);

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsSetCallWaiting!nSimID=%d\n",0x081005be)), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSetCallWaiting!nSimID check failed\n",0x081005bf)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSetCallWaiting!other SIM busy, call this API later\n",0x081005c0)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    if (nMode != 0 && nMode != 1)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSetCallWaiting!invalid parameter MODE\n",0x081005c1)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(0 != nClass) //add by wulc  for gcf-ss test
    {
        nCategory = GetBSCategory(nClass);
        nProcCode = GetBSProtocolCode(nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSetCallWaiting!invalid parameter class\n",0x081005c2)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        nClass = 11;

    }
    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSetCallWaiting!malloc return NULL\n",0x081005c3)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

    sOutMsg->ss_Code = ss_CW;

    if (nMode == 1)
        sOutMsg->ss_Operation = ss_Activate;
    else if (nMode == 0)
        sOutMsg->ss_Operation = ss_Deactivate;

    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCode         = nProcCode;
    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCodeCategory = nCategory;

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nClass            = nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hsm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hsm, nUTI, 0); // hameina[+] 2006.10.23
    CFW_SetAoProcCode(hsm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsSetCallWaiting!END\n",0x081005c4)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallWaiting);
    return ret;

}

UINT32 CFW_SsQueryCallForwarding(UINT8 nReason, UINT8 nClass, UINT16 nUTI
                                 , CFW_SIM_ID nSimID
                                )
{
    HAO hSm;
    UINT32 ret      = ERR_SUCCESS;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryCallForwarding);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("In CFW_CmQueryCallForwarding, nReason=%d, nClass=%d\n",0x081005c5)), nReason, nClass);

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_CmQueryCallForwarding,nSimID=%d\n",0x081005c6)), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallForwarding, check nSimID failed\n",0x081005c7)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallForwarding, other sim busy, call this API later\n",0x081005c8)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


#if 0// remove by wulc ,for ss string *#43# when in calling
    // Frank: we have modified CC and SS to mutex for bug 16383 start.
    if (CFW_GetAoCount(CFW_CC_SRV_ID, nSimID) > 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]the CC is working now!\n",0x081005c9));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
#endif
    // Frank: we have modified CC and SS to mutex for bug 16383 end.
    if(0 != nClass) //add by wulc  for gcf-ss test
    {
        nCategory = GetBSCategory(nClass);
        nProcCode = GetBSProtocolCode(nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallForwarding, invalid parameter nClass\n",0x081005ca)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        nClass = 11;
    }
    if (nReason > 5)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallForwarding, invalid parameter nReason\n",0x081005cb)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryCallForwarding, malloc return NULL\n",0x081005cc)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

    switch (nReason)
    {
        case CFW_SS_FORWARDING_REASON_UNCONDITIONAL:
            sOutMsg->ss_Code = ss_CFU;
            break;
        case CFW_SS_FORWARDING_REASON_MOBILEBUSY:
            sOutMsg->ss_Code = ss_CFB;
            break;
        case CFW_SS_FORWARDING_REASON_NOREPLY:
            sOutMsg->ss_Code = ss_CFNRY;
            break;
        case CFW_SS_FORWARDING_REASON_NOTREACHABLE:
            sOutMsg->ss_Code = ss_CFNRC;
            break;
        case CFW_SS_FORWARDING_REASON_ALL_CALLFORWARDING:
            sOutMsg->ss_Code = ss_CFA;
            break;
        case CFW_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING:
            sOutMsg->ss_Code = ss_CFC;
            break;
    }
    sOutMsg->ss_Operation                                          = ss_Interrogate;
    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCode         = nProcCode;
    sOutMsg->ss_Params.ss_CallWaitingParams.ss_ServiceCodeCategory = nCategory;

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nClass            = nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_CmQueryCallForwarding, end\n",0x081005cd)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryCallForwarding);
    return ret;
}

UINT32 CFW_SsSetCallForwarding(CFW_SS_SET_CALLFORWARDING_INFO *pCallForwarding, UINT16 nUTI
                               , CFW_SIM_ID nSimID
                              )
{
    BOOL bPresent = FALSE;
    HAO hSm;
    UINT32 ret      = ERR_SUCCESS;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    api_SsActionReq_t *sOutMsg = NULL;
    SS_SM_INFO *proc = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsSetCallForwarding);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("In CFW_CmSetCallForwarding nMode=%x,nClass=%x\n",0x081005ce), pCallForwarding->nMode,
              pCallForwarding->nClass);
    if (pCallForwarding == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_INVALID_PARAMETER;
    }

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsSetCallForwarding,nSimID=%d\n",0x081005cf)), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSetCallForwarding,check nSimID failed\n",0x081005d0)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSetCallForwarding, Other SIM busy, call this API later\n",0x081005d1)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    if (pCallForwarding == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSetCallForwarding, first parameter is NULL\n",0x081005d2)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (pCallForwarding->nNumber.nDialNumberSize > 41)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_CME_DIAL_STRING_TOO_LONG;
    }

    if (pCallForwarding->nReason > 5)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_INVALID_PARAMETER;
    }

    if (pCallForwarding->nMode == 3 && pCallForwarding->nNumber.pDialNumber == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_INVALID_PARAMETER;
    }
    if (pCallForwarding->nMode > 4)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_INVALID_PARAMETER;
    }
    if(0 != pCallForwarding->nClass) //add by wulc  for gcf-ss test
    {
        nCategory = GetBSCategory(pCallForwarding->nClass);
        nProcCode = GetBSProtocolCode(pCallForwarding->nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSetCallForwarding, invalid parameter nClass\n",0x081005d3)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        pCallForwarding->nClass = 11;

    }
#if 0
    if (proc->nApi_SS_Operation == 3)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("===============设置===========\n",0x081005d4));
    }
    if (proc->nApi_SS_Operation == 4)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("===============取消设置===========\n",0x081005d5));
    }
#endif

    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSetCallForwarding, malloc return NULL\n",0x081005d6)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

    switch (pCallForwarding->nReason)
    {
        case CFW_SS_FORWARDING_REASON_UNCONDITIONAL:
            sOutMsg->ss_Code = ss_CFU;
            break;
        case CFW_SS_FORWARDING_REASON_MOBILEBUSY:
            sOutMsg->ss_Code = ss_CFB;
            break;
        case CFW_SS_FORWARDING_REASON_NOREPLY:
            sOutMsg->ss_Code = ss_CFNRY;
            break;
        case CFW_SS_FORWARDING_REASON_NOTREACHABLE:
            sOutMsg->ss_Code = ss_CFNRC;
            break;
        case CFW_SS_FORWARDING_REASON_ALL_CALLFORWARDING:
            sOutMsg->ss_Code = ss_CFA;
            break;
        case CFW_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING:
            sOutMsg->ss_Code = ss_CFC;
            break;
    }

    switch (pCallForwarding->nMode)
    {
        case 0:
            sOutMsg->ss_Operation = ss_Deactivate;
            break;
        case 1:
            sOutMsg->ss_Operation = ss_Activate;
            break;
        case 3:
            sOutMsg->ss_Operation = ss_Register;
            break;
        case 4:
            sOutMsg->ss_Operation = ss_Erasure;
            break;
    }

    if((pCallForwarding->nNumber.nDialNumberSize <= 0)&&(3 == pCallForwarding->nMode))
    {
        sOutMsg->ss_Operation = ss_Activate;
    }

    sOutMsg->ss_Params.ss_CallForwardingParams.ss_ServiceCode         = nProcCode;
    sOutMsg->ss_Params.ss_CallForwardingParams.ss_ServiceCodeCategory = nCategory;

    if (pCallForwarding->nMode == 3)
    {
        if (pCallForwarding->nNumber.nDialNumberSize > 0)
        {
            SUL_MemCopy8(&sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToNumber[1],
                         pCallForwarding->nNumber.pDialNumber, pCallForwarding->nNumber.nDialNumberSize);
        }

        sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToNumber[0] = pCallForwarding->nNumber.nType;
        sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToNumberLen = 1 + pCallForwarding->nNumber.nDialNumberSize;

    }

    sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToNumber[0] = pCallForwarding->nNumber.nType;
    sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToNumberLen = 1 + pCallForwarding->nNumber.nDialNumberSize;

    // subaddress
    sOutMsg->ss_Params.ss_CallForwardingParams.forwardedToSubaddressLen = 0;

    // No Reply Condition Time
    if (pCallForwarding->nReason == CFW_SS_FORWARDING_REASON_NOREPLY)

    {
        bPresent = sOutMsg->ss_Params.ss_CallForwardingParams.bNoReplyConditionTimePresent = TRUE;
    }
    else
        bPresent = sOutMsg->ss_Params.ss_CallForwardingParams.bNoReplyConditionTimePresent = FALSE; // ??????

    if (bPresent != FALSE)
    {
        sOutMsg->ss_Params.ss_CallForwardingParams.noReplyConditionTime = pCallForwarding->nTime;
    }
    proc->nTI  = 0x80;
    proc->pMsg = (VOID *)sOutMsg;

    proc->nClass            = pCallForwarding->nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_SsSetCallForwarding, END\n",0x081005d7)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetCallForwarding);
    return ret;
}

UINT32 CFW_SsSetFacilityLock(UINT16 nFac, UINT8 *pBufPwd, UINT8 nPwdSize, UINT8 nClass, UINT8 nMode, UINT16 nUTI
                             , CFW_SIM_ID nSimID
                            )
{
    HAO hSm;
    UINT32 ret      = ERR_SUCCESS;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    api_SsActionReq_t *sOutMsg = NULL;
    SS_SM_INFO *proc = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsSetFacilityLock);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("In CFW_CmSetFacilityLock operation=0X%x, bs=0X%x\n",0x081005d8)), nMode, nClass);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("In CFW_CmSetFacilityLock nFac=0X%x, nPwdSize=0X%x\n",0x081005d9)), nFac, nPwdSize);
    CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pBufPwd, nPwdSize, 16);
    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_CmSetFacilityLock, nSimID=%d\n",0x081005da)), nSimID);

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, check nSimID failed\n",0x081005db)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, Other SIM busy, call this API later\n",0x081005dc)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    if (nFac != CFW_STY_FAC_TYPE_AO && nFac != CFW_STY_FAC_TYPE_OI && nFac != CFW_STY_FAC_TYPE_OX
            && nFac != CFW_STY_FAC_TYPE_AI && nFac != CFW_STY_FAC_TYPE_AB && nFac != CFW_STY_FAC_TYPE_AG
            && nFac != CFW_STY_FAC_TYPE_AC && nFac != CFW_STY_FAC_TYPE_IR)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, invalid parameter nFac\n",0x081005dd)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (nMode != 0 && nMode != 1)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, invalid parameter nMode\n",0x081005de)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((0 != nClass) && (1 != nClass)) //Modify for android bug 23894
    {
        nCategory = GetBSCategory(nClass);
        nProcCode = GetBSProtocolCode(nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, invalid parameter nClass\n",0x081005df)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        nClass = 11;

    }
    if ((nPwdSize > 4) || ((nPwdSize) && (nPwdSize < 5) && (pBufPwd == NULL)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, invalid parameter nPwdSize and pBufPwd\n",0x081005e0)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmSetFacilityLock, malloc return NULL\n",0x081005e1)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
    switch (nFac)
    {
        case CFW_STY_FAC_TYPE_AO:
            sOutMsg->ss_Code = ss_BAOC;
            break;
        case CFW_STY_FAC_TYPE_OI:
            sOutMsg->ss_Code = ss_BOIC;
            break;
        case CFW_STY_FAC_TYPE_OX:
            sOutMsg->ss_Code = ss_BOIC_ExHC;
            break;
        case CFW_STY_FAC_TYPE_AI:
            sOutMsg->ss_Code = ss_BAIC;
            break;
        case CFW_STY_FAC_TYPE_IR:
            sOutMsg->ss_Code = ss_BIC_ROAM;
            break;
        case CFW_STY_FAC_TYPE_AB:
            sOutMsg->ss_Code = ss_AllBarringSS;
            break;
        case CFW_STY_FAC_TYPE_AG:
            sOutMsg->ss_Code = ss_BarringOfOutgoingCalls;
            break;
        case CFW_STY_FAC_TYPE_AC:
            sOutMsg->ss_Code = ss_BarringOfIncomingCalls;
            break;
    }
    switch (nMode)
    {
        case CFW_CM_MODE_UNLOCK:
            sOutMsg->ss_Operation = ss_Deactivate;
            break;
        case CFW_CM_MODE_LOCK:
            sOutMsg->ss_Operation = ss_Activate;
            break;
    }
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCode         = nProcCode;
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCodeCategory = nCategory;

    // newpassword  newpasswordAgain
    sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordAgainPresent = FALSE;
    sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordPresent      = FALSE;

    // password
    if (nPwdSize == 0)
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bPasswordPresent = FALSE;
    }
    else if (nPwdSize > 0 && nPwdSize < 5)
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bPasswordPresent = TRUE;
        SUL_MemCopy8(sOutMsg->ss_Params.ss_CallBarringParams.ss_Password, pBufPwd, nPwdSize);
    }

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nClass            = nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_SsSetFacilityLock, end\n",0x081005e2)));

    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSetFacilityLock);
    return ret;
}

UINT32 CFW_SsQueryFacilityLock(UINT16 nFac, UINT8 nClass, UINT16 nUTI
                               , CFW_SIM_ID nSimID
                              )
{
    HAO hSm;
    UINT32 ret = ERR_SUCCESS;

    SS_SM_INFO *proc = NULL;
    api_SsActionReq_t *sOutMsg = NULL;
    UINT8 nProcCode = NO_SUCH_CODE;
    UINT8 nCategory = ss_ServicecodeNotApplicable;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsQueryFacilityLock);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("1,In CFW_CmQueryFacilityLock, nFac=0x%x, nClass=%d\n",0x081005e3)), nFac, nClass);

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_CmQueryFacilityLock, nSimID=%d\n",0x081005e4)), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, check nSimID failed\n",0x081005e5)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, other SIM busy, call this API later\n",0x081005e6)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);

    if (nFac != CFW_STY_FAC_TYPE_AO && nFac != CFW_STY_FAC_TYPE_OI && nFac != CFW_STY_FAC_TYPE_OX
            && nFac != CFW_STY_FAC_TYPE_AI && nFac != CFW_STY_FAC_TYPE_IR)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, invalid param nFac\n",0x081005e7)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(0 != nClass) //add by wulc  for gcf-ss test
    {
        nCategory = GetBSCategory(nClass);
        nProcCode = GetBSProtocolCode(nClass, nCategory);
        if (nProcCode == NO_SUCH_CODE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, invalid param nClass\n",0x081005e8)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        nClass = 11;

    }
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, malloc return NULL\n",0x081005e9)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("3,Malloc for proc ok, next to malloc for sOutMsg\n",0x081005ea)));
    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

    // [[hameina [+] 2006.10.23 : check the malloc result.
    if (NULL == sOutMsg)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmQueryFacilityLock, malloc for stack message, return NULL\n",0x081005eb)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
        return ERR_NO_MORE_MEMORY;
    }

    // ]]hameina [+] end 2006.10.23
    switch (nFac)
    {
        case CFW_STY_FAC_TYPE_AO:
            sOutMsg->ss_Code = ss_BAOC;
            break;
        case CFW_STY_FAC_TYPE_OI:
            sOutMsg->ss_Code = ss_BOIC;
            break;
        case CFW_STY_FAC_TYPE_OX:
            sOutMsg->ss_Code = ss_BOIC_ExHC;
            break;
        case CFW_STY_FAC_TYPE_AI:
            sOutMsg->ss_Code = ss_BAIC;
            break;
        case CFW_STY_FAC_TYPE_IR:
            sOutMsg->ss_Code = ss_BIC_ROAM;
            break;
        case CFW_STY_FAC_TYPE_AB:
            sOutMsg->ss_Code = ss_AllBarringSS;
            break;
        case CFW_STY_FAC_TYPE_AG:
            sOutMsg->ss_Code = ss_BarringOfOutgoingCalls;
            break;
        case CFW_STY_FAC_TYPE_AC:
            sOutMsg->ss_Code = ss_BarringOfIncomingCalls;
            break;
    }

    sOutMsg->ss_Operation                                            = ss_Interrogate;
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCode           = nProcCode;
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCodeCategory   = nCategory;
    sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordAgainPresent = 0;
    sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordPresent      = 0;
    sOutMsg->ss_Params.ss_CallBarringParams.bPasswordPresent         = 0;

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nClass            = nClass;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);  // to add  here  every function
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("6,end of CFW_SsQueryFacilityLock,ret =0x%x\n",0x081005ec)), ret);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsQueryFacilityLock);
    return ret;
}

UINT32 CFW_SsChangePassword(UINT16 nFac,
                            UINT8 *pBufOldPwd, UINT8 nOldPwdSize, UINT8 *pBufNewPwd, UINT8 nNewPwdSize, UINT16 nUTI
                            , CFW_SIM_ID nSimID
                           )
{
    HAO hSm;

    SS_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;

    api_SsActionReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsChangePassword);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsChangePassword,begin, nFac =0x%x\n",0x081005ed)), nFac);
    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsChangePassword, nSimID=%d\n",0x081005ee)), nSimID);
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsChangePassword, check nSimID failed\n",0x081005ef)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsChangePassword, other SIM  busy, call this API later\n",0x081005f0)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }


    if (nOldPwdSize > 4 || nNewPwdSize > 4)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (nFac != CFW_STY_FAC_TYPE_AO && nFac != CFW_STY_FAC_TYPE_OI && nFac != CFW_STY_FAC_TYPE_OX
            && nFac != CFW_STY_FAC_TYPE_AI && nFac != CFW_STY_FAC_TYPE_AB && nFac != CFW_STY_FAC_TYPE_AG
            && nFac != CFW_STY_FAC_TYPE_AC && nFac != CFW_STY_FAC_TYPE_IR)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsChangePassword, invalid param nFac\n",0x081005f1)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((nOldPwdSize > 0) && (pBufOldPwd == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsChangePassword, invalid param nOldPwdSize/pBufOldPwd\n",0x081005f2)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CmChangePassword, old password:\n",0x081005f3)));
    CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pBufOldPwd, nOldPwdSize, 16);

    if ((nNewPwdSize > 0) && (pBufNewPwd == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsChangePassword, invalid param nNewPwdSize/pBufNewPwd\n",0x081005f4)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CmChangePassword, new password:\n",0x081005f5)));
    CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pBufNewPwd, nNewPwdSize, 16);
    CFW_SetServiceId(CFW_SS_SRV_ID);
    CHECKUTI(nUTI);
    proc = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_CmChangePassword, malloc return NULL\n",0x081005f6)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

    sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

    switch (nFac)
    {
        case CFW_STY_FAC_TYPE_AO:
        case CFW_STY_FAC_TYPE_OI:
        case CFW_STY_FAC_TYPE_OX:
        case CFW_STY_FAC_TYPE_AI:
        case CFW_STY_FAC_TYPE_AB:
        case CFW_STY_FAC_TYPE_AG:
        case CFW_STY_FAC_TYPE_AC:
        case CFW_STY_FAC_TYPE_IR:
            sOutMsg->ss_Code = ss_AllBarringSS;
            break;
    }

    sOutMsg->ss_Operation                                          = ss_RegisterPassword;
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCode         = API_SS_TELEPHONY;  // ????????
    sOutMsg->ss_Params.ss_CallBarringParams.ss_ServiceCodeCategory = ss_TeleserviceCode;  // ????

    // old password
    if (nOldPwdSize > 0)
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bPasswordPresent = TRUE;
        SUL_MemCopy8(sOutMsg->ss_Params.ss_CallBarringParams.ss_Password, pBufOldPwd, nOldPwdSize);
    }
    else
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bPasswordPresent = FALSE;
    }

    if (nNewPwdSize > 0)
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordAgainPresent = TRUE;
        sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordPresent      = TRUE;
        SUL_MemCopy8(sOutMsg->ss_Params.ss_CallBarringParams.ss_newPassword, pBufNewPwd, nNewPwdSize);
        SUL_MemCopy8(sOutMsg->ss_Params.ss_CallBarringParams.ss_newPasswordAgain, pBufNewPwd, nNewPwdSize);
    }
    else
    {
        sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordAgainPresent = FALSE;
        sOutMsg->ss_Params.ss_CallBarringParams.bNewPasswordPresent      = FALSE;
    }

    proc->nTI               = 0x80;
    proc->pMsg              = (VOID *)sOutMsg;
    proc->nApi_SS_Code      = sOutMsg->ss_Code;
    proc->nApi_SS_Operation = sOutMsg->ss_Operation;

    // register SS AO
    hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_CmChangePassword, end\n",0x081005f7)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SsChangePassword);
    return ret;
}

UINT32 CFW_SsTerminate(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    // UINT8 nAoCount = 0;
    // UINT8 nAoIndex = 0;
    HAO hAo = 0;
    SS_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsTerminate);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsTerminate!nSimID=%d\n",0x081005f8)), nSimID);

    ret = CFW_CheckSimId(nSimID);

    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsTerminate!nSimID check failed\n",0x081005f9)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsTerminate);
        return ret;
    }

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsSendUSSD, nSimID=%d\n",0x081005fa)), nSimID);

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSendUSSD, other SIM busy, call this API later\n",0x081005fb)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    nAOCount = CFW_GetAoCount(CFW_SS_SRV_ID, nSimID);

    UINT8 nAoIndex;
    for (nAoIndex = 0; nAoIndex < nAOCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_SS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);

        if (proc != NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("In USSD send proc nAoIndex %d, ussdflag %d proc->nUTI_Mmi %d\n",0x081005fc), nAoIndex,
                      proc->nUSSDFlag, proc->nUTI_Mmi);

            if (FALSE == proc->nUSSDFlag)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmii %d\n",0x081005fd), proc->nUTI_Mmi);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SsSendUSSD,There are other ss operation exist\n",0x081005fe));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CME_OPERATION_NOT_ALLOWED;
            }

            if (proc->nUTI_Mmi == nUTI)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SsSendUSSD, ok Same uti %d\n",0x081005ff), nUTI);
                api_SsActionReq_t *sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));

                if (sOutMsg == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,msg Malloc return NULL\n",0x08100600)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_NO_MORE_MEMORY;
                }
                proc->pMsg = sOutMsg;
                break;
            }

            /*
            else if(proc->nUTI_Mmi!=nUTI)
            {
            if(TRUE==proc->nUSSDFlag)
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("There are other USSD exsit\n",0x08100601));
            return ERR_CME_OPERATION_NOT_ALLOWED;
            }
            */
            else
                proc = NULL;
        }
    }


    if(0 == hAo )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsTerminate!AO is NULL!\n",0x08100602)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsTerminate);
        return ERR_INVALID_PARAMETER;
    }

    if(NULL == proc)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsTerminate!AO data is NULL!\n",0x08100603)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsTerminate);
        return ERR_CFW_NOT_EXIST_AO;
    }

    //proc->nCancelFlag = 1;
#if 0
    // register SS AO
    hAo = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);
#endif
    CFW_SetUTI(hAo, nUTI, 0); // hameina[+] 2006.10.23
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsTerminate!END\n",0x08100604)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SsTerminate);
    return (ERR_SUCCESS);
}
// -----------------------------------------------------------------------------------------------------------------------------
// parameter
// pUsdString[in]:     pointer the to USSD string. It should be decoded codes.
// nUsdStringSize[in]:   size of param pUsdString. different meaning in different nOption, see below
// nOption[in]:      operation option of the USSD string. available value can be divided into two groups,
// one is for USSD v2:
// 0 (response the NETWORK with an error), in this case ,param nUsdStringSize must be input as api_SsUSSDErrorPresent_t, not just UINT8.
// 1 (response the NETWORK with reject), in this case ,param nUsdStringSize must be input as api_SsProblemCodeTag_t, not just UINT8.
// 2 (response the NETWORK with MS release request)
// 3 (response the NETWORK with user data or invoke USSD to network)
// the other is for USSD v1:
// 128 (response the NETWORK with an error),  in this case ,param nUsdStringSize must be input as api_SsUSSDErrorPresent_t, not just UINT8.
// 129 (response the NETWORK with reject), in this case ,param nUsdStringSize must be input as api_SsProblemCodeTag_t, not just UINT8.
// 130 (response the NETWORK with MS release request)
// 131 (response the NETWORK with user data or invoke USSD to network)
// nDcs[in]:   decode scheme for pUsdString, available only when nOption support USSD V2.
// -------------------------------------------------------------------------------------------------------------------------------
// author: Qiaoguangjun 2007.6.5
// Hameina [mod] 2009.3.24
// ------------------------------------------------------------------------------------------------------
UINT32 CFW_SsSendUSSD(UINT8 *pUsdString, UINT8 nUsdStringSize, UINT8 nOption, UINT8 nDcs, UINT16 nUTI
                      , CFW_SIM_ID nSimID
                     )
{
#ifdef USSD_ENDABLE
    UINT32 ret = ERR_SUCCESS;
    HAO hSm    = 0;

    SS_SM_INFO *proc = NULL;

    // UINT8 nPresent;
    api_SsActionReq_t *sOutMsg = NULL;
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    HAO hAo        = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SsSendUSSD);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("In CFW_SsSendUSSD, nOption=%d, nDcs=%d\n",0x08100605), nOption, nDcs);

    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In CFW_SsSendUSSD, nSimID=%d\n",0x08100606)), nSimID);

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSendUSSD, check nSimID failed\n",0x08100607)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
        return ret;
    }

    // check if other SIM operate on SS module
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_SS_SRV_ID, nSimId);

    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]In CFW_SsSendUSSD, other SIM busy, call this API later\n",0x08100608)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    nAoCount = CFW_GetAoCount(CFW_SS_SRV_ID, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SsSendUSSD, nAoCount %d\n",0x08100609), nAoCount);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_SS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);

        if (proc != NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("In USSD send proc nAoIndex %d, ussdflag %d proc->nUTI_Mmi %d\n",0x0810060a), nAoIndex,
                      proc->nUSSDFlag, proc->nUTI_Mmi);

            if (FALSE == proc->nUSSDFlag)
            {

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmii %d\n",0x0810060b), proc->nUTI_Mmi);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_SsSendUSSD,There are other ss operation exist\n",0x0810060c));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CME_OPERATION_NOT_ALLOWED;
            }
            if (proc->nUTI_Mmi == nUTI)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SsSendUSSD, ok Same uti %d\n",0x0810060d), nUTI);
                sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
                if (sOutMsg == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,msg Malloc return NULL\n",0x0810060e)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_NO_MORE_MEMORY;
                }
                proc->pMsg = sOutMsg;
                hSm        = hAo;
                break;
            }

            /*
                        else if(proc->nUTI_Mmi!=nUTI)
                        {
                            if(TRUE==proc->nUSSDFlag)
                            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("There are other USSD exsit\n",0x0810060f));
                            return ERR_CME_OPERATION_NOT_ALLOWED;
                        }
            */
            else
                proc = NULL;
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SsSendUSSD, nUsdStringSize=%d\n",0x08100610)), nUsdStringSize);

    switch (nOption)
    {
        case 0:  // MS return erorr v2
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 0, USSD  MS return ERROR!\n",0x08100611)));
            if (NULL == pUsdString || 0 == nUsdStringSize)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR, pUsdString null \n",0x08100612)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR proc null\n",0x08100613)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_error;
            sOutMsg->ss_Code      = ss_USSD_V2;
            sOutMsg->ss_Operation = proc->nApi_SS_Operation;
            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_FacilityMessageType;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDReturnError;

            // ---------------------------------------------------------------------------------------
            // in this case, the param nUsdStringSize is used as api_SsUSSDErrorPresent_t, I think it's error type
            // -------------------------------------------------------------------------------------
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorPresent = nUsdStringSize;

            switch (nUsdStringSize)
            {
                case ss_ProcessUSSDataErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_ProcessUSSDataError =
                        pUsdString[0];
                    break;
                case ss_ProcessUSSRequestErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_ProcessUSSRequestError =
                        pUsdString[0];
                    break;
                case ss_USSRequestErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_USSRequestError =
                        pUsdString[0];
                    break;
                case ss_USSNotifyErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_USSNotifyError = pUsdString[0];
                    break;
                default:
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD, ss_GeneralProblemCode  ERR v2\n",0x08100614)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_CFW_INVALID_PARAMETER;
                    break;
            }
            sOutMsg->ss_Params.ss_USSDParams.TI = proc->nTI;  // NOT sure about that parameter, to be reviewed

            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            return ERR_SUCCESS;
        }
        break;
        case 1:  // MS return reject v2
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 1, USSD  MS return reject! \n",0x08100615)));

            if (NULL == pUsdString || 0 == nUsdStringSize)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR, pUsdString null \n",0x08100616)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR proc null\n",0x08100617)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_reject;
            sOutMsg->ss_Code      = ss_USSD_V2;
            sOutMsg->ss_Operation = proc->nApi_SS_Operation;
            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                                         = ss_FacilityMessageType;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                                       = ss_USSDReturnReject;
            sOutMsg->ss_Params.ss_USSDParams.TI                                                         = proc->nTI;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCodeTag = nUsdStringSize;

            switch (nUsdStringSize)
            {

                case API_SS_GENERAL_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_GeneralProblemCode =
                        pUsdString[0];
                    break;
                case API_SS_INVOKE_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_InvokeProblemCode =
                        pUsdString[0];
                    break;
                case API_SS_RETURN_RESULT_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_ReturnErrorProblemCodes = pUsdString[0];
                    break;
                case API_SS_RETURN_ERROR_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_ReturnResultProblemCodes = pUsdString[0];
                    break;

                default:

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD, ss_GeneralProblemCode  ERR v2\n",0x08100618)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_CFW_INVALID_PARAMETER;
                    break;
            }

            // proc->nTI = nUTI;
            // proc->nUTI_mmi =nUTI;
            // proc->pMsg = (VOID*)sOutMsg;
            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            return ERR_SUCCESS;

        }
        break;

        case 2:  // relese complete v2
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 2, USSD  MS release! \n",0x08100619)));
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR proc null\n",0x0810061a)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_release;

            sOutMsg->ss_Code      = ss_USSD_V2;
            sOutMsg->ss_Operation = proc->nApi_SS_Operation;

            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));

            sOutMsg->ss_Params.ss_USSDParams.TI                   = proc->nTI;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_ReleaseCompleteMessageType;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDNoComponentPresent;

            // proc->nTI = nUTI;
            // proc->nUTI_mmi =nUTI;
            // proc->pMsg = (VOID*)sOutMsg;
            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmi = %d\n ",0x0810061b), nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Code	 = %d\n ",0x0810061c), sOutMsg->ss_Code);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Operation %d\n ",0x0810061d), sOutMsg->ss_Operation);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x0810061e),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x0810061f),
                      sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            return ERR_SUCCESS;
        }
        break;
        case 3:  // MS发起 ussd 或返回网络发起的 ussd 操作v2
        {
            // 如果是返回网络发起的ussd操作，需要区分对待request and notify
            // 对于requset，需要返回数据，对于notify，不需要返回数据
            if (proc == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 3,CFW_SsSendUSSD, Send register, PROC IS NULL\n",0x08100620)));

                // hameina[+] 2009.6.25 the last condition to verify the correct string length
                if (NULL == pUsdString || 0 == nUsdStringSize || nUsdStringSize > SS_USSD_ARG_STRING_MAX_LEN)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Paramater  ERR, pUsdString null \n",0x08100621)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_CFW_INVALID_PARAMETER;
                }
                proc    = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
                sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
                if (proc == NULL || sOutMsg == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD, Malloc return NULL\n",0x08100622)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

                // proc->nUSSD_State=USSD_AO_State_idle;

                sOutMsg->ss_Code      = ss_USSD_V2;
                sOutMsg->ss_Operation = ss_ProcessUnstructuredSSRequest;
                SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                                  = ss_RegisterMessageType;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                                = ss_USSDInvoke;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme = nDcs;
                //if(!pUsdString[nUsdStringSize-1])
                //  pUsdString[nUsdStringSize-1] = 0x1a;

                sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = nUsdStringSize;
                CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pUsdString, nUsdStringSize, 16);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nUsdStringSize %d\n",0x08100623)), nUsdStringSize);
                SUL_MemCopy8(sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString, pUsdString,
                             SS_USSD_ARG_STRING_MAX_LEN > (nUsdStringSize) ? (nUsdStringSize) : SS_USSD_ARG_STRING_MAX_LEN);

                proc->nTI               = 0xff;
                proc->nUTI_Mmi          = nUTI;
                proc->pMsg              = (VOID *)sOutMsg;
                proc->nApi_SS_Code      = sOutMsg->ss_Code;
                proc->nApi_SS_Operation = sOutMsg->ss_Operation;
                proc->nUSSDFlag         = TRUE;

                /*
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmi = %d\n ",0x08100624),nUTI);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Code   = %d\n ",0x08100625),sOutMsg->ss_Code);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Operation %d\n ",0x08100626),sOutMsg->ss_Operation);

                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x08100627),sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x08100628),sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDDataCodingScheme = %d\n ",0x08100629),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR(".ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = %d\n ",0x0810062a),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength);

                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810062b),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[0]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810062c),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[1]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810062d),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[2]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810062e),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[3]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString =0x%x\n ",0x0810062f),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[4]);
                 */

                // register SS AO
                hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

                CFW_SetUTI(hSm, nUTI, 0);
                CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

                return ERR_SUCCESS;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 3, CFW_SsSendUSSD,USSD Send facility PROC NOT NULL\n",0x08100630)));

                // proc->nUSSD_State=USSD_AO_State_requst;

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nApi_SS_Operation = %d\n ",0x08100631), proc->nApi_SS_Operation);

                sOutMsg->ss_Code = ss_USSD_V2;

                sOutMsg->ss_Operation = proc->nApi_SS_Operation;  // + to distinguish the notify event and request event
                SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsActionReqParams_t));
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_FacilityMessageType;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDReturnResult;
                sOutMsg->ss_Params.ss_USSDParams.TI                   = proc->nTI;

                if (ss_UnstructuredSSRequest == sOutMsg->ss_Operation)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD return for  ss_UnstructuredSSRequest\n",0x08100632)));

                    if (NULL == pUsdString || 0 == nUsdStringSize)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]Paramater  ERR, string null\n",0x08100633)));
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                        return ERR_CFW_INVALID_PARAMETER;
                    }
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme = nDcs;

                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = nUsdStringSize;
                    SUL_MemCopy8(sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString, pUsdString,
                                 SS_USSD_ARG_STRING_MAX_LEN >
                                 (nUsdStringSize + 1) ? (nUsdStringSize + 1) : SS_USSD_ARG_STRING_MAX_LEN);
                }
                else

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD return for  ss_UnstructuredSSNotify\n",0x08100634)));
                proc->nApi_SS_Code      = sOutMsg->ss_Code;
                proc->nApi_SS_Operation = sOutMsg->ss_Operation;

                /*
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmi = %d\n ",0x08100635),nUTI);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Code   = %d\n ",0x08100636),sOutMsg->ss_Code);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Operation %d\n ",0x08100637),sOutMsg->ss_Operation);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType = %d\n ",0x08100638),sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = %d\n ",0x08100639),sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDDataCodingScheme = %d\n ",0x0810063a),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDDataCodingScheme);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR(".ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength = %d\n ",0x0810063b),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDStringLength);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString = %x\n ",0x0810063c),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[0]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString = %x\n ",0x0810063d),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[1]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString = %x\n ",0x0810063e),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[2]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString = %x\n ",0x0810063f),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[3]);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_USSDString = %x\n ",0x08100640),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDArgs.ss_USSDString[4]);
                 */
                CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD Send to do proc \n",0x08100641)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

                return ERR_SUCCESS;
            }

        }
        break;
        case 128:  // MS return erorr. v1
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 128, CFW_SsSendUSSD\n",0x08100642)));
            if (NULL == pUsdString || 0 == nUsdStringSize)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]Paramater  string is NULL\n",0x08100643)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]proc is null\n",0x08100644)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_error;

            sOutMsg->ss_Code      = ss_USSD_V1;
            sOutMsg->ss_Operation = ss_ProcessUnstructuredSSData;
            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                               = ss_FacilityMessageType;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                             = ss_USSDReturnError;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorPresent = nUsdStringSize;

            switch (nUsdStringSize)
            {
                case ss_ProcessUSSDataErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_ProcessUSSDataError =
                        pUsdString[0];
                    break;
                case ss_ProcessUSSRequestErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_ProcessUSSRequestError =
                        pUsdString[0];
                    break;
                case ss_USSRequestErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_USSRequestError =
                        pUsdString[0];
                    break;
                case ss_USSNotifyErrorPresent:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_USSNotifyError = pUsdString[0];
                    break;
                default:
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]ss_GeneralProblemCode  ERR v2\n",0x08100645)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_CFW_INVALID_PARAMETER;
                    break;
            }

            // sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDError.ss_USSDErrorSel.ss_ProcessUSSDataError=pUsdString[0];
            sOutMsg->ss_Params.ss_USSDParams.TI = proc->nTI;

            // proc->nTI = nUTI;
            // proc->nUTI_mmi =nUTI;
            // proc->pMsg = (VOID*)sOutMsg;
            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            return ERR_SUCCESS;
        }
        break;
        case 129:  // MS return reject v1
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 129\n",0x08100646)));
            if (NULL == pUsdString || 0 == nUsdStringSize)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR] INPUT string is null\n",0x08100647)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR] proc is null\n",0x08100648)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_reject;

            sOutMsg->ss_Code      = ss_USSD_V1;
            sOutMsg->ss_Operation = ss_ProcessUnstructuredSSData;
            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                                       = ss_USSDReturnReject;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                                         = ss_FacilityMessageType;
            sOutMsg->ss_Params.ss_USSDParams.TI                                                         = proc->nTI;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCodeTag = nUsdStringSize;

            // sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode=pUsdString[0];
            switch (nUsdStringSize)
            {

                case API_SS_GENERAL_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_GeneralProblemCode =
                        pUsdString[0];
                    break;
                case API_SS_INVOKE_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_InvokeProblemCode =
                        pUsdString[0];
                    break;
                case API_SS_RETURN_RESULT_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_ReturnErrorProblemCodes = pUsdString[0];
                    break;
                case API_SS_RETURN_ERROR_PROBLEM_TAG:
                    sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_ProblemCodeTagandCode.ss_ProblemCode.ss_ReturnResultProblemCodes = pUsdString[0];
                    break;

                default:

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]ss_GeneralProblemCode  ERR v1\n",0x08100649)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_CFW_INVALID_PARAMETER;
                    break;
            }

            // proc->nTI = nUTI;
            // proc->nUTI_mmi =nUTI;
            // proc->pMsg = (VOID*)sOutMsg;
            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            return ERR_SUCCESS;
        }
        break;
        case 130:  // relese complete. v1
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 129\n",0x0810064a)));
            if (NULL == proc)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR], proc null\n",0x0810064b)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }

            // proc->nUSSD_State=USSD_AO_State_release;

            sOutMsg->ss_Code      = ss_USSD_V1;
            sOutMsg->ss_Operation = ss_ProcessUnstructuredSSData;

            SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
            sOutMsg->ss_Params.ss_USSDParams.TI = proc->nTI;

            sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType   = ss_ReleaseCompleteMessageType;
            sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType = ss_USSDNoComponentPresent;

            // proc->nTI = 0x80;
            // proc->nUTI_mmi =nUTI;
            // proc->pMsg = (VOID*)sOutMsg;
            proc->nApi_SS_Code      = sOutMsg->ss_Code;
            proc->nApi_SS_Operation = sOutMsg->ss_Operation;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsSendUSSD, return ok\n",0x0810064c)));
            return ERR_SUCCESS;

        }
        break;
        case 131:  // MS发起 ussd 或返回网络发起的 ussd 操作v1
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("case 131,CFW_SsSendUSSD\n",0x0810064d)));

            if (NULL == pUsdString || 0 == nUsdStringSize)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD, input string NULL\n",0x0810064e)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                return ERR_CFW_INVALID_PARAMETER;
            }
            CSW_TC_MEMBLOCK(CFW_SS_TS_ID, pUsdString, nUsdStringSize, 16);

            if (NULL == proc)
            {

                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  MS v1 Send register! \n",0x0810064f)));
                proc    = (SS_SM_INFO *)CSW_SS_MALLOC(SIZEOF(SS_SM_INFO));
                sOutMsg = (api_SsActionReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SsActionReq_t));
                if (proc == NULL || sOutMsg == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD,Malloc return NULL\n",0x08100650)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory32(proc, SIZEOF(SS_SM_INFO));

                // proc->nUSSD_State=USSD_AO_State_idle;

                sOutMsg->ss_Code      = ss_USSD_V1;
                sOutMsg->ss_Operation = ss_ProcessUnstructuredSSData;
                SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                                 = ss_RegisterMessageType;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                               = ss_USSDInvoke;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5StringLength = nUsdStringSize;
                SUL_MemCopy8(sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5String, pUsdString,
                             SS_USSD_USER_DATA_IA5_STRING_MAX_LEN >
                             (nUsdStringSize) ? (nUsdStringSize) : SS_USSD_USER_DATA_IA5_STRING_MAX_LEN);

                proc->nTI               = 0xff;
                proc->nUTI_Mmi          = nUTI;
                proc->pMsg              = (VOID *)sOutMsg;
                proc->nApi_SS_Code      = sOutMsg->ss_Code;
                proc->nApi_SS_Operation = sOutMsg->ss_Operation;
                proc->nUSSDFlag         = TRUE;

                /*
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nUTI_Mmi = %d\n ",0x08100651),nUTI);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nApi_SS_Code = %d\n ",0x08100652),proc->nApi_SS_Code);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nApi_SS_Operation = %d\n ",0x08100653),proc->nApi_SS_Operation);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("component = %d\n ",0x08100654),sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("msg type = %d\n ",0x08100655),sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType);

                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("pUsdString = %s\n ",0x08100656),pUsdString);
                   CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("ss_IA5String = %s\n ",0x08100657),sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5String);
                 */
                // register SS AO
                hSm = CFW_RegisterAo(CFW_SS_SRV_ID, proc, CFW_SsAoProc, nSimID);

                CFW_SetUTI(hSm, nUTI, 0);
                CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsSendUSSD, return ok\n",0x08100658)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);

                return ERR_SUCCESS;

            }
            else
            {
                // proc->nUSSD_State=USSD_AO_State_requst;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("USSD  MS v1 return facility! \n",0x08100659)));

                sOutMsg->ss_Code      = ss_USSD_V1;
                sOutMsg->ss_Operation = ss_ProcessUnstructuredSSData;
                SUL_ZeroMemory8(&(sOutMsg->ss_Params), SIZEOF(api_SsUSSDParams_t));
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDMessageType                                 = ss_FacilityMessageType;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDComponentType                               = ss_USSDReturnResult;
                sOutMsg->ss_Params.ss_USSDParams.TI                                                 = proc->nTI;
                sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5StringLength = nUsdStringSize;
                SUL_MemCopy8(sOutMsg->ss_Params.ss_USSDParams.ss_USSDParamSel.ss_USSDUserData.ss_IA5String, pUsdString,
                             SS_USSD_USER_DATA_IA5_STRING_MAX_LEN >
                             (nUsdStringSize) ? (nUsdStringSize) : SS_USSD_USER_DATA_IA5_STRING_MAX_LEN);

                // proc->nTI = nUTI;

                // proc->nUTI_mmi =nUTI;
                // proc->pMsg = (VOID*)sOutMsg;
                proc->nApi_SS_Code      = sOutMsg->ss_Code;
                proc->nApi_SS_Operation = sOutMsg->ss_Operation;

                CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SsSendUSSD, return ok\n",0x0810065a)));

                return ERR_SUCCESS;
            }

        }
        break;

        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsSendUSSD, invalid option\n",0x0810065b)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
            return ERR_CFW_INVALID_PARAMETER;
            break;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_SsSendUSSD);
    return ret;
#else
    return ERR_CFW_NOT_SUPPORT;
#endif
}

UINT32 CFW_SsReSetACM(UINT32 iCurValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

UINT32 CFW_SsGetACM(UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

UINT32 CFW_SsSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

UINT32 CFW_SsGetACMMax(UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

UINT32 CFW_SsSetPUCT(UINT8 iCurrency[3], UINT16 iEPPU, INT8 iEX, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

UINT32 CFW_SsGetPUCT(UINT16 nUTI)
{
    UINT32 ret = ERR_SUCCESS;

    return ret;
}

#if 0
PRIVATE api_SsCode_t ss_GetSSProCode(T_COMM_eSsCode iMmiSSCode)
{
    UINT8 iCount;

    for (iCount = 0; iCount < (sizeof(sSsServCodeLists) / sizeof(T_COMM_sSsServCodeList)); iCount++)
    {
        if (iMmiSSCode == sSsServCodeLists[iCount].MMISSCode)
        {
            break;
        }
    }
    if (iCount == (sizeof(sSsServCodeLists) / sizeof(T_COMM_sSsServCodeList)))
    {
        return (ss_InvalidSSCode);
    }
    else
    {
        return (sSsServCodeLists[iCount].ProSSCode);
    }

}
#endif


// *******************************************************
// Function     : GetMmiServCode
// Description  : This function is called by COMM self to transition SS
// protocol code to SS MMI code
// code used in protocol stack
// Arguments    :[in]  iSSProCode:  service code used in protocol
//
// Return value :  SSMmiCode:    service code provided to MMI
// *********************************************************
PRIVATE T_COMM_eSsCode ss_GetMmiServCode(api_SsCode_t iSSProCode)
{

    UINT8 iCount;

    CSW_PROFILE_FUNCTION_ENTER(ss_GetMmiServCode);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_GetMmiServCode, input param iSSProCode=%d\n",0x0810065c)), iSSProCode);
    for (iCount = 0; iCount < (sizeof(sSsServCodeLists) / sizeof(T_COMM_sSsServCodeList)); iCount++)
    {
        if (iSSProCode == sSsServCodeLists[iCount].ProSSCode)
        {
            break;
        }
    }
    if (iCount == (sizeof(sSsServCodeLists) / sizeof(T_COMM_sSsServCodeList)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]ss_GetMmiServCode, return COMM_SS_NO_CODE\n",0x0810065d)));
        CSW_PROFILE_FUNCTION_EXIT(ss_GetMmiServCode);
        return (COMM_SS_NO_CODE);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_GetMmiServCode, return =%d\n",0x0810065e)), sSsServCodeLists[iCount].MMISSCode);
        CSW_PROFILE_FUNCTION_EXIT(ss_GetMmiServCode);
        return (sSsServCodeLists[iCount].MMISSCode);
    }

}

PRIVATE UINT8 GetBSCategory(UINT8 iBSMmiCode)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("GetBSCategory, input param iBSMmiCode=%d\n",0x0810065f)), iBSMmiCode);
    if (iBSMmiCode < 19)
        return (ss_TeleserviceCode);
    else
        return (ss_BearerServiceCode);
}

// ******************************************************
// Function     : GetBSProtocolCode
// Description  : This function is called by COMM self to get Basic service
// code used in protocol stack
// Arguments    :  iBSMmiCode[in] Basic service code input from MMI
// iBSCategory[in] Basic Service Code Category
//
// Return value :  ProtocolCode: Basic service code used in protocol
// ******************************************************
PRIVATE UINT8 GetBSProtocolCode(UINT8 iBSMmiCode, T_COMM_eSsBasicServiceCodeCategory iBSCategory)
{

    UINT8 iCount;

    CSW_PROFILE_FUNCTION_ENTER(GetBSProtocolCode);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("GetBSProtocolCode, input param iBSMmiCode=%d, iBSCategory=%d\n",0x08100660)), iBSMmiCode,
              iBSCategory);
    switch (iBSCategory)
    {
        case COMM_SS_TeleserviceCode:
        {
            for (iCount = 0; iCount < (sizeof(sSsTeleBSCodeLists) / sizeof(T_COMM_sSsTeleBSCodeList)); iCount++)
            {
                if (iBSMmiCode == sSsTeleBSCodeLists[iCount].MMIBSCode)
                {
                    break;
                }
            }
            if (iCount == (sizeof(sSsTeleBSCodeLists) / sizeof(T_COMM_sSsTeleBSCodeList)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("GetBSProtocolCode,return :NO_SUCH_CODE\n",0x08100661)));
                CSW_PROFILE_FUNCTION_EXIT(GetBSProtocolCode);
                return (NO_SUCH_CODE);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("GetBSProtocolCode,COMM_SS_TeleserviceCode,return :%d\n",0x08100662)),
                          (sSsTeleBSCodeLists[iCount].ProtocolCode));
                CSW_PROFILE_FUNCTION_EXIT(GetBSProtocolCode);
                return (UINT8)(sSsTeleBSCodeLists[iCount].ProtocolCode);
            }
            break;
        }
        case COMM_SS_BearerServiceCode:
        {
            for (iCount = 0; iCount < (sizeof(sSsBearBSCodeLists) / sizeof(T_COMM_sSsBearBSCodeList)); iCount++)
            {
                if (iBSMmiCode == sSsBearBSCodeLists[iCount].MMIBSCode)
                {
                    break;
                }
            }
            if (iCount == (sizeof(sSsBearBSCodeLists) / sizeof(T_COMM_sSsBearBSCodeList)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("GetBSProtocolCode,return :NO_SUCH_CODE\n",0x08100663)));
                CSW_PROFILE_FUNCTION_EXIT(GetBSProtocolCode);
                return (NO_SUCH_CODE);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("GetBSProtocolCode,COMM_SS_BearerServiceCode,return :%d\n",0x08100664)),
                          (sSsBearBSCodeLists[iCount].ProtocolCode));
                CSW_PROFILE_FUNCTION_EXIT(GetBSProtocolCode);
                return (UINT8)(sSsBearBSCodeLists[iCount].ProtocolCode);
            }
            break;

        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("GetBSProtocolCode,return : default, NO_SUCH_CODE\n",0x08100665)));
            CSW_PROFILE_FUNCTION_EXIT(GetBSProtocolCode);
            return (NO_SUCH_CODE);
    }

}

// *******************************************************
// Function     : GetBSMmiCode
// Description  : This function is called by COMM self to transition BS
// protocol code to BS MMI code
// code used in protocol stack
// Arguments    :  [in]iBSProctolCode:[]Basic service code used in protocol
// [in]iBSCategory:[]Basic Service Code Category
//
// Return value :  BSMmiCode:   Basic service code provided to MMI
// *******************************************************
PRIVATE UINT8 GetBSMmiCode(UINT8 iBSProctolCode, api_SsBasicServiceCodeCategory_t iBSCategory)
{

    UINT8 iCount;

    CSW_PROFILE_FUNCTION_ENTER(GetBSMmiCode);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("GetBSMmiCode,input param iBSProctolCode=%d,iBSCategory=%d\n",0x08100666)), iBSProctolCode,
              iBSCategory);
    switch (iBSCategory)
    {
        case COMM_SS_TeleserviceCode:
        {
            for (iCount = 0; iCount < (sizeof(sSsTeleBSCodeLists) / sizeof(T_COMM_sSsTeleBSCodeList)); iCount++)
            {
                if (iBSProctolCode == sSsTeleBSCodeLists[iCount].ProtocolCode)
                {
                    break;
                }
            }
            if (iCount == (sizeof(sSsTeleBSCodeLists) / sizeof(T_COMM_sSsTeleBSCodeList)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]GetBSMmiCode,COMM_SS_TeleserviceCode, failed\n",0x08100667)));
                CSW_PROFILE_FUNCTION_EXIT(GetBSMmiCode);
                return (NO_SUCH_CODE);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("GetBSMmiCode,COMM_SS_TeleserviceCode, return %d\n",0x08100668)),
                          sSsTeleBSCodeLists[iCount].MMIBSCode);
                CSW_PROFILE_FUNCTION_EXIT(GetBSMmiCode);
                return (UINT8)(sSsTeleBSCodeLists[iCount].MMIBSCode);
            }
            break;
        }
        case COMM_SS_BearerServiceCode:
        {
            for (iCount = 0; iCount < (sizeof(sSsBearBSCodeLists) / sizeof(T_COMM_sSsBearBSCodeList)); iCount++)
            {
                if (iBSProctolCode == sSsBearBSCodeLists[iCount].ProtocolCode)
                {
                    break;
                }
            }
            if (iCount == (sizeof(sSsBearBSCodeLists) / sizeof(T_COMM_sSsBearBSCodeList)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]GetBSMmiCode,COMM_SS_BearerServiceCode, failed\n",0x08100669)));
                CSW_PROFILE_FUNCTION_EXIT(GetBSMmiCode);
                return (NO_SUCH_CODE);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("GetBSMmiCode,COMM_SS_BearerServiceCode, return %d\n",0x0810066a)),
                          sSsBearBSCodeLists[iCount].MMIBSCode);
                CSW_PROFILE_FUNCTION_EXIT(GetBSMmiCode);
                return (UINT8)(sSsBearBSCodeLists[iCount].MMIBSCode);
            }
            break;

        }
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]GetBSMmiCode,in default, failed\n",0x0810066b)));
            CSW_PROFILE_FUNCTION_EXIT(GetBSMmiCode);
            return (NO_SUCH_CODE);
    }

}

// **********************************************************
// Function     : ss_SetMmiBitStatus
// Description  : this fuction is to set the status_bit(*p1) according to the
// status and nClass
// Arguments    :  (in) status:   1,active ;0 not active
// [in] iBSCategory :
// [in] nBSCode  :
// [out] p1 :
// [in] nClass:
// (out) p1: status_bit
// (in)nClass:class_bit
// Return value :  VOID
// ***********************************************************
PRIVATE VOID ss_SetMmiBitStatus(T_COMM_eSsBasicServiceCodeCategory iBSCategory, UINT8 nBSCode, UINT8 status, UINT32 *p1,
                                UINT32 nClass)
{

    CSW_PROFILE_FUNCTION_ENTER(ss_SetMmiBitStatus);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("ss_SetMmiBitStatus,input param iBSCategory=%d,nBSCode=%d\n",0x0810066c)), iBSCategory, nBSCode);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("ss_SetMmiBitStatus,input param status=%d,nClass=%d\n",0x0810066d)), status, nClass);

    switch (iBSCategory)
    {
        case COMM_SS_TeleserviceCode:  // *pEnable = MemoryInfo.Clip_Value;
            switch (nBSCode)
            {
                // case LOCAL_SS_ALL_TELESERVICES:
                case LOCAL_SS_ALL_SPEECH_TELESERVICES:
                case LOCAL_SS_TELEPHONY:
                case LOCAL_SS_EMERGENCY_CALLS:
                    if ((CFW_IS_BIT_SET(nClass, 0)) && (CFW_IS_BIT_SET(status, 0)))
                        CFW_SET_BIT(*p1, 0);
                    break;
                case LOCAL_SS_ALL_FAX_SERVICES:
                case LOCAL_SS_FAX_GROUP3_ALTER_SPEECH:
                case LOCAL_SS_AUTO_FAX_GROUP3:
                case LOCAL_SS_FAX_GROUP4:
                    if ((CFW_IS_BIT_SET(nClass, 2)) && (CFW_IS_BIT_SET(status, 0)))
                        CFW_SET_BIT(*p1, 2);
                    break;
                case LOCAL_SS_ALL_SMS:
                case LOCAL_SS_SM_MT_PP:
                case LOCAL_SS_SM_MO_PP:
                    if ((CFW_IS_BIT_SET(nClass, 3)) && (CFW_IS_BIT_SET(status, 0)))
                        CFW_SET_BIT(*p1, 3);
                    break;
            }
            break;
        case COMM_SS_BearerServiceCode:
            switch(nBSCode)
            {
                case LOCAL_SS_allSynchronousServices:
                    if ((CFW_IS_BIT_SET(nClass, 4)) && (CFW_IS_BIT_SET(status, 0)))
                        CFW_SET_BIT(*p1, 4);
                    break;

                case LOCAL_SS_allAsynchronousServices:
                    if ((CFW_IS_BIT_SET(nClass, 5)) && (CFW_IS_BIT_SET(status, 0)))
                        CFW_SET_BIT(*p1, 5);
                    break;
                default:
                    if ((CFW_IS_BIT_SET(nClass, 1)) && (CFW_IS_BIT_SET(status, 0)))
                    {
                        CFW_SET_BIT(*p1, 1);
                    }
                    break;
            }

        default:
            break;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_SetMmiBitStatus,output param *p1=%d\n",0x0810066e)), *p1);

    CSW_PROFILE_FUNCTION_EXIT(ss_SetMmiBitStatus);
    return;
}

PRIVATE UINT32 ss_GetMMiClass(UINT8 nClass)
{

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_GetMMiClass,input param nClass=%d\n",0x0810066f)), nClass);
    switch (nClass)
    {
        case COMM_SS_allAsynchronousServices:
            return(32);
        case COMM_SS_allSynchronousServices:
            return(16);
        case COMM_SS_ALL_SMS:  // sms
            return (8);
        case COMM_SS_ALL_FAX_SERVICES: // fax
            return (4);
        case COMM_SS_AllBearerServices:
            return (2);
        case COMM_SS_TELEPHONY:  // voice
            return (1);
        default:
            return (0);
    }
}

PRIVATE VOID ss_SetCF_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCF,
                           SS_SM_INFO *proc)
{
    UINT8 nNum = 0;
    UINT8 i;
    UINT32 nUTI;
    UINT8 Len    = 0;
    UINT8 nType  = CFW_TELNUMBER_TYPE_UNKNOWN;
    UINT8 nclass = 0;

    CSW_PROFILE_FUNCTION_ENTER(ss_SetCF_Conf);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_SetCF_Conf, input param hAo=%d\n",0x08100670)), hAo);

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    // Basic service code

    pSetCF->nClass = proc->nClass;

    // mode operation
    switch (proc->nApi_SS_Operation)
    {
        case ss_Deactivate:
            pSetCF->nMode = 0;
            break;
        case ss_Activate:
            pSetCF->nMode = 1;
            break;
        case ss_Register:
            pSetCF->nMode = 3;
            break;
        case ss_Erasure:
            pSetCF->nMode = 4;
            break;
    }

    // reason  ss_code
    pSetCF->nReason = ss_GetMmiServCode(proc->nApi_SS_Code);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Mmi_reason=%d\n",0x08100671), pSetCF->nReason);

    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_bForwardingFeatureListPresent != FALSE)
    {
        nNum = pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.ss_numberOfForwardingFeatureListElements;
        for (i = 0; i < nNum; i++)
        {

            pSetCF->nClass = proc->nClass;
            nclass         =
                GetBSMmiCode(pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                             ss_forwardingFeatureListElement[i].ss_basicServiceCode,
                             pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                             ss_forwardingFeatureListElement[i].ss_serviceCodeCatagory);

            if (pSetCF->nClass != nclass || pInMsg->ss_Code != proc->nApi_SS_Code)
            {
                continue;
            }
            else
            {
                // Forwarded_to_number
                if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                        ss_forwardingFeatureListElement[i].ss_bForwardedToNumberPresent != FALSE)
                {
                    // Len=pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                    // ss_forwardingFeatureListElement[i].ss_numberOfForwardedToNumberElements;
                    // BCD2Stri(&(pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                    // ss_forwardingFeatureListElement[i].ss_forwardedToNumber[0]),
                    // &(pSetCF->number.pDialNumber[0]),Len);
                    //
                    // nType=pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                    // ss_forwardingFeatureListElement[i].ss_forwardedToNumber[0];
                    // if (nType==CFW_TELNUMBER_TYPE_INTERNATIONAL)
                    // pSetCF->number.nType=CFW_TELNUMBER_TYPE_INTERNATIONAL;
                    // else
                    // pSetCF->number.nType=CFW_TELNUMBER_TYPE_UNKNOWN;
                    //
                    // pSetCF->number.nDialNumberSize=(UINT8)SUL_Strlen(pSetCF->number.pDialNumber);
                    Len = pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                          ss_forwardingFeatureListElement[i].ss_numberOfForwardedToNumberElements;
                    if (Len > 1)
                    {
                        SUL_MemCopy8(pSetCF->nNumber.pDialNumber,
                                     &(pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                                       ss_forwardingFeatureListElement[i].ss_forwardedToNumber[1]), Len - 1);

                        pSetCF->nNumber.nDialNumberSize = (UINT8)Len - 1;
                    }
                    else
                    {
                        pSetCF->nNumber.nDialNumberSize = 0;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Len <=1 !! \n",0x08100672));
                    }

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("the following is Network Num\n",0x08100673));
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("0x%x,0x%x,0x%x--0x%x,0x%x,0x%x\n",0x08100674)),
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[0],
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[1],
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[2],
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[3],
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[4],
                              pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                              ss_forwardingFeatureListElement[i].ss_forwardedToNumber[5]);

                    nType =
                        pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.ss_forwardingFeatureListElement[i].
                        ss_forwardedToNumber[0];
                    if (nType == CFW_TELNUMBER_TYPE_INTERNATIONAL)
                        pSetCF->nNumber.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                    else
                        pSetCF->nNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTR("ForwardedToNum:%x,%x,%x,%x, %x,%x\n",0x08100675),
                              pSetCF->nNumber.pDialNumber[0],
                              pSetCF->nNumber.pDialNumber[1],
                              pSetCF->nNumber.pDialNumber[2],
                              pSetCF->nNumber.pDialNumber[3],
                              pSetCF->nNumber.pDialNumber[4],
                              pSetCF->nNumber.pDialNumber[5]);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("NumSize=%x,NumType=%d\n",0x08100676), pSetCF->nNumber.nDialNumberSize, pSetCF->nNumber.nType);

                }

                // pForwardingInd->padding

                // No reply condition time
                if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.ss_ForwardingFeatureList.
                        ss_forwardingFeatureListElement[i].ss_bNoReplyConditionTimePresent != FALSE)
                {
                    pSetCF->nTime = pInMsg->ss_ReturnResult.ss_RetRslt.ss_FI.
                                    ss_ForwardingFeatureList.ss_forwardingFeatureListElement[i].ss_noReplyConditionTime;

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("time=%d\n",0x08100677), pSetCF->nTime);

                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_SetCF_Conf,api_reason=0x%x\n",0x08100678), pInMsg->ss_Code);
            }
        }
    }

    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_SetCF_Conf, uti=%d\n",0x08100679), nUTI);

    CFW_PostNotifyEvent(EV_CFW_SS_SET_CALL_FORWARDING_RSP, (UINT32)pSetCF, SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO),
                        nUTI | (nSimID << 24), 0);

    CSW_PROFILE_FUNCTION_EXIT(ss_SetCF_Conf);
    return;

    // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
}

PRIVATE VOID ss_InterCFList_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_QUERY_CALLFORWARDING_RSP *pForwardingInd,
                                 SS_SM_INFO *proc)
{
    UINT8 nNum    = 0;
    UINT8 i       = 0;
    UINT8 nstatus = 0;
    UINT32 nUTI;
    UINT8 nType = CFW_TELNUMBER_TYPE_UNKNOWN;
    UINT8 nLen  = 0;

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);

    CSW_PROFILE_FUNCTION_ENTER(ss_InterCFList_Conf);

    // Basic service code

    pForwardingInd->nClass = proc->nClass;
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf, MMIClass=%x\n",0x0810067a), pForwardingInd->nClass);

    // reason  ss_code
    pForwardingInd->nReason = ss_GetMmiServCode(proc->nApi_SS_Code);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_InterCFList_Conf,Mmi_reason=%x\n",0x0810067b)), pForwardingInd->nReason);

    nNum = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.ss_numberOfForwardingFeatureListElements;
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_InterCFList_Conf, loop nNum=%x\n",0x0810067c)), nNum);
    for (i = 0; i < nNum; i++)
    {
        pForwardingInd->nClass =
            GetBSMmiCode(pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.ss_forwardingFeatureListElement[i].
                         ss_basicServiceCode,
                         pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.ss_forwardingFeatureListElement[i].
                         ss_serviceCodeCatagory);

        if (pForwardingInd->nClass != proc->nClass)

        {
            continue;
        }
        else
        {
            // number
            if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.ss_forwardingFeatureListElement[i].
                    ss_bForwardedToNumberPresent != FALSE)
            {
                nLen =
                    pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.ss_forwardingFeatureListElement[i].
                    ss_numberOfForwardedToNumberElements;
                SUL_MemCopy8(pForwardingInd->nNumber.nTelNumber,
                             &(pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.
                               ss_forwardingFeatureListElement[i].ss_forwardedToNumber[1]), nLen - 1);

                // Number Len
                if (nLen > 1)
                {
                    pForwardingInd->nNumber.nSize = nLen - 1;
                }

                // Number Type
                pForwardingInd->nNumber.nType = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.
                                                ss_forwardingFeatureListElement[i].ss_forwardedToNumber[0];
                nType = pForwardingInd->nNumber.nType;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf, API_NumType=%d\n",0x0810067d), nType);

            }
            /*CSW_TRACE(CFW_SS_TS_ID | C_DETAIL, "ss_InterCFList_Conf, number=%x,%x,%x,%x,  %x,%x,%x,%x\n",
                      pForwardingInd->nNumber.nTelNumber[0],
                      pForwardingInd->nNumber.nTelNumber[1],
                      pForwardingInd->nNumber.nTelNumber[2],
                      pForwardingInd->nNumber.nTelNumber[3],
                      pForwardingInd->nNumber.nTelNumber[4],
                      pForwardingInd->nNumber.nTelNumber[5],
                      pForwardingInd->nNumber.nTelNumber[6], pForwardingInd->nNumber.nTelNumber[7]);*/

            // status
            if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.
                    ss_FFL.ss_forwardingFeatureListElement[i].ss_bStatusPresent != FALSE)
            {
                nstatus = pInMsg->ss_ReturnResult.ss_RetRslt.
                          ss_INT.ss_IntRsltChoice.ss_FFL.ss_forwardingFeatureListElement[i].ss_status;
                nstatus                 = nstatus & 0x01;
                pForwardingInd->nStatus = nstatus;

            }
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf, status=%x\n",0x0810067e), pForwardingInd->nStatus);

            // No reply condition time
            if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.
                    ss_forwardingFeatureListElement[i].ss_bNoReplyConditionTimePresent != FALSE)
            {
                pForwardingInd->nTime = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_FFL.
                                        ss_forwardingFeatureListElement[i].ss_noReplyConditionTime;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf, time=%x\n",0x0810067f), pForwardingInd->nTime);

            }
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf api_reason=%x\n",0x08100680), pInMsg->ss_Code);
        }
    }

    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_InterCFList_Conf,uti=%d\n",0x08100681), nUTI);

    CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CALL_FORWARDING_RSP, (UINT32)pForwardingInd,
                        (SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP)), nUTI | (nSimID << 24), 0);

    // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
    CSW_PROFILE_FUNCTION_EXIT(ss_InterCFList_Conf);
    return;
}

#ifdef SS_NO_RETURNRESULT_HANDLE

// This function is to deal with case no return result back from network.
// We ,Both CSW and STACK,are not sure about this case, In the real network, it is successful case.
// So we take  operations that need no data back as successful, and that need data back as fail.
// And it may be not precision. We just do it acroding to DMX's explation and there may be some bugs.

PRIVATE VOID ss_SetCF_NoRet_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCF,
                                 SS_SM_INFO *proc)
{
    UINT32 nUTI;

    // UINT8 Len=0;
    UINT8 nType = CFW_TELNUMBER_TYPE_UNKNOWN;

    // UINT8 nclass=0;

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);

    CSW_PROFILE_FUNCTION_ENTER(ss_SetCF_NoRet_Conf);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ss_SetCF_NoRet_Conf, begin\n",0x08100682));

    // Basic service code

    pSetCF->nClass = proc->nClass;

    // mode operation
    switch (proc->nApi_SS_Operation)
    {
        case ss_Deactivate:
            pSetCF->nMode = 0;
            break;
        case ss_Activate:
            pSetCF->nMode = 1;
            break;
        case ss_Register:
            pSetCF->nMode = 3;
            break;
        case ss_Erasure:
            pSetCF->nMode = 4;
            break;
    }

    pSetCF->nNumber.nType           = nType;
    pSetCF->nNumber.nDialNumberSize = 0;

    // reason  ss_code
    pSetCF->nReason = ss_GetMmiServCode(proc->nApi_SS_Code);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Mmi_reason=%d\n",0x08100683), pSetCF->nReason);

    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x08100684), nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("mode=%d\n",0x08100685), pSetCF->nMode);

    CFW_PostNotifyEvent(EV_CFW_SS_SET_CALL_FORWARDING_RSP, (UINT32)pSetCF, SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO),
                        nUTI | (nSimID << 24), 0);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ss_SetCF_NoRet_Conf, end\n",0x08100686));
    CSW_PROFILE_FUNCTION_EXIT(ss_SetCF_NoRet_Conf);

    // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
}

PRIVATE VOID ss_Api_Cnf_No_Retresult(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc)
{
    UINT32 nEventID = 0;
    UINT32 param1   = 0;
    UINT32 param2   = 34;
    UINT8 nType     = 0xFB;
    UINT32 nUTI;
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);

    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_Api_Cnf_No_Retresult, No result uti=%x\n",0x08100687), nUTI);
    param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

    switch (proc->nApi_SS_Code)
    {
        case ss_CLIP:
            nEventID = EV_CFW_SS_QUERY_CLIP_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CLIP_No ret\n",0x08100688)));
            break;
        case ss_CLIR:
            nEventID = EV_CFW_SS_QUERY_CLIR_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CLIR_No ret\n",0x08100689)));
            break;
        case ss_COLP:
            nEventID = EV_CFW_SS_QUERY_COLP_RSP;
            param2   = 0;
            nType    = 0;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_COLP_No ret\n",0x0810068a)));
            break;
        case ss_COLR:
            nEventID = EV_CFW_SS_QUERY_COLR_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_COLr_No ret\n",0x0810068b)));
            break;
        case ss_CW:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_CALL_WAITING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CW_No ret\n",0x0810068c)));
            }
            else
            {
                nEventID = EV_CFW_SS_SET_CALL_WAITING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CW_No ret\n",0x0810068d)));
                param2 = 0;
                nType  = 0;
            }
            if (pInMsg->ss_Operation == ss_Activate)
                param1 = 1;
            else if (pInMsg->ss_Operation == ss_Deactivate)
                param1 = 0;

            break;
        case ss_CFA:
        case ss_CFU:
        case ss_CFC:
        case ss_CFB:
        case ss_CFNRY:
        case ss_CFNRC:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_CALL_FORWARDING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CF_No ret\n",0x0810068e)));
            }
            else
            {
                CFW_SS_SET_CALLFORWARDING_RSP_INFO *pSetCFInd = NULL;
                pSetCFInd = (CFW_SS_SET_CALLFORWARDING_RSP_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO) + 24);
                if (pSetCFInd == NULL)
                {
                    UINT32 param3 = 0;

                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]ss_Api_Cnf_No_Retresult, 15,Malloc Mem ERR\n",0x0810068f)));
                    CFW_GetUTI(hAo, &nUTI);
                    param3 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);

                    CFW_PostNotifyEvent(EV_CFW_SS_SET_CALL_FORWARDING_RSP, param3, 14, nUTI | (nSimID << 24), 0xFA);
                    return;
                }
                SUL_ZeroMemory8(pSetCFInd, SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO) + 24);
                pSetCFInd->nNumber.pDialNumber = (UINT8 *)((UINT8 *)pSetCFInd + SIZEOF(CFW_SS_SET_CALLFORWARDING_RSP_INFO));

                ss_SetCF_NoRet_Conf(hAo, pInMsg, pSetCFInd, proc);

                // nEventID=EV_CFW_SS_SET_CALL_FORWARDING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CF_No ret\n",0x08100690)));
                return;
            }
            break;
        case ss_AllBarringSS:
        case ss_BarringOfOutgoingCalls:
        case ss_BAOC:
        case ss_BOIC:
        case ss_BOIC_ExHC:
        case ss_BarringOfIncomingCalls:
        case ss_BAIC:
        case ss_BIC_ROAM:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_FACILITY_LOCK_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CB_No ret\n",0x08100691)));
            }
            else if (pInMsg->ss_Operation == ss_RegisterPassword)
            {
                nEventID = EV_CFW_SS_CHANGE_PWD_RSP;
                param2   = 0;
                nType    = 0;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CHANGE_PWD_No ret\n",0x08100692)));
            }
            else
            {
                UINT32 nStatus = 0;
                UINT8 nClass = 1;

                nEventID = EV_CFW_SS_SET_FACILITY_LOCK_RSP;
                nType    = 0;

                if (proc->nApi_SS_Operation == ss_Activate)
                    nStatus = 1;
                else
                    nStatus = 0;

                param2 = (nClass << 8) | nStatus;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CB_No ret\n",0x08100693)));
            }
            break;
        case ss_USSD_V2:

            nEventID = EV_CFW_SS_SEND_USSD_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SEND_USSD_V2_No ret\n",0x08100694)));
            break;
        case ss_USSD_V1:
            nEventID = EV_CFW_SS_SEND_USSD_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SEND_USSD_V1_No ret\n",0x08100695)));
            break;
    }

    CFW_PostNotifyEvent(nEventID, param1, param2, nUTI | (nSimID << 24), nType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ss_Api_Cnf_No_Retresult, end\n",0x08100696)));

}
#endif

PRIVATE VOID ss_Api_Err_Ind(HAO hAo, api_SsErrorInd_t *pInMsg, SS_SM_INFO *proc)
{
    UINT32 nEventID = 0;
    UINT32 param1   = 0;
    UINT32 param2   = 0;
    UINT8 nType     = 0;
    UINT32 nUTI;

    CFW_SS_USSD_IND_INFO *pUSSD_info_temp = NULL;
    CSW_PROFILE_FUNCTION_ENTER(ss_Api_Err_Ind);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("[ERROR]ss_Api_Err_Ind, pInMsg->ss_ErrorSource:%x\n",0x08100697), pInMsg->ss_ErrorSource);
    switch (pInMsg->ss_ErrorSource)
    {
        case ss_LocalError:  // CISS Module returned a local error, look in ss_Errors.ss_LocalErrorCode
            nType  = 0x0FA;
            param2 = pInMsg->ss_Errors.ss_LocalErrorCode;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("local err:%d\n",0x08100698), param2);
            break;

        // Nwk returned a return error component, look in ss_Errors.ss_NetworkErrorCode
        case ss_ReturnErrorComponent:
            nType  = 0x0FB;
            param2 = pInMsg->ss_Errors.ss_NetworkErrorCode;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Network return err:%d\n",0x08100699), param2);
            break;

        // Network returned a reject component, look in ss_Errors.ss_ProblemCodeandTag
        case ss_RejectComponent:

            if(((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x80) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x00))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x80) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x01))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x80) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x02))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x00))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x02))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x03))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x04))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x05))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x06))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x81) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x07))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x82) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x00))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x82) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x01))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x82) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x02))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x83) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x00))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x83) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x01))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x83) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x02))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x83) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x03))
                    || ((pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag == 0x83) && (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode == 0x04))
              )
            {
                nType  = 0x0FA;////modify by wulc for gcf test 31.9.1.2
            }
            else
                nType  = 0x0FC;
            param2 = (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCodeTag << 8) |
                     (pInMsg->ss_Errors.ss_ProblemCodeTagandCode.ss_ProblemCode);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Network reject:%d\n",0x0810069a), param2);
            break;
        case ss_CauseIE: // Nwk returned a Cause IE, look in ss_Errors.ss_CauseIEInfo
            if( ss_USSD_V2 == proc->nApi_SS_Code )
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ss_CauseValue 0x%x\n",0x0810069b)), pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue);
                //0 0   1   1   1   0   1   29. Facility rejected
                if((0x1D != pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue) &&
                        (0x26 != pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue)&&
                        (0x15 != pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue)&&
                        (0x1F != pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue))
                {
                    nType = 0x00;
                    pUSSD_info_temp = (CFW_SS_USSD_IND_INFO *)CSW_SS_MALLOC(SIZEOF(CFW_SS_USSD_IND_INFO));
                    if(NULL == pUSSD_info_temp)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]CFW_SsMtAoProc, 2,Malloc Mem ERR\n",0x0810069c)));
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SsMtAoProc);
                        return ;
                    }
                    SUL_ZeroMemory32(pUSSD_info_temp, SIZEOF(CFW_SS_USSD_IND_INFO));
                    pUSSD_info_temp->pUsdString = NULL;

                    pUSSD_info_temp->nDcs = 0x00;

                    pUSSD_info_temp->nStingSize = 0x00;
                    pUSSD_info_temp->nOption = 6; //USSD terminated by network without display anything
                    param2 = pInMsg->ss_Code;
                }
                else
                {
                    //nType  = 0x0FC; //modify by wulc for gcf test 31.9.1.2
                    nType  = 0x0FA;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Network reject:%d\n",0x0810069d), param2);
                    break;
                }
            }
            else
            {
                nType  = 0x0FD;
                param2 = pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseValue;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" return CauseIEInfo error: CauseValue=%d\n",0x0810069e), param2);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" return CauseIEInfo error: CodingStandard=%d\n",0x0810069f),
                          pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseIECodingStandard);
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" return CauseIEInfo error: Location= %d\n",0x081006a0),
                          pInMsg->ss_Errors.ss_CauseIEInfo.ss_CauseIELocation);
            }

            break;
    } // pForwardingInd->reason=ss_GetMmiServCode(proc->nApi_SS_Code);

    param1 = (pInMsg->ss_Code << 16) | (pInMsg->ss_Operation);
    CFW_GetUTI(hAo, &nUTI);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("proc->nApi_SS_Code=%d\n",0x081006a1)), proc->nApi_SS_Code);
    switch (proc->nApi_SS_Code)
    {
        case ss_CLIP:
            nEventID = EV_CFW_SS_QUERY_CLIP_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CLIP_Err\n",0x081006a2)));
            break;
        case ss_CLIR:
            nEventID = EV_CFW_SS_QUERY_CLIR_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CLIR_Err\n",0x081006a3)));
            break;
        case ss_COLP:
            nEventID = EV_CFW_SS_QUERY_COLP_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_COLP_Err\n",0x081006a4)));
            break;
        case ss_COLR:
            nEventID = EV_CFW_SS_QUERY_COLR_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_COLr_Err\n",0x081006a5)));
            break;
        case ss_CW:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_CALL_WAITING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CW_Err\n",0x081006a6)));
            }
            else
            {
                nEventID = EV_CFW_SS_SET_CALL_WAITING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CW_Err\n",0x081006a7)));
            }
            break;
        case ss_CFA:
        case ss_CFU:
        case ss_CFC:
        case ss_CFB:
        case ss_CFNRY:
        case ss_CFNRC:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_CALL_FORWARDING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CF_Err\n",0x081006a8)));
            }
            else
            {
                nEventID = EV_CFW_SS_SET_CALL_FORWARDING_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CF_Err\n",0x081006a9)));
            }
            break;
        case ss_AllBarringSS:
        case ss_BarringOfOutgoingCalls:
        case ss_BAOC:
        case ss_BOIC:
        case ss_BOIC_ExHC:
        case ss_BarringOfIncomingCalls:
        case ss_BAIC:
        case ss_BIC_ROAM:
            if (pInMsg->ss_Operation == ss_Interrogate)
            {
                nEventID = EV_CFW_SS_QUERY_FACILITY_LOCK_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("QUERY_CB_Err\n",0x081006aa)));
            }
            else if (pInMsg->ss_Operation == ss_RegisterPassword)
            {
                nEventID = EV_CFW_SS_CHANGE_PWD_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CHANGE_PWD_Err\n",0x081006ab)));
            }
            else
            {
                nEventID = EV_CFW_SS_SET_FACILITY_LOCK_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SET_CB_Err\n",0x081006ac)));
            }
            break;
        case ss_USSD_V2:
            if(( ss_CauseIE == pInMsg->ss_ErrorSource ) && ( 0x00 == nType ))
            {
                param1 = (UINT32)pUSSD_info_temp;
                nEventID    = EV_CFW_SS_USSD_IND;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("EV_CFW_SS_USSD_IND Extern\n",0x081006ad)));
            }
            else
            {
                nEventID    = EV_CFW_SS_SEND_USSD_RSP;
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("SEND_USSD_V2_Err\n",0x081006ae)));

            }
            break;
        case ss_USSD_V1:
            nEventID = EV_CFW_SS_SEND_USSD_RSP;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SEND_USSD_Err\n",0x081006af)));
            break;
    }

    CFW_PostNotifyEvent(nEventID, param1, param2, nUTI | (nSimID << 24), nType);

    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("ss_Api_Err_Ind,end, eventid=0x%x,param1=0x%x, param2=0x%x\n",0x081006b0)), nEventID, param1,
              param2);
    CSW_PROFILE_FUNCTION_EXIT(ss_Api_Err_Ind);
    return;
}

PRIVATE VOID ss_Inter_BSGroupList_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc)
{
    UINT32 param1  = 0;
    UINT32 param2  = 0;
    UINT32 nCalss  = 0;
    UINT32 nStatus = 0;
    UINT8 nNum     = 0;
    UINT32 nUTI;
    UINT8 i;

    CSW_PROFILE_FUNCTION_ENTER(ss_Inter_BSGroupList_Conf);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);

    // class p2
    nCalss = ss_GetMMiClass(proc->nClass);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_Inter_BSGroupList_Conf, pInMsg->ss_Code=%d\n",0x081006b1), pInMsg->ss_Code);

    // status    p1
    nNum = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_BSL.numberOfBasicServiceGroupElements;
    for (i = 0; i < nNum; i++)
    {
        if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_BSL.ss_BasicServiceGroupElement[i].
                ss_ServiceCodeCategory != COMM_SS_ServicecodeNotApplicable)
        {
            ss_SetMmiBitStatus(pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_BSL.
                               ss_BasicServiceGroupElement[i].ss_ServiceCodeCategory,
                               pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_BSL.
                               ss_BasicServiceGroupElement[i].BasicServiceCode, 1, &nStatus, nCalss);

        }
    }
    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006b2), nUTI);
    switch (pInMsg->ss_Code)
    {
        case ss_CW:
            param1 = nStatus;
            param2 = nCalss;

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CALL_WAITING_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_CW(ACTIVE) :TI=%x ,param1=%x, param2=%x \n",0x081006b3), pInMsg->TI, param1, param2);
            break;
        case ss_AllBarringSS:
        case ss_BarringOfOutgoingCalls:
        case ss_BAOC:
        case ss_BOIC:
        case ss_BOIC_ExHC:
        case ss_BAIC:
        case ss_BIC_ROAM:
        case ss_BarringOfIncomingCalls:

            // param1
            param1 = ss_GetMmiServCode(proc->nApi_SS_Code);
            param2 = (nCalss << 8) | nStatus;

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_FACILITY_LOCK_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Inter_Baring:(Active)Ti=%x,p1=%x,p2=%x\n",0x081006b4), pInMsg->TI, param1, param2);
            break;
        default:
            break;
    }

    // CFW_SetAoState(hAo,CFW_SM_STATE_IDLE);
    CSW_PROFILE_FUNCTION_EXIT(ss_Inter_BSGroupList_Conf);
    return;
}

PRIVATE VOID ss_SetCBInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc)
{
    UINT32 nUTI;
    UINT32 param1  = 0;
    UINT32 param2  = 0;
    UINT32 nStatus = 0;
    UINT32 nCalss  = 0;
    UINT8 nTemp    = 0;
    UINT8 i;

    CSW_PROFILE_FUNCTION_ENTER(ss_SetCBInfo_Conf);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_SetCBInfo_Conf,pInMsg->ss_Operation=%d\n",0x081006b5), pInMsg->ss_Operation);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    switch (pInMsg->ss_Operation)
    {
        case ss_RegisterPassword:  // register password
            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006b6), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_CHANGE_PWD_RSP, 0, 0, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ActionCnf_Register_pwd\n",0x081006b7));

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        case ss_Activate:
        case ss_Deactivate:

            // param1  (ss_code | mode)
            if (proc->nApi_SS_Operation == ss_Activate)
                nTemp = 1;
            else
                nTemp = 0;

            param1 = (ss_GetMmiServCode(proc->nApi_SS_Code) << 16) | nTemp;

            // pamar2  (class | status)
            nCalss = ss_GetMMiClass(proc->nClass);

            // status p1
            for (i = 0; i < pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.ss_numberOfCallBarringFeatureListElements; i++)
            {
                if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.ss_callBarringFeatureList[i].ss_bStatusPresent != FALSE)
                {
                    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.ss_callBarringFeatureList[i].ss_bStatusPresent != FALSE)
                    {
                        nTemp = pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.ss_callBarringFeatureList[i].ss_status;
                    }
                    nTemp = nTemp & 0x01;
                }
                ss_SetMmiBitStatus(pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.
                                   ss_callBarringFeatureList[i].
                                   ss_serviceCodeCatagory,
                                   pInMsg->ss_ReturnResult.ss_RetRslt.ss_CB.
                                   ss_callBarringFeatureList[i].ss_basicServiceCode, nTemp, &nStatus, nCalss);
            }
            param2 = (nCalss << 8) | nStatus;
            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006b8), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_SET_FACILITY_LOCK_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Set_Baring:operation=%x, para1=%x, para2=%x \n",0x081006b9),
                      pInMsg->ss_Operation, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        default:
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("should not be here 2!\n",0x081006ba)));
            break;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ss_SetCBInfo_Conf, end\n",0x081006bb));
    CSW_PROFILE_FUNCTION_EXIT(ss_SetCBInfo_Conf);
    return;
}

PRIVATE UINT8 ss_Inter_StatusInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc)
{
    UINT8 ret          = 0;
    UINT32 nstatus     = 0;
    UINT32 nclass      = 0;
    UINT32 param1      = 0;
    UINT32 param2      = 0;
    UINT8 presentation = 0;
    UINT32 nUTI;

    CFW_SS_QUERY_CALLFORWARDING_RSP *pForwardingInd = NULL;

    CSW_PROFILE_FUNCTION_ENTER(ss_Inter_StatusInfo_Conf);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ss_Inter_StatusInfo_Conf, begin, pInMsg->ss_Code=%d\n",0x081006bc), pInMsg->ss_Code);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    switch (pInMsg->ss_Code)
    {
        case ss_CLIP:
            if (CFW_CfgGetClip(&presentation, nSimID) == ERR_SUCCESS)
                param1 = presentation;
            else
                param1 = 0;
            nstatus  = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            nstatus  = (nstatus & 0x04) >> 2;
            param2   = nstatus;

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006bd), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CLIP_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_CLIP:TI=%x ,param1=%x, param2=%x \n",0x081006be), pInMsg->TI, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???  位置
            break;
        case ss_CLIR:  // status_not provided
            if (CFW_CfgGetClir(&presentation, nSimID) == ERR_SUCCESS)
                param1 = presentation;
            else
                param1 = 0;

            nstatus = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            param2  = (nstatus & 0x04) >> 2; // provided or not

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006bf), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CLIR_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("ActionCnf_Query_CLIR(not provided) :TI=%x ,param1=%x\n",0x081006c0), pInMsg->TI, param1);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        case ss_COLP:
            if (CFW_CfgGetColp(&presentation, nSimID) == ERR_SUCCESS)
                param1 = presentation;
            else
                param1 = 0;
            nstatus  = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            nstatus  = (nstatus & 0x04) >> 2;
            param2   = nstatus;

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006c1), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_COLP_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_COLP:TI=%x ,param1=%x, param2=%x \n",0x081006c2), pInMsg->TI, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//?? 位置
            break;
        case ss_COLR:
            param1  = 0;
            nstatus = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            nstatus = (nstatus & 0x04) >> 2;
            param2  = nstatus;

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006c3), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_COLR_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_COLP:TI=%x ,param1=%x, param2=%x \n",0x081006c4), pInMsg->TI, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        case ss_CW:  // (inter_cw_not active)
            nstatus = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            nstatus = nstatus & 0x01;
            param1  = 0;
            param2  = ss_GetMMiClass(proc->nClass);

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006c5), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CALL_WAITING_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_CW(NO ACTIVE) :TI=%x ,param1=%x, param2=%x\n",0x081006c6), pInMsg->TI, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        case ss_AllBarringSS:  // (inter_cb_not active)
        case ss_BarringOfOutgoingCalls:
        case ss_BAOC:
        case ss_BOIC:
        case ss_BOIC_ExHC:
        case ss_BarringOfIncomingCalls:
        case ss_BAIC:
        case ss_BIC_ROAM:
            param1 = ss_GetMmiServCode(proc->nApi_SS_Code);

            // param2
            nstatus = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            nstatus = nstatus & 0x01;
            nclass  = 0;
            nclass  = ss_GetMMiClass(proc->nClass);
            param2  = nclass << 8 | nstatus;

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006c7), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_FACILITY_LOCK_RSP, param1, param2, nUTI | (nSimID << 24), 0);

            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_Baring:(no Actv)Ti=%x,p1=0x%x,p2=0x%x\n",0x081006c8), pInMsg->TI, param1, param2);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        case ss_CFA:
        case ss_CFU:
        case ss_CFC:
        case ss_CFB:
        case ss_CFNRC:
        case ss_CFNRY: // inter_cf_not_active or acitve && quiescent
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ActionCnf_Query_CF(not active)!\n",0x081006c9));
            pForwardingInd = (CFW_SS_QUERY_CALLFORWARDING_RSP *)CSW_SS_MALLOC(SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP));
            if (pForwardingInd == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Malloc Mem ERR\n",0x081006ca)));
                CSW_PROFILE_FUNCTION_EXIT(ss_Inter_StatusInfo_Conf);
                return (1);
            }
            SUL_ZeroMemory32(pForwardingInd, SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP));
            pForwardingInd->nReason = ss_GetMmiServCode(proc->nApi_SS_Code);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Ti=%x,reason=%x\n",0x081006cb)), pInMsg->TI, pForwardingInd->nReason);

            pForwardingInd->nClass = proc->nClass;
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("class=%x\n",0x081006cc)), pForwardingInd->nClass);

            nstatus                 = pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_Status;
            pForwardingInd->nStatus = (UINT8)(nstatus & 0x01);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("status=%x\n",0x081006cd)), pForwardingInd->nStatus);

            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006ce), nUTI);

            CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CALL_FORWARDING_RSP, (UINT32)pForwardingInd,
                                SIZEOF(CFW_SS_QUERY_CALLFORWARDING_RSP), nUTI | (nSimID << 24), 0);

            // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???位置
            break;
        default:
            break;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ss_Inter_StatusInfo_Conf end\n",0x081006cf));
    CSW_PROFILE_FUNCTION_EXIT(ss_Inter_StatusInfo_Conf);
    return ret;
}

PRIVATE VOID ss_Inter_GSInfo_Conf(HAO hAo, api_SsActionCnf_t *pInMsg, SS_SM_INFO *proc)
{
    UINT8 param   = 0;
    UINT32 param1 = 0;
    UINT32 param2 = 0;
    UINT32 nUTI;

    CSW_PROFILE_FUNCTION_ENTER(ss_Inter_GSInfo_Conf);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    if (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_GSI.bCliRestrictionOptionPresent != FALSE)  // 4 here
    {
        switch (pInMsg->ss_ReturnResult.ss_RetRslt.ss_INT.ss_IntRsltChoice.ss_GSI.cliRestrictionOption)
        {
            case 0:  // permanent
                param2 = 1;
                break;
            case 1:  // temporary presentation restricted
                param2 = 3;
                break;
            case 2:  // temporary presentation allowed
                param2 = 4;
                break;
            default: // unknown
                param2 = 2;
                break;
        }
    }
    if (CFW_CfgGetClir(&param, nSimID) == ERR_SUCCESS)
        param1 = param;
    else
        param1 = 0;
    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("uti=%d\n",0x081006d0), nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send the EV_CFW_SS_QUERY_CLIR_RSP to MMI",0x081006d1));

    CFW_PostNotifyEvent(EV_CFW_SS_QUERY_CLIR_RSP, (UINT32)param1, param2, nUTI | (nSimID << 24), 0);

    // CFW_SetAoState( hAo, CFW_SM_STATE_IDLE);//???
    CSW_TRACE(_CSW|TLEVEL(CFW_SS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("ActionCnf_Query_CLIR(provided) :TI=%d ,param1=%d,param2=%d\n",0x081006d2), pInMsg->TI, param1, param2);
    CSW_PROFILE_FUNCTION_EXIT(ss_Inter_GSInfo_Conf);
}

#endif // ENABLE

