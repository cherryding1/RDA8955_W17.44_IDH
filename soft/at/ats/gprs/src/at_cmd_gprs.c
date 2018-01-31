/*********************************************************
 *
 * File Name
 *        at_cmd_gprs.c
 * Author
 *         Caoxh
 * Date
 *         2008/03/18
 * Descriptions:
 *        Commands specific to MTs supporting the Packet Domain.
 *
 *********************************************************/
#ifdef CFW_GPRS_SUPPORT
#include "stdio.h"
#include "cfw.h"
#include "at.h"
#include "at_sa.h"  // caoxh[+]2008-04-19
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cfg.h"
#include "itf_api.h"
#ifdef LTE_NBIOT_SUPPORT
#include "ctype.h"
#endif
#include "sockets.h"
#define AT_MAX_PPP_ID                   8

#ifdef __MULTI_RAT__
extern UINT32 CFW_GprsSetReqQosUmts(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
extern UINT32 CFW_GprsGetReqQosUmts(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
#endif
#ifdef LTE_NBIOT_SUPPORT

extern int  nvmWriteStatic(void);
extern UINT8 nbiot_nvGetVersionControl(void);
extern void nbiot_nvSetVersionControl(UINT8 version);
extern u8 nbiot_nvGetDirectIpMode(void);
extern void nbiot_nvSetDirectIpMode(u8 value);
extern UINT8 nbiot_nvGetEdrxEnable(void);
extern void nbiot_nvSetEdrxEnable(UINT8 enable);
extern UINT8 nbiot_nvGetEdrxValue(void);
extern void nbiot_nvSetEdrxValue(UINT8 value);
extern UINT8 nbiot_nvGetEdrxPtw(void);
extern void nbiot_nvSetEdrxPtw(UINT8 ptw);
extern UINT8 nbiot_nvGetPsmEnable(void);
extern void nbiot_nvSetPsmEnable(UINT8 psmEnable);
extern UINT8 nbiot_nvGetPsmT3412(void);
extern void nbiot_nvSetPsmT3412(UINT8 t3412Val);
extern UINT8 nbiot_nvGetPsmT3324(void);
extern void nbiot_nvSetPsmT3324(UINT8 t3324Val);
extern UINT8 nbiot_nvGetCiotReport(void);
extern void nbiot_nvSetCiotReport(UINT8 ciotReport);
extern UINT8 nbiot_nvGetCiotNonip(void);
extern void nbiot_nvSetCiotNonip(UINT8 nonip);
extern UINT8 nbiot_nvGetCiotCpciot(void);
extern void nbiot_nvSetCiotCpciot(UINT8 cpciot);
extern UINT8 nbiot_nvGetCiotUpciot(void);
extern void nbiot_nvSetCiotUpciot(UINT8 upciot);
extern UINT8 nbiot_nvGetCiotErwopdn(void);
extern void nbiot_nvSetCiotErwopdn(UINT8 erwopdn);
extern UINT8 nbiot_nvGetCiotSmsWoCombAtt(void);
extern void nbiot_nvSetCiotSmsWoCombAtt(UINT8 sms_wo_comb_att);
extern UINT8 nbiot_nvGetCiotApnRateCtrl(void);
extern void nbiot_nvSetCiotApnRateCtrl(UINT8 apn_rate_control);
extern UINT8 nbiot_nvGetCiotEpco(void);
extern void nbiot_nvSetCiotEpco(UINT8 epco);
extern UINT8 nbiot_nvGetDefaultPdnType(void);
extern void nbiot_nvSetDefaultPdnType(UINT8 pdnType);
extern UINT8* nbiot_nvGetDefaultApn(UINT8 i);
extern void nbiot_nvSetDefaultApn(UINT8 i, UINT8 *apn, UINT8 apnLen);
extern UINT8 nbiot_nvGetHccpEnable(void);
extern void nbiot_nvSetHccpEnable(UINT8 enable);
extern UINT8 nbiot_nvGetHccpMaxcid(void);
extern void nbiot_nvSetHccpMaxcid(UINT16 maxCid);
extern UINT8 nbiot_nvGetHccpProfile(void);
extern void nbiot_nvSetHccpProfile(UINT8 profile);
extern UINT8 nbiot_nvGetNasSigLowPri(void);
extern void nbiot_nvSetNasSigLowPri(UINT8 nasSigLowPri);
extern UINT8 nbiot_nvGetNasSigDualPri(void);
extern void nbiot_nvSetNasSigDualPri(UINT8 overrideNasSigLowPri);
extern UINT8 nbiot_nvGetExceptionDataReporting(void);
extern void nbiot_nvSetExceptionDataReporting(UINT8 exceptionDataReportingAllowed);
extern UINT8 nbiot_nvGetT3245Behave(void);
extern void nbiot_nvSetT3245Behave(UINT8 t3245Behaviour);

extern UINT16 nonIpMtu;
extern CFW_GPRS_PDPCONT_INFO *PdpContList[CFW_SIM_COUNT][7];
#endif

BOOL CFW_GprsSendDataAvaliable(UINT8 nSimID);
extern UINT8 ppp_alloc_entity(UINT8 simid, UINT8 cid, UINT8 muxid);
extern void ppp_InitPPPVarsByIdx(UINT8 idx);
UINT32 AT_CFW_GprsAct(UINT8 nState, UINT8 nCid, UINT16 nUTI, CFW_SIM_ID nSimID, COS_CALLBACK_FUNC_T func)
{
    UINT8 idx = 0xFF;
    UINT32 ret = 0;
    ret =  CFW_GprsActEX(nState, nCid, nUTI, nSimID, 0, func);
    return ret;
}

#ifdef LTE_NBIOT_SUPPORT
UINT32 AT_CFW_GprsSecAct(UINT8 nState, UINT8 nCid, UINT8 nPCid,UINT16 nUTI, CFW_SIM_ID nSimID)
{
    UINT32 ret = 0;
    ret =  CFW_GprsSecPdpAct(nState, nCid, nPCid, nUTI, nSimID);
    return ret;
}
#endif
struct GPRS_Act_Wait_Att_Rsp_Info
{
    UINT8 wait_state;
    UINT8 act_state;
    UINT8 act_count;
    UINT8 cid[AT_PDPCID_MAX + 1];
    UINT8 nSim;
    UINT8 nDLCI;
};

static struct GPRS_Act_Wait_Att_Rsp_Info g_uGPRS_Act_Wait_Att_Rsp_Info = {0,};

// Number of cids waiting for response from network for PDP activation
//UINT8 g_uAtWaitActRsp      = 0;
UINT8 g_uAtWaitActRsp[MAX_DLC_NUM]       = {0,};
UINT8 g_Wait_AttRsp_in_Act = 0; // 1-waiting,0-response, 0xff err

extern VOID AT_NW_Result_OK(UINT32 uReturnValue,
                            UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim);
// Attach State
UINT8 g_uAttState = CFW_GPRS_DETACHED;
// Attach State
UINT8 g_uAtGprsAttState = 1;  // +CGATT: <state>
#ifdef LTE_NBIOT_SUPPORT
UINT8 g_uAtCereg = 1;
#endif

UINT8 g_uAtGprsCidNum   = 0;  // yy [add] 2008-6-4 for bug ID 8678
UINT8 g_uATGprsCGEVFlag = 0;  // for control auto report +CGEV to TE
typedef enum
{
    AUTO_RSP_STU_OFF_PDONLY,
    AUTO_RSP_STU_ON_PDONLY,
    AUTO_RSP_STU_MOD_CAP_PDONLY,
    AUTO_RSP_STU_MOD_CAP_PD_CS,
    AUTO_RSP_STATUS_MAX,
} AT_GPRS_AUTO_RSP;

//
// Set the return condition.
//
#define GPRSRETURN(c,r) \
    if ( c ) \
return r;
AT_Gprs_CidInfo g_staAtGprsCidInfo_e[4][AT_PDPCID_MAX + 1] = { {{1,},}, };
extern UINT8 at_GprsGetMatchedPdpCid(CFW_GPRS_PDPCONT_INFO *pPdpInfo);
extern UINT32 at_GprsGetCtxDefStatus(UINT8 cid);

UINT8 DlciGetCid(UINT8 nDLCI)
{
    AT_TC(g_sw_GPRS, "%s, nDLCI = %d,", __func__, nDLCI);
    int i = AT_PDPCID_MIN;
    UINT8 nSim = AT_SIM_ID(nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    for (i = AT_PDPCID_MIN; i <= AT_PDPCID_MAX; i++)
    {
        // if (g_staAtGprsCidInfo[i].nDLCI == nDLCI)
        // {
        //     AT_TC(g_sw_GPRS, "%s, nDLCI = %d,cid=%d", __func__, nDLCI, g_staAtGprsCidInfo[i].uCid);
        //     return g_staAtGprsCidInfo[i].uCid;
        // }
        if (g_staAtGprsCidInfo[i].uState == 1)
        {
            return g_staAtGprsCidInfo[i].uCid;
        }
    }
    return 0;
}

#ifdef LTE_NBIOT_SUPPORT
// Set PCid in EPS MT Dedicated Bearer Activate Procedure
UINT8 AT_GprsSetPCid(UINT8 nCid, UINT8 nPCid, UINT8 nSim)
{
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    
    SUL_MemCopy8(&g_staAtGprsCidInfo[nCid], &g_staAtGprsCidInfo[nPCid], sizeof(AT_Gprs_CidInfo));

    g_staAtGprsCidInfo[nCid].uCid  = nCid;
    g_staAtGprsCidInfo[nCid].uPCid = nPCid;
    return 0;
}
#endif

UINT8 AT_GprsGetPdpAutoStatus(VOID)
{
    return gATCurrentu8nURCAuto;
}

void AT_GprsStatusInd(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEv;
    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    UINT8 nSim = CfwEv.nFlag;

    AT_TC(g_sw_GPRS, "We got 111 EV_CFW_GPRS_STATUS_IND");
#ifdef LTE_NBIOT_SUPPORT
    CFW_NW_STATUS_INFO sStatus;
    UINT8 respond[100] = {0};
    if(CFW_NWGetRat(nSim) == CFW_RAT_NBIOT_ONLY) //nbiot branch
    {
        CFW_GprsGetstatus(&sStatus, nSim);
        if(1 != AT_Cereg_respond(respond, &sStatus, pEvent->nParam1, FALSE))
        {
            AT_TC(g_sw_GPRS, "g_uAtCereg: %d", g_uAtCereg);
            return;
        }
        at_CmdReportUrcText(nSim, respond);
    }
    else
#else
    {
        if (gATCurrentCgreg != 0)
        {
            UINT8 respond[30] = {0x00,};
            AT_Sprintf(respond, "+CGREG: %d", pEvent->nParam1);
            at_CmdReportUrcText(nSim, respond);
        }
    }
#endif
    AT_ZERO_PARAM1(pEvent);
}

#if defined(CFW_GPRS_SUPPORT) && !defined(AT_WITHOUT_GPRS)
static void PsDownlinkDataHandler(CFW_GPRS_DATA *pGprsData, UINT8 cid, UINT8 nDLCI)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
    at_CmdWrite(engine, pGprsData->pData, pGprsData->nDataLength);
}
#endif

// ---- ------------temp area begin --------------
// [[ yy [del] 2008-7-1 for CSW update
// [[ yy [del] 2008-7-1 for CSW update

extern UINT8 CFW_GprsGetSmsSeviceMode(void);

extern UINT8 CFW_GprsSetSmsSeviceMode(UINT8 nService);

#ifndef LTE_NBIOT_SUPPORT
static 
#endif
VOID AT_GPRS_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEv;
    //CFW_GPRS_PDPCONT_INFO *pPdpInfo;
    INT32 iResult;
    UINT8 uUIT;
    UINT8 nCid;
    UINT8 nS0Staus;
    TCHAR buffer[100] = { 0 };
    UINT8 nSim;
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;
    AT_TC(g_sw_TCPIP, "AT_GPRS_AsyncEventProcess Got %s: %d,0x%x,0x%x,0x%x",
        TS_GetEventName(pEvent->nEventId),pEvent->nEventId,pEvent->nParam1,pEvent->nParam2,pEvent->nParam3);

    switch (CfwEv.nEventId)
    {
    case EV_CFW_GPRS_CXT_ACTIVE_IND:
        AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_CXT_ACTIVE_IND");
        if (0xF0 == CfwEv.nType)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, CfwEv.nUTI, nSim);
            break;
        }

        //pPdpInfo = (CFW_GPRS_PDPCONT_INFO *)(CfwEv.nParam1);
        // Get matching pdp contex.
        //nCid = at_GprsGetMatchedPdpCid(pPdpInfo);
        nCid = CfwEv.nParam1;
        pEvent->nParam1 = 0;
        AT_TC(g_sw_GPRS, "KEN :: the mathing cid = %d", nCid);
        if (nCid == AT_PDPCID_ERROR)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, CfwEv.nUTI, nSim);
            break;
        }
        if (AUTO_RSP_STU_OFF_PDONLY == gATCurrentu8nURCAuto)
        {
            SUL_StrPrint(buffer, "A activae request of cid=<%d> has araising\n Please type AT+CGANS=1 accept...\n", nCid);
            AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_OK, 0, buffer, AT_StrLen(buffer), CfwEv.nUTI, nSim);
        }
        else
        {
            nS0Staus = AT_GC_CfgGetS0Value();
            if (AUTO_RSP_STU_ON_PDONLY == gATCurrentu8nURCAuto ||
                    (nS0Staus > 0 &&
                     (AUTO_RSP_STU_MOD_CAP_PD_CS == gATCurrentu8nURCAuto || AUTO_RSP_STU_MOD_CAP_PDONLY == gATCurrentu8nURCAuto)))
            {
                uUIT = 200 + 6 + nCid;
                iResult = AT_CFW_GprsAct(AT_GPRS_ACT, nCid, CfwEv.nUTI, nSim, AT_GPRS_AsyncEventProcess);
                AT_TC(g_sw_GPRS, "KEN :: AUTO_RSP_STU_ON_PDONLY cid = %d, AT_CFW_GprsAct result = 0X%8x", nCid, iResult);
                if (ERR_SUCCESS != iResult)
                {
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, CfwEv.nUTI, nSim);
                    break;
                }

                if (g_uAtWaitActRsp[CfwEv.nUTI] > 0)
                {
                    g_uAtWaitActRsp[CfwEv.nUTI]--;
                }

                AT_TC(g_sw_GPRS, "KEN :: Asyn g_uAtWaitActRsp = %d", g_uAtWaitActRsp[CfwEv.nUTI]);
            }
        }
        break;

    case EV_CFW_GPRS_ATT_RSP:
        pEvent->nParam1 = 0;
        AT_GPRS_Att_Rsp(&CfwEv);
        break;

    case EV_CFW_GPRS_CXT_DEACTIVE_IND:
        AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_CXT_DEACTIVE_IND g_uAtGprsCidNum %d", g_uAtGprsCidNum);
		UINT8 dlci = AT_ASYN_GET_DLCI(nSim);
	     if(g_uAtGprsCidNum)
                    g_uAtGprsCidNum--;
	     if (g_uAtWaitActRsp[CfwEv.nUTI] > 0)
                    g_uAtWaitActRsp[CfwEv.nUTI]--;
#ifdef LTE_NBIOT_SUPPORT
		 memset(&g_staAtGprsCidInfo[CfwEv.nParam1], 0x00, sizeof(AT_Gprs_CidInfo));
#else
	     memset(&g_staAtGprsCidInfo[CfwEv.nUTI], 0x00, sizeof(AT_Gprs_CidInfo));
#endif
        if (g_uATGprsCGEVFlag == 1)
        {
            UINT8 nResp[50] = {0x00,};
            AT_Sprintf(nResp, "+CGEV: NW DEACT  \"IP\", ,%d", CfwEv.nParam1);
                
                AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_CXT_DEACTIVE_IND g_uAtGprsCidNum %d", g_uAtGprsCidNum);
            AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nResp, AT_StrLen(nResp), dlci, nSim);
            pEvent->nParam1 = 0;
        }
#ifdef LTE_NBIOT_SUPPORT
		extern VOID ping_timeout(UINT32 *param);
		extern BOOL ping_socket_is_invalid();
		extern void ping_result_statics(UINT8 nDLC, UINT8 *Response);
		if(!ping_socket_is_invalid())
        {
            UINT8 reponse[100] = {0};
            COS_StopCallbackTimer(CSW_AT_TASK_HANDLE,ping_timeout,dlci);
            ping_result_statics(dlci,reponse);
        }
#endif
        break;
    case EV_CFW_GPRS_ACT_RSP:
        pEvent->nParam1 = 0;
        AT_GPRS_Act_Rsp(&CfwEv);
        break;
    case EV_CFW_GPRS_MOD_IND:
    {
        CFW_GprsCtxModifyAcc(AT_ASYN_GET_DLCI(nSim), pEvent->nParam1, nSim);
        pEvent->nParam1 = 0;

        UINT8 nResp[30] = {0x00,};
        AT_Sprintf(nResp, "+CGDMODIND:%d", pEvent->nParam1);

        AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nResp, AT_StrLen(nResp),
                          AT_ASYN_GET_DLCI(nSim), nSim);
    }
    break;
    case EV_CFW_GPRS_STATUS_IND:
	{
        AT_GprsStatusInd(pEvent);
        pEvent->nParam1 = 0;	
	}
    break;
	
#ifdef  LTE_NBIOT_SUPPORT
	case EV_CFW_GPRS_MOD_RSP:
    {
          AT_TC(g_sw_GPRS, "At get EV_CFW_GPRS_MOD_RSP,CfwEv.nParam1 is 0x%x, CfwEv.nParam2 is 0x%x",CfwEv.nParam1,CfwEv.nParam2);
          if(CfwEv.nParam2 == ERR_SUCCESS)
          {
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEv.nUTI, nSim);
          }
          else
          {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, CfwEv.nUTI, nSim);
          }
    }
    return;

	
	
    case EV_CFW_GPRS_APNCR_IND:
	{
	    UINT8  state;
	    AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_APNCR_IND");
		UINT8 cid = ((CfwEv.nParam2 >> 8) & 0xff);
		CFW_GetGprsActState(cid, &state, CfwEv.nFlag);
		if(state == CFW_GPRS_ACTIVED)
		{
            g_staAtGprsCidInfo[cid].apnacAer = CfwEv.nParam2 & 0xff;
            g_staAtGprsCidInfo[cid].apnacUnit = CfwEv.nType;
            g_staAtGprsCidInfo[cid].apnacRate = CfwEv.nParam1;
	        AT_TC(g_sw_GPRS, "apn rate control, aer:%d, unit: %d, rate: %d", g_staAtGprsCidInfo[cid].apnacAer, g_staAtGprsCidInfo[cid].apnacUnit,g_staAtGprsCidInfo[cid].apnacRate);
        }
		else
		{
		    AT_TC(g_sw_GPRS, "apn rate control, pdn is not active!");
		}
	    break;
	}
    case EV_CFW_GPRS_SEC_CTX_ACTIVE_IND:
    {
        AT_TC(g_sw_GPRS, "At got EV_CFW_GPRS_SEC_CTX_ACTIVE_IND");

        if (g_uATGprsCGEVFlag == 1)
        {
            UINT8 nResp[50] = {0x00,};
            AT_Sprintf(nResp, "+CGEV: NW ACT  \"IP\", ,%d", CfwEv.nParam1);
            AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nResp, AT_StrLen(nResp), AT_ASYN_GET_DLCI(nSim), nSim);
        }
    }
    return;
#endif
    case EV_CFW_GPRS_DATA_IND:
        PsDownlinkDataHandler((CFW_GPRS_DATA *)pEvent->nParam1, LOUINT8(pEvent->nParam2), CfwEv.nUTI);
		pEvent->nParam1 = 0;
        break;
    default:

        // AT_SS_EVENT_FAIL_RETURN(ppstResult, ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0);
        break;
    }
    AT_FREE(pEvent);
}

void _DebugOutPut(UINT8 *pData, UINT16 nLen)
{
    AT_TC(g_sw_GPRS, "FTP LOG: len=%d, pData=%s\n", nLen, pData);
}

extern INT32 CFW_FTPGetFile(UINT8 *filename,UINT32 offset);
//#define FTP_DATA_PROCESS
//INT32 AT_FTP_GetTheNewestFile(UINT8 *OFileName);
//#define FTP_FILE_NAME_LEN 100

VOID AT_GPRS_Act_Rsp(CFW_EVENT *pCfwEv)
{
    INT32 iResult;
    UINT8 nSim = pCfwEv->nFlag;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(pCfwEv->nUTI);

    if (g_uAtWaitActRsp[pCfwEv->nUTI] > 0)
    {
        g_uAtWaitActRsp[pCfwEv->nUTI]--;
    }

    if ((pCfwEv->nType != CFW_GPRS_ACTIVED) && (pCfwEv->nType != CFW_GPRS_DEACTIVED))
    {
        if (AT_isWaitingEvent(EV_CFW_GPRS_ACT_RSP, nSim, pCfwEv->nUTI))
        {
            AT_DelWaitingEvent(EV_CFW_GPRS_ACT_RSP, nSim, pCfwEv->nUTI);

            AT_TC(g_sw_GPRS, "<cid> %d, active/deactive operation failed: %d", pCfwEv->nParam2, pCfwEv->nParam1);
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pCfwEv->nUTI, nSim);
        }
        else
            AT_TC(g_sw_GPRS, "---xxxx not found Wait resp, in act_rsp:line %d", __LINE__);
        return;
    }
    else
    {
        if (CFW_GPRS_ACTIVED == pCfwEv->nType)
        {
            if(g_uAtGprsCidNum < AT_PDPCID_MAX)
                g_uAtGprsCidNum++;
            AT_TC(g_sw_GPRS, "<cid> %d, active operation success", pCfwEv->nParam1);
        }
        else
        {
            if(g_uAtGprsCidNum)
                g_uAtGprsCidNum--;
            AT_TC(g_sw_GPRS, "<cid> %d, deactive operation success", pCfwEv->nParam1);
#ifdef LTE_NBIOT_SUPPORT
				extern VOID ping_timeout(UINT32 *param);
				extern BOOL ping_socket_is_invalid();
				extern void ping_result_statics(UINT8 nDLC, UINT8 *Response);
				if(!ping_socket_is_invalid())
				{
					UINT8 reponse[100] = {0};
					COS_StopCallbackTimer(CSW_AT_TASK_HANDLE,ping_timeout,pCfwEv->nUTI);
					ping_result_statics(pCfwEv->nUTI,reponse);
				}
#endif
        }
        at_CmdRespOK(engine);
    }    
}

// This function to process the AT+CGDCONT command. include set,read and test command.
//
// Set command
// +CGDCONT=[<cid> [,<PDP_type> [,<APN> [,<PDP_addr> [,<d_comp> [,<h_comp>]]]]]]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGDCONT?
// Possible response(s)
// +CGDCONT: <cid>, <PDP_type>, <APN>,<PDP_addr>, <d_comp>, <h_comp>
// [<CR><LF>+CGDCONT: <cid>, <PDP_type>, <APN>,<PDP_addr>, <d_comp>, <h_comp>
// [...]]
//
// Test command
// +CGDCONT=?
// Possible response(s)
// +CGDCONT: (range of supported <cid>s), <PDP_type>,,,(list of supported <d_comp>s),
// (list of supported <h_comp>s)[<CR><LF>+CGDCONT: (range of supported <cid>s), <PDP_type>,,,
// (list of supported <d_comp>s),
// (list of supported <h_comp>s)


VOID AT_GPRS_CmdFunc_CGDCONT(AT_CMD_PARA *pParam)
{
    UINT8 *pResp = NULL;
    INT32 iResult;
    UINT8 uCid   = 0;
    UINT8 uDComp = 0;
    UINT8 uHComp = 0;
    UINT16 uSize;
    UINT8 *pPdpType   = NULL;
    UINT8 *pApn       = NULL;
    UINT8 *userName       = NULL;
    UINT8 *passWord       = NULL;
    UINT8 *pPdpAddr   = NULL;
    UINT8 pPdpAddrStr[AT_GPRS_PDPADDR_MAX_LEN] = {0,};
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    UINT8 uState      = 0;
#ifdef LTE_NBIOT_SUPPORT	
    UINT8 uIPv4AddrAlloc;
    UINT8 uEmy_ind;
    UINT8 uCscf;
    UINT8	iMcnFlag;
    UINT8 nSlpi;
#endif
    CFW_GPRS_PDPCONT_INFO sPdpCont;
    CFW_GPRS_QOS stTmpQos     = { 0, 0, 0, 0, 0 };
    CFW_GPRS_QOS stTmpNullQos = { 3, 4, 3, 4, 16 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_Gprs_CidInfo stCidTemp;

    AT_MemSet(&stCidTemp, 0, SIZEOF(AT_Gprs_CidInfo));
    AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));

    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        AT_TC(g_sw_GPRS, "AT+CGDCONT:NULL == pParam");
        goto _func_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:Get the parameter cout failed.err code = %d.", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
#ifdef LTE_NBIOT_SUPPORT
        if (0 == uParamCount || 11 < uParamCount)
#else
        if (0 == uParamCount || 8 < uParamCount)
#endif
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:Parameter count error. count  = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // Get cid
        if (1 <= uParamCount)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get cid error");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: over cid limit");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
#ifndef AT_NO_GPRS
            CFW_GprsSetReqQos(uCid, &stTmpNullQos, nSim);
#endif
            // This case undefine the spicafied cid
            if (1 == uParamCount)
            {
                if (0 == g_staAtGprsCidInfo[uCid].uCid)
                {
                    AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: g_staAtGprsCidInfo[uCid].uCid = 0");
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto _func_fail;
                }
#ifndef AT_NO_GPRS
                iResult = CFW_GprsGetReqQos(uCid, &stTmpQos, nSim);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: over cid limit");
                    uErrCode = ERR_AT_CME_EXE_FAIL;
                    goto _func_fail;
                }

                iResult = CFW_GprsSetReqQos(uCid, &stTmpNullQos, nSim);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: CFW_GprsSetReqQos");
                    uErrCode = ERR_AT_CME_EXE_FAIL;
                    goto _func_fail;
                }
                iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: CFW_GprsSetPdpCxt");
                    iResult = CFW_GprsSetReqQos(uCid, &stTmpQos, nSim);
                    uErrCode = ERR_AT_CME_EXE_FAIL;
                    goto _func_fail;
                }
#endif
                g_staAtGprsCidInfo[uCid].uCid  = 0;
                g_staAtGprsCidInfo[uCid].nDLCI = 0;
                goto _func_success;
            }

        }

        // 2#
        // Get PDP_type
        if (2 <= uParamCount)
        {
            uIndex++;
            pPdpType = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);   
            if (NULL == pPdpType)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc pPdpType");
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }

            AT_MemSet(pPdpType, 0, AT_GPRS_APN_MAX_LEN + 1);
            uSize = AT_GPRS_PDPTYPE_MAX_LEN;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, pPdpType, &uSize);
			if (((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL)) || uSize < 1 )
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get pPdpType");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
	        if(iResult != ERR_AT_UTIL_CMD_PARA_NULL)
	        {
                if (AT_MemCmp("IP", pPdpType, SIZEOF("IP")) == 0)
            {
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
            }
            else if (AT_MemCmp("IPV6", pPdpType, SIZEOF("IPV6")) == 0)
            {
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IPV6;
            }
            else if (AT_MemCmp("IPV4V6", pPdpType, SIZEOF("IPV4V6")) == 0)
            {
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IPV4V6;
            }
            else if (AT_MemCmp("PPP", pPdpType, SIZEOF("PPP")) == 0)
            {
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_PPP;
            }
#ifdef LTE_NBIOT_SUPPORT
			else if (AT_MemCmp("Non-IP", pPdpType,SIZEOF("Non-IP")) == 0)
			{
			    sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_NONIP;
			}
#endif
            else
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: pPdpType:%s not recognize", pPdpType);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        }
        // 3#
        // Get APN
        if (3 <= uParamCount)
        {
            uIndex++;

            pApn = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);
            if (NULL == pApn)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc APN");
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }

            uSize = AT_GPRS_APN_MAX_LEN;
            AT_MemSet(pApn, 0, AT_GPRS_APN_MAX_LEN + 1);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, pApn, &uSize);
			if ((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL))
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get APN");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
	        if(iResult != ERR_AT_UTIL_CMD_PARA_NULL)
	        {
            if (0 != uSize)
            {
                sPdpCont.pApn     = pApn;
                sPdpCont.nApnSize = uSize;
            }
            else
            {
                sPdpCont.pApn = NULL;
            }
        }
        }
        // 4#
        // Get PDP_addr
        if (4 <= uParamCount)
        {
            uIndex++;

            pPdpAddr = AT_MALLOC(AT_GPRS_PDPADDR_MAX_LEN + 1);
            if (NULL == pPdpAddr)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc PDPADDR");
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }

            uSize = AT_GPRS_PDPADDR_MAX_LEN;
            AT_MemSet(pPdpAddrStr, 0, AT_GPRS_PDPADDR_MAX_LEN + 1);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, pPdpAddrStr, &uSize);
            if ((ERR_SUCCESS != iResult) && (ERR_AT_UTIL_CMD_PARA_NULL != iResult)) // add by wulc for IP addr is NULL
            {
                AT_TC(g_sw_GPRS, "AT+CGDCONT:ERR_AT_UTIL_CMD_PARA_NULL, get PDPADDRSTR");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
			if(ERR_AT_UTIL_CMD_PARA_NULL != iResult)
			{
            if (0 != uSize)
            {
                iResult = AT_GPRS_IPStringToPDPAddr(pPdpAddrStr, uSize, pPdpAddr, (UINT8 *)&uSize);
                AT_TC(g_sw_GPRS, "AT+CGDCONT:AT_GPRS_IPStringToPDPAddr, ret = %d.uSize=%d", iResult, uSize);
                if (iResult < 0)
                {
                    sPdpCont.pPdpAddr = NULL;
                    sPdpCont.nPdpAddrSize = 0;
                }
                else
                {
                    AT_TC(g_sw_GPRS, "pPdpAddr: %d,%d,%d,%d", pPdpAddr[0], pPdpAddr[1], pPdpAddr[2], pPdpAddr[3]);
                    if ((pPdpAddr[0] == 0) && (pPdpAddr[1] == 0)
                            &&(pPdpAddr[2] == 0) &&(pPdpAddr[3] == 0))
                    {
                        sPdpCont.pPdpAddr = NULL;
                        sPdpCont.nPdpAddrSize = 0;
                    }
                    else
                    {
                        sPdpCont.pPdpAddr = pPdpAddr;
                        sPdpCont.nPdpAddrSize = uSize;
                    }
                }
            }
            else
            {
                sPdpCont.pPdpAddr = NULL;
                sPdpCont.nPdpAddrSize = 0;
            }
        }
        }

        // 5#
        // Get d_comp
        if (5 <= uParamCount)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uDComp, &uSize);

            if (((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult)) || uSize < 1)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get DCOMP");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
			if(ERR_AT_UTIL_CMD_PARA_NULL != iResult)
			{
            if (CFW_GPRS_PDP_D_COMP_OFF != uDComp &&
                    CFW_GPRS_PDP_D_COMP_ON != uDComp && CFW_GPRS_PDP_D_COMP_V42 != uDComp && CFW_GPRS_PDP_D_COMP_V44 != uDComp)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: D_COMP:%d check fail", uDComp);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            sPdpCont.nDComp = uDComp;
        }
        }
        // 6#
        // Get h_comp
        if (6 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:6 <= uParamCount.");
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uHComp, &uSize);

            if (((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult)) || uSize < 1)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get H_COMP");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
			if(ERR_AT_UTIL_CMD_PARA_NULL != iResult)
			{
            if (CFW_GPRS_PDP_H_COMP_OFF != uHComp &&
                    CFW_GPRS_PDP_H_COMP_ON != uHComp &&
                    CFW_GPRS_PDP_H_COMP_RFC1144 != uHComp &&
                    CFW_GPRS_PDP_H_COMP_RFC2507 != uHComp && CFW_GPRS_PDP_H_COMP_RFC3095 != uHComp)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: H_COMP:%d check fail", uHComp);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            sPdpCont.nHComp = uHComp;
			}
        }
#ifdef LTE_NBIOT_SUPPORT	
        if (7 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  7 <= uParamCount.");
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uIPv4AddrAlloc, &uSize);
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
	  if (8 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  8 <= uParamCount.");
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uEmy_ind, &uSize);
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
	  if (9 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  9 <= uParamCount.");
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCscf, &uSize);
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
	 if(10 <= uParamCount)
	 {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  10 <= uParamCount.");
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &iMcnFlag, &uSize);
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
	
        }
	 if(11 <= uParamCount)
	 {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  11 <= uParamCount.");
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &nSlpi, &uSize);
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
			if(ERR_AT_UTIL_CMD_PARA_NULL != iResult)
			{
			    if((nSlpi != 0) && (nSlpi != 1))
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
	     sPdpCont.nSlpi = nSlpi;
			}
        }
#else
        // 7#
        // Get APN
        if (7 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:  7 <= uParamCount.");

            uIndex++;
            userName = AT_MALLOC(AT_GPRS_USR_MAX_LEN + 1);
            if (NULL == userName)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc USERNAME");
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }

            uSize = AT_GPRS_USR_MAX_LEN;
            AT_MemSet(userName, 0, AT_GPRS_USR_MAX_LEN + 1);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, userName, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get USERNAME");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 != uSize)
            {
                sPdpCont.pApnUser     = userName;
                sPdpCont.nApnUserSize = uSize;
            }
            else
            {
                sPdpCont.pApnUser = NULL;
            }
        }
        // 8#
        // Get APN
        if (8 <= uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDCONT:8 <= uParamCount.");

            uIndex++;
            passWord = AT_MALLOC(AT_GPRS_PAS_MAX_LEN + 1);
            if (NULL == passWord)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc PASSWORD");
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }

            uSize = AT_GPRS_PAS_MAX_LEN;
            AT_MemSet(passWord, 0, AT_GPRS_PAS_MAX_LEN + 1);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, passWord, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: get PASSWORD");
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 != uSize)
            {
                sPdpCont.pApnPwd = passWord;
                sPdpCont.nApnPwdSize = uSize;
            }
            else
            {
                sPdpCont.pApnPwd = NULL;
                sPdpCont.nApnPwdSize = 0;
            }
        }
#endif
        if (g_staAtGprsCidInfo[uCid].uCid > 0)
        {
            AT_MemCpy(&stCidTemp, &g_staAtGprsCidInfo[uCid], SIZEOF(AT_Gprs_CidInfo));
            g_staAtGprsCidInfo[uCid].uCid         = 0;
#ifdef LTE_NBIOT_SUPPORT
            g_staAtGprsCidInfo[uCid].uPCid        = 0;
#endif
            g_staAtGprsCidInfo[uCid].nDLCI        = 0;
            g_staAtGprsCidInfo[uCid].nApnSize     = 0;
            g_staAtGprsCidInfo[uCid].nPdpAddrSize = 0;
            g_staAtGprsCidInfo[uCid].nHComp       = 0;
            g_staAtGprsCidInfo[uCid].nDComp       = 0;
        }

        g_staAtGprsCidInfo[uCid].uCid  = uCid;
        g_staAtGprsCidInfo[uCid].nDLCI = pParam->nDLCI;
        g_staAtGprsCidInfo[uCid].nPdpType = sPdpCont.nPdpType;
        g_staAtGprsCidInfo[uCid].nDComp = sPdpCont.nDComp;
        g_staAtGprsCidInfo[uCid].nHComp = sPdpCont.nHComp;

        if (sPdpCont.nApnSize != 0)
        {
            g_staAtGprsCidInfo[uCid].nApnSize = sPdpCont.nApnSize;
            AT_StrCpy(g_staAtGprsCidInfo[uCid].pApn, sPdpCont.pApn);
        }

        if (sPdpCont.nPdpAddrSize != 0)
        {
            g_staAtGprsCidInfo[uCid].nPdpAddrSize = sPdpCont.nPdpAddrSize;
            AT_StrCpy(g_staAtGprsCidInfo[uCid].pPdpAddr, sPdpCont.pPdpAddr);
        }
        if (sPdpCont.nApnPwdSize != 0)
        {
            g_staAtGprsCidInfo[uCid].nPasswordSize = sPdpCont.nApnPwdSize;
            AT_StrCpy(g_staAtGprsCidInfo[uCid].uaPassword, sPdpCont.pApnPwd);
        }

        if (sPdpCont.nApnUserSize != 0)
        {
            g_staAtGprsCidInfo[uCid].nUsernameSize = sPdpCont.nApnUserSize;
            AT_StrCpy(g_staAtGprsCidInfo[uCid].uaUsername, sPdpCont.pApnUser);
        }

#ifndef AT_NO_GPRS
        // Call csw api to set the pdp cxt.
        iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
#endif
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: CFW_GprsSetPdpCxt");
            AT_MemCpy(&g_staAtGprsCidInfo[uCid], &stCidTemp, SIZEOF(AT_Gprs_CidInfo));
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            goto _func_success;
        }

    }

    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 *pTempStr = NULL;

        pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
        if (NULL == pResp)
        {
            AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc tmp RESP");
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        AT_MemSet(pResp, 0, (AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);

        pTempStr = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);
        if (NULL == pTempStr)
        {
            AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: malloc TEMPSTR");
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));
            AT_MemSet(pResp, 0, (AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            AT_MemSet(pTempStr, 0, AT_GPRS_APN_MAX_LEN);

            if ((g_staAtGprsCidInfo[uCid].uCid != 0) && (nSim == AT_SIM_ID(g_staAtGprsCidInfo[uCid].nDLCI)))
            {
                CFW_GetGprsActState(g_staAtGprsCidInfo[uCid].uCid, &uState, nSim);
                switch (g_staAtGprsCidInfo[uCid].nPdpType)
                {
                case CFW_GPRS_PDP_TYPE_IP:
                    AT_StrCpy(pTempStr, "\"IP\"");  // yy [mod] 2008-5-19 for bug ID 8458
                    break;

                /*
                   case CFW_GPRS_PDP_TYPE_IPV6:
                   AT_StrCpy(pTempStr, "\"IPV6\""); // yy [mod] 2008-5-19 for bug ID 8458
                   break;

                   case CFW_GPRS_PDP_TYPE_PPP:
                   AT_StrCpy(pTempStr, "\"PPP\""); // yy [mod] 2008-5-19 for bug ID 8458
                   break;
                   */
                default:
                    pTempStr[0] = 0;  // Should be here
                    break;
                }

                AT_Sprintf(pResp, "+CGDCONT:%d,%s,", g_staAtGprsCidInfo[uCid].uCid, pTempStr);

                if (g_staAtGprsCidInfo[uCid].nApnSize != 0)
                {
                    AT_MemSet(pTempStr, 0, AT_GPRS_APN_MAX_LEN);
                    AT_Sprintf(pTempStr, "\"%s\",", g_staAtGprsCidInfo[uCid].pApn);
                    AT_StrCat(pResp, pTempStr);
                }
                else
                {
                    AT_StrCat(pResp, ",");
                }
                AT_MemSet(pTempStr, 0, AT_GPRS_APN_MAX_LEN);

                if (g_staAtGprsCidInfo[uCid].nPdpAddrSize != 0)
                {
                    AT_MemSet(pTempStr, 0, AT_GPRS_APN_MAX_LEN);
                    AT_Sprintf(pTempStr, "\"%s\",", g_staAtGprsCidInfo[uCid].pPdpAddr);
                    AT_StrCat(pResp, pTempStr);
                }
                else
                {
                    AT_StrCat(pResp, ",");
                }

                AT_MemSet(pTempStr, 0, AT_GPRS_APN_MAX_LEN);
                AT_Sprintf(pTempStr, "%d,%d", g_staAtGprsCidInfo[uCid].nDComp, g_staAtGprsCidInfo[uCid].nHComp);
                AT_StrCat(pResp, pTempStr);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
            }
            else
            {
                // Do nothing.
            }
        }

        if (NULL == pResp)
        {
            AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: RESP is empty");
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);
        if (NULL == pResp)
        {
            AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: test malloc pRESP");
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }
        AT_MemZero(pResp, 128);
        AT_StrCpy(pResp, "+CGDCONT: (1..7), (IP,IPV6,PPP),(0..3),(0..4)");

        goto _func_success;
    }
    else
    {
        AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: unknown CMD type");
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }

_func_success:

    if (NULL != pResp)
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    else
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);

    if (NULL != pApn)
    {
        AT_FREE(pApn);
        pApn = NULL;
    }

    if (NULL != passWord)
    {
        AT_FREE(passWord);
        passWord = NULL;
    }
    if (NULL != userName)
    {
        AT_FREE(userName);
        userName = NULL;
    }
    if (NULL != pPdpType)
    {
        AT_FREE(pPdpType);
        pPdpType = NULL;
    }
    if (NULL != pPdpAddr)
    {
        AT_FREE(pPdpAddr);
        pPdpAddr = NULL;
    }

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;

_func_fail:

    AT_TC(g_sw_GPRS, "ERROR! AT+CGDCONT: Failed");
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

    if (NULL != pApn)
    {
        AT_FREE(pApn);
        pApn = NULL;
    }
    if (NULL != passWord)
    {
        AT_FREE(passWord);
        passWord = NULL;
    }
    if (NULL != userName)
    {
        AT_FREE(userName);
        userName = NULL;
    }
    if (NULL != pPdpType)
    {
        AT_FREE(pPdpType);
        pPdpType = NULL;
    }
    if (NULL != pPdpAddr)
    {
        AT_FREE(pPdpAddr);
        pPdpAddr = NULL;
    }

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;
}

//Set command set second pdp context with p_cid to first pdp context with cid
//Read command list all non-NULL pdp context
//Test command  show the range of cid, pdp type, d compress and h compress
VOID AT_GPRS_CmdFunc_CGDSCONT(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 uCid   = 0;
    UINT8 uP_Cid   = 0;
    UINT8 uDComp = 0;
    UINT8 uHComp = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;

    UINT8 *pApn       = NULL;
    UINT8 *userName       = NULL;
    UINT8 *passWord       = NULL;
    UINT8 *pPdpAddr   = NULL;

    UINT8 Cid;

    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_GPRS_PDPCONT_INFO sPdpCont;
    CFW_GPRS_QOS stTmpQos     = { 0, 0, 0, 0, 0 };
    CFW_GPRS_QOS stTmpNullQos = { 3, 4, 3, 4, 16 };

    // input parameter check.
    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        AT_TC(g_sw_GPRS, "AT+CGDSCONT:NULL == pParam");
        goto _func_fail;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 nDLCI = pParam->nDLCI;
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_Gprs_CidInfo stCidTemp;

    AT_MemSet(&stCidTemp, 0, SIZEOF(AT_Gprs_CidInfo));
    AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));

    AT_TC(g_sw_GPRS, "AT+CGDSCONT: start,pParam->iType =%d",pParam->iType);
    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == pParam->iType)
    {
        // Get the count of parameters.
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGDSCONT:Get the parameter count Failed. ErrorCode = %d.", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // This command parameter count  largest or equat 1.
        if (uParamCount < 1 || 4 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGDSCONT:Parameter count error. parameter count  = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        uSize   = 1;
        uIndex  = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGDSCONT:Get 1st Parameter count error. uIndex  = %d.", uIndex);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX)
        {
            AT_TC(g_sw_GPRS, "AT+CGDSCONT:1st Parameter Error. uCid  = %d.", uCid);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        AT_TC(g_sw_GPRS, "AT+CGDSCONT:uCid  = %d.", uCid);
        Cid = uCid;
#ifndef AT_NO_GPRS
        iResult = CFW_GprsGetReqQos(uCid, &stTmpQos, nSim);
#endif
        if(uParamCount >= 2)
        {
            uSize   = 1;
            uIndex  = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uP_Cid, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (uP_Cid < AT_PDPCID_MIN || uP_Cid > AT_PDPCID_MAX)
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            AT_TC(g_sw_GPRS, "AT+CGDSCONT:uP_Cid  = %d.", uP_Cid);

            uSize = AT_GPRS_APN_MAX_LEN + 1;
            pApn = AT_MALLOC(uSize);
            if (NULL == pApn)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pApn, 0, uSize);
            sPdpCont.pApn = pApn;

            uSize = AT_GPRS_PDPADDR_MAX_LEN + 1;
            pPdpAddr = AT_MALLOC(uSize);
            if (NULL == pPdpAddr)
            {
                AT_FREE(pApn);
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pPdpAddr, 0, uSize);
            sPdpCont.pPdpAddr = pPdpAddr;

            uSize = AT_GPRS_USR_MAX_LEN + 1;
            userName = AT_MALLOC(uSize);
            if (NULL == userName)
            {
                AT_FREE(pApn);
                AT_FREE(pPdpAddr);
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(userName, 0, uSize);
            sPdpCont.pApnUser = userName;

            uSize = AT_GPRS_PAS_MAX_LEN + 1;
            passWord = AT_MALLOC(uSize);
            if (NULL == passWord)
            {
                AT_FREE(pApn);
                AT_FREE(pPdpAddr);
                AT_FREE(userName);
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(passWord, 0, uSize);
            sPdpCont.pApnPwd = passWord;

#ifndef AT_NO_GPRS
            iResult = CFW_GprsGetPdpCxt(uP_Cid, &sPdpCont, nSim);
#endif
            if (ERR_SUCCESS != iResult)
            {
CGDSCONT_EXIT:
                AT_FREE(pApn);
                AT_FREE(pPdpAddr);
                AT_FREE(userName);
                AT_FREE(passWord);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            // 3rd parameter#
            // Get d_comp
            if (3 <= uParamCount)
            {
                AT_TC(g_sw_GPRS, "AT+CGDSCONT:3 <= uParamCount.");
                uSize = 1;
                uIndex++;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uDComp, &uSize);
                if (ERR_SUCCESS != iResult || uSize < 1)
                {
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto CGDSCONT_EXIT;
                }
                if (CFW_GPRS_PDP_D_COMP_OFF != uDComp &&
                        CFW_GPRS_PDP_D_COMP_ON != uDComp && CFW_GPRS_PDP_D_COMP_V42 != uDComp && CFW_GPRS_PDP_D_COMP_V44 != uDComp)
                {
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto CGDSCONT_EXIT;
                }
                sPdpCont.nDComp = uDComp;
            }

            // 4th parameter#
            // Get h_comp
            if (4 == uParamCount)
            {
                AT_TC(g_sw_GPRS, "AT+CGDSCONT:4 <= uParamCount.");
                uSize = 1;
                uIndex++;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uHComp, &uSize);
                if (ERR_SUCCESS != iResult || uSize < 1)
                {
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto CGDSCONT_EXIT;
                }
                if (CFW_GPRS_PDP_H_COMP_OFF != uHComp &&
                        CFW_GPRS_PDP_H_COMP_ON != uHComp &&
                        CFW_GPRS_PDP_H_COMP_RFC1144 != uHComp &&
                        CFW_GPRS_PDP_H_COMP_RFC2507 != uHComp && CFW_GPRS_PDP_H_COMP_RFC3095 != uHComp)
                {
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto CGDSCONT_EXIT;
                }
                sPdpCont.nHComp = uHComp;
            }

        }
        else     //if uParamCount == 1, sPdpCont has been clear. otherwise it hold context from uP_Cid
        {
            AT_FREE(sPdpCont.pApn);
            AT_FREE(sPdpCont.pPdpAddr);
            AT_FREE(sPdpCont.pApnUser);
            AT_FREE(sPdpCont.pApnPwd);
            Cid = 0;
            nDLCI = 0;
            AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));
        }
        AT_TC(g_sw_GPRS, "AT+CGDSCONT:CFW_GprsSetReqQos.");
#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetReqQos(uCid, &stTmpNullQos, nSim);
#endif
        if (iResult != ERR_SUCCESS)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto CGDSCONT_EXIT;
        }
#ifndef AT_NO_GPRS
        AT_TC(g_sw_GPRS, "AT+CGDSCONT:CFW_GprsSetPdpCxt.uCid = %d",uCid);
        iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
#endif
        if (iResult != ERR_SUCCESS)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsSetReqQos(uCid, &stTmpQos, nSim);
#endif
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto CGDSCONT_EXIT;
        }
        AT_TC(g_sw_GPRS, "params: %s, %s", pApn, pPdpAddr);
        AT_TC(g_sw_GPRS, "uCid: %d", uCid);
        AT_TC(g_sw_GPRS, "nPdpType: %d", sPdpCont.nPdpType);
        AT_TC(g_sw_GPRS, "nDComp: %d", sPdpCont.nDComp);
        AT_TC(g_sw_GPRS, "nHComp: %d", sPdpCont.nHComp);
        AT_TC(g_sw_GPRS, "nApnSize: %d", sPdpCont.nApnSize);
        AT_TC(g_sw_GPRS, "nPdpAddrSize: %d", sPdpCont.nPdpAddrSize);
        AT_TC(g_sw_GPRS, "AT+CGDSCONT:CFW_GprsSetPdpCxt.");

        g_staAtGprsCidInfo[uCid].uCid  = Cid;
#ifdef LTE_NBIOT_SUPPORT
        g_staAtGprsCidInfo[uCid].uPCid = uP_Cid;
#endif
        g_staAtGprsCidInfo[uCid].nDLCI = nDLCI;
        g_staAtGprsCidInfo[uCid].nPdpType = sPdpCont.nPdpType;
        g_staAtGprsCidInfo[uCid].nDComp = sPdpCont.nDComp;
        g_staAtGprsCidInfo[uCid].nHComp = sPdpCont.nHComp;

        g_staAtGprsCidInfo[uCid].nApnSize = sPdpCont.nApnSize;
        if (sPdpCont.nApnSize != 0)
            AT_StrCpy(g_staAtGprsCidInfo[uCid].pApn, sPdpCont.pApn);

        g_staAtGprsCidInfo[uCid].nPdpAddrSize = sPdpCont.nPdpAddrSize;
        if (sPdpCont.nPdpAddrSize != 0)
            AT_StrCpy(g_staAtGprsCidInfo[uCid].pPdpAddr, sPdpCont.pPdpAddr);

        g_staAtGprsCidInfo[uCid].nPasswordSize = sPdpCont.nApnPwdSize;
        if (sPdpCont.nApnPwdSize != 0)
            AT_StrCpy(g_staAtGprsCidInfo[uCid].uaPassword, sPdpCont.pApnPwd);

        g_staAtGprsCidInfo[uCid].nUsernameSize = sPdpCont.nApnUserSize;
        if (sPdpCont.nApnUserSize != 0)
            AT_StrCpy(g_staAtGprsCidInfo[uCid].uaUsername, sPdpCont.pApnUser);
    }
    else if (AT_CMD_READ == pParam->iType)    // Read command
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            //uSize = (AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2;
            uSize = 20;
            UINT8 nResp[uSize];
            UINT8 nComp[uSize];
            AT_MemSet(nResp, 0, uSize);
            AT_MemSet(nComp, 0, uSize);
            AT_TC(g_sw_GPRS, "AT+CGDSCONT: uCid =%d.",uCid);

            if ((g_staAtGprsCidInfo[uCid].uCid != 0) && (nSim == AT_SIM_ID(g_staAtGprsCidInfo[uCid].nDLCI)))
            {
                AT_Sprintf(nResp, "+CGDSCONT:%d,%d,", g_staAtGprsCidInfo[uCid].uCid, g_staAtGprsCidInfo[uCid].uCid);
                AT_Sprintf(nComp, "%d,%d", g_staAtGprsCidInfo[uCid].nDComp, g_staAtGprsCidInfo[uCid].nHComp);

                AT_StrCat(nResp, nComp);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, nResp, AT_StrLen(nResp), pParam->nDLCI, nSim);
            }
        }
    }
    else if (AT_CMD_TEST == pParam->iType)    // Test command.
    {
        UINT8 nResp[60];
        UINT8 uState = 0;
        AT_MemZero(nResp, 60);

        uSize = 20;
        UINT8 nActive[uSize];
        AT_MemSet(nActive, 0, uSize);
        uSize = 0;
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            if ((g_staAtGprsCidInfo[uCid].uCid != 0) && (nSim == AT_SIM_ID(g_staAtGprsCidInfo[uCid].nDLCI)))
            {
                CFW_GetGprsActState(g_staAtGprsCidInfo[uCid].uCid, &uState, nSim);
                AT_TC(g_sw_GPRS, "AT+CGDSCONT: uState =%d.",uState);

                if(AT_GPRS_ACT != uState)
                    continue;

                AT_TC(g_sw_GPRS, "%s",nResp);
                if(uSize != 0)
                    AT_Sprintf(nResp,",%d",g_staAtGprsCidInfo[uCid].uCid);
                else
                {
                    AT_Sprintf(nResp,"%d",g_staAtGprsCidInfo[uCid].uCid);
                    uSize = 1;
                }
                AT_TC(g_sw_GPRS, "nResp =%s",nResp);
                AT_TC(g_sw_GPRS, "nActive =%s",nActive);
                AT_StrCat(nActive,nResp);
            }
        }
        AT_Sprintf(nResp, "+CGDCONT: (1..7),(%s),(IP,IPV6,PPP),(0..3),(0..4)",nActive);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nResp, AT_StrLen(nResp), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;

_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

extern UINT8 SUL_ascii2hex(UINT8* pInput, UINT8* pOutput);
//first parameter is ascii string holding MINvalue.MAXvalue
//Range is output parameter, High 16 bits hold  MINvalue, low 16 bits hold MAXvalue
//return 0 if error, other return 1
UINT8 at_GetRange(UINT8*StrRang, UINT32* Range)
{
    UINT8 i = 0;
    UINT16 value = 0;
    UINT8 length = AT_StrLen(StrRang);
    AT_TC(g_sw_GPRS, "StrRang = %s",StrRang);
    for(i = 0; i < length; i++)
    {
        UINT8 c = StrRang[i];
        if((c >= '0') && (c <= '9'))
        {
            c = c - '0';
            value = value *10 + c;
            AT_TC(g_sw_GPRS, "value = %d",value);
        }
        else if(c == '.')
        {
            if(i > 5)
                return 0;
            *Range = value;
            value = 0;
            AT_TC(g_sw_GPRS, "*Range = %d",*Range);
        }
        else
            return 0;
    }
    //*Range = htons(*Range) << 16;
    //*Range |= htons(value);
    *Range = *Range << 16;
    *Range |= value;
    //*Range = htonl(*Range);
    AT_TC(g_sw_GPRS, "High 16 Range = %d",*Range >> 16);
    AT_TC(g_sw_GPRS, "Low 16 Range = %d",(*Range)& 0xFFFF);

    return 1;
}


UINT8 at_ascii2hex(UINT8* pInput, UINT8* pOutput)
{
    UINT16 i = 0;
    UINT16 length = AT_StrLen(pInput);
    UINT16 j = length;

    UINT8 nData;
    UINT8* pBuffer = pOutput;
    UINT8* pIn = pInput;
    AT_TC(g_sw_GPRS, "length = %d", length);

    if((length & 0x01) == 0x01)
    {
        nData  = pIn[0];
        if((nData >= '0') && (nData <= '9'))
            *pBuffer = nData - '0';
        else if((nData >= 'a') && (nData <= 'f'))
            *pBuffer = nData - 'a' + 10;
        else if((nData >= 'A') && (nData <= 'F'))
            *pBuffer = nData - 'A' + 10;
        else
            return 0;
        length--;
        pBuffer++;
        pIn++;
    }

    for(i = 0; i < length; i++)
    {
        if((i & 0x01 )  == 0x01)            // even is high 4 bits, it should be shift 4 bits for left.
            *pBuffer = (*pBuffer) << 4;
        else
            *pBuffer = 0;

        nData = pIn[i];
        AT_TC(g_sw_GPRS, "nData = %d",nData);
        if((nData >= '0') && (nData <= '9'))
            *pBuffer |= nData - '0';
        else if((nData >= 'a') && (nData <= 'f'))
            *pBuffer |= nData - 'a' + 10;
        else if((nData >= 'A') && (nData <= 'F'))
            *pBuffer |= nData - 'A' + 10;
        else
            return 0;
        AT_TC(g_sw_GPRS, "*pBuffer = %x",*pBuffer);
        AT_TC(g_sw_GPRS, "nData = %x",nData);
        if((i & 0x01 )  == 0x01)
            pBuffer++;
    }
    if((j & 0x01 )  == 0x01)
        length++;
    AT_TC(g_sw_GPRS, "pOutput = %x,%x,%x, length = %d",pOutput[0],pOutput[1],pOutput[2], (length + 1) >> 1);
    return (length + 1) >> 1;
}
#ifdef LTE_NBIOT_SUPPORT
CFW_TFT_SET g_At_Tft_Set[CFW_SIM_NUMBER][AT_PDPCID_MAX] = {0,};

//at+cgtft=1,7,254,123.32.23.43.89.67.65.125,234,1234.5432,6789.8876,8432d,235.54
VOID AT_GPRS_CmdFunc_CGTFT(AT_CMD_PARA *pParam)
{
    UINT8 cid = 0;
    UINT8 packet_filter_id = 0;         //packet  filter identifier
    UINT8 eval_prece_index = 0;         //evaluation precedence index

    UINT8 protocol = 0;                 //protocol number
    UINT32 local_port_range = 0;         //local port range
    UINT32 remote_port_range = 0;           //remote port range
    UINT32 ipsec_index = 0;             //ipsec security parameter index
    UINT8  direction = 0;
    UINT32 flow_label = 0;
    UINT16 tos;

    INT32 Result = 0;
    UINT8 Count = 0;
    UINT32 uErrCode = ERR_AT_CME_PARAM_INVALID;
    UINT8* pPacketFilter;
    UINT8 uOffset;
    UINT8 uLength;
    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        AT_TC(g_sw_GPRS, "AT+CGTFT: Input parameter is NULL!");
        goto FAILED_EXIT;
    }

    UINT8 nDLCI = pParam->nDLCI;
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    if (AT_CMD_SET == pParam->iType)
    {
        UINT16 Size = 0;
        UINT8 Index = 0;
        Result = AT_Util_GetParaCount(pParam->pPara, &Count);
        
        hal_HstSendEvent(0x06240900);
        hal_HstSendEvent(Count);

        if (ERR_SUCCESS != Result)
        {
            AT_TC(g_sw_GPRS, "Get the parameter count Failed. ErrorCode = %d.", Result);
            goto FAILED_EXIT;
        }
        
        if(Count > 0)
        {
            Size   = 1;
            Index  = 0;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &cid, &Size);
            if ((ERR_SUCCESS != Result) ||(cid < AT_PDPCID_MIN) ||(cid > AT_PDPCID_MAX))
            {
                AT_TC(g_sw_GPRS, "Get 1st Parameter error! Result = %x, cid = %d", Result, cid);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: cid = %d", cid);
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(cid);
            memset(&(g_At_Tft_Set[nSim][cid]), 0, sizeof(CFW_TFT_SET));
            if(Count == 1)
            {
                goto OK_EXIT;
            }
        }
        if(Count > 1)
        {
            Size   = 1;
            Index  = 1;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &packet_filter_id, &Size);
            if ((ERR_SUCCESS != Result) ||(packet_filter_id < 1) ||(packet_filter_id > 8))
            {
                AT_TC(g_sw_GPRS, "Get second Parameter error! Result = %x, pfid = %d", Result, packet_filter_id);
                goto FAILED_EXIT;
            }
            g_At_Tft_Set[nSim][cid].Pf_ID = packet_filter_id;
            g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x8000;
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(packet_filter_id);
            AT_TC(g_sw_GPRS, "CGTFT: packet_filter_id = %d", packet_filter_id);
        }

        if(Count > 2)
        {
            Size   = 1;
            Index  = 2;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &eval_prece_index, &Size);
            if ((ERR_SUCCESS != Result))
            {
                AT_TC(g_sw_GPRS, "Get third Parameter error! Result = %x, pfid = %d", Result, eval_prece_index);
                goto FAILED_EXIT;
            }
            g_At_Tft_Set[nSim][cid].Pf_EvaPreIdx = eval_prece_index;
            g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x4000;
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(eval_prece_index);
            AT_TC(g_sw_GPRS, "CGTFT: eval_prece_index = %d", eval_prece_index);
        }
        if(Count > 3)
        {
            Size = 34; /* For Example: "192.168.100.100.255.255.255.255" */
            Index  = 3;
            UINT8 SourceMask[34] = {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &SourceMask, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result) ||(AT_StrLen(SourceMask) > 31))
                {
                    AT_TC(g_sw_GPRS, "Get fourth Parameter error! Result = %x", Result);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: SourceMask = %s", SourceMask);

                UINT8 i, k = 0;
                UINT8 nAddrLen = 0;
                for(i = 0; i < AT_StrLen(SourceMask); i++) // try to find fourth . and set it to 0
                {
                    if(SourceMask[i] == '.')
                        k++;
                    if(k == 4)
                    {
                        k = 0xFF;
                        SourceMask[i] = 0;
                        i++;
                        break;
                    }
                }
                if(k != 0xFF)
                {
                    AT_TC(g_sw_GPRS, "SourceMask address is invalid(%s)", SourceMask);
                    goto FAILED_EXIT;
                }
                
                AT_TC(g_sw_GPRS, "source address is %s", SourceMask);
                AT_TC(g_sw_GPRS, "mask address is %s",  SourceMask + i);

                if(AT_GPRS_IPStringToPDPAddr(SourceMask, i, &g_At_Tft_Set[nSim][cid].Pf_RmtAddrIPv4[0], &nAddrLen) != 1)
                {
                    AT_TC(g_sw_GPRS, "source address is invalid(%s)", SourceMask);
                    goto FAILED_EXIT;
                }
                if(nAddrLen > 4)
                {
                    AT_TC(g_sw_GPRS, "source address len error (%s)", SourceMask);
                    goto FAILED_EXIT;
                }

                if(AT_GPRS_IPStringToPDPAddr(&(SourceMask[i]), AT_StrLen(SourceMask + i), &g_At_Tft_Set[nSim][cid].Pf_RmtAddrIPv4[4], &nAddrLen) != 1)
                {
                    AT_TC(g_sw_GPRS, "mask address is invalid(%s)", SourceMask + i);
                    goto FAILED_EXIT;
                }
                if(nAddrLen > 4)
                {
                    AT_TC(g_sw_GPRS, "mask address len error (%s)", SourceMask + i);
                    goto FAILED_EXIT;
                }
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x2000;
            }
            hal_HstSendEvent(0x06240900+Index);

        }

        if(Count > 4)
        {
            Size   = 1;
            Index  = 4;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &protocol, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if (ERR_SUCCESS != Result)
                {
                    AT_TC(g_sw_GPRS, "Get fifth Parameter error! Result = %x, protocol = %d", Result, protocol);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: protocol = 0x%x", protocol);
                g_At_Tft_Set[nSim][cid].Pf_PortNum_NH = protocol;

                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0800;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(protocol);
        }

        if(Count > 5)
        {
            Size = 14; /* For Example: "60000.60000" */
            Index  = 5;
            UINT8 port_range[14]= {0};      //local port range
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &port_range, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if((ERR_SUCCESS != Result) ||(at_GetRange(port_range, &local_port_range) == 0))
                {
                    AT_TC(g_sw_GPRS, "Get sixth Parameter error! Result = %x, local_port_range = %d", Result, local_port_range);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: local_port_range = %d", local_port_range);
                g_At_Tft_Set[nSim][cid].Pf_LocalPortRangeMin = local_port_range>>16;
                g_At_Tft_Set[nSim][cid].Pf_LocalPortRangeMax = local_port_range&0xFFFF; 
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0400;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(g_At_Tft_Set[nSim][cid].Pf_LocalPortRangeMin);
            hal_HstSendEvent(g_At_Tft_Set[nSim][cid].Pf_LocalPortRangeMax);
         }
        
        if(Count > 6)
        {
            Size   = 14;
            Index  = 6;
            UINT8 port_range[14]= {0};  //remote port range
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &port_range, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result)||(at_GetRange(port_range, &remote_port_range) == 0))
                {
                    AT_TC(g_sw_GPRS, "Get seventh Parameter error! Result = %x, source_port_range = %d", Result, remote_port_range);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: remote_port_range = %d", remote_port_range);
                g_At_Tft_Set[nSim][cid].Pf_RemotPortRangeMin = remote_port_range >> 16;
                g_At_Tft_Set[nSim][cid].Pf_RemotPortRangeMax = remote_port_range&0xFFFF;
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0200;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(g_At_Tft_Set[nSim][cid].Pf_RemotPortRangeMin);
            hal_HstSendEvent(g_At_Tft_Set[nSim][cid].Pf_RemotPortRangeMax);
        }

        if(Count > 7)
        {
            Size   = 9;
            Index  = 7;
            UINT8 ipsec[9]= {0};
            UINT8 ipsec_hex[9] = {0};
            int i;
            UINT8 hex_len;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &ipsec, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result) || (AT_StrLen(ipsec) > 8))
                {
                    AT_TC(g_sw_GPRS, "Get 8th Parameter error! Result = %x, ipsec_index = %d", Result, ipsec_index);
                    goto FAILED_EXIT;
                }
                if((hex_len = SUL_ascii2hex(ipsec, ipsec_hex)) == 0)
                {
                    AT_TC(g_sw_GPRS, "Get 8th Parameter error!format error");
                    goto FAILED_EXIT;
                }
                ipsec_index = 0;
                for(i = 0; i < hex_len;i++)
                {
                    ipsec_index = (ipsec_index<<8) +ipsec_hex[i];
                }
                g_At_Tft_Set[nSim][cid].Pf_SPI = ipsec_index;
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0100;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(ipsec_index);
        }

        if(Count > 8)
        {
            Size   = 10;
            Index  = 8;
            UINT8 type_of_service[10]= {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &type_of_service, &Size);
            UINT32 uTypeOfService = 0;
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result)||(at_GetRange(type_of_service, &uTypeOfService) == 0))
                {
                    AT_TC(g_sw_GPRS, "Get 9th Parameter error! Result = %x, tos = %d", Result, uTypeOfService);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: uTypeOfService = %x", uTypeOfService);
                //uTypeOfService = ntohl(uTypeOfService);
                if((( uTypeOfService>> 16) > 255) || ((uTypeOfService & 0xFFFF) > 255))
                {
                    AT_TC(g_sw_GPRS, "Get 9th Parameter error!high tos = %d, low = %d", uTypeOfService>>16, uTypeOfService&0xFFFF);
                    goto FAILED_EXIT;
                }
                tos=(uTypeOfService>>8)|(uTypeOfService&0xFF);
                //tos=htons(tos);
                AT_TC(g_sw_GPRS, "CGTFT: uTypeOfService = %x", uTypeOfService);
                AT_TC(g_sw_GPRS, "CGTFT: high tos = %d, low = %d", tos>>8, tos&0xFF);

                g_At_Tft_Set[nSim][cid].Pf_TOS_TC[0] = tos>>8;
                g_At_Tft_Set[nSim][cid].Pf_TOS_TC[1] = tos&0xFF;
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0080;
            }
            hal_HstSendEvent(0x06240900+Index);
        }

        if(Count > 9)
        {
            Size   = 6;
            Index  = 9;
            UINT8 FlowLabel[6]= {0};
            UINT8 FlowLabel_hex[6] = {0};
            int i;
            UINT8 hex_len;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &FlowLabel, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result) || (AT_StrLen(FlowLabel) > 5))
                {
                    AT_TC(g_sw_GPRS, "Get 10th Parameter error! Result = %x", Result);
                    goto FAILED_EXIT;
                }
                if((hex_len = SUL_ascii2hex(FlowLabel, FlowLabel_hex)) == 0)
                {
                    goto FAILED_EXIT;
                }
                flow_label = 0;
                for(i = 0; i < hex_len;i++)
                {
                    flow_label = (flow_label<<8) + FlowLabel_hex[i];
                }
                g_At_Tft_Set[nSim][cid].Pf_FlowLabel = flow_label;
                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0100;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(flow_label);
        }
        if(Count > 10)
        {//Pf_Direct
            Size   = 1;
            Index  = 10;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &direction, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if (ERR_SUCCESS != Result)
                {
                    AT_TC(g_sw_GPRS, "Get 11th Parameter error! Result = %x, direction = %d", Result, direction);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: direction = %d", direction);
                g_At_Tft_Set[nSim][cid].Pf_Direct = direction;

                g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0020;
            }
            hal_HstSendEvent(0x06240900+Index);
            hal_HstSendEvent(direction);
        }
        if(Count > 11)
        {
            Size   = 31;
            Index  = 11;
            UINT8 SourceMask[32] = {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &SourceMask, &Size);
            if(ERR_AT_UTIL_CMD_PARA_NULL != Result)
            {
                if ((ERR_SUCCESS != Result) ||(AT_StrLen(SourceMask) > 31))
                {
                    AT_TC(g_sw_GPRS, "Get 12th Parameter error! Result = %x", Result);
                    goto FAILED_EXIT;
                }
                AT_TC(g_sw_GPRS, "CGTFT: SourceMask = %s", SourceMask);

                UINT8 i, k = 0;
                UINT8 nAddrLen = 0;
                for(i = 0; i < AT_StrLen(SourceMask); i++) // try to find fourth . and set it to 0
                {
                    if(SourceMask[i] == '.')
                        k++;
                    if(k == 4)
                    {
                        k = 0xFF;
                        SourceMask[i] = 0;
                        i++;
                        break;
                    }
                }
                if(k != 0xFF)
                {
                    AT_TC(g_sw_GPRS, "SourceMask address is invalid(%s)", SourceMask);
                    goto FAILED_EXIT;
                }
                
                AT_TC(g_sw_GPRS, "source address is %s", SourceMask);
                AT_TC(g_sw_GPRS, "mask address is %s",  SourceMask + i);

                if(AT_GPRS_IPStringToPDPAddr(SourceMask, i, &g_At_Tft_Set[nSim][cid].Pf_LocalAddrIPv4[0], &nAddrLen) != 1)
                {
                    AT_TC(g_sw_GPRS, "source address is invalid(%s)", SourceMask);
                    goto FAILED_EXIT;
                }
                if(nAddrLen > 4)
                {
                    AT_TC(g_sw_GPRS, "source address len error (%s)", SourceMask);
                    goto FAILED_EXIT;
                }

                if(AT_GPRS_IPStringToPDPAddr(&(SourceMask[i]), AT_StrLen(SourceMask) - i, &g_At_Tft_Set[nSim][cid].Pf_LocalAddrIPv4[4], &nAddrLen) != 1)
                {
                    AT_TC(g_sw_GPRS, "mask address is invalid(%s)", SourceMask + i);
                    goto FAILED_EXIT;
                }
                if(nAddrLen > 4)
                {
                    AT_TC(g_sw_GPRS, "mask address len error (%s)", SourceMask + i);
                    goto FAILED_EXIT;
                }
            }
            hal_HstSendEvent(0x06240900+Index);
            g_At_Tft_Set[nSim][cid].Pf_BitMap |= 0x0010;
        }
    #ifdef LTE_NBIOT_SUPPORT
        Result = CFW_GprsSetCtxTFT(cid, &g_At_Tft_Set[nSim][cid], nSim);
    #endif
        if(Result == ERR_SUCCESS)
            goto OK_EXIT;
        else
            goto FAILED_EXIT;
    }
    else if(AT_CMD_READ == pParam->iType)
    {
        UINT8 nResp[1024]= {0};
        UINT8 i = 0;
        AT_TC(g_sw_GPRS, "CGTFT: AT_CMD_READ");
        for(i = 0; i< 7; i++)
        {
            if(g_At_Tft_Set[nSim][i].Pf_ID == 0)
                   continue;
            AT_Sprintf(nResp, "+CGTFT: %d, %d, %d, %d.%d.%d.%d.%d.%d.%d.%d, %d,%d.%d,%d.%d,%x,%d.%d",
                                            i,g_At_Tft_Set[nSim][i].Pf_ID, g_At_Tft_Set[nSim][i].Pf_EvaPreIdx,
                                            g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[0],g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[1],
                                            g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[2],g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[3],
                                            g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[4],g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[5],
                                            g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[6],g_At_Tft_Set[nSim][i].Pf_RmtAddrIPv4[7],
                                            g_At_Tft_Set[nSim][i].Pf_PortNum_NH,
                                            g_At_Tft_Set[nSim][i].Pf_LocalPortRangeMin,g_At_Tft_Set[nSim][i].Pf_LocalPortRangeMax,
                                            g_At_Tft_Set[nSim][i].Pf_RemotPortRangeMin,g_At_Tft_Set[nSim][i].Pf_RemotPortRangeMax,
                                            g_At_Tft_Set[nSim][i].Pf_SPI,
                                            g_At_Tft_Set[nSim][i].Pf_TOS_TC[0],g_At_Tft_Set[nSim][i].Pf_TOS_TC[1]);

            
            PAT_CMD_RESULT pResult = AT_CreateRC(CMD_FUNC_CONTINUE,
                                                 CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME,
                                                 0, nResp, SUL_Strlen((CONST TCHAR *)nResp), nDLCI);
            AT_Notify2ATM(pResult, nDLCI);
            if (pResult != NULL)
                AT_FREE(pResult);
        }
        goto OK_EXIT;
    }
    else if(AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_GPRS, "CGTFT: AT_CMD_TEST");
        UINT8 nResp[]="(IP), (1-8),(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255),4,(0-65535).(0-65535),(0-65535).(0-65535),(0-FFFFFFFF),(0-255).(0-255)";
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nResp, SUL_Strlen((CONST TCHAR *)nResp), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        goto FAILED_EXIT;
    }

    goto OK_EXIT;

OK_EXIT:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;

FAILED_EXIT:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

VOID AT_GPRS_CmdFunc_CGCMOD(AT_CMD_PARA *pParam)
{
    UINT32 uErrCode = ERR_SUCCESS;
    UINT8 uIndex = 0;
    UINT8 uCid = 0;
    UINT16 uSize;
    INT32 iResult;
    UINT8* pResp = NULL;
    UINT8 uParamCount = 0;
    CFW_GPRS_QOS* pQos = NULL;
    CFW_TFT* pTft = NULL;
    UINT8 uUti = pParam->nDLCI;
    
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    
    AT_Gprs_CidInfo* g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGCMOD:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == pParam->iType)
    {
        // Get the count of parameters.
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGCMOD:get para count fai");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // check  parameters count.
        if (uParamCount < AT_PDPCID_MIN || uParamCount > AT_PDPCID_MAX)
        {
            AT_TC(g_sw_GPRS, "AT+CGCMOD:uParamCount error,uParamCount = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        pQos = (CFW_GPRS_QOS*)AT_MALLOC(sizeof(CFW_GPRS_QOS));
        if (NULL == pQos)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        pTft = (CFW_TFT*)AT_MALLOC(sizeof(CFW_TFT));
        if (NULL == pTft)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        // Get the parameter list.
        for(uIndex = 0; uIndex < uParamCount; uIndex++)
        {
            uSize   = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGCMOD:Get Para failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX || (0 == g_staAtGprsCidInfo[uCid].uCid))
            {
                AT_TC(g_sw_GPRS, "AT+CGCMOD:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            #ifdef LTE_NBIOT_SUPPORT
            iResult = CFW_GprsCtxEpsModify(uUti, uCid, nSim);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGCMOD:CFW_GprsCtxEpsModify() failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_EXE_FAIL;
                goto _func_fail;
            }
            #endif
        }
    }
    // Test command.
    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGCMOD:(1,......7)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }

    _func_success:

    //AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    
    if(NULL != pQos)
    {
        AT_FREE(pQos);
        pQos = NULL;
    }

    if(NULL != pTft)
    {
        AT_FREE(pTft);
        pTft = NULL;
    }
    return;

    _func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    if(NULL != pQos)
    {
        AT_FREE(pQos);
        pQos = NULL;
    }

    if(NULL != pTft)
    {
        AT_FREE(pTft);
        pTft = NULL;
    }
    return;
}

#else
//Packet filter component type identifier
#define IPV4_SRC_ADDR_TYPE      0x10        //IPv4 source address
#define IPV6_SRC_ADDR_TYPE      0x20        //IPv6 source address
#define PROTOCOL_IDENT_TYPE     0x30        //Protocol identifier/Next header type
#define SINGLE_DEST_PORT_TYPE   0x40        //Singe destination port type
#define DEST_PORT_RANGE_TYPE    0x41        //Destination prot range type
#define SINGLE_SRC_PORT_TYPE        0x50        //Single source port type
#define SRC_PORT_RANGE_TYPE     0x51        //Source port range type
#define SECURITY_PARA_INDEX_TYPE    0x60        //Security parameter index type
#define TOS_OR_TRAFFIC_CLASS_TYPE   0x70        //Type of service/Traffic class type
#define FLOW_LABEL_TYPE         0x80        //Flow label type

#if 0
struct sTrafficFlowTemplate //Traffic Flow Template
{
//  UINT8 uTrafficFlowIE;       //Traffic flow template IEI
//  UINT8 uLength;
    UINT8 uOperNum;         //TFT operation code and Number of packet filters
    UINT8 PacketFilterList[0];  //Packet filter list
}
#endif

struct sTrafficFlowTemplate
{
    UINT8 uLength;
    UINT8 uTFT[255];            //257 - 3 should be OK.
};

struct sTrafficFlowTemplate g_TrafficFlowTemplate[CFW_SIM_NUMBER][7]= {{{0,""}},};
UINT8 setTrafficFlowTemplate(UINT8* uTFT, UINT8 type, UINT8* data)
{
    UINT8 size = 0;
    switch(type)
    {
    case IPV4_SRC_ADDR_TYPE:
        uTFT[0] = type;
        uTFT[1] = 8;
        UINT8 i = 0;
        for(i = 0; i< 8; i++)
            uTFT[2 + i] = data[i];
        size =10;
        break;
    case SECURITY_PARA_INDEX_TYPE:
    case SRC_PORT_RANGE_TYPE:
    case DEST_PORT_RANGE_TYPE   :
        uTFT[0] = type;
        uTFT[1] = 4;
        uTFT[2] = data[0];
        uTFT[3] = data[1];
        uTFT[4] = data[2];
        uTFT[5] = data[3];
        size =6;
        break;
    case PROTOCOL_IDENT_TYPE:
        uTFT[0] = type;
        uTFT[1] = 1;
        uTFT[2] = data[0];
        size = 3;
        break;
    case TOS_OR_TRAFFIC_CLASS_TYPE:
        uTFT[0] = type;
        uTFT[1] = 2;
        uTFT[2] = data[0];
        uTFT[3] = data[1];
        size = 4;
        break;
    }
    AT_TC(g_sw_GPRS, "AT+CGTFT: size = %d",size);
    AT_TC_MEMBLOCK(g_sw_GPRS,  uTFT, 10, 16);
    return size;
}

//at+cgtft=1,7,254,123.32.23.43.89.67.65.125,234,1234.5432,6789.8876,8432d,235.54
VOID AT_GPRS_CmdFunc_CGTFT(AT_CMD_PARA *pParam)
{
    UINT8 cid = 0;
    UINT8 packet_filter_id = 0;         //packet  filter identifier
    UINT8 eval_prece_index = 0;         //evaluation precedence index

    struct
    {
        in_addr addr;           //source address
        in_addr mask;           //subnet mask
    } addr_mask;

    UINT8 protocol = 0;                 //protocol number
    UINT32 dest_port_range = 0;         //destination port range
    UINT32 source_port_range = 0;           //source port range
    UINT32 ipsec_index = 0;             //ipsec security parameter index
    //UINT16 tos __attribute__ ((aligned(4)));  //type of service and mask
    UINT16 tos;

    INT32 Result = 0;
    UINT8 Count = 0;
    UINT32 uErrCode = ERR_AT_CME_PARAM_INVALID;

    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        AT_TC(g_sw_GPRS, "AT+CGTFT: Input parameter is NULL!");
        goto FAILED_EXIT;
    }

    UINT8 nDLCI = pParam->nDLCI;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (AT_CMD_SET == pParam->iType)
    {
        Result = AT_Util_GetParaCount(pParam->pPara, &Count);
        if (ERR_SUCCESS != Result)
        {
            AT_TC(g_sw_GPRS, "Get the parameter count Failed. ErrorCode = %d.", Result);
            goto FAILED_EXIT;
        }

        UINT16 Size = 0;
        UINT8 Index = 0;

        if(Count > 0)
        {
            Size   = 1;
            Index  = 0;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &cid, &Size);
            if ((ERR_SUCCESS != Result) ||(cid < AT_PDPCID_MIN) ||(cid > AT_PDPCID_MAX))
            {
                AT_TC(g_sw_GPRS, "Get 1st Parameter error! Result = %x, cid = %d", Result, cid);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: cid = %d", cid);
            if(Count == 1)
            {
                memset(g_TrafficFlowTemplate[nSim][cid].uTFT, 0,255);
                g_TrafficFlowTemplate[nSim][cid].uLength = 0;
                goto OK_EXIT;
            }
        }
        if(Count > 1)
        {
            Size   = 1;
            Index  = 1;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &packet_filter_id, &Size);
            if ((ERR_SUCCESS != Result) ||(packet_filter_id < 1) ||(packet_filter_id > 8))
            {
                AT_TC(g_sw_GPRS, "Get second Parameter error! Result = %x, pfid = %d", Result, packet_filter_id);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: packet_filter_id = %d", packet_filter_id);
        }

        if(Count > 2)
        {
            Size   = 1;
            Index  = 2;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &eval_prece_index, &Size);
            if ((ERR_SUCCESS != Result))
            {
                AT_TC(g_sw_GPRS, "Get third Parameter error! Result = %x, pfid = %d", Result, eval_prece_index);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: eval_prece_index = %d", eval_prece_index);
        }
        //pPacketFilter point to the packet filter will to be accessed.
        UINT8* pPacketFilter = g_TrafficFlowTemplate[nSim][cid].uTFT;
        UINT8 uOffset = 0;
        UINT8 uLength = g_TrafficFlowTemplate[nSim][cid].uLength;

        while(uLength - uOffset> 0)
        {
            if(pPacketFilter[0] == 0)
                break;
            if(pPacketFilter[0] == packet_filter_id)
            {
                if(pPacketFilter[1] != eval_prece_index)
                    goto FAILED_EXIT;
                else
                {
                    //pPacketFilter += 3;   //point to component id(packet_filter_id, eval_prece_index, length)
                    AT_TC(g_sw_GPRS, "CGTFT: pPacketFilter = %x,pPacketFilter[0] = %x", pPacketFilter,pPacketFilter[0]);
                    break;          // found it.
                }
            }
            else
            {
                uOffset =  3 + pPacketFilter[2];        //one byte for packet filter iden, one for evaluation one for length
                pPacketFilter += uOffset;
                AT_TC(g_sw_GPRS, "CGTFT: uOffset = %x, pPacketFilter = %x", uOffset, pPacketFilter);
                continue;
            }
        }
        if(uLength + 3 > 254)
            goto FAILED_EXIT;
        UINT8* pLength = 0;
        if(pPacketFilter[0] == 0)
        {
            AT_TC(g_sw_GPRS, "CGTFT: g_TrafficFlowTemplate[nSim][cid].uTFT = %x, g_TrafficFlowTemplate[nSim][cid].uTFT + uOffset = %x",
                  g_TrafficFlowTemplate[nSim][cid].uTFT, g_TrafficFlowTemplate[nSim][cid].uTFT + uOffset);
            AT_TC(g_sw_GPRS, "CGTFT: New Packet Filter ID = %d, pPacketFilter = %x", packet_filter_id, pPacketFilter);
            pPacketFilter[0] = packet_filter_id;
            pPacketFilter[1] = eval_prece_index;
            pPacketFilter[2] = 0;
            pLength = pPacketFilter + 2;
            pPacketFilter += 3;     //pPacketFilter point to content.
            g_TrafficFlowTemplate[nSim][cid].uLength += 3;
        }
        else
        {
            //this is not last one, move following to current position.
            UINT8 uLen = pPacketFilter - g_TrafficFlowTemplate[nSim][cid].uTFT;
            AT_TC(g_sw_GPRS, "CGTFT: g_TrafficFlowTemplate[nSim][cid].uLength = %d",g_TrafficFlowTemplate[nSim][cid].uLength);
            AT_TC(g_sw_GPRS, "CGTFT: uLen = %d, pPacketFilter[2] = %x", uLen, pPacketFilter[2]);

            UINT8 follow = uLen + 3 + pPacketFilter[2];
            AT_TC(g_sw_GPRS, "CGTFT: follow = %d, pPacketFilter = %x", follow, pPacketFilter);
            follow = g_TrafficFlowTemplate[nSim][cid].uLength -  follow;
            g_TrafficFlowTemplate[nSim][cid].uLength -= pPacketFilter[2];
            if(follow > 0);
            {
                memcpy(pPacketFilter, pPacketFilter + 3 + pPacketFilter[2], follow);
                pPacketFilter += follow;
                memset(pPacketFilter, 0 , 255 - follow - uLen);
                AT_TC(g_sw_GPRS, "CGTFT: follow = %d, pPacketFilter = %x", follow, pPacketFilter);
            }
            pPacketFilter[0] = packet_filter_id;
            pPacketFilter[1] = eval_prece_index;
            pPacketFilter[2] = 0;
            pLength = pPacketFilter + 2;
            pPacketFilter += 3; //point to component id(packet_filter_id, eval_prece_index, length)
            //g_TrafficFlowTemplate[nSim][cid].uLength -= uLen;
            AT_TC(g_sw_GPRS, "CGTFT: g_TrafficFlowTemplate[nSim][cid].uTFT = %x",g_TrafficFlowTemplate[nSim][cid].uTFT);
            AT_TC(g_sw_GPRS, "CGTFT: g_TrafficFlowTemplate[nSim][cid].uLength = %d,uLen = %d", g_TrafficFlowTemplate[nSim][cid].uLength,uLen);
        }
        uLength = 0;
        uOffset = 0;
        if(Count > 3)
        {
            Size   = 31;
            Index  = 3;
            UINT8 SourceMask[32] = {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &SourceMask, &Size);
            if ((ERR_SUCCESS != Result) ||(strlen(SourceMask) > 31))
            {
                AT_TC(g_sw_GPRS, "Get fourth Parameter error! Result = %x", Result);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: SourceMask = %s", SourceMask);

            UINT8 i, k = 0;
            for(i = 0; i < strlen(SourceMask); i++) // try to find fourth . and set it to 0
            {
                if(SourceMask[i] == '.')
                    k++;
                if(k == 4)
                {
                    k = 0xFF;
                    SourceMask[i] = 0;
                    i++;
                    break;
                }
            }

            if(k != 0xFF)
            {
                AT_TC(g_sw_GPRS, "SourceMask address is invalid(%s)", SourceMask);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "source address is %s", SourceMask);
            AT_TC(g_sw_GPRS, "mask address is %s",  SourceMask + i);

            if(0 == inet_aton(SourceMask, &addr_mask.addr))
            {
                AT_TC(g_sw_GPRS, "source address is invalid(%s)", SourceMask);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: source_addr = 0x%x", addr_mask.addr);
            if(0 == inet_aton(SourceMask + i, &addr_mask.mask))
            {
                AT_TC(g_sw_GPRS, "mask address is invalid(%s)", SourceMask + i);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: subnet_mask = 0x%x", addr_mask.mask);
            AT_TC(g_sw_GPRS, "CGTFT: source_addr = 0x%x", addr_mask.addr);
            uLength = setTrafficFlowTemplate(pPacketFilter, IPV4_SRC_ADDR_TYPE, (UINT8*)&addr_mask);
            pPacketFilter += uLength;
            uOffset += uLength;
            //g_TrafficFlowTemplate[nSim][cid].uLength += uLength;
        }

        if(Count > 4)
        {
            Size   = 1;
            Index  = 4;
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_UINT8, &protocol, &Size);
            if (ERR_SUCCESS != Result)
            {
                return;
                AT_TC(g_sw_GPRS, "Get fifth Parameter error! Result = %x, protocol = %d", Result, protocol);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: protocol = 0x%x", protocol);
            uLength = setTrafficFlowTemplate(pPacketFilter, PROTOCOL_IDENT_TYPE, &protocol);
            pPacketFilter += uLength;
            uOffset += uLength;
            //g_TrafficFlowTemplate[nSim][cid].uLength += uLength;
        }

        if(Count > 5)
        {
            Size   = 12;
            Index  = 5;
            UINT8 port_range[12]= {0};      //destination port range
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &port_range, &Size);
            if((ERR_SUCCESS != Result) ||(at_GetRange(port_range, &dest_port_range) == 0))
            {
                AT_TC(g_sw_GPRS, "Get sixth Parameter error! Result = %x, dest_port_range = %d", Result, dest_port_range);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: dest_port_range = %d", dest_port_range);
            uLength = setTrafficFlowTemplate(pPacketFilter, SRC_PORT_RANGE_TYPE, (UINT8*)&dest_port_range);
            pPacketFilter += uLength;
            uOffset += uLength;
            //g_TrafficFlowTemplate[nSim][cid].uLength += uLength;
        }
        if(Count > 6)
        {
            Size   = 12;
            Index  = 6;
            UINT8 port_range[12]= {0};  //source port range
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &port_range, &Size);
            if ((ERR_SUCCESS != Result)||(at_GetRange(port_range, &source_port_range) == 0))
            {
                AT_TC(g_sw_GPRS, "Get seventh Parameter error! Result = %x, source_port_range = %d", Result, source_port_range);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: source_port_range = %d", source_port_range);
            uLength = setTrafficFlowTemplate(pPacketFilter, DEST_PORT_RANGE_TYPE, (UINT8*)&source_port_range);
            pPacketFilter += uLength;
            uOffset += uLength;
            //g_TrafficFlowTemplate[nSim][cid].uLength += uLength;
        }

        if(Count > 7)
        {
            Size   = 8;
            Index  = 7;
            UINT8 ipsec[9]= {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &ipsec, &Size);
            if ((ERR_SUCCESS != Result) || (strlen(ipsec) > 8))
            {
                AT_TC(g_sw_GPRS, "Get 8th Parameter error! Result = %x, ipsec_index = %d", Result, ipsec_index);
                goto FAILED_EXIT;
            }
            if(SUL_ascii2hex(ipsec, (UINT8*)&ipsec_index) == 0)
            {
                AT_TC(g_sw_GPRS, "Get 8th Parameter error! ipsec = %d",ipsec);
                goto FAILED_EXIT;
            }
            UINT8* p = (UINT8*)&ipsec_index;
//          AT_TC(g_sw_GPRS, "CGTFT: ipsec_index = 0x%x%x%x%x", *(p++), *(p++), *(p++), *(p++));
//          p = (UINT8*)&ipsec_index;
            INT8 i = 0;
            INT8 j = 4 - ((strlen(ipsec)+1) >> 1);
            AT_TC(g_sw_GPRS, "Index = %d,strlen(ipsec) = %d",Index,strlen(ipsec));
            for(i=0; i < 4; i++)
            {
                if(3 -j - i >= 0)
                    p[3-i] = p[3-j - i];
                else
                    p[3 - i] = 0;
            }
            uLength = setTrafficFlowTemplate(pPacketFilter, SECURITY_PARA_INDEX_TYPE, (UINT8*)&ipsec_index);
            pPacketFilter += uLength;
            uOffset += uLength;
            //g_TrafficFlowTemplate[nSim][cid].uLength += uLength;
        }

        if(Count > 8)
        {
            Size   = 7;
            Index  = 8;
            UINT8 type_of_service[8]= {0};
            Result = AT_Util_GetParaWithRule(pParam->pPara, Index, AT_UTIL_PARA_TYPE_STRING, &type_of_service, &Size);
            UINT32 uTypeOfService = 0;
            if ((ERR_SUCCESS != Result)||(at_GetRange(type_of_service, &uTypeOfService) == 0))
            {
                AT_TC(g_sw_GPRS, "Get 9th Parameter error! Result = %x, tos = %d", Result, uTypeOfService);
                goto FAILED_EXIT;
            }
            AT_TC(g_sw_GPRS, "CGTFT: uTypeOfService = %x", uTypeOfService);
            uTypeOfService = ntohl(uTypeOfService);
            if((( uTypeOfService>> 16) > 255) || ((uTypeOfService & 0xFFFF) > 255))
            {
                AT_TC(g_sw_GPRS, "Get 9th Parameter error!high tos = %d, low = %d", uTypeOfService>>16, uTypeOfService&0xFFFF);
                goto FAILED_EXIT;
            }
            tos=(uTypeOfService>>8)|(uTypeOfService&0xFF);
            tos=htons(tos);
            AT_TC(g_sw_GPRS, "CGTFT: uTypeOfService = %x", uTypeOfService);
            AT_TC(g_sw_GPRS, "CGTFT: high tos = %d, low = %d", tos>>8, tos&0xFF);
            uLength = setTrafficFlowTemplate(pPacketFilter, TOS_OR_TRAFFIC_CLASS_TYPE, (UINT8*)&tos);
            pPacketFilter += uLength;
            uOffset += uLength;
        }
        g_TrafficFlowTemplate[nSim][cid].uLength += uOffset;
        *pLength = uOffset;     //Update length of packet filter id
        AT_TC_MEMBLOCK(g_sw_GPRS,  g_TrafficFlowTemplate[nSim][cid].uTFT, 255, 16);
        AT_TC(g_sw_GPRS, "CGTFT: g_TrafficFlowTemplate[nSim][cid].uLength = %x", g_TrafficFlowTemplate[nSim][cid].uLength);

        goto OK_EXIT;
    }
    else if(AT_CMD_READ == pParam->iType)
    {
        AT_TC(g_sw_GPRS, "CGTFT: AT_CMD_READ");
        UINT8 nResp[1024]= {0};
        UINT8 i = 0,j = 0,k=0;
        for(i = 0; i< 7; i++)
        {
            UINT8* pValue = g_TrafficFlowTemplate[nSim][i].uTFT;
            if(pValue[0] == 0)
                continue;

            UINT8 pid = pValue[0];
            UINT8 evaluid=pValue[1];
            UINT8 uLength = pValue[2];
            //UINT8 uLength = g_TrafficFlowTemplate[nSim][i].uLength;
            UINT8 IPv4[16] = {0};
            UINT8 Mask[16] = {0};
            UINT8 IPversion = 0;
            UINT8 SourcePort[32]= {0};
            UINT8 DestPort[32]= {0};
            UINT32 ipes = 0;
            UINT8 TosMask[8]= {0};
            pValue += 3;
            j =AT_Sprintf(nResp,"+CGTFT: %d, %d, %d", i, pid,evaluid);

            for(k=0; k< uLength; k+=2)
            {
                UINT8 type = *pValue++;
                k += *pValue++;     //point to Value
                switch(type)
                {
                case IPV4_SRC_ADDR_TYPE:
                {
                    in_addr temp;
                    AT_TC(g_sw_GPRS, "CGTFT: pIPv4  = %x.%x.%x.%x",pValue[0],pValue[1],pValue[2],pValue[3]);
                    temp.s_addr = pValue[3]<<24|pValue[2]<<16|pValue[1]<<8|pValue[0];
                    UINT8* p = inet_ntoa(temp);
                    memcpy(IPv4, p, strlen(p));
                    pValue += 4;
                    AT_TC(g_sw_GPRS, "CGTFT: pMask = %x.%x.%x.%x",pValue[0],pValue[1],pValue[2],pValue[3]);
                    temp.s_addr = pValue[3]<<24|pValue[2]<<16|pValue[1]<<8|pValue[0];
                    p = inet_ntoa(temp);
                    memcpy(Mask, p, strlen(p));
                    pValue += 4;
                    AT_TC(g_sw_GPRS, "CGTFT: pIPv4.pMask  = %s.%s",IPv4,Mask);
                    break;
                }
                case PROTOCOL_IDENT_TYPE:
                    IPversion = *pValue;
                    pValue++;
                    AT_TC(g_sw_GPRS, "CGTFT: IPversion  = %d", IPversion);
                    break;
                case DEST_PORT_RANGE_TYPE:
                {
                    UINT16 mini = pValue[1]<<8|pValue[0];
                    AT_TC(g_sw_GPRS, "CGTFT: DestPort  = %x", mini);
                    pValue += 2;
                    UINT16 max = pValue[1]<<8|pValue[0];
                    AT_Sprintf(DestPort,"%d.%d", ntohs(mini), ntohs(max));
                    pValue += 2;
                    AT_TC(g_sw_GPRS, "CGTFT: DestPort  = %s", DestPort);
                    break;
                }
                case SRC_PORT_RANGE_TYPE:
                {
                    UINT16 mini = pValue[1]<<8|pValue[0];
                    AT_TC(g_sw_GPRS, "CGTFT: SourcePort  = %x", mini);
                    pValue += 2;
                    UINT16 max = pValue[1]<<8|pValue[0];
                    AT_Sprintf(SourcePort,"%d.%d", ntohs(mini), ntohs(max));
                    pValue += 2;
                    AT_TC(g_sw_GPRS, "CGTFT: SourcePort  = %s", SourcePort);
                    break;
                }
                case SECURITY_PARA_INDEX_TYPE:
                {
                    ipes = pValue[3]<< 24| pValue[2]<<16|pValue[1]<<8|pValue[0];
                    ipes = ntohl(ipes);
                    AT_TC(g_sw_GPRS, "CGTFT: ipes  = %x", ipes);
                    pValue += 4;
                    break;
                }
                case TOS_OR_TRAFFIC_CLASS_TYPE:
                {
                    UINT16 temp = pValue[1] << 8| pValue[0];
                    temp = ntohs(temp);
                    AT_Sprintf(TosMask,"%d.%d", temp >> 8, temp & 0xFF);
                    pValue += 2;
                    AT_TC(g_sw_GPRS, "CGTFT: tos  = %s", TosMask);
                    break;
                }
                default:
                    goto FAILED_EXIT;
                }
            }
            if(IPv4[0] != 0)
                j+=AT_Sprintf(nResp + j,",%s.%s", IPv4, Mask);
            //else
            //  j+=AT_Sprintf(nResp + j,",");

            if(IPversion != 0)
                j+=AT_Sprintf(nResp + j,",%d", IPversion);
            //else
            //  j+=AT_Sprintf(nResp + j,",");
            if(SourcePort[0] != 0)
                j+=AT_Sprintf(nResp + j,",%s", SourcePort);
            //else
            //  j+=AT_Sprintf(nResp + j,",");
            if(DestPort[0] != 0)
                j+=AT_Sprintf(nResp + j,",%s", DestPort);
            //else
            //  j+=AT_Sprintf(nResp + j,",");
            if(ipes != 0)
                j+=AT_Sprintf(nResp + j,",%X", ipes);
            //else
            //  j+=AT_Sprintf(nResp + j,",");
            if(TosMask[0] != 0)
                j+=AT_Sprintf(nResp + j,",%s", TosMask);
            //else
            //  j+=AT_Sprintf(nResp + j,",");

            PAT_CMD_RESULT pResult = AT_CreateRC(CMD_FUNC_CONTINUE,
                                                 CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME,
                                                 0, nResp, SUL_Strlen(nResp), nDLCI);
            AT_Notify2ATM(pResult, nDLCI);
            if (pResult != NULL)
                AT_FREE(pResult);
        }
        goto OK_EXIT;
    }
    else if(AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_GPRS, "CGTFT: AT_CMD_TEST");
        UINT8 nResp[]="(IP), (1-8),(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255).(0-255),4,(0-65535).(0-65535),(0-65535).(0-65535),(0-FFFFFFFF),(0-255).(0-255)";
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nResp, SUL_Strlen(nResp), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        goto FAILED_EXIT;
    }

    goto OK_EXIT;

OK_EXIT:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;

FAILED_EXIT:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}
#endif //LTE_NBIOT_SUPPORT
// This function to process the AT+CGQREQ command. include set,read and test command.
//
// Set command
// +CGQREQ=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGQREQ?
// Possible response(s)
// +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
// [<CR><LF>+CGQREQ: <cid>, <precedence >, <delay>, <reliability.>, <peak>, <mean>
// […]]
//
// Test command
// +CGQREQ=?
// Possible response(s)
// +CGQREQ: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [<CR><LF>+CGQREQ: <PDP_type>, (list of supported <precedence>s), (list of supported
// <delay>s), (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// […]]
//
VOID AT_GPRS_CmdFunc_CGQREQ(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 *pResp = NULL;
    UINT8 uCid   = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_GPRS_QOS sQos = { 0, 0, 0, 0, 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGQREQ:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == pParam->iType)
    {

        // Get the count of parameters.
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGQREQ:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // This command have 6 parameters.
        if (uParamCount < 1 || uParamCount > 6)
        {
            AT_TC(g_sw_GPRS, "AT+CGQREQ:uParamCount error,uParamCount = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // Get the parameter list.

        // 1#
        // Get cir
        if (uParamCount >= 1)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get cir failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            // yy [mod] 2008-5-20 for bug ID 8487
            if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX || (0 == g_staAtGprsCidInfo[uCid].uCid))
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 2#
        // Get precedence
        if (uParamCount >= 2)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sQos.nPrecedence, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get precedence failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            // if ignore,set it zero.
            if (0 == uSize)
            {
                sQos.nPrecedence = 0;
            }
            else if (sQos.nPrecedence > 3)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param sQos.nPrecedence error,sQos.nPrecedence = %d.", sQos.nPrecedence);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 3#
        // Get delay
        if (uParamCount >= 3)
        {
            uIndex++;
            uSize   = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sQos.nDelay, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get delay failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            // if ignore,set it zero.
            if (0 == uSize)
            {
                sQos.nDelay = 0;
            }
            else if (sQos.nDelay > 4)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param sQos.nDelay error,sQos.nDelay = %d.", sQos.nDelay);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 4#
        // Get reliability
        if (uParamCount >= 4)
        {
            uIndex++;
            uSize   = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sQos.nReliability, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get reliability failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sQos.nReliability = 0;
            }
            else if (sQos.nReliability > 5)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param sQos.nReliability error,sQos.nReliability = %d.", sQos.nReliability);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }

        // 5#
        // Get peak
        if (uParamCount >= 5)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sQos.nPeak, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get peak failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sQos.nPeak = 0;
            }
            else if (sQos.nPeak > 9)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param sQos.nPeak error,sQos.nPeak = %d.", sQos.nPeak);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

        }

        // 6#
        // Get mean
        if (uParamCount >= 6)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sQos.nMean, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get mean failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sQos.nMean = 0;
            }
            else if (18 < sQos.nMean && 31 != sQos.nMean)
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:The param sQos.nMean error,sQos.nMean = %d.", sQos.nMean);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetReqQos(uCid, &sQos, nSim);
#endif
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGQREQ:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;

            }
            AT_MemSet(pResp, 0, SIZEOF(pResp));

            // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
            AT_Sprintf(pResp, "+CGQREQ:%d,%d,%d,%d,%d,%d",
                       uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
            goto _func_success;
        }

    }

    // Read command
    else if (AT_CMD_READ == pParam->iType)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS

            iResult = CFW_GprsGetReqQos(uCid, &sQos, nSim);
#endif
            if (ERR_SUCCESS == iResult)
            {
                if (NULL == pResp)
                {
                    pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);

                    if (NULL == pResp)
                    {
                        uErrCode = ERR_AT_CME_NO_MEMORY;
                        goto _func_fail;

                    }
                }

                AT_MemSet(pResp, 0, SIZEOF(pResp));

                // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
                AT_Sprintf(pResp, "+CGQREQ:%d,%d,%d,%d,%d,%d",
                           uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:CFW_GprsGetReqQos() failed,iResult = 0x%x,uCid = %d.", iResult, uCid);

                // The cid of specified is NULL.
                // Do nothing.
            }
        }

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }

    // Test command.
    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;

        }

        // [[ yy [mod] 2008-5-13 for bug ID 8472
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGQREQ: IP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGQREQ: IPV6, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGQREQ: PPP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;

        // ]] yy [mod] 2008-5-13 for bug ID 8472
    }
    else
    {
        // [[ yy [mod] 2008-6-3 for bug ID
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;

        // ]] yy [mod] 2008-6-3 for bug ID
        return;
    }

_func_success:

    // AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, (UINT16)AT_StrLen(pResp), pParam->nDLCI);
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;

_func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;
}

#ifdef __MULTI_RAT__

// This function to process the AT+CGEQREQ command. include set,read and test command.
// Set command

VOID AT_GPRS_CmdFunc_CGEQREQ(AT_CMD_PARA *pParam)
{
    UINT32 iResult;
    UINT8 *pResp = NULL;
    UINT8 uCid   = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    UINT16 uPara = 0;
    UINT8 arrCharacterSet[10] = {0,};
    CFW_GPRS_QOS sQos = {0,};

    CFW_SIM_ID nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGEQREQ:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((ERR_SUCCESS != iResult)||(uParamCount < 1 )|| (uParamCount > 13))
        {
            AT_TC(g_sw_GPRS, "AT+CGEQREQ:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult || uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX /*|| (0 == g_staAtGprsCidInfo[uCid].uCid)*/)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQREQ:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        for(uIndex=1; uIndex<uParamCount; uIndex++)
        {

            if((uIndex!=8)&&(uIndex!=9))
            {
                uSize =2;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &uPara, &uSize);
                if(ERR_AT_UTIL_CMD_PARA_NULL == iResult)
                {
                    uPara = 0;
                }
            }
            else
            {
                uSize  = SIZEOF(arrCharacterSet);
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, arrCharacterSet, &uSize);
                if(ERR_AT_UTIL_CMD_PARA_NULL == iResult)
                {
                    arrCharacterSet[0] = 1;
                }
            }
            if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex,iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            switch(uIndex)
            {
            case 1:
                sQos.nTrafficClass = uPara + 1;
                break;
            case 2:
                sQos.nMaximumbitrateUL = uPara;
                break;
            case 3:
                sQos.nMaximumbitrateDL = uPara;
                break;
            case 4:
                sQos.nGuaranteedbitrateUL = uPara;
                break;
            case 5:
                sQos.nGuaranteedbitrateDL = uPara;
                break;
            case 6:
                if(0x0 == uPara)
                {
                    uPara = 2; //(see 3GPP TS 23.107 [81])
                }
                else if(0x1 == uPara)
                {
                    uPara = 1;//(see 3GPP TS 23.107 [81])
                }
                else if(0x2 == uPara)
                {
                    uPara = 0;
                }
                else
                {
                    uPara = 3;
                }
                sQos.nDeliveryOrder = uPara;
                break;
            case 7:
                sQos.nMaximumSDUSize = uPara/10;
                break;
            case 8:
                if(0 == AT_StrCmp(arrCharacterSet,"1E2"))
                {
                    uPara = 1;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"7E3"))
                {
                    uPara = 2;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E3"))
                {
                    uPara = 3;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E4"))
                {
                    uPara = 4;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E5"))
                {
                    uPara = 5;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E6"))
                {
                    uPara = 6;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E1"))
                {
                    uPara = 7;
                }
                else
                {
                    uPara = 1;
                }
                sQos.nSDUErrorRatio = uPara;
                break;
            case 9:
                if(0 == AT_StrCmp(arrCharacterSet,"5E2"))
                {
                    uPara = 1;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E2"))
                {
                    uPara = 2;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"5E3"))
                {
                    uPara = 3;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"4E3"))
                {
                    uPara = 4;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E3"))
                {
                    uPara = 5;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E4"))
                {
                    uPara = 6;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E5"))
                {
                    uPara = 7;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E6"))
                {
                    uPara = 8;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E7"))
                {
                    uPara = 9;
                }
                else
                {
                    uPara = 1;
                }
                sQos.nResidualBER = uPara;
                break;
            case 10:
                if(0x0 == uPara)
                {
                    uPara = 3; //(see 3GPP TS 23.107 [81]) 0 - no       0 1 1  Erroneous SDUs are not delivered ('no')
                }
                else if(0x1 == uPara)
                {
                    uPara = 2;//(see 3GPP TS 23.107 [81]) 1 - yes      0 1 0   Erroneous SDUs are delivered ('yes')
                }
                else if(0x2 == uPara)
                {
                    uPara = 1;
                }
                else if(0x3 == uPara)
                {
                    uPara = 0;
                }
                else
                {
                    uPara = 7;
                }
                sQos.nDeliveryOfErroneousSDU = uPara;
                break;
            case 11:
                sQos.nTransferDelay = uPara;
                break;
            case 12:
                sQos.nTrafficHandlingPriority = uPara;
                break;
            default:
                break;

            }

        }

#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetReqQosUmts(uCid, &sQos, nSim);
#endif
        switch(sQos.nSDUErrorRatio)
        {
        case 1:
            sQos.nSDUErrorRatio = 0x1E2;
            break;
        case 2:
            sQos.nSDUErrorRatio = 0x7E3;
            break;
        case 3:
            sQos.nSDUErrorRatio = 0x1E3;
            break;
        case 4:
            sQos.nSDUErrorRatio = 0x1E4;
            break;
        case 5:
            sQos.nSDUErrorRatio = 0x1E5;
            break;
        case 6:
            sQos.nSDUErrorRatio = 0x1E6;
            break;
        case 7:
            sQos.nSDUErrorRatio = 0x1E1;
            break;
        default:
            break;
        }
        switch(sQos.nResidualBER)
        {
        case 1:
            sQos.nResidualBER = 0x5E2;
            break;
        case 2:
            sQos.nResidualBER = 0x1E2;
            break;
        case 3:
            sQos.nResidualBER = 0x5E3;
            break;
        case 4:
            sQos.nResidualBER = 0x4E3;
            break;
        case 5:
            sQos.nResidualBER = 0x1E3;
            break;
        case 6:
            sQos.nResidualBER = 0x1E4;
            break;
        case 7:
            sQos.nResidualBER = 0x1E5;
            break;
        case 8:
            sQos.nResidualBER = 0x1E6;
            break;
        case 9:
            sQos.nResidualBER = 0x1E7;
            break;
        default:
            break;
        }


        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQREQ:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pResp, 0, SIZEOF(pResp));
            AT_Sprintf(pResp, "+CGEQREQ:%d,%d,%d,%d,%d,%d,%d,%d,%x,%x,%d,%d,%d",
                       uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL, sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                       sQos.nGuaranteedbitrateDL,sQos.nDeliveryOrder,sQos.nMaximumSDUSize,sQos.nSDUErrorRatio,
                       sQos.nResidualBER,sQos.nDeliveryOfErroneousSDU,sQos.nTransferDelay,sQos.nTrafficHandlingPriority);
            goto _func_success;
        }


    }
    else if (AT_CMD_READ == pParam->iType)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS

            iResult = CFW_GprsGetReqQosUmts(uCid, &sQos, nSim);
#endif
            switch(sQos.nSDUErrorRatio)
            {
            case 1:
                sQos.nSDUErrorRatio = 0x1E2;
                break;
            case 2:
                sQos.nSDUErrorRatio = 0x7E3;
                break;
            case 3:
                sQos.nSDUErrorRatio = 0x1E3;
                break;
            case 4:
                sQos.nSDUErrorRatio = 0x1E4;
                break;
            case 5:
                sQos.nSDUErrorRatio = 0x1E5;
                break;
            case 6:
                sQos.nSDUErrorRatio = 0x1E6;
                break;
            case 7:
                sQos.nSDUErrorRatio = 0x1E1;
                break;
            default:
                break;
            }
            switch(sQos.nResidualBER)
            {
            case 1:
                sQos.nResidualBER = 0x5E2;
                break;
            case 2:
                sQos.nResidualBER = 0x1E2;
                break;
            case 3:
                sQos.nResidualBER = 0x5E3;
                break;
            case 4:
                sQos.nResidualBER = 0x4E3;
                break;
            case 5:
                sQos.nResidualBER = 0x1E3;
                break;
            case 6:
                sQos.nResidualBER = 0x1E4;
                break;
            case 7:
                sQos.nResidualBER = 0x1E5;
                break;
            case 8:
                sQos.nResidualBER = 0x1E6;
                break;
            case 9:
                sQos.nResidualBER = 0x1E7;
                break;
            default:
                break;
            }
            switch(sQos.nDeliveryOfErroneousSDU)
            {
            case 0:
                sQos.nDeliveryOfErroneousSDU = 0x3;
                break;
            case 1:
                sQos.nDeliveryOfErroneousSDU = 0x2;
                break;
            case 2:
                sQos.nDeliveryOfErroneousSDU = 0x1;
                break;
            case 3:
                sQos.nDeliveryOfErroneousSDU = 0x0;
                break;
            default:
                sQos.nDeliveryOfErroneousSDU = 0x7;
                break;
            }

            switch(sQos.nDeliveryOrder)
            {
            case 0:
                sQos.nDeliveryOrder = 0x2;
                break;
            case 1:
                sQos.nDeliveryOrder = 0x1;
                break;
            case 2:
                sQos.nDeliveryOrder = 0x0;
                break;
            case 3:
                sQos.nDeliveryOrder = 0x3;
                break;
            default:
                sQos.nDeliveryOrder = 0x0;
                break;
            }
            if (ERR_SUCCESS == iResult)
            {
                if (NULL == pResp)
                {
                    pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
                    if (NULL == pResp)
                    {
                        uErrCode = ERR_AT_CME_NO_MEMORY;
                        goto _func_fail;
                    }
                }
                AT_MemSet(pResp, 0, SIZEOF(pResp));
                AT_Sprintf(pResp, "+CGEQREQ:%d,%d,%d,%d,%d,%d,%d,%d,%x,%x,%d,%d,%d",
                           uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL, sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                           sQos.nGuaranteedbitrateDL,sQos.nDeliveryOrder,sQos.nMaximumSDUSize,sQos.nSDUErrorRatio,
                           sQos.nResidualBER,sQos.nDeliveryOfErroneousSDU,sQos.nTransferDelay,sQos.nTrafficHandlingPriority);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT+CGQREQ:CFW_GprsGetReqQos() failed,iResult = 0x%x,uCid = %d.", iResult, uCid);
            }
        }

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }

    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        AT_MemSet(pResp, 0, 128);
        AT_StrCpy(pResp, "+CGEQREQ:IP,(0,1,2,3), , , , ,(0,1), , , ,(0,1,2), , ");
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGEQREQ: IPV6,");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGEQREQ: PPP,");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;

    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }

_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;

_func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;
}


VOID AT_GPRS_CmdFunc_CGEQMIN(AT_CMD_PARA *pParam)
{
    UINT32 iResult;
    UINT8 *pResp = NULL;
    UINT8 uCid   = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    UINT8 uPara = 0;
    UINT8 arrCharacterSet[10] = {0,};
    CFW_GPRS_QOS sQos = {0,};


    CFW_SIM_ID nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGEQMIN:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((ERR_SUCCESS != iResult)||(uParamCount < 1 )|| (uParamCount > 13))
        {
            AT_TC(g_sw_GPRS, "AT+CGEQMIN:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult || uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX /*|| (0 == g_staAtGprsCidInfo[uCid].uCid)*/)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQMIN:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        for(uIndex=1; uIndex<uParamCount; uIndex++)
        {

            if((uIndex!=8)&&(uIndex!=9))
            {
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uPara, &uSize);
            }
            else
            {
                uSize  = SIZEOF(arrCharacterSet);
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, arrCharacterSet, &uSize);
            }
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQMIN:Get precedence failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            switch(uIndex)
            {
            case 1:
                sQos.nTrafficClass = uPara;
                break;
            case 2:
                sQos.nMaximumbitrateUL = uPara;
                break;
            case 3:
                sQos.nMaximumbitrateDL = uPara;
                break;
            case 4:
                sQos.nGuaranteedbitrateUL = uPara;
                break;
            case 5:
                sQos.nGuaranteedbitrateDL = uPara;
                break;
            case 6:
                sQos.nDeliveryOrder = uPara;
                break;
            case 7:
                sQos.nMaximumSDUSize = uPara;
                break;
            case 8:
                if(0 == AT_StrCmp(arrCharacterSet,"1E2"))
                {
                    uPara = 1;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"7E3"))
                {
                    uPara = 2;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E3"))
                {
                    uPara = 3;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E4"))
                {
                    uPara = 4;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E5"))
                {
                    uPara = 5;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E6"))
                {
                    uPara = 6;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E1"))
                {
                    uPara = 7;
                }
                else
                {
                    uPara = 1;
                }
                sQos.nSDUErrorRatio = uPara;
                break;
            case 9:
                if(0 == AT_StrCmp(arrCharacterSet,"5E2"))
                {
                    uPara = 1;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E2"))
                {
                    uPara = 2;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"5E3"))
                {
                    uPara = 3;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"4E3"))
                {
                    uPara = 4;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E3"))
                {
                    uPara = 5;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E4"))
                {
                    uPara = 6;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E5"))
                {
                    uPara = 7;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E6"))
                {
                    uPara = 8;
                }
                else if(0 == AT_StrCmp(arrCharacterSet,"1E7"))
                {
                    uPara = 9;
                }
                else
                {
                    uPara = 1;
                }
                sQos.nResidualBER = uPara;
                break;
            case 10:
                sQos.nDeliveryOfErroneousSDU = uPara;
                break;
            case 11:
                sQos.nTransferDelay = uPara;
                break;
            case 12:
                sQos.nTrafficHandlingPriority = uPara;
                break;
            default:
                break;

            }

        }

#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetReqQosUmts(uCid, &sQos, nSim);
#endif
        switch(sQos.nSDUErrorRatio)
        {
        case 1:
            sQos.nSDUErrorRatio = 0x1E2;
            break;
        case 2:
            sQos.nSDUErrorRatio = 0x7E3;
            break;
        case 3:
            sQos.nSDUErrorRatio = 0x1E3;
            break;
        case 4:
            sQos.nSDUErrorRatio = 0x1E4;
            break;
        case 5:
            sQos.nSDUErrorRatio = 0x1E5;
            break;
        case 6:
            sQos.nSDUErrorRatio = 0x1E6;
            break;
        case 7:
            sQos.nSDUErrorRatio = 0x1E1;
            break;
        default:
            break;
        }
        switch(sQos.nResidualBER)
        {
        case 1:
            sQos.nResidualBER = 0x5E2;
            break;
        case 2:
            sQos.nResidualBER = 0x1E2;
            break;
        case 3:
            sQos.nResidualBER = 0x5E3;
            break;
        case 4:
            sQos.nResidualBER = 0x4E3;
            break;
        case 5:
            sQos.nResidualBER = 0x1E3;
            break;
        case 6:
            sQos.nResidualBER = 0x1E4;
            break;
        case 7:
            sQos.nResidualBER = 0x1E5;
            break;
        case 8:
            sQos.nResidualBER = 0x1E6;
            break;
        case 9:
            sQos.nResidualBER = 0x1E7;
            break;
        default:
            break;
        }


        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQREQ:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pResp, 0, SIZEOF(pResp));
            AT_Sprintf(pResp, "+CGEQREQ:%d,%d,%d,%d,%d,%d,%d,%d,%x,%x,%d,%d,%d",
                       uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL, sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                       sQos.nGuaranteedbitrateDL,sQos.nDeliveryOrder,sQos.nMaximumSDUSize,sQos.nSDUErrorRatio,
                       sQos.nResidualBER,sQos.nDeliveryOfErroneousSDU,sQos.nTransferDelay,sQos.nTrafficHandlingPriority);
            goto _func_success;
        }


    }
    else if (AT_CMD_READ == pParam->iType)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsGetReqQosUmts(uCid, &sQos, nSim);
#endif
            switch(sQos.nSDUErrorRatio)
            {
            case 1:
                sQos.nSDUErrorRatio = 0x1E2;
                break;
            case 2:
                sQos.nSDUErrorRatio = 0x7E3;
                break;
            case 3:
                sQos.nSDUErrorRatio = 0x1E3;
                break;
            case 4:
                sQos.nSDUErrorRatio = 0x1E4;
                break;
            case 5:
                sQos.nSDUErrorRatio = 0x1E5;
                break;
            case 6:
                sQos.nSDUErrorRatio = 0x1E6;
                break;
            case 7:
                sQos.nSDUErrorRatio = 0x1E1;
                break;
            default:
                break;
            }
            switch(sQos.nResidualBER)
            {
            case 1:
                sQos.nResidualBER = 0x5E2;
                break;
            case 2:
                sQos.nResidualBER = 0x1E2;
                break;
            case 3:
                sQos.nResidualBER = 0x5E3;
                break;
            case 4:
                sQos.nResidualBER = 0x4E3;
                break;
            case 5:
                sQos.nResidualBER = 0x1E3;
                break;
            case 6:
                sQos.nResidualBER = 0x1E4;
                break;
            case 7:
                sQos.nResidualBER = 0x1E5;
                break;
            case 8:
                sQos.nResidualBER = 0x1E6;
                break;
            case 9:
                sQos.nResidualBER = 0x1E7;
                break;
            default:
                break;
            }

            if (ERR_SUCCESS == iResult)
            {
                if (NULL == pResp)
                {
                    pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);

                    if (NULL == pResp)
                    {
                        uErrCode = ERR_AT_CME_NO_MEMORY;
                        goto _func_fail;

                    }
                }

                AT_MemSet(pResp, 0, SIZEOF(pResp));
                AT_Sprintf(pResp, "+CGEQMIN:%d,%d,%d,%d,%d,%d,%d,%d,%x,%x,%d,%d,%d",
                           uCid, sQos.nTrafficClass, sQos.nMaximumbitrateUL, sQos.nMaximumbitrateDL, sQos.nGuaranteedbitrateUL,
                           sQos.nGuaranteedbitrateDL,sQos.nDeliveryOrder,sQos.nMaximumSDUSize,sQos.nSDUErrorRatio,
                           sQos.nResidualBER,sQos.nDeliveryOfErroneousSDU,sQos.nTransferDelay,sQos.nTrafficHandlingPriority);

                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT+CGEQMIN:CFW_GprsGetReqQos() failed,iResult = 0x%x,uCid = %d.", iResult, uCid);

            }
        }

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }

    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);

        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }

        AT_MemSet(pResp, 0, 128);
        AT_StrCpy(pResp, "+CGEQMIN:IP,(0,1,2,3), , , , ,(0,1), , , ,(0,1,2), , ");
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);
        AT_StrCpy(pResp, "+CGEQMIN: IPV6,");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_MemSet(pResp, 0, 128);

        AT_StrCpy(pResp, "+CGEQMIN: PPP,");

        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;

    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }

_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }

    return;

_func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    return;
}

#endif


UINT32 CFW_getDnsServerbyPdp(UINT8 nCid, UINT8 nSimID );

VOID AT_GPRS_CmdFunc_CGPDNSADDR(AT_CMD_PARA *pParam)
{

    INT32 iResult;
    UINT8 uParamCount;
    UINT8 uCid;
    UINT16 uSize;
    UINT8 uIndex = 0;
    UINT8 i      = 0;
    UINT8 uFlag = 0;


    UINT8 uaOutputStr[100] = { 0 };


    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    // input parameter check.

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        AT_TC(g_sw_GPRS, "+CGPDNSADDR:uParamCount %d", uParamCount);

        if (uParamCount > AT_PDPCID_MAX)
        {
            // do something...
            // whether we can trim the redunced cid in tail..?
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // will return all the PDP ctx defined,now csw has defined 7 items[1-7].
        if (uParamCount == 0)
        {
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            // disp specified cids
            AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));

            for (i = 0; i < uParamCount; i++)
            {
                uSize   = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
                AT_TC(g_sw_GPRS, "AT_Util_GetParaWithRule uCid %d",uCid);

                if (ERR_SUCCESS != iResult)
                {
                    AT_TC(g_sw_GPRS, "AT_Util_GetParaWithRule iResult %d",iResult);
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                AT_TC(g_sw_GPRS, "g_staAtGprsCidInfo[uCid].uCid %d",g_staAtGprsCidInfo[uCid].uCid);

                if (g_staAtGprsCidInfo[uCid].uCid == 0)
                {
                    continue;
                }
                UINT32 nDNSIP = 0x00;
#ifndef AT_NO_GPRS
                nDNSIP = CFW_getDnsServerbyPdp(uCid,nSim);
#endif
                AT_TC(g_sw_GPRS, "CFW_getDnsServerbyPdp nDNSIP 0x%x",nDNSIP);

                AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));
                AT_Sprintf(uaOutputStr, "+CGPDNSADDR: %d,\"%d.%d.%d.%d\"", uCid,
                           nDNSIP & 0xFF,(nDNSIP>>8) & 0xFF,(nDNSIP>>16) & 0xFF,(nDNSIP>>24) & 0xFF);

                AT_MemCpy(&uaOutputStr[AT_StrLen(uaOutputStr) - 1], "\"", 1);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                  nSim);
            }
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        break;

    case AT_CMD_TEST:
    {
        UINT16 uLen = SIZEOF("+CGPDNSADDR:(");

        AT_MemSet(uaOutputStr, 0, 100);

        AT_MemCpy(uaOutputStr, "+CGPDNSADDR:(", uLen);
        uLen -= 1;

        for (uIndex = AT_PDPCID_MIN; uIndex <= AT_PDPCID_MAX; uIndex++)
        {
            if (g_staAtGprsCidInfo[uIndex].uCid)
            {
                AT_Sprintf(&uaOutputStr[uLen], "%d,", g_staAtGprsCidInfo[uIndex].uCid);
                uLen += 2;
                uFlag = 1;
            }
        }

        if (uFlag)
        {
            uaOutputStr[uLen - 1] = ')';
            uaOutputStr[uLen] = '\0';
        }
        else
        {
            AT_MemCpy(uaOutputStr, "+CGPDNSADDR:()", sizeof("+CGPDNSADDR:()"));
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)uaOutputStr, uLen + 1, pParam->nDLCI, nSim);
        break;
    }

    default:
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;

}





// This function to process the AT+CGQMIN command. include set,read and test command.
//
// Set command
// +CGQMIN=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGQMIN?
// Possible response(s)
// +CGQMIN: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
// [<CR><LF>+CGQMIN: <cid>, <precedence >, <delay>, <reliability.>, <peak>, <mean>
// […]]
//
// Test command
// +CGQMIN=?
// Possible response(s)
// +CGQMIN: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// [<CR><LF>+CGQMIN: <PDP_type>, (list of supported <precedence>s), (list of supported <delay>s),
// (list of supported <reliability>s) , (list of supported <peak>s), (list of supported <mean>s)
// […]]
VOID AT_GPRS_CmdFunc_CGQMIN(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 uCid   = 0;
    CFW_GPRS_QOS sQos = { 0, 0, 0, 0, 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    // Set command.
    if (AT_CMD_SET == pParam->iType)
    {
        // This command have 6 parameters.
        if (pParam->paramCount < 1 || pParam->paramCount > 6)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uCid = at_ParamDefUintInRange(pParam->params[0], 0, AT_PDPCID_MIN, AT_PDPCID_MAX, &paramok);
        sQos.nPrecedence = at_ParamDefUintInRange(pParam->params[1], 0, 0, 3, &paramok);
        sQos.nDelay = at_ParamDefUintInRange(pParam->params[2], 0, 0, 4, &paramok);
        sQos.nReliability = at_ParamDefUintInRange(pParam->params[3], 0, 0, 5, &paramok);
        sQos.nPeak = at_ParamDefUintInRange(pParam->params[4], 0, 0, 9, &paramok);
        sQos.nMean = at_ParamDefUintInRange(pParam->params[5], 0, 0, 31, &paramok);
        if (!paramok && (sQos.nMean > 18 && sQos.nMean != 31))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

#ifndef AT_NO_GPRS
        iResult = CFW_GprsSetMinQos(uCid, &sQos, nSim);
#endif
        if (ERR_SUCCESS != iResult)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        uint8_t rsp[64];
        // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
        AT_Sprintf(rsp, "+CGQMIN:%d,%d,%d,%d,%d,%d",
                    uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }

    // Read command
    else if (AT_CMD_READ == pParam->iType)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsGetMInQos(uCid, &sQos, nSim);
#endif
            if (ERR_SUCCESS != iResult)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

            uint8_t rsp[64];
            // +CGQREQ: <cid>, <precedence >, <delay>, <reliability>, <peak>, <mean>
            AT_Sprintf(rsp, "+CGQMIN:%d,%d,%d,%d,%d,%d",
                        uCid, sQos.nPrecedence, sQos.nDelay, sQos.nReliability, sQos.nPeak, sQos.nMean);
            at_CmdRespInfoText(pParam->engine, rsp);
        }
        at_CmdRespOK(pParam->engine);
    }

    // Test command.
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CGQMIN: IP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");
        at_CmdRespInfoText(pParam->engine, "+CGQMIN: PPP, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");
        at_CmdRespInfoText(pParam->engine, "+CGQMIN: IPV6, (0..3), (0..4), (0..5) , (0..9), (0..18,31)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// This function to process the AT+CGATT command. include set,read and test command.
//
// Set command
// +CGATT= [<state>]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGATT?
// Possible response(s)
// +CGATT: <state>
//
// Test command
// +CGATT=?
// Possible response(s)
// +CGATT: (list of supported <state>s)
//

VOID AT_GPRS_CmdFunc_CGATT(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 pResp[16] = {0,};
    UINT8 uState = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT8 uIndex      = 0;
    UINT8 gState      = 0;
    UINT8 AttDetachType = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if(CLASS_TYPE_CC == AT_Get_ClassType())
    {
        AT_TC(g_sw_GPRS, "Gprs in class cc mode,can not attach");
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_TC(g_sw_GPRS, "AT+CGATT:NULL == pParam->pPara");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "GetParaCount,iResult = %d. ", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        if (1 != uParamCount)
        {
            AT_TC(g_sw_GPRS, "uParamCount = %d. ", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        uSize = 1;
        uIndex = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uState, &uSize);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "Get state failed,iResult = %d. ", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        if (uState > 2)
        {
            AT_TC(g_sw_GPRS, "uState > 2,uState = %d. ", uState);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
#ifndef AT_NO_GPRS
        iResult = CFW_GetGprsAttState(&gState, nSim);
#endif
        if ((ERR_SUCCESS != iResult) || ((0 != gState) && (1 != gState)&& (2 != gState)))
        {
            AT_TC(g_sw_GPRS, "CFW_GetGprsAttState() failed,iResult = 0x%x. ", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }

        g_uAttState = gState;
        if(uState == 2) //Add for gcf test by wulc
        {
            if (g_uAttState == uState)
            {
                AT_Sprintf(pResp, "+CGATT:%d", uState);
                goto _func_success;
            }
            AttDetachType = API_PS_ATTACH_GPRS|API_PS_ATTACH_FOR;
#ifndef AT_NO_GPRS
            iResult = CFW_AttDetach(CFW_GPRS_ATTACHED, pParam->nDLCI,AttDetachType, nSim,AT_GPRS_AsyncEventProcess);
#endif
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "CFW_GprsAtt failed,iResult = 0x%x. ", iResult);
                if (ERR_CME_OPERATION_NOT_ALLOWED == iResult)
                {
                    uErrCode = ERR_AT_CME_GPRS_SVR_NOT_ALLOWED;
                }
                else if (ERR_MENU_ALLOC_MEMORY == iResult)
                {
                    uErrCode = ERR_AT_CME_MEMORY_FULL;
                }
                else
                {
                    uErrCode = ERR_AT_CME_EXE_FAIL;
                }
                goto _func_fail;
            }
            else
            {
                goto _func_success_asyn;
            }
        }
        else if((gState == 1)||(gState == 0))
        {
            if (g_uAttState == uState)
            {
                AT_Sprintf(pResp, "+CGATT:%d", uState);
                goto _func_success;
            }
#ifndef AT_NO_GPRS
            iResult = CFW_GprsAttEX(uState, pParam->nDLCI, nSim,AT_GPRS_AsyncEventProcess);
#endif
            AT_TC(g_sw_GPRS, "CFW_GprsAtt,iResult = 0x%x. ", iResult);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "CFW_GprsAtt failed,iResult = 0x%x. ", iResult);
                if (ERR_CME_OPERATION_NOT_ALLOWED == iResult)
                    uErrCode = ERR_AT_CME_GPRS_SVR_NOT_ALLOWED;
                else if (ERR_MENU_ALLOC_MEMORY == iResult)
                    uErrCode = ERR_AT_CME_MEMORY_FULL;
                else
                    uErrCode = ERR_AT_CME_EXE_FAIL;
                goto _func_fail;
            }
            else
            {
                AT_AddWaitingEvent(EV_CFW_GPRS_ATT_RSP, nSim, pParam->nDLCI);
                goto _func_success_asyn;
            }
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        uState = 0;
#ifndef AT_NO_GPRS
        iResult = CFW_GetGprsAttState(&uState, nSim);
#endif
        if ((ERR_SUCCESS != iResult) || ((0 != uState) && (1 != uState)))
        {
            AT_TC(g_sw_GPRS, "CFW_GetGprsAttState() failed,iResult = 0x%x. ", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }

        AT_Sprintf(pResp, "+CGATT:%d", uState);
        goto _func_success;
    }

    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_StrCpy(pResp, "+CGATT:(0,1)");
        goto _func_success;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }

_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    return;

_func_success_asyn:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, AT_GPRS_ATTDELAY, 0, 0, pParam->nDLCI, nSim);
    return;

_func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

VOID AT_GPRS_Act_Get_Att_Rsp(UINT8 att_state)
{
    INT32 iResult;
    UINT8 i = 0;

    UINT8 nSim = g_uGPRS_Act_Wait_Att_Rsp_Info.nSim;
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    AT_TC(g_sw_GPRS, "get AT_GPRS_Act_Get_Att_Rsp");
    if (att_state != CFW_GPRS_ATTACHED)
    {
        AT_TC(g_sw_GPRS, "AT_GPRS_Act_Get_Att_Rsp err");
        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME,
                           g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI, nSim);
        return;
    }

    // clean wait state
    g_uGPRS_Act_Wait_Att_Rsp_Info.wait_state = 0;

    for (i = 0; i < g_uGPRS_Act_Wait_Att_Rsp_Info.act_count; i++)
    {

        iResult = AT_CFW_GprsAct(g_uGPRS_Act_Wait_Att_Rsp_Info.act_state, g_uGPRS_Act_Wait_Att_Rsp_Info.cid[i],
                                 g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI, g_uGPRS_Act_Wait_Att_Rsp_Info.nSim,AT_GPRS_AsyncEventProcess);

        AT_AddWaitingEvent(EV_CFW_GPRS_ACT_RSP, g_uGPRS_Act_Wait_Att_Rsp_Info.nSim, g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI);
        AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  iResult=%d", iResult);
        // if(( iResult != CFW_GPRS_ACTIVED ) && (iResult != CFW_GPRS_DEACTIVED))
        if (iResult != ERR_SUCCESS)
        {
            if (ERR_CFW_INVALID_PARAMETER == iResult)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI,
                                   nSim);
            }
            else if (ERR_CFW_NOT_EXIST_FREE_UTI == iResult)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME,
                                   g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI, nSim);
            }
            else if (ERR_CFW_UTI_IS_BUSY == iResult)
            {
                AT_TC(g_sw_GPRS, "UTI busy in exe       in AT_GPRS_CmdFunc_CGACT\n");

                // yy [mod] 2008-5-20 for bug ID 8477
                AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME,
                                   g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI, nSim);
            }
            else
            {
                // yy [mod] 2008-5-20 for bug ID 8477
                AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME,
                                   g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI, nSim);
                AT_TC(g_sw_GPRS, "unknown return value in exe       in AT_GPRS_CmdFunc_CGACT\n");
            }

            // [MOD] by yy  at 2008-04-21 end
            return;
        }
        else
        {
            g_uAtWaitActRsp[g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI]++;

            // AT_TC(g_sw_GPRS,  "exe       in AT_GPRS_CmdFunc_CGACT, success(cid):%d\n",uCid);
        }
        g_staAtGprsCidInfo[g_uGPRS_Act_Wait_Att_Rsp_Info.cid[i]].uState = g_uGPRS_Act_Wait_Att_Rsp_Info.act_state;
    }
}

VOID AT_GPRS_Att_Rsp(CFW_EVENT *pEvent)
{
    UINT8 uErrCode;
    UINT8 respond[80] = { 0 };    
    UINT8 pResp[16] = {0,};

    CFW_NW_STATUS_INFO sStatus;

    UINT8 nSim = pEvent->nFlag;
    AT_TC(g_sw_GPRS, "Recv:EV_CFW_GPRS_ATT_RSP,nParam1 =0x%x,nType =0x%x.pEvent->nUTI = %d\n", pEvent->nParam1,
          pEvent->nType, pEvent->nUTI);

    if (pEvent->nParam1 != ERR_SUCCESS)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pEvent->nUTI, nSim);
        return;
    }

    if ((pEvent->nType != CFW_GPRS_ATTACHED) && (pEvent->nType != CFW_GPRS_DETACHED))
    {
        AT_GPRS_Result_Err(ERR_CME_UNKNOWN, CMD_ERROR_CODE_TYPE_CME, pEvent->nUTI, nSim);
        return;
    }

    if ((g_uGPRS_Act_Wait_Att_Rsp_Info.wait_state == 1) && (g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI == pEvent->nUTI))
    {
        AT_GPRS_Act_Get_Att_Rsp(pEvent->nType);
        return;
    }

#ifndef AT_NO_GPRS
    uErrCode = CFW_GprsGetstatus(&sStatus, nSim);
#endif
    if (ERR_SUCCESS == uErrCode)
    {
        UINT8 uAttStat = 0;
        AT_Sprintf(pResp, "+CGATT:%d", pEvent->nType);
        if (CFW_GPRS_ATTACHED == pEvent->nType)
        {
            // csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, CSW_SYS_FREQ_78M);
            g_uAttState = CFW_GPRS_ATTACHED;

            if (1 == sStatus.nStatus) // home
            {
                uAttStat = AT_GPRS_ATT_HOME_OK;
            }
            else if (5 == sStatus.nStatus)  // roaming
            {
                uAttStat = AT_GPRS_ATT_ROAM_OK;
            }
        }
        else
        {
            g_uAttState = CFW_GPRS_DETACHED;
            uAttStat = AT_GPRS_ATT_NOTREG;  // detached
        }

        AT_MemSet(respond, 0, sizeof(respond));
#ifdef LTE_NBIOT_SUPPORT
        if(CFW_NWGetRat(nSim)==CFW_RAT_NBIOT_ONLY)
        {
            if(0==AT_Cereg_respond(respond, &sStatus, uAttStat,  FALSE))
            {
                AT_MemSet(respond, 0, sizeof(respond));
            }
        }
        else
#endif
        {
        switch (gATCurrentCgreg)
        {
        case 1:
            AT_Sprintf(respond, "+CGREG: %d", uAttStat);
            break;

        case 2:
            if (CFW_GPRS_ATTACHED == pEvent->nType)
            {
                AT_Sprintf(respond, "+CGREG: %d,\"%X%X\",\"%X%X\"",
                           uAttStat, sStatus.nAreaCode[0], sStatus.nAreaCode[1], sStatus.nCellId[0], sStatus.nCellId[1]);
            }
            else
            {
                AT_Sprintf(respond, "+CGREG: %d", uAttStat);
            }
            break;
        }
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pEvent->nUTI, nSim);
    }
    else
    {
        AT_TC(g_sw_GPRS, "Get GPRS status error");
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pEvent->nUTI, nSim);
        return;
    }
}

VOID AT_GPRS_CmdFunc_CGACT(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    INT32 iResult              = 0;
    UINT8 uState               = 0;
    UINT16 uSize                = 0;
    UINT8 uCidState            = 0;
    UINT8 uNumCidToBeAct       = 0;
    UINT8 uaCid[AT_PDPCID_MAX] = { 0 };
    #ifdef LTE_NBIOT_SUPPORT
    UINT8 uaPCid[AT_PDPCID_MAX] = {0,};
    #endif
    UINT8 uaUTI[AT_PDPCID_MAX] = { 0 };
    UINT8 gState               = 0;
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    UINT8 n                    = 0;
    UINT8 i                    = 0;
    UINT8 j                    = 0;
    UINT8 AttDetachType = 0;
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_TC(g_sw_GPRS, "+CGACT: nSim= %d; nDLCI= 0x%x", nSim, pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if(CLASS_TYPE_CC == AT_Get_ClassType())
    {
        //class cc mode,can not active
        AT_TC(g_sw_GPRS, "Gprs in class cc mode,can not active");
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
#ifndef AT_NO_GPRS
    // get gprs att state
    iResult = CFW_GetGprsAttState(&gState, nSim);
#endif
    if ((ERR_SUCCESS != iResult) || ((0 != gState) && (1 != gState)))
    {
        AT_TC(g_sw_GPRS, "CFW_GetGprsAttState() failed,iResult = 0x%x. ", iResult);
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        g_uAttState = gState;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if ((iResult != ERR_SUCCESS) || (uParamCount < 1))
        {
            // the param "<state>" become a must
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT, parameters error or no parameters offered \n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (g_uAtWaitActRsp[pParam->nDLCI] != 0)
        {
            AT_TC(g_sw_GPRS, "PDP activation process is waiting network response");
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (uParamCount > AT_PDPCID_MAX + 1 /* "1" refs to uState */ )
        {
            // uplimit of maxium PDP allowed to be activated
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT, exceed num of maxium allowed active PDP \n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uState, &uSize);
        if ((iResult != ERR_SUCCESS) || (uState > 1))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if ((AT_GPRS_ACT == uState) && (g_uAtGprsCidNum > AT_PDPCID_MAX))
        {
            AT_TC(g_sw_GPRS, "PDP activation exceed maximum limitation.");
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (uParamCount > 1)
        {
            // get&check the specified <cid>
            i = 0;
            j = 1;
            while (j < uParamCount)
            {
                uSize  = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, j, AT_UTIL_PARA_TYPE_UINT8, &uaCid[i], &uSize);
                if (iResult != ERR_SUCCESS)
                {
                    AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
#ifndef AT_NO_GPRS
                CFW_GetGprsActState(uaCid[i], &uCidState, nSim);
#endif
#ifdef LTE_NBIOT_SUPPORT
                uaPCid[i] = g_staAtGprsCidInfo[uaCid[i]].uPCid;
#endif
                if (uCidState != uState)
                {
                    // if the state of the cid  is not what we want,then save the cid, else ignore it.
                    i++;
                    uNumCidToBeAct++;
                }

                j++;
            }
        }
        else
        {
            // check all defined cids
            j = 0;
            for (i = 1; i <= AT_PDPCID_MAX; i++)
            {
                if ((g_staAtGprsCidInfo[i].uCid != 0) && (nSim == AT_SIM_ID(g_staAtGprsCidInfo[i].nDLCI)))
                {
#ifndef AT_NO_GPRS
                    CFW_GetGprsActState(g_staAtGprsCidInfo[i].uCid, &uCidState, nSim);
#endif
                    if (uCidState != uState)
                    {
                        // if the cid state is not what we want,then save the cid, else ignore it.
#ifdef LTE_NBIOT_SUPPORT
                        uaPCid[j] = g_staAtGprsCidInfo[g_staAtGprsCidInfo[i].uCid].uPCid;
#endif
                        uaCid[j++] = g_staAtGprsCidInfo[i].uCid;
                        uNumCidToBeAct++;
                    }
                }
            }
        }

        // if not attach, Attach GPRS.
#ifndef LTE_NBIOT_SUPPORT
        if ((g_uAttState == 0) && (uState == 1) && (uNumCidToBeAct != 0))
        {
            AT_TC(g_sw_GPRS, "CFW_GprsAtt state = 0. ");
            AttDetachType = API_PS_ATTACH_GPRS|API_PS_ATTACH_FOR;
#ifndef AT_NO_GPRS
            iResult = CFW_AttDetach(CFW_GPRS_ATTACHED, pParam->nDLCI,AttDetachType, nSim,AT_GPRS_AsyncEventProcess);// Modify for GCF test case 12.2.1.11 PS attach / accepted / follow-on request pending indicator set
#endif
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "CFW_GprsAtt failed,iResult = 0x%x. ", iResult);
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                g_uGPRS_Act_Wait_Att_Rsp_Info.wait_state = 1;
                g_uGPRS_Act_Wait_Att_Rsp_Info.act_state  = uState;
                g_uGPRS_Act_Wait_Att_Rsp_Info.act_count  = uNumCidToBeAct;
                g_uGPRS_Act_Wait_Att_Rsp_Info.nSim       = nSim;
                g_uGPRS_Act_Wait_Att_Rsp_Info.nDLCI      = pParam->nDLCI;
                for (i = 0; i < uNumCidToBeAct; i++)
                {
                    g_uGPRS_Act_Wait_Att_Rsp_Info.cid[i] = uaCid[i];
                }
            }
            AT_TC(g_sw_GPRS, "wait AT_GPRS_Act_Get_Att_Rsp");
            return;
        }
#endif
        AT_TC(g_sw_GPRS,
              "exe       in AT_GPRS_CmdFunc_CGACT,  %d need to execute:%d, %d, the state of the rest is already satisfied",
              uNumCidToBeAct, uaCid[0], uaCid[1]);

        // begin execute ...
        if ((uParamCount > 1) && (uNumCidToBeAct != 0))
        {
            // active/deactive specified <cid>
            for (i = 0; i < uNumCidToBeAct; i++)
            {
                // active/deactive PDP
                AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  uState:%d, uaCid[%d]:%d, uaUTI[%d]:%d", uState, i,
                      uaCid[i], i, uaUTI[i]);
#ifdef LTE_NBIOT_SUPPORT
                AT_TC(g_sw_GPRS, "uaPCid[%d]:%d", i, uaPCid[i]);
                if(uaPCid[i] > 0)
                {
                    iResult = AT_CFW_GprsSecAct(uState, uaCid[i], uaPCid[i], pParam->nDLCI, nSim);
                }
                else
#endif
                {
                iResult = AT_CFW_GprsAct(uState, uaCid[i], pParam->nDLCI, nSim,AT_GPRS_AsyncEventProcess);
                }
                AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  iResult=%d", iResult);

                // if(( iResult != CFW_GPRS_ACTIVED ) && (iResult != CFW_GPRS_DEACTIVED))
                if (iResult != ERR_SUCCESS)
                {
                    if (ERR_CFW_INVALID_PARAMETER == iResult)
                    {
                        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else if (ERR_CFW_NOT_EXIST_FREE_UTI == iResult)
                    {
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else if (ERR_CFW_UTI_IS_BUSY == iResult)
                    {
                        AT_TC(g_sw_GPRS, "UTI busy in exe       in AT_GPRS_CmdFunc_CGACT\n");
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_TC(g_sw_GPRS, "unknown return value in exe       in AT_GPRS_CmdFunc_CGACT\n");
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }

                    AT_TC(g_sw_GPRS,
                          "exe       in AT_GPRS_CmdFunc_CGACT, fail at(cid):%d, but still %d activation requestion is on the fly\n",
                          uaCid[i], g_uAtWaitActRsp[pParam->nDLCI]);
                    return;
                }
                else
                {
                    g_uAtWaitActRsp[pParam->nDLCI]++;
                }
                g_staAtGprsCidInfo[uaCid[i]].uState = uState;

                g_staAtGprsCidInfo[ uaCid[i]].uCid = uaCid[i];
            }

            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  %d cid is waiting for network response", g_uAtWaitActRsp[pParam->nDLCI]);
            AT_AddWaitingEvent(EV_CFW_GPRS_ACT_RSP, nSim, pParam->nDLCI);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, AT_GPRS_ACTIVEDELAY, NULL, 0, pParam->nDLCI, nSim);
            return;

        }
        else if ((uParamCount > 1) && (0 == uNumCidToBeAct))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT, the state of all specified cid is already satified");
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, AT_GPRS_ACTIVEDELAY, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else if ((1 == uParamCount) && (0 != uNumCidToBeAct))
        {
            // active/deactive all <cid>, this can be merged with the first branch
            for (i = 0; ((i < uNumCidToBeAct) && (i < AT_PDPCID_MAX)); i++)
            {
                // active/deactive PDP
                AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  uState:%d, uaCid[%d]:%d, uaUTI[%d]:%d",
                      uState, i, uaCid[i], i, uaUTI[i]);
#ifdef LTE_NBIOT_SUPPORT
                AT_TC(g_sw_GPRS, "uaPCid[%d]:%d", i, uaPCid[i]);
                if(uaPCid[i] > 0)
                {
                    iResult = AT_CFW_GprsSecAct(uState, uaCid[i], uaPCid[i], pParam->nDLCI, nSim);
                }
                else
#endif
                {
                iResult = AT_CFW_GprsAct(uState, uaCid[i], pParam->nDLCI, nSim,AT_GPRS_AsyncEventProcess);
                }
                if (iResult != ERR_SUCCESS)
                {
                    if (ERR_CFW_INVALID_PARAMETER == iResult)
                    {
                        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else if (ERR_CFW_NOT_EXIST_FREE_UTI == iResult)
                    {
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else if (ERR_CFW_UTI_IS_BUSY == iResult)
                    {
                        AT_TC(g_sw_GPRS, "UTI busy in exe       in AT_GPRS_CmdFunc_CGACT\n");
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_GPRS_Result_Err(ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        AT_TC(g_sw_GPRS, "unknown return value in exe       in AT_GPRS_CmdFunc_CGACT\n");
                    }

                    AT_TC(g_sw_GPRS,
                          "exe       in AT_GPRS_CmdFunc_CGACT, fail at(cid):%d, but still %d activation requestion is on the fly\n",
                          uaCid[i], g_uAtWaitActRsp[pParam->nDLCI]);
                    return;
                }
                else
                {
                    g_uAtWaitActRsp[pParam->nDLCI]++;
                }
                g_staAtGprsCidInfo[uaCid[i]].uState = uState;
            }

            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT,  %d cid is waiting for network response", g_uAtWaitActRsp[pParam->nDLCI]);
            AT_AddWaitingEvent(EV_CFW_GPRS_ACT_RSP, nSim, pParam->nDLCI);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, AT_GPRS_ACTIVEDELAY, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else if ((1 == uParamCount) && (0 == uNumCidToBeAct))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGACT, the state of all defined cid is already satified");
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, AT_GPRS_ACTIVEDELAY, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        n = 0;

        for (i = 1; i <= AT_PDPCID_MAX; i++)  // yy [mod] 2008-5-26 for bug ID 8588
        {
            AT_TC(g_sw_GPRS, "+CGACT: i= %d; nSim= %d; nDLCI= 0x%x", i, nSim, g_staAtGprsCidInfo[i].nDLCI);
            if ((g_staAtGprsCidInfo[i].uCid != 0) && (nSim == AT_SIM_ID(g_staAtGprsCidInfo[i].nDLCI)))
            {
#ifndef AT_NO_GPRS
                iResult = CFW_GetGprsActState(g_staAtGprsCidInfo[i].uCid, &uState, nSim);
#endif
                if (iResult != ERR_SUCCESS)
                {
                    continue;
                }
                else
                {
                    if (CFW_GPRS_ACTIVED == uState )
                    {
                        g_staAtGprsCidInfo[i].uState = uState;
                        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
                        AT_Sprintf(AtRet, "+CGACT: %d, %d", g_staAtGprsCidInfo[i].uCid, g_staAtGprsCidInfo[i].uState);
                        at_CmdRespInfoText(pParam->engine, AtRet);
                        n++;
                    }
                }
            }
            else
            {
                AT_TC(g_sw_GPRS, "+CGACT: g_staAtGprsCidInfo[%d].uCid == 0", i);
            }
        }

        if (0 == n)
        {
            AT_MemSet(AtRet, 0, SIZEOF(AtRet));
            AT_Sprintf(AtRet, "+CGACT: 0,0");
            at_CmdRespInfoText(pParam->engine, AtRet);
        }
        at_CmdRespOK(pParam->engine);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CGACT: (0,1)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}

#ifdef GCF_TEST
struct CGSEND_Status
{
    UINT8 nDLCI;
    UINT8 nCid;
    UINT32 nDataLen;
    CFW_GPRS_DATA *pGprsData;
};

static struct CGSEND_Status s_CGSEND_Status = {0, 0, 0, NULL};

UINT8 AT_GPRS_Get_CGSEND_Channel()
{
    AT_TC(g_sw_GPRS, "AT_GPRS_Get_CGSEND_Channel = %d", s_CGSEND_Status.nDLCI);
    return s_CGSEND_Status.nDLCI;
}

VOID AT_GPRS_CGSEND_Stop()
{
    if (s_CGSEND_Status.pGprsData != NULL)
        CSW_TCPIP_FREE(s_CGSEND_Status.pGprsData);

    s_CGSEND_Status.nDLCI = 0;
    s_CGSEND_Status.pGprsData = NULL;
    s_CGSEND_Status.nDataLen = 0;
    AT_KillCgsendTimer();
}

VOID AT_GPRS_CGSEND_TimerHandler(UINT8 nSim)
{
    UINT32 uFixedSendSize = 2000;
    UINT8 nWaitCount = 0;

    AT_TC(g_sw_GPRS, "AT_GPRS_CGSEND_TimerHandler");
    if (s_CGSEND_Status.pGprsData == NULL)
    {
        s_CGSEND_Status.pGprsData = CSW_TCPIP_MALLOC(uFixedSendSize + 4);
        SUL_ZeroMemory8(s_CGSEND_Status.pGprsData, uFixedSendSize);
    }

    if (s_CGSEND_Status.nDataLen > 0)
    {
        while(!CFW_GprsSendDataAvaliable(nSim))
        {
            /* check GPRS send ready for 7.5s. */
            if (nWaitCount > 5)
            {
			AT_TC(g_sw_GPRS, "ERROR, GPRS send data is not avaliable");
			AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME,  s_CGSEND_Status.nDLCI, nSim);
			return;
		}
            nWaitCount++;
            COS_Sleep(1500);
        }

        if (s_CGSEND_Status.nDataLen > uFixedSendSize)
            s_CGSEND_Status.pGprsData->nDataLength = uFixedSendSize;
        else
            s_CGSEND_Status.pGprsData->nDataLength = s_CGSEND_Status.nDataLen;

        s_CGSEND_Status.nDataLen -= s_CGSEND_Status.pGprsData->nDataLength;
#ifdef LTE_NBIOT_SUPPORT
		if (ERR_SUCCESS != CFW_GprsSendData(s_CGSEND_Status.nCid, s_CGSEND_Status.pGprsData,0,0, nSim))
#else
        if (ERR_SUCCESS != CFW_GprsSendData(s_CGSEND_Status.nCid, s_CGSEND_Status.pGprsData, nSim))
#endif
        {
            AT_TC(g_sw_GPRS, "------CFW_GprsSendData error");
            return;
        }

        AT_TC(g_sw_GPRS, "AT_GPRS_CGSEND_TimerHandler left len=%d", s_CGSEND_Status.nDataLen);
        if (s_CGSEND_Status.nDataLen == 0)
            AT_GPRS_CGSEND_Stop();
        else
            AT_SetCgsendTimer(1500);
    }
}

BOOL AT_KillCgsendTimer(VOID)
{
    AT_TC(g_sw_UART, "AT_KillCgsendTimer");
    COS_StopCallbackTimer(CSW_AT_TASK_HANDLE, AT_GPRS_CGSEND_TimerHandler, 0);
    return TRUE;
}

BOOL AT_SetCgsendTimer(UINT32 nElapse)
{
    AT_TC(g_sw_UART, "AT_SetCgsendTimer");
    COS_StartCallbackTimer(CSW_AT_TASK_HANDLE, nElapse * HAL_TICK1S, AT_GPRS_CGSEND_TimerHandler, 0);
    return TRUE;
}

#endif

VOID AT_GPRS_CmdFunc_CGSEND(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef GCF_TEST
    UINT16 uSize = 1;
    UINT8 uCid;
    INT32 iResult = 0;
    UINT32 uDataLen;

    iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
    if (ERR_SUCCESS != iResult)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    uSize = 4;
    iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT32, &uDataLen, &uSize);
    if (ERR_SUCCESS != iResult )
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    s_CGSEND_Status.nDLCI = pParam->nDLCI;
    s_CGSEND_Status.nCid = uCid;
    s_CGSEND_Status.nDataLen = uDataLen;
    AT_SetCgsendTimer(1500);
#endif

    /*
    UINT32 uRet;
    CFW_GPRS_DATA *pGprsData = NULL;
    UINT32 uFixedSendSize = 2000;

    pGprsData = CSW_TCPIP_MALLOC(uFixedSendSize+ 4);
    SUL_ZeroMemory8(pGprsData->pData, uFixedSendSize);

    #ifdef __USE_SMD__
    UINT32 offSet= 0;
    IP_DATA_BUFFER_T* pdata;
    if(0==smd_alloc_ul_ipdata_buf(&offSet, 1))
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return ;
    }
    pdata =( IP_DATA_BUFFER_T* )(MEMORY_BASE_ADDR + offSet);
    memcpy(pdata->ipData, pGprsData->pData,uDataLen);
    pdata->hdr.nextOffset = 0;
    pdata->hdr.cid =  uCid;
    pdata->hdr.length = uDataLen;
    ppp_SendPsInfo (offSet,uCid,nSim);

    if(NULL != pGprsData)
    {
        CSW_TCPIP_FREE(pGprsData);
    }
    */
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;
}

// ----------------------------------------------------------------------------------------------
// Set command                               |   Test command                          |
// AT+CGPADDR=[<cid>,[                     |     AT+ CGPADDR =?                      |
// <cid>, [                     |                                         |
// <cid>,                       |                                         |
// [,...]]]]                    |                                         |
// Response(s)                               |   Response(s)                           |
// Success:                              |     Success:                            |
// +CGPADDR: <cid>,<PDP_addr>[<CR><LF>   |       +CGPADDR: (list of defined <cid>s)|
// +CGPADDR: <cid>,<PDP_addr>[...]]      |       OK                                |
// OK                                    |                                         |
// Fail:                                 |                                         |
// ERROR                             |     Fail:                               |
// |       ERROR                             |
// ----------------------------------------------------------------------------------------------
// [[ yy [mod] 2008-4-28 for no "\r\n" in string, which is send to the serial port.
// [MOD] by yy  at 2008-04-21 begin
VOID AT_GPRS_CmdFunc_CGPADDR(AT_CMD_PARA *pParam)
{
    INT32 iResult;

    UINT8 uParamCount;
    UINT8 uCid;
    UINT16 uSize;
    UINT8 uIndex = 0;
    UINT8 i      = 0;

    UINT8 uCnt;
    UINT8 uT;

    UINT8 n     = 0;
    UINT8 uFlag = 0;
#ifdef LTE_NBIOT_SUPPORT
    const ip6_addr_t *ip6_addr;
    UINT8 len;
    UINT8 ip4addr[4];
#endif
    CFW_GPRS_PDPCONT_INFO stPdpCtxInfo;
    UINT8 pApn[100]    = { 0 };
    UINT8 pPdpAddr[18] = { 0 };
    UINT8 pApnUser[20] = { 0 };
    UINT8 pApnPwd[20]  = { 0 };

    stPdpCtxInfo.pApn     = pApn;
    stPdpCtxInfo.pPdpAddr = pPdpAddr;
    stPdpCtxInfo.pApnUser = pApnUser;
    stPdpCtxInfo.pApnPwd  = pApnPwd;
#ifdef LTE_NBIOT_SUPPORT
    UINT8 uaTempStr[9] = { 0 };

#define OUTSTR_LEN 100 // yy [mod] 2008-4-29
#else
    UINT8 uaTempStr[3] = { 0 };

#define OUTSTR_LEN 25 // yy [mod] 2008-4-29
#endif
    UINT8 uaOutputStr[OUTSTR_LEN] = { 0 };


    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    // input parameter check.

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:

        // check whether the parameter is the valid parameter...
        // TODO...
        // [[ yy [del] 2008-5-21 for bug ID 8467
        // ]] yy [del] 2008-5-21 for bug ID 8467

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        AT_TC(g_sw_GPRS, "+CGPADDR:uParamCount %d", uParamCount);

        if (uParamCount > AT_PDPCID_MAX)
        {
            // do something...
            // whether we can trim the redunced cid in tail..?
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // will return all the PDP ctx defined,now csw has defined 7 items[1-7].
        if (uParamCount == 0)
        {
            uIndex = 0;
            AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));

            for (i = AT_PDPCID_MIN; i <= AT_PDPCID_MAX; i++)  // yy [del] 2008-5-26 for code review
            {

                // Do not clear the uState defination...?
                if (0 == g_staAtGprsCidInfo[i].uCid)
                {
                    continue;
                }
                else
                {
                    n++;
                }

                AT_MemSet(&stPdpCtxInfo, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));

                AT_MemSet(pApn, 0, 100);
                AT_MemSet(pPdpAddr, 0, 18);
                AT_MemSet(pApnUser, 0, 20);
                AT_MemSet(pApnPwd, 0, 20);
                stPdpCtxInfo.pApn     = pApn;
                stPdpCtxInfo.pPdpAddr = pPdpAddr;
                stPdpCtxInfo.pApnUser = pApnUser;
                stPdpCtxInfo.pApnPwd  = pApnPwd;
#ifndef AT_NO_GPRS
                iResult = CFW_GprsGetPdpCxt(g_staAtGprsCidInfo[i].uCid, &stPdpCtxInfo, nSim);
#endif
                if (ERR_SUCCESS != iResult)
                {
                    AT_TC(g_sw_GPRS, "Query PDP context err");
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else
                {

                    if (0 == stPdpCtxInfo.nPdpAddrSize)
                    {
                        // yy [mod] for bug ID: 8456
                        AT_Sprintf(uaOutputStr, "+CGPADDR: %d,\"0.0.0.0\"", g_staAtGprsCidInfo[i].uCid);
                        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                          nSim);
                    }
                    else
                    {
                        uT = 0;
                            AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));
                        // yy [mod] for bug ID: 8456
                        AT_Sprintf(uaOutputStr, "+CGPADDR: %d,\"", g_staAtGprsCidInfo[i].uCid);
#ifdef LTE_NBIOT_SUPPORT
                            if(stPdpCtxInfo.PdnType == 1)  //ipv4
                            {
#endif
		                        for (uCnt = 0; uCnt < stPdpCtxInfo.nPdpAddrSize; uCnt++)
		                        {
		                            AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
		                            AT_Sprintf(uaTempStr, "%d.",
		                                       (stPdpCtxInfo.pPdpAddr[uCnt] >> 4) * 16 + ((stPdpCtxInfo.pPdpAddr[uCnt]) & 0x0F));
		                            AT_StrCat(uaOutputStr, uaTempStr);
		                        }
#ifdef LTE_NBIOT_SUPPORT
                            }
					       else if(stPdpCtxInfo.PdnType == 2)  //ipv6
					       {
					            ip6_addr = CFW_GprsGetPdpIpv6Address(uCid, nSim);
							    if(ip6_addr != NULL)
							    {
							        for (uCnt = 0; uCnt < 4; uCnt++)
			                                    {
			                                        AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                        AT_Sprintf(uaTempStr, "%02x.%02x.%02x.%02x.",(UINT8)(htonl(ip6_addr->addr[uCnt]) >> 24), (UINT8)((htonl(ip6_addr->addr[uCnt]) >> 16)&0xff),
													(UINT8)((htonl(ip6_addr->addr[uCnt]) >> 8)&0xff), (UINT8)((htonl(ip6_addr->addr[uCnt]))&0xff));
			                                        AT_StrCat(uaOutputStr, uaTempStr);
			                                    }
							    }
					       }
							else if(stPdpCtxInfo.PdnType == 3)  //ipv4v6
							{
							    CFW_GprsGetPdpIpv4Addr(uCid, &len, ip4addr, nSim);
							    for (uCnt = 0; uCnt < 4; uCnt++)
			                                {
			                                    AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                    AT_Sprintf(uaTempStr, "%d.",ip4addr[uCnt]);
			                                    AT_StrCat(uaOutputStr, uaTempStr);
			                                }
							    AT_MemCpy(&uaOutputStr[AT_StrLen(uaOutputStr) - 1], "\",\"", 3);
							    ip6_addr = CFW_GprsGetPdpIpv6Address(uCid,nSim);
							    if(ip6_addr != NULL)
							    {
							        for (uCnt = 0; uCnt < 4; uCnt++)
			                                    {
			                                        AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                        AT_Sprintf(uaTempStr, "%02x.%02x.%02x.%02x.",(UINT8)(ntohl(ip6_addr->addr[uCnt]) >> 24), (UINT8)((ntohl(ip6_addr->addr[uCnt]) >> 16)&0xff),
													(UINT8)((ntohl(ip6_addr->addr[uCnt]) >> 8)&0xff), (UINT8)((ntohl(ip6_addr->addr[uCnt]))&0xff));
			                                        AT_StrCat(uaOutputStr, uaTempStr);
			                                    }
							    }
							}
							else
							{
							    AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
			                                return;
							}
#endif
                        AT_MemCpy(&uaOutputStr[AT_StrLen(uaOutputStr) - 1], "\"", 1);
                        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                          nSim);
                    }
                }
            }

            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            // disp specified cids
            AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));

            for (i = 0; i < uParamCount; i++)
            {
                uSize   = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
                AT_TC(g_sw_GPRS, "CFW_GprsGetPdpCxt uCid %d",uCid);
                if(uCid > AT_PDPCID_MAX)
                {
                    //for bug ID 3534
                    AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if (ERR_SUCCESS != iResult)
                {
                    AT_TC(g_sw_GPRS, "CFW_GprsGetPdpCxt iResult %d",iResult);
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                AT_TC(g_sw_GPRS, "CFW_GprsGetPdpCxt g_staAtGprsCidInfo[uCid].uCid %d",g_staAtGprsCidInfo[uCid].uCid);

                if (g_staAtGprsCidInfo[uCid].uCid == 0)
                {

                    // [[ yy [mod] 2008-6-4 for bug ID 8680
                    continue;

                    // ]] yy [mod] 2008-6-4 for bug ID 8680
                }

                AT_MemSet(&stPdpCtxInfo, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));
                AT_MemSet(pApn, 0, 100);
                AT_MemSet(pPdpAddr, 0, 18);
                AT_MemSet(pApnUser, 0, 20);
                AT_MemSet(pApnPwd, 0, 20);
                stPdpCtxInfo.pApn     = pApn;
                stPdpCtxInfo.pPdpAddr = pPdpAddr;
                stPdpCtxInfo.pApnUser = pApnUser;
                stPdpCtxInfo.pApnPwd  = pApnPwd;
#ifndef AT_NO_GPRS
                iResult = CFW_GprsGetPdpCxt(uCid, &stPdpCtxInfo, nSim);
#endif
                AT_TC(g_sw_GPRS, "CFW_GprsGetPdpCxt iResult %d",iResult);

                if (ERR_SUCCESS == iResult)
                {
                    if (0 == stPdpCtxInfo.nPdpAddrSize)
                    {
                        AT_Sprintf(uaOutputStr, "+CGPADDR: %d,\"0.0.0.0\"", uCid);  // yy [mod] for bug ID: 8456
                        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                          nSim);
                    }
                    else
                    {
                        uT = 0;
                        AT_MemSet(uaOutputStr, 0, sizeof(uaOutputStr));
                        AT_Sprintf(uaOutputStr, "+CGPADDR: %d,\"", uCid); // yy [mod] for bug ID: 8456
#ifdef LTE_NBIOT_SUPPORT
                            if(stPdpCtxInfo.PdnType == 1)  //ipv4
                            {
#endif
		                        for (uCnt = 0; uCnt < stPdpCtxInfo.nPdpAddrSize; uCnt++)
		                        {
		                            AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
		                            AT_Sprintf(uaTempStr, "%d.",
		                                       (stPdpCtxInfo.pPdpAddr[uCnt] >> 4) * 16 + ((stPdpCtxInfo.pPdpAddr[uCnt]) & 0x0F));
		                            AT_StrCat(uaOutputStr, uaTempStr);
		                        }
#ifdef LTE_NBIOT_SUPPORT
                            }
					       else if(stPdpCtxInfo.PdnType == 2)  //ipv6
					       {
					           ip6_addr = CFW_GprsGetPdpIpv6Address(uCid,nSim);
							    if(ip6_addr != NULL)
							    {
							        for (uCnt = 0; uCnt < 4; uCnt++)
			                                    {
			                                        AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                        AT_Sprintf(uaTempStr, "%02x.%02x.%02x.%02x.",(UINT8)(htonl(ip6_addr->addr[uCnt]) >> 24), (UINT8)((htonl(ip6_addr->addr[uCnt]) >> 16)&0xff),
													(UINT8)((htonl(ip6_addr->addr[uCnt]) >> 8)&0xff), (UINT8)((htonl(ip6_addr->addr[uCnt]))&0xff));
			                                        AT_StrCat(uaOutputStr, uaTempStr);
			                                    }
							    }
					       }
							else if(stPdpCtxInfo.PdnType == 3)  //ipv4v6
							{
							    CFW_GprsGetPdpIpv4Addr(uCid, &len, ip4addr, nSim);
							    for (uCnt = 0; uCnt < 4; uCnt++)
			                                {
			                                    AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                    AT_Sprintf(uaTempStr, "%d.",ip4addr[uCnt]);
			                                    AT_StrCat(uaOutputStr, uaTempStr);
			                                }
							    AT_MemCpy(&uaOutputStr[AT_StrLen(uaOutputStr) - 1], "\",\"", 3);
							    ip6_addr = CFW_GprsGetPdpIpv6Address(uCid,nSim);
							    if(ip6_addr != NULL)
							    {
							        for (uCnt = 0; uCnt < 4; uCnt++)
			                                    {
			                                        AT_MemSet(uaTempStr, 0, sizeof(uaTempStr));
			                                        AT_Sprintf(uaTempStr, "%02x.%02x.%02x.%02x.",(UINT8)(ntohl(ip6_addr->addr[uCnt]) >> 24), (UINT8)((ntohl(ip6_addr->addr[uCnt]) >> 16)&0xff),
													(UINT8)((ntohl(ip6_addr->addr[uCnt]) >> 8)&0xff), (UINT8)((ntohl(ip6_addr->addr[uCnt]))&0xff));
			                                        AT_StrCat(uaOutputStr, uaTempStr);
			                                    }
							    }
							}
							else
							{
							    AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
			                                return;
							}
#endif
                        AT_MemCpy(&uaOutputStr[AT_StrLen(uaOutputStr) - 1], "\"", 1);
                        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                          nSim);
                    }
                }
                else if(ERR_NOT_SUPPORT == iResult)
                {
                    
                    AT_Sprintf(uaOutputStr, "+CGPADDR: %d,\"0.0.0.0\"", uCid);  // for bug ID: 3532
                    AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, uaOutputStr, AT_StrLen(uaOutputStr), pParam->nDLCI,
                                      nSim);
                }
                else
                {
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

            }

            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        break;

    case AT_CMD_TEST:
        // [[ yy [mod] 2008-04-23 for bug ID:8183
    {
        UINT16 uLen = SIZEOF("+CGPADDR:(");

        AT_MemSet(uaOutputStr, 0, OUTSTR_LEN);

        AT_MemCpy(uaOutputStr, "+CGPADDR:(", uLen);
        uLen -= 1;

        for (uIndex = AT_PDPCID_MIN; uIndex <= AT_PDPCID_MAX; uIndex++)
        {
            if (g_staAtGprsCidInfo[uIndex].uCid)
            {
                AT_Sprintf(&uaOutputStr[uLen], "%d,", g_staAtGprsCidInfo[uIndex].uCid);
                uLen += 2;
                uFlag = 1;
            }
        }

        if (uFlag)
        {
            uaOutputStr[uLen - 1] = ')';
            uaOutputStr[uLen] = '\0';
        }
        else
        {
            AT_MemCpy(uaOutputStr, "+CGPADDR:()", sizeof("+CGPADDR:()"));
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)uaOutputStr, uLen + 1, pParam->nDLCI, nSim);
        break;
    }

    // ]] yy [mod] 2008-04-23 for bug ID:8183

    default:
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;

}

// ]] yy [mod] 2008-4-28 for no "\r\n" in string, which is send to the serial port.
// [MOD] by yy  at 2008-04-21 end

// [[ yy [mod] 2008-4-28 for ATE test
#if defined(TEST_STUB_1) || defined(TEST_STUB_2)

VOID AT_GPRS_CmdFunc_CGDATA(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    // Force to enter PS mode.
    AT_CmdModeSwitch(AT_CMD_MODE_SWITCH_CGDATA, pParam->nDLCI);
}
#else
// ]] yy [mod] 2008-4-28 for ATE test
VOID AT_GPRS_CmdFunc_CGDATA(AT_CMD_PARA *pParam)
{
    UINT8 uCidState            = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount > 2)
        {
            // uplimit of maxium PDP allowed to be activated
            AT_TC(g_sw_GPRS, "AT_GPRS_CmdFunc_CGDATA, paramCount=%d \n",pParam->paramCount);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        
        bool paramok = true;
        const uint8_t *mode = at_ParamStr(pParam->params[0], &paramok);
        const uint8_t cid = at_ParamUintInRange(pParam->params[1], 0, AT_PDPCID_MAX, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (AT_StrCmp(mode, "PPP") != 0)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT));

        CFW_GetGprsActState(cid, &uCidState, nSim);
        if (uCidState != CFW_GPRS_ACTIVED)
        {
            AT_TC(g_sw_GPRS, "request <cid> not actived cid =%d",cid);
        }        
#if defined(CFW_PPP_SUPPORT)
        AT_PppProcess(pParam->nDLCI,nSim,cid);
#endif
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CGDATA: (\"PPP\")");
        at_CmdRespOK(pParam->engine);
        return;
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#endif
// ------------------------------------------------------------------------------------------
// Set command         |   Read command          |   Test command                          |
// AT+CGAUTO=[<n>]   |     AT+ CGAUTO?         |     AT+ CGAUTO =?                       |
// Response(s)         |   Response(s)           |   Response(s)                           |
// Success:        |     Success:            |     Success:                            |
// OK          |       +CGAUTO: <n>      |       +CGAUTO: (list of supported <n>s) |
// Fail:           |       OK                |       OK                                |
// ERROR       |     Fail:               |     Fail:                               |
// |       ERROR             |       ERROR                             |
// ------------------------------------------------------------------------------------------
VOID AT_GPRS_CmdFunc_CGAUTO(AT_CMD_PARA *pParam)
{
    UINT32 iResult;

    UINT8 uParamCount;
    UINT8 uStatus;
    UINT16 uSize;
    UINT8 attState;

    UINT8 pOutPutBuff[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // input parameter check.

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if (uParamCount > 1)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (FALSE == AT_Util_CheckValidateU8(pParam->pPara))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uStatus, &uSize);

        if (ERR_SUCCESS != iResult)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // [[ yy [mod] 2008-4-23 for Bug ID:8186
        if (uStatus > 3)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // ]] yy [mod ] 2008-4-23 for Bug ID:8186

        // Set the status of the auto response..
        gATCurrentu8nURCAuto = uStatus;

        if (uStatus == AUTO_RSP_STU_ON_PDONLY)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GetGprsAttState(&attState, nSim);
#endif
            if (ERR_SUCCESS != iResult)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (attState == CFW_GPRS_DETACHED)
            {
#ifndef AT_NO_GPRS
                iResult = CFW_GprsAttEX(CFW_GPRS_ATTACHED, pParam->nDLCI, nSim,AT_GPRS_AsyncEventProcess);
#endif
                if (ERR_SUCCESS != iResult)
                {
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
        }

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        // To do...
        // May be famat the string output...
        SUL_StrPrint(pOutPutBuff, "+CGAUTO: %d \0", gATCurrentu8nURCAuto);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutPutBuff, AT_StrLen(pOutPutBuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        SUL_StrPrint(pOutPutBuff, "+CGAUTO: (0-3)\0");

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutPutBuff, AT_StrLen(pOutPutBuff), pParam->nDLCI, nSim);
        break;

    default:
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;

}

VOID AT_GPRS_CmdFunc_CGANS(AT_CMD_PARA *pParam)
{
    INT8 iResult      = 0;
    UINT8 uParamCount = 0;
    UINT8 uResponse   = 0;
    UINT8 uaL2P[10]   = { 0 };
    UINT8 uCidState   = 0;
    UINT8 uCid        = 0;
    UINT8 uUTI        = 0;
    UINT16 uParamLen   = 0;
    UINT8 AtRet[100] = { 0 }; /* max 20 charactors per cid */
    CFW_GPRS_PDPCONT_INFO PdpCont_Info = {0,}; // yy [mod] 2008-6-27 for bug ID 8887

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // pPdpCont_Info = (CFW_GPRS_PDPCONT_INFO *)pParam->pPara;

        AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS\n");

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if ((iResult != ERR_SUCCESS) || (0 == uParamCount) || (uParamCount > 3))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, parameters error or no parameters offered \n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // <response>
        uParamLen = 1;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uResponse, &uParamLen);

        if ((iResult != ERR_SUCCESS) || (uResponse > 1))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // <L2P>  ??? not supported, do nothing.
        if (uParamCount > 1)
        {
            uParamLen = sizeof(uaL2P);
            iResult   = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, uaL2P, &uParamLen);

            if (iResult != ERR_SUCCESS)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        // <cid>
        if (uParamCount > 2)
        {
            uParamLen = 1;
            iResult   = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uParamLen);

            if ((iResult != ERR_SUCCESS) || (uCid > AT_PDPCID_MAX))
            {
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        // [[ yy [add] 2008-6-27 for bug ID 8887
        // PDP type check
        if (!AT_MemCmp(uaL2P, "PPP", 3))
        {
            PdpCont_Info.nPdpType = CFW_GPRS_PDP_TYPE_PPP;  // yy [mod] 2008-6-27 for bug ID 8887
        }
        else if (!AT_MemCmp(uaL2P, "IP", 2))
        {
            PdpCont_Info.nPdpType = CFW_GPRS_PDP_TYPE_IP; // yy [mod] 2008-6-27 for bug ID 8887
        }
        else if (!AT_MemCmp(uaL2P, "IPV6", 4))
        {
            PdpCont_Info.nPdpType = CFW_GPRS_PDP_TYPE_IPV6; // yy [mod] 2008-6-27 for bug ID 8887
        }
        else
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, PDP type error\n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // ]] yy [add] 2008-6-27 for bug ID 8887
#ifndef AT_NO_GPRS

        iResult = CFW_GetGprsActState(uCid, &uCidState, nSim);
#endif
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, get PDP state error.");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (CFW_GPRS_ACTIVED == uCidState)
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, the speicified PDP is alread activated");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (PdpCont_Info.nPdpType != g_staAtGprsCidInfo[uCid].nPdpType) // yy [mod] 2008-6-27 for bug ID 8887
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, PDP type not match: %d, %d\n", PdpCont_Info.nPdpType, g_staAtGprsCidInfo[uCid].nPdpType); // yy [mod] 2008-6-27 for bug ID 8887
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // PDP address check
        if (g_staAtGprsCidInfo[uCid].nPdpAddrSize != 0)
        {
            // yy [mod] 2008-6-27 for bug ID 8887
            if (AT_MemCmp(PdpCont_Info.pPdpAddr, g_staAtGprsCidInfo[uCid].pPdpAddr, g_staAtGprsCidInfo[uCid].nPdpAddrSize))
            {
                AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGANS, PDP address not match\n");
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        iResult = AT_GetFreeUTI(CFW_GPRS_SRV_ID, &uUTI);

        if (iResult != ERR_SUCCESS)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // execute ...
        if (0 == uResponse)
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsManualRej(uUTI, uCid, nSim);
#endif
            if (iResult != ERR_SUCCESS)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
#ifndef AT_NO_GPRS
            iResult = CFW_GprsManualAcc(uUTI, uCid, nSim);
#endif
            if (iResult != ERR_SUCCESS)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(&AtRet, 0, SIZEOF(AtRet));

        // yy [mod] 2008-5-20 for bug ID 8468
        AT_MemCpy(AtRet, "+CGANS: (0,1) (\"PPP\")", AT_StrLen("+CGANS: (0,1) (\"PPP\")"));
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

        return;
    }

}

UINT8 g_uClassType = 0;
UINT8 AT_Get_ClassType()
{
    return g_uClassType;
}
VOID AT_Set_ClassType(UINT8 Class)
{
    g_uClassType = Class;
}
VOID AT_GPRS_CmdFunc_CGCLASS(AT_CMD_PARA *pParam)
{
    UINT8 iResult     = 0;
    UINT8 uParamCount = 0;
    UINT16 uSize = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 arrCharacterSet[5] = { 0 };
    UINT8 AtRet[50]   = { 0X00, };



    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGEREP, parameters error or no parameters offered \n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        uSize   = SIZEOF(arrCharacterSet);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &arrCharacterSet, &uSize);


        if (iResult != ERR_SUCCESS)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (AT_StrCmp(arrCharacterSet, "A") == 0)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else if (AT_StrCmp(arrCharacterSet, "B") == 0)
        {
            AT_Set_ClassType(CLASS_TYPE_B);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else if (AT_StrCmp(arrCharacterSet, "CG") == 0)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else if (AT_StrCmp(arrCharacterSet, "CC") == 0)
        {

            AT_Set_ClassType(CLASS_TYPE_CC);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }


    }

    else if (AT_CMD_READ == pParam->iType)
    {
        if(CLASS_TYPE_B == AT_Get_ClassType())
        {
            AT_Sprintf(AtRet, "+CGCLASS: B");
        }
        else if(CLASS_TYPE_CC == AT_Get_ClassType())
        {
            AT_Sprintf(AtRet, "+CGCLASS: CC");
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_Sprintf(AtRet, "+CGCLASS: B,CC,CG(NO SUPPORT),A(NO SUPPORT)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;

}

VOID AT_GPRS_CmdFunc_CGEREP(AT_CMD_PARA *pParam)
{
    UINT8 iResult     = 0;
    UINT8 uParamCount = 0;
    UINT16 uSize       = 0;
    UINT8 uState      = 0;
    UINT8 AtRet[20]   = { 0X00, };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CGEREP, parameters error or no parameters offered \n");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get <state>
        uSize = 1;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uState, &uSize);

        if ((iResult != ERR_SUCCESS) || ((uState != 1) && (uState != 0)))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (uState == 1)
        {
            g_uATGprsCGEVFlag = 1;
        }
        else
        {
            g_uATGprsCGEVFlag = 0;
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        if (g_uATGprsCGEVFlag == 1)
        {
            AT_Sprintf(AtRet, "+CGEREP:1");
        }
        else if (g_uATGprsCGEVFlag == 0)
        {
            AT_Sprintf(AtRet, "+CGEREP:0");
        }
        else
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_Sprintf(AtRet, "+CGEREP: (0,1)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;

    // #endif
}

// This function to process the AT+CGREG command. include set,read and test command.
//
// Set command
// +CGREG= [<n>]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGREG?
// Possible response(s)
// +CGREG: <n>,<stat>[,<lac>,<ci>]
// +CME ERROR: <err>
// Test command
// +CGREG=?
// Possible response(s)
// +CGREG: (list of supported <n>s)
//

VOID AT_GPRS_CmdFunc_CGREG(AT_CMD_PARA *pParam)
{
    UINT8 respond[80] = {0};
    INT32 iResult;
    UINT8 n = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_NW_STATUS_INFO sStatus;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGREG:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_TC(g_sw_GPRS, "AT+CGREG:NULL == pParam->pPara");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS || uParamCount != 1)
        {
            AT_TC(g_sw_GPRS, "AT+CGREG:Get Param count failed,iResult = 0x%x", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        uSize = SIZEOF(n);
        uIndex = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &n, &uSize);

        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGREG:Get n failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        if (n > 2)
        {
            AT_TC(g_sw_GPRS, "AT+CGREG:The Prama n is error,n = 0x%x.", n);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        gATCurrentCgreg = n;
        goto _func_success;
    }
    else if (AT_CMD_READ == pParam->iType)  // Read command
    {
        UINT8  g_uAttState_t = 0;
        CFW_GetGprsAttState(&g_uAttState_t, nSim);

        if(CFW_GPRS_ATTACHED == g_uAttState_t )
        {

            uErrCode = CFW_GprsGetstatus(&sStatus, nSim);
        }
        else
        {
            uErrCode = CFW_NwGetStatus(&sStatus, nSim);
            sStatus.PsType = 0;
        }

        if (uErrCode != ERR_SUCCESS)
        {
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        AT_TC(g_sw_GPRS, "nCellId[0]:%d, nCellId[1]:%d", sStatus.nCellId[0], sStatus.nCellId[1]);

        if ((0x00 != sStatus.nStatus) && (0x01 != sStatus.nStatus) && (0x05 != sStatus.nStatus))
        {
            sStatus.nStatus = 0;
        }
        if (gATCurrentCgreg == 2)
        {
            AT_Sprintf(respond, "+CGREG: %d,%d,\"%X%X\",\"%X%X\",%d",
                       gATCurrentCgreg,
                       sStatus.nStatus,
                       sStatus.nAreaCode[0], sStatus.nAreaCode[1], sStatus.nCellId[0], sStatus.nCellId[1], sStatus.PsType);
        }
        else
        {
            AT_Sprintf(respond, "+CGREG: %d,%d", gATCurrentCgreg, sStatus.nStatus);
        }
        goto _func_success;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_StrCpy(respond, "+CGREG: (0,1,2)");
        goto _func_success;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }
_func_success:

    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, respond, AT_StrLen(respond), pParam->nDLCI, nSim);
    return;

_func_fail:

    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;

}

// This function to process the AT+CGSMS command. include set,read and test command.
//
// Set command
// +CGSMS= [<service>]
// Possible response(s)
// OK
// ERROR
//
// Read command
// +CGSMS?
// Possible response(s)
// +CGSMS: <service>
// +CME ERROR: <err>
// Test command
// +CGSMS=?
// Possible response(s)
// +CGSMS: (list of currently available <service>s)
//
VOID AT_GPRS_CmdFunc_CGSMS(AT_CMD_PARA *pParam)
{
    UINT8 AtRet[20] = {0,};
    INT32 iResult;
    UINT8 uService = 1;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS || uParamCount > 1)
        {
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        else if (1 == uParamCount)
        {
            uSize   = SIZEOF(uService);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uService, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        iResult = CFW_GprsSetSmsSeviceMode(uService);
        if (ERR_SUCCESS == iResult)
        {
            goto _func_success;
        }
        else
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
    }
    else if (AT_CMD_READ == pParam->iType)  // Read command
    {
        uService = CFW_GprsGetSmsSeviceMode();
        switch (uService)
        {
        case 0:
            AT_StrCpy(AtRet, "+CGSMS:0");
            break;
        case 1:
            AT_StrCpy(AtRet, "+CGSMS:1");
            break;
        case 2:
            AT_StrCpy(AtRet, "+CGSMS:2");
            break;
        case 3:
            AT_StrCpy(AtRet, "+CGSMS:3");
            break;
        default:
            AT_StrCpy(AtRet, "+CGSMS:0");
            break;
        }
        goto _func_success;
    }
    else
    {
        AT_StrCpy(AtRet, "+CGSMS:0,1,2,3");
        goto _func_success;
    }
_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);

    return;
_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

// -------------------------------------------------------------------------
// Set command         |   Read command          |   Test command
// AT+CRC=[<mode>]   |     AT+ CRC?            |     AT+ CRC =?
// Response(s)         |   Response(s)           |   Response(s)
// Success:        |     Success:            |     Success:
// OK          |       +CRC: <mode>      |       +CRC: (list of supported <mode>s)
// Fail:           |       OK                |       OK
// ERROR       |     Fail:               |     Fail:
// |       ERROR             |       ERROR
// --------------------------------------------------------------------
VOID AT_GPRS_CmdFunc_CRC(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 uParamCnt;

    UINT16 uSize;
    UINT8 uMode;

#define STR_LEN 20

    UINT8 pOutbuff[STR_LEN] = { 0 };

    // input parameter check.
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCnt);

        if (uParamCnt > 1)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (FALSE == AT_Util_CheckValidateU8(pParam->pPara))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uMode, &uSize);

        if (ERR_SUCCESS != iResult)
        {
            AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (uMode > 1)
        {
            AT_TC(g_sw_GPRS, "KEN :: +CRC set Mode invalide!!!");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        gATCurrentCrc = uMode;
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        SUL_ZeroMemory8(pOutbuff, STR_LEN);
        SUL_StrPrint(pOutbuff, "+CRC: (0,1)");  // yy [mod] for Bug ID: 8193
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutbuff, AT_StrLen(pOutbuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        SUL_ZeroMemory8(pOutbuff, STR_LEN);
        SUL_StrPrint(pOutbuff, "+CRC: %d", gATCurrentCrc);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutbuff, AT_StrLen(pOutbuff), pParam->nDLCI, nSim);
        break;

    default:
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_GPRS_CmdFunc_CGCID(AT_CMD_PARA *pParam)
{
    UINT8 nRet            = 0;
    UINT8 nCID            = 0;
    UINT8 pResp[20]       = { 0x00, };
    UINT8 uHelpString[30] = { 0X00, };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {
        case AT_CMD_EXE:
            if (pParam->pPara == NULL)
            {
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
#ifndef AT_NO_GPRS
                nRet = CFW_GetFreeCID(&nCID, nSim);
#endif
                AT_TC(g_sw_GPRS, "AT+CGCID:	Free CID = %d\n\r", nCID);
                if (ERR_SUCCESS == nRet)
                {
                    AT_Sprintf(pResp, "+CGCID:%d", nCID);
                    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    AT_GPRS_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
            break;
        case AT_CMD_TEST:
            AT_StrCpy(uHelpString, "+CGCID: (1-7)");
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uHelpString, AT_StrLen(uHelpString), pParam->nDLCI, nSim);
            break;
        default:
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }
    return;
}

VOID AT_GPRS_Result_OK(UINT32 uReturnValue,
                       UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}
VOID AT_GPRS_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;

    AT_BACKSIMID(nSim);
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

UINT8 at_GprsGetMatchedPdpCid(CFW_GPRS_PDPCONT_INFO *pPdpInfo)
{
    return AT_PDPCID_ERROR;
}

UINT32 at_GprsGetCtxDefStatus(UINT8 cid)
{
    return (ERR_CFW_GPRS_INVALID_CID);
}

INT32 AT_GPRS_IPStringToPDPAddr(UINT8 *pPdpAddrStr, UINT8 uSize, UINT8 *pPdpAddr, UINT8 *pAddrSize)
{
    UINT8 i       = 0;
    UINT8 j       = 0;
    UINT8 m       = 0;
    UINT8 uBuf[4] = { 0 };
    UINT8 uIpNum  = 0;
    UINT8 addrsize = 0;

    AT_TC(g_sw_GPRS, "pPdpAddrStr: %s, uSize: %d", pPdpAddrStr, uSize);

    if ((NULL == pPdpAddrStr) || (0 == uSize))
    {
        return - 1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pPdpAddrStr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pPdpAddrStr[j++];
        }
        uIpNum = atoi(uBuf);
        AT_TC(g_sw_GPRS, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

        pPdpAddr[addrsize ++] = uIpNum;
        AT_MemSet(uBuf, 0, 4);

        j++;
    }

    *pAddrSize = addrsize;
    return 1;

}

#ifdef LTE_NBIOT_SUPPORT
INT32 AT_GPRS_IPv4v6AddrAnalyzer(UINT8 *pPdpAddr, UINT8 uSize, UINT8 *iptype)
{
    UINT8 i       = 0;
    UINT8 j       = 0;
    UINT8 m       = 0;
    UINT8 uBuf[4] = { 0 };
    UINT16 uIpNum  = 0;
    //UINT8 iptype = 0;

    AT_TC(g_sw_GPRS, "pPdpAddr: %s, uSize: %d", pPdpAddr, uSize);

    if ((NULL == pPdpAddr) || (0 == uSize))
    {
        return - 1;
    }

    for(i = 0; i < uSize; i++)
    {
       if(pPdpAddr[i] == '.')
       {
           *iptype = 0;  //ip4
           break;
       }
	   else if(pPdpAddr[i] == ':')
	   {
	      *iptype = 1;  //ip6
	      break;
	   }
    }

    if(*iptype == 0)
    {
        for (i = 0; i < 4; i++)
        {
            m = 0;

            while ((pPdpAddr[j] != '.') && (j < uSize))
            {
                uBuf[m++] = pPdpAddr[j++];
            }

            uIpNum = atoi(uBuf);

            AT_TC(g_sw_GPRS, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

            if ((uBuf[0] != '0') && (uIpNum < 1 || uIpNum > 254))
            {
                return - 1;
            }

            AT_MemSet(uBuf, 0, 4);

            j++;
        }
    }
    else if(*iptype == 1)
    {
        for(i = 0; i < uSize; i++)
        {
            if(pPdpAddr[i] == ':')
	         continue;
	     else
	     {
	         if(isxdigit(pPdpAddr[i]) == FALSE)
	         {
	             return -1;
	         }
	     }
        }
	 AT_TC(g_sw_GPRS, "ip6 addr: %s", pPdpAddr);
    }
	else
	{
	    return 2;
	}

    return 1;
}
#endif
INT32 AT_GPRS_IPAddrAnalyzer(const UINT8 *pPdpAddr, UINT8 uSize)
{
    UINT8 i       = 0;
    UINT8 j       = 0;
    UINT8 m       = 0;
    UINT8 uBuf[4] = { 0 };
    UINT8 uIpNum  = 0;

    AT_TC(g_sw_GPRS, "pPdpAddr: %s, uSize: %d", pPdpAddr, uSize);

    if ((NULL == pPdpAddr) || (0 == uSize))
    {
        return - 1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pPdpAddr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pPdpAddr[j++];
        }

        uIpNum = atoi(uBuf);

        AT_TC(g_sw_GPRS, "uBuf: %s, uIpNum: %d, j: %d", uBuf, uIpNum, j);

        if ((uBuf[0] != '0') && (uIpNum < 1 || uIpNum > 254))
        {
            return - 1;
        }

        AT_MemSet(uBuf, 0, 4);

        j++;
    }

    return 1;
}

VOID PPP_SetInvoidByMuxid(UINT8 Muxid );
VOID AT_CLearGprsCtx(UINT8 nDLCI)
{
    g_uAtWaitActRsp[nDLCI] = 0;
    if(g_uAtGprsCidNum)
        g_uAtGprsCidNum--;
}

#ifdef LTE_NBIOT_SUPPORT
VOID AT_TCPIP_CmdFunc_NIPDATA(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 uCidState            = 0;
    UINT16 uSize                = 0;
    UINT8 cid                      =0;
    UINT8 pdnType              = 0;
    CFW_GPRS_DATA *pGprsData = NULL;
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_TC(g_sw_GPRS, "+NIPDATA: nSim= %d; nDLCI= 0x%x", nSim, pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "exe       in AT_TCPIP_CmdFunc_NIPDATA, parameters is NULL\n");
	 errCode = ERR_AT_CME_PARAM_INVALID;
        goto nonip_result;
		        //AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);

    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if ((iResult != ERR_SUCCESS) || (uParamCount < 2))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_TCPIP_CmdFunc_NIPDATA, parameters error or no parameters offered \n");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nonip_result;
        }

        if (uParamCount == 2)
        {
            uSize   = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &cid, &uSize);

            if (iResult != ERR_SUCCESS)
            {
               errCode = ERR_AT_CME_PARAM_INVALID;
               goto nonip_result;
            }
            if (0 == g_staAtGprsCidInfo[cid].uCid)
            {
                AT_TC(g_sw_GPRS, "request <cid> not defined ");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nonip_result;
            }
#ifndef AT_NO_GPRS
            CFW_GetGprsActState(cid, &uCidState, nSim);
#endif
            if (uCidState != CFW_GPRS_ACTIVED)
            {
                 AT_TC(g_sw_GPRS, "pdp have not be activated!!");
                 errCode = ERR_AT_CME_PARAM_INVALID;
                 goto nonip_result;
            }
	     if(PdpContList[nSim][cid - 1] != NULL)
	     {
	         pdnType = PdpContList[nSim][cid - 1]->PdnType;
		  if(pdnType != CFW_GPRS_PDP_TYPE_NONIP)
		  {
		      AT_TC(g_sw_GPRS, "invalid pdn type ");
		      errCode = ERR_AT_CMS_INVALID_PARA;
                    goto nonip_result;
		  }
	     }
	     uSize = AT_GPRS_NONIP_DATA_MAX_LEN;
            if(pGprsData == NULL)
            {
                if((pGprsData = AT_MALLOC(AT_GPRS_NONIP_DATA_MAX_LEN+4+1)) == NULL)
                {
		      errCode = ERR_AT_CME_NO_MEMORY;
                    goto nonip_result;
                }
            }
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, pGprsData->pData, &uSize);

	     if(uSize > 0)
	     {
	         if(uSize > nonIpMtu) /*exceed non Ip Mtu*/
	         {
	             errCode = ERR_AT_CME_TEXT_LONG;
                    goto nonip_result;
	         }
		  pGprsData->nDataLength = uSize;
	         while(!CFW_GprsSendDataAvaliable(nSim))
                    COS_Sleep(1500);
		  if (ERR_SUCCESS != CFW_GprsSendData(cid, pGprsData, 0,0,nSim))
                {
                    AT_TC(g_sw_GPRS, "------CFW_GprsSendData error");
		      errCode = ERR_AT_CMS_UNKNOWN_ERROR;
                }
		  goto nonip_result;
	     }
	     else
	     {
	         AT_TC(g_sw_GPRS, "nonip data size is 0");
	         errCode = ERR_AT_CME_PARAM_INVALID;
                goto nonip_result;
	     }

        }
	 else
	 {
	     errCode = ERR_AT_CME_PARAM_INVALID;
	     AT_TC(g_sw_GPRS, "paras count is invalid");
            goto nonip_result;
	 }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+NIPDATA: (cid,\"This is Non-IP Data\")");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

nonip_result:
    if(pGprsData!=NULL)
    {
        AT_FREE(pGprsData);
    }
    if(errCode == CMD_FUNC_SUCC)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
	 return;
    }
    else
    {
        AT_TC(g_sw_GPRS, "errCode : %d", errCode);
        AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	 return;
    }
    
}

void NonIpDataHandler( UINT8 cid, CFW_GPRS_DATA *pGprsData, UINT8 nSimId)
{
    UINT8 retRsp[3500] = {0};
    UINT8 crtdcp_string[3020] = {0};
    if(pGprsData)
    { 
	   if(pGprsData->nDataLength != 0)
	   {
	      if(TRUE== AT_UtilOctetToString(pGprsData->pData, pGprsData->nDataLength, crtdcp_string))
	      {
	          AT_Sprintf(retRsp, "+NIPDATA: %d, %d, \"%s\"", cid, pGprsData->nDataLength, crtdcp_string);
	      }
	      else
	      {
	          AT_TC(g_sw_GPRS, "AT_UtilOctetToString return FALSE");
		      goto label_free_mem;
	      }
	   }
	   else
	   {
	     AT_Sprintf(retRsp, "+NIPDATA: %d, 0, \"\"", cid);
	   }
       AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, retRsp, AT_StrLen(retRsp),
                              AT_ASYN_GET_DLCI(nSimId), nSimId);
label_free_mem:
     if(pGprsData)
	     CSW_GPRS_FREE(pGprsData);
    }
}
VOID AT_GPRS_CmdFunc_CGEQOS(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 *pResp = NULL;
    UINT8 uCid   = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_EQOS sEqos = { 0, 0, 0, 0, 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGEQOS:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOS:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount < 1 || uParamCount > 6)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOS:uParamCount error,uParamCount = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get cir failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX || (0 == g_staAtGprsCidInfo[uCid].uCid))
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        if (uParamCount >= 2)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &sEqos.nQci, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get precedence failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            if (0 == uSize)
            {
                sEqos.nQci = 0;
            }

        }
        if (uParamCount >= 3)
        {
            uIndex++;
            uSize   = 4;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &sEqos.nDlGbr, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get delay failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            if (0 == uSize)
            {
                sEqos.nDlGbr = 0;
            }
        }
        if (uParamCount >= 4)
        {
            uIndex++;
            uSize   = 4;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &sEqos.nUlGbr, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get reliability failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sEqos.nUlGbr = 0;
            }
            
        }
        if (uParamCount >= 5)
        {
            uSize = 4;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &sEqos.nDlMbr, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get peak failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sEqos.nDlMbr = 0;
            }
        }
        if (uParamCount >= 6)
        {
            uSize = 4;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &sEqos.nUlMbr, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:Get mean failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }

            if (0 == uSize)
            {
                sEqos.nUlMbr = 0;
            }
        }
        iResult = CFW_GprsSetCtxEQos(uCid,&sEqos,nSim);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOS:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pResp, 0, SIZEOF(pResp));

            AT_Sprintf(pResp, "+CGEQOS:%d,%d,%d,%d,%d,%d",
                       uCid, sEqos.nQci,sEqos.nDlGbr,sEqos.nUlGbr,sEqos.nDlMbr,sEqos.nUlMbr);
            goto _func_success;
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        for (uCid = AT_PDPCID_MIN; uCid <= AT_PDPCID_MAX; uCid++)
        {
            iResult = CFW_GprsGetCtxEQos(uCid, &sEqos, nSim);
            if (ERR_SUCCESS == iResult)
            {
                if (NULL == pResp)
                {
                    pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
                    if (NULL == pResp)
                    {
                        uErrCode = ERR_AT_CME_NO_MEMORY;
                        goto _func_fail;
                    }
                }
                AT_MemSet(pResp, 0, SIZEOF(pResp));
                AT_Sprintf(pResp, "+CGEQOS:%d,%d,%d,%d,%d,%d",
                       uCid, sEqos.nQci,sEqos.nDlGbr,sEqos.nUlGbr,sEqos.nDlMbr,sEqos.nUlMbr);
                AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOS:CFW_GprsGetReqQos() failed,iResult = 0x%x,uCid = %d.", iResult, uCid);
            }
        }
        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp[0] = 0;
            goto _func_success;
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);
        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }
        AT_MemSet(pResp, 0, 128);
        AT_StrCpy(pResp, "+CGEQOS: (1..11), (0..9), , , , ");
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }
_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    return;
_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    return;
}

VOID AT_GPRS_CmdFunc_CGEQOSRDP(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 *pResp = NULL;
    UINT8 uCid   = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_EQOS sEqos = { 0, 0, 0, 0, 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount < 1 || uParamCount > 2)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:uParamCount error,uParamCount = %d.", uParamCount);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 1;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uCid, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:Get cir failed,iResult = 0x%x.", iResult);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
            if (uCid < AT_PDPCID_MIN || uCid > AT_PDPCID_MAX || (0 == g_staAtGprsCidInfo[uCid].uCid))
            {
                AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:The param uCid error,uCid = %d.", uCid);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
	 iResult = CFW_GprsGetCtxEQos(uCid,&sEqos,nSim);
        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_GPRS, "AT+CGEQOSRDP:CFW_GprsSetReqQos() failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_EXE_FAIL;
            goto _func_fail;
        }
        else
        {
            pResp = AT_MALLOC((AT_GPRS_PDPADDR_MAX_LEN + AT_GPRS_APN_MAX_LEN + AT_GPRS_APN_MAX_LEN + 20) * 2);
            if (NULL == pResp)
            {
                uErrCode = ERR_AT_CME_NO_MEMORY;
                goto _func_fail;
            }
            AT_MemSet(pResp, 0, SIZEOF(pResp));
            AT_Sprintf(pResp, "+CGEQOSRDP:%d,%d,%d,%d,%d,%d",
                       uCid, sEqos.nQci,sEqos.nDlGbr,sEqos.nUlGbr,sEqos.nDlMbr,sEqos.nUlMbr);
            goto _func_success;
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        pResp = AT_MALLOC(128);
        if (NULL == pResp)
        {
            uErrCode = ERR_AT_CME_NO_MEMORY;
            goto _func_fail;
        }
        AT_MemSet(pResp, 0, 128);
        AT_StrCpy(pResp, "+CGEQOSRDP: (1..11), (0..9), , , , ");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
        AT_FREE(pResp);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }
_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pResp, AT_StrLen(pResp), pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    return;
_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    if (NULL != pResp)
    {
        AT_FREE(pResp);
        pResp = NULL;
    }
    return;
}

VOID AT_GPRS_CmdFunc_CSODCP(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 uCidState            = 0;
    UINT16 uSize                = 0;
    UINT8 cid                      =0;
    UINT16 cpdata_len = 0;
    UINT16 real_cpdata_len = 0;
    UINT8    rai = 0;
    UINT8    type_of_user_data = 0;
    CFW_GPRS_DATA *pGprsData = NULL;
    UINT8     cpdata[3010] = {0};
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_TC(g_sw_GPRS, "+CSODCP: nSim= %d; nDLCI= 0x%x", nSim, pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CSODCP, parameters is NULL\n");
	 errCode = ERR_AT_CME_PARAM_INVALID;
        goto csodcp_result;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if ((iResult != ERR_SUCCESS) || (uParamCount < 3) || (uParamCount > 5))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_GPRS_CmdFunc_CSODCP, parameters error or invalid parameters count offered \n");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto csodcp_result;
        }

        if (uParamCount >= 3)
        {
            uSize   = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &cid, &uSize);

            if (iResult != ERR_SUCCESS)
            {
               errCode = ERR_AT_CME_PARAM_INVALID;
               goto csodcp_result;
            }
            if (0 == g_staAtGprsCidInfo[cid].uCid)
            {
                AT_TC(g_sw_GPRS, "request <cid> not defined ");
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto csodcp_result;
            }
#ifndef AT_NO_GPRS
            CFW_GetGprsActState(cid, &uCidState, nSim);
#endif
            if (uCidState != CFW_GPRS_ACTIVED)
            {
                 AT_TC(g_sw_GPRS, "pdp have not be activated!!");
                 errCode = ERR_AT_CME_PARAM_INVALID;
                 goto csodcp_result;
            }
           uSize = 2;
	    iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16, &cpdata_len, &uSize);
           if((cpdata_len==0) || (cpdata_len > 1500))
           {
               AT_TC(g_sw_GPRS, "invalid cpdata length");
                 errCode = ERR_AT_CME_PARAM_INVALID;
                 goto csodcp_result;
           }
            if(pGprsData == NULL)
            {
                if((pGprsData = AT_MALLOC(cpdata_len+4+1)) == NULL)
                {
		      errCode = ERR_AT_CME_NO_MEMORY;
                    goto csodcp_result;
                }
            }
           uSize = sizeof(cpdata);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, cpdata, &uSize);

	     if (uParamCount >= 4)
	     {
	         uSize = 1;
	         iResult = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &rai, &uSize);
	         if(rai > 2)
	         {
	             errCode = ERR_AT_CME_PARAM_INVALID;
                    goto csodcp_result;
	         }
	     }
	     if (uParamCount >= 5)
	     {
	         uSize = 1;
	         iResult = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT8, &type_of_user_data, &uSize);
		   if(type_of_user_data > 1)
	         {
	             errCode = ERR_AT_CME_PARAM_INVALID;
                    goto csodcp_result;
	         }
	     }
            while(!CFW_GprsSendDataAvaliable(nSim))
                COS_Sleep(1500);
	     if(FALSE ==AT_UtilStringToOctet(cpdata, cpdata_len,pGprsData->pData))
	     {
	         AT_TC(g_sw_GPRS, "AT_UtilStringToOctet return FALSE------input octet is invalid");
	         errCode = ERR_AT_CME_PARAM_INVALID;
                goto csodcp_result;
	     }
	     pGprsData->nDataLength = cpdata_len;
            if (ERR_SUCCESS != CFW_GprsSendData(cid, pGprsData, rai,type_of_user_data,nSim))
            {
                AT_TC(g_sw_GPRS, "------CFW_GprsSendData error");
                errCode = ERR_AT_CMS_UNKNOWN_ERROR;
            }
            goto csodcp_result;

        }
	 else
	 {
	     errCode = ERR_AT_CME_PARAM_INVALID;
	     AT_TC(g_sw_GPRS, "paras count is less");
            goto csodcp_result;
	 }
	 
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CSODCP: cid, cpdata_length, \"cpdata\" [, RAI [, type_of_user_data]] ");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

csodcp_result:
    if(pGprsData!=NULL)
    {
        AT_FREE(pGprsData);
    }
    if(errCode == CMD_FUNC_SUCC)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
	 return;
    }
    else
    {
        AT_TC(g_sw_GPRS, "errCode : %d", errCode);
        AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	 return;
    }
    
}

UINT8  crtdcp_reporting;
void AT_GPRS_CmdFunc_CRTDCP(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 nCount = 0;
    UINT8 reporting = 0;
    UINT16 nSize = 0;
    CFW_SIM_ID nSim = (CFW_SIM_ID)AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CRTDCP: pParam = NULL");
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0,0);
        return;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &nCount);
        if ((iResult != ERR_SUCCESS) || (nCount != 1))
        {
            AT_TC(g_sw_GPRS, " AT_GPRS_CmdFunc_CRTDCP: Parameter = %s\n", pParam->pPara);
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI,nSim);
            return;
        }
	 nSize = SIZEOF(reporting);
	 iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &reporting, &nSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT_GPRS_CmdFunc_CRTDCP: pParam->pPara1 = %s\n", pParam->pPara);
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI,nSim);
            return;
        }
	 if(reporting > 1)
	 {
            AT_TC(g_sw_GPRS, "AT_GPRS_CmdFunc_CRTDCP: pParam->pPara2 = %s\n", pParam->pPara);
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI,nSim);
            return;
        }
	 crtdcp_reporting = reporting;
	 AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0,  NULL, 0, pParam->nDLCI,nSim);
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8* pTestRsp = "+CRTDCP: 0/1";
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0,  pTestRsp, AT_StrLen(pTestRsp), pParam->nDLCI,nSim);
        return;
    }
    else if(AT_CMD_READ == pParam->iType)
    {
        UINT8 pTestRsp[20];
	 AT_Sprintf(pTestRsp, "+CRTDCP: %d", crtdcp_reporting);
	 AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0,  pTestRsp, AT_StrLen(pTestRsp), pParam->nDLCI,nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_GPRS, "AT_GPRS_CmdFunc_CRTDCP: This type does not supported!");
        AT_GPRS_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI,nSim);
        return;
    }	
}

void CrtdcpDataHandler( UINT8 cid, CFW_GPRS_DATA *pGprsData, UINT8 nSimId)
{
    //UINT8 crtdcp_data[1500] = {0};
    UINT8 retRsp[3500] = {0};
    UINT8 crtdcp_string[3020] = {0};
   // UINT8 crtdcp_len = 0;
    if(pGprsData)
    {
       // AT_MemCpy(crtdcp_data, pGprsData->pData, pGprsData->nDataLength);
	// crtdcp_len = pGprsData->nDataLength;
	 
	 if(pGprsData->nDataLength != 0)
	 {
	      if(TRUE== AT_UtilOctetToString(pGprsData->pData, pGprsData->nDataLength, crtdcp_string))
	      {
	          AT_Sprintf(retRsp, "+CRTDCP: %d, %d, \"%s\"", cid, pGprsData->nDataLength, crtdcp_string);
	      }
	      else
	      {
	          AT_TC(g_sw_GPRS, "AT_UtilOctetToString return FALSE");
		   goto label_free_mem;
	      }
	 }
	 else
	 {
	     AT_Sprintf(retRsp, "+CRTDCP: %d, 0, \"\"", cid);
	 }
        AT_GPRS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, retRsp, AT_StrLen(retRsp),
                              AT_ASYN_GET_DLCI(nSimId), nSimId);//need not clear current command
label_free_mem:
        if(pGprsData)
	     CSW_GPRS_FREE(pGprsData);
    }
}

VOID AT_TCPIP_CmdFunc_DIRECTIP(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    AT_TC(g_sw_GPRS, "AT+DIRECTIP: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+DIRECTIP: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto directip_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+DIRECTIP: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto directip_fail;
        }
        if (1 != uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+DIRECTIP: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto directip_fail;
        }
        if (uParamCount == 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+DIRECTIP: get para directip fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto directip_fail;
            }
            if (ucByte > 1)
            {
                AT_TC(g_sw_GPRS, "AT+DIRECTIP: get para directip error. value = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto directip_fail;
            }
            nbiot_nvSetDirectIpMode(ucByte);
            AT_TC(g_sw_GPRS, "AT+DIRECTIP: get para directip=%d successfully", ucByte);
        }
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+DIRECTIP: %d", nbiot_nvGetDirectIpMode());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+DIRECTIP: value=[0-1](0-default, use internal lwip; 1-direct ip packet from external mcu)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
directip_fail:
    AT_TC(g_sw_GPRS, "AT+DIRECTIP: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}
VOID AT_GPRS_CmdFunc_VERSIONCONTROL(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_GPRS, "AT+VERCTRL: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+VERCTRL: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto verctrl_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+VERCTRL: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto verctrl_fail;
        }
        if (0 == uParamCount || 1 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+VERCTRL: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto verctrl_fail;
        }

        //enable
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+VERCTRL: get para verctrl fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }

            if (ucByte > 2)
            {
                AT_TC(g_sw_GPRS, "AT+VERCTRL: get para verctrl error. enable = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto verctrl_fail;
            }
            nbiot_nvSetVersionControl(ucByte);
            AT_TC(g_sw_GPRS, "AT+VERCTRL: get para verctrl=%d successfully", ucByte);
        }
      
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+VERCTRL: %d", nbiot_nvGetVersionControl());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+VERCTRL: enable=[0-2](0-storeroom; 1-gcf; 2-product)");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

verctrl_fail:
    AT_TC(g_sw_GPRS, "AT+VERCTRL: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

VOID AT_GPRS_CmdFunc_CFGDFTPDN(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    UINT8 uPdnTypeIdx = 0;
    UINT8 uPdnTypeTable[4] = {1,2,3,5};
    UINT8 uApnIndexTable[6] = {0xff,0,1,2,0xff,3};
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 apn[AT_GPRS_APN_MAX_LEN + 1];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfgdftpdn_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgdftpdn_fail;
        }
        if (0 == uParamCount || 2 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgdftpdn_fail;
        }

        //pdnType
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: get para pdnType fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }

            if (uSize != 0)               
            {
                /*24.301 9.9.4.10*/
                if ((ucByte == 0) || (ucByte == 4) || (ucByte > 5))
                {
                    AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: get para pdnType illegal. pdnType = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgdftpdn_fail;
                }
                AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: get para pdnType=%d successfully", ucByte);
                nbiot_nvSetDefaultPdnType(ucByte);
            }
        }

        //apn
        if (uParamCount >= 2)
        {
            uIndex++;
            AT_MemSet(apn, 0, AT_GPRS_APN_MAX_LEN + 1);
            uSize = AT_GPRS_APN_MAX_LEN;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, &apn[0], &uSize);
            if (ERR_SUCCESS != iResult)
            {
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgdftpdn_fail;
            }

            if (uSize == 0)
            {
                apn[0] = '\0';
                nbiot_nvSetDefaultApn(uApnIndexTable[nbiot_nvGetDefaultPdnType()], &apn[0], 1);
            }
            else
            {
                nbiot_nvSetDefaultApn(uApnIndexTable[nbiot_nvGetDefaultPdnType()], &apn[0], uSize+1);
            }
            AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: get para apn=%s successfully", apn);
        }

        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGDFTPDN: defaultPdnType=%d; ", nbiot_nvGetDefaultPdnType());
        for (uPdnTypeIdx = 0; uPdnTypeIdx < 4; uPdnTypeIdx++)
        {
            AT_Sprintf(AtRet, "%s [%d]pdnType=%d,apn=%s; ", AtRet, uPdnTypeIdx, uPdnTypeTable[uPdnTypeIdx],
                nbiot_nvGetDefaultApn(uPdnTypeIdx));
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGDFTPDN: pdnType=[1,2,3,5], apn=\"string\"");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cfgdftpdn_fail:
    AT_TC(g_sw_GPRS, "AT+CFGDFTPDN: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CFGCIOT(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_GPRS, "AT+CFGCIOT: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CFGCIOT: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfgciot_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CFGCIOT: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgciot_fail;
        }
        if (0 == uParamCount || 7 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CFGCIOT: Parameter count error. count  = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgciot_fail;
        }

        //nonip
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para nonip fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para nonip error. nonip = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotNonip(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para nonip=%d successfully", ucByte);
            }
        }

        //cpciot
        if (uParamCount >= 2)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para cpciot fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para cpciot error. cpciot = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotCpciot(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para cpciot=%d successfully", ucByte);
            }
        }

        //upciot
        if (uParamCount >= 3)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para upciot fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 3)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para upciot error. upciot = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotUpciot(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para upciot=%d successfully", ucByte);
            }
        }

        //erwopdn
        if (uParamCount >= 4)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para erwopdn fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 2)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para erwopdn error. erwopdn = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotErwopdn(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para erwopdn=%d successfully", ucByte);
            }
        }

        //sms_wo_comb_att
        if (uParamCount >= 5)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para sms_wo_comb_att fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para sms_wo_comb_att error. sms_wo_comb_att = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotSmsWoCombAtt(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para sms_wo_comb_att=%d successfully", ucByte);
            }
        }

        //apn_rate_control
        if (uParamCount >= 6)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para apn_rate_control fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para apn_rate_control error. apn_rate_control = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotApnRateCtrl(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para apn_rate_control=%d successfully", ucByte);
            }
        }

        //epco
        if (uParamCount >= 7)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para epco fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgciot_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para epco error. epco = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgciot_fail;
                }
                nbiot_nvSetCiotEpco(ucByte);
                AT_TC(g_sw_GPRS, "AT+CFGCIOT: get para epco=%d successfully", ucByte);
            }
        }


        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGCIOT: nonip=%d, cpciot=%d, upciot=%d, erwopdn=%d, sms_wo_comb_att=%d, apn_rate_control=%d, epco=%d",
            nbiot_nvGetCiotNonip(), 
            nbiot_nvGetCiotCpciot(),
            nbiot_nvGetCiotUpciot(),
            nbiot_nvGetCiotErwopdn(), 
            nbiot_nvGetCiotSmsWoCombAtt(),
            nbiot_nvGetCiotApnRateCtrl(),
            nbiot_nvGetCiotEpco());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);        
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGCIOT: nonip=[0-1], cpciot=[0-1](NBIoT Ignore), upciot=[0-3], erwopdn=[0-2], sms_wo_comb_att=[0-1], apn_rate_control=[0-1], epco=[0-1]");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cfgciot_fail:
    AT_TC(g_sw_GPRS, "AT+CFGCIOT: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CEDRXS(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 aucBitStr[8];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */
    UINT8 nvChanged = FALSE;
    UINT8 edrxEnable = 0;

    AT_TC(g_sw_GPRS, "AT+CEDRXS: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CEDRXS: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cedrxs_fail;
    }
    edrxEnable = nbiot_nvGetEdrxEnable();

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CEDRXS: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cedrxs_fail;
        }
        if (0 == uParamCount || 3 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CEDRXS: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cedrxs_fail;
        }

        //mode
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+CEDRXS: get para mode fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cedrxs_fail;
            }

            if (ucByte > 3)
            {
                AT_TC(g_sw_GPRS, "AT+CEDRXS: get para mode error. mode = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cedrxs_fail;
            }

            AT_TC(g_sw_GPRS, "AT+CEDRXS: get para mode=%d successfully", ucByte);            
            if (ucByte == 0)
            {
                if ((edrxEnable == 1)
                    || (edrxEnable == 2))
                    nvChanged = TRUE;
            }
            else if (ucByte == 1)
            {
                if ((edrxEnable == 0)
                    || (edrxEnable == 3))
                    nvChanged = TRUE;
            }
            else if (ucByte == 2)// 2 Enable the use of eDRX and enable the unsolicited result code
            {
                if ((edrxEnable == 0)
                    || (edrxEnable == 3))
                    nvChanged = TRUE;
            }
            // 3 Disable the use of eDRX and discard all parameters for eDRX or, if available, 
            //reset to the manufacturer specific default values.
            else if (ucByte == 3)
            {
                if ((edrxEnable == 1)
                    || (edrxEnable == 2))
                    nvChanged = TRUE;

                nbiot_nvSetEdrxValue(0);
                AT_TC(g_sw_GPRS, "AT+CEDRXS: reset para, edrxEnable=%d, edrxValue=%d", 
                    ucByte, nbiot_nvGetEdrxValue());
            }
            nbiot_nvSetEdrxEnable(ucByte);
        }

        if ((edrxEnable == 1)
            || (edrxEnable == 2))
        {
            //AcT-type
            if (uParamCount >=2)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CEDRXS: get para actType fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cedrxs_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte != 5)    //5	E-UTRAN (NB-S1 mode)
                    {
                        AT_TC(g_sw_GPRS, "AT+CEDRXS: actType value NOT supported. actType = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cedrxs_fail;
                    }
                    AT_TC(g_sw_GPRS, "AT+CEDRXS: get para actType=%d successfully", ucByte);
                }
            }

            //edrxValue
            if (uParamCount >= 3)
            {
                uIndex++;
                uSize = 8;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, &aucBitStr[0], &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CEDRXS: get para edrxValue fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cedrxs_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (AT_Util_BitStrToByte(&ucByte, &aucBitStr[0]) == FALSE)
                    {
                        AT_TC(g_sw_GPRS, "AT+CPSMS: para edrxValue illegal. edrxValue = %s", &aucBitStr[0]);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cedrxs_fail;
                    }

                    if (ucByte > 15)
                    {
                        AT_TC(g_sw_GPRS, "AT+CEDRXS: get para edrxValue error. edrxValue = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cedrxs_fail;
                    }
                    if (nbiot_nvGetEdrxValue() != ucByte)
                        nvChanged = TRUE;
                    nbiot_nvSetEdrxValue(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CEDRXS: get para edrxValue=%d successfully", ucByte);
                }
            }
        }

        if (nvChanged)
            CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_Util_ByteToBitStr(&aucBitStr[0], nbiot_nvGetEdrxValue(), 4);
    
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CEDRXS: %d,5,%s",
            edrxEnable, 
            &aucBitStr[0]);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CEDRXS: mode=[0-3], AcT-type=5, Requested_eDRX_value=\"4bitString eg.0100 \"");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cedrxs_fail:
    AT_TC(g_sw_GPRS, "AT+CEDRXS: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CGAPNRC(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 cid = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */
    UINT8 aer;
    UINT8 unit;
    UINT32 rate;
    UINT8 i = 0;
    UINT16 apnRetLen = 0;
    AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_TC(g_sw_GPRS, "AT+CGAPNRC: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CGAPNRC: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cgapnrc_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CGAPNRC: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cgapnrc_fail;
        }
        if (1 != uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CGAPNRC: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cgapnrc_fail;
        }

        //cid
        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
        if (iResult != ERR_SUCCESS || uSize == 0)
        {
            AT_TC(g_sw_GPRS, "AT+CGAPNRC: get para n fail. iResult = %d, uSize = %d", iResult, uSize);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cgapnrc_fail;
        }

        cid = ucByte;
        aer = g_staAtGprsCidInfo[cid].apnacAer;
        unit = g_staAtGprsCidInfo[cid].apnacUnit;
        rate = g_staAtGprsCidInfo[cid].apnacRate;
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CGAPNRC: %d,%d,%d,%d", cid, aer, unit, rate);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if ((AT_CMD_EXE == pParam->iType)
        || (AT_CMD_READ == pParam->iType))
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        for(i = 0; i < AT_PDPCID_MAX+1; i++)
        {
            if(g_staAtGprsCidInfo[i].uState == AT_GPRS_ACT)
            {
                apnRetLen = AT_Sprintf(AtRet + apnRetLen, "\r\n+CGAPNRC: %d,%d,%d,%d", \
					g_staAtGprsCidInfo[i].uCid, g_staAtGprsCidInfo[i].apnacAer, g_staAtGprsCidInfo[i].apnacUnit, g_staAtGprsCidInfo[i].apnacRate);
            }
        }
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        apnRetLen += AT_Sprintf(AtRet + apnRetLen, "+CGAPNRC: ");
        for(i = 0; i < AT_PDPCID_MAX+1; i++)
        {
            if(g_staAtGprsCidInfo[i].uState == AT_GPRS_ACT)
            {
                apnRetLen += AT_Sprintf(AtRet + apnRetLen, "%d,", g_staAtGprsCidInfo[i].uCid);
            }
        }
	 AtRet[apnRetLen-1] = 0;
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cgapnrc_fail:
    AT_TC(g_sw_GPRS, "AT+CGAPNRC: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CFGEDRX(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */
    UINT8 nvChanged = FALSE;

    AT_TC(g_sw_GPRS, "AT+CFGEDRX: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CFGEDRX: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfgedrx_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CFGEDRX: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgedrx_fail;
        }
        if (0 == uParamCount || 3 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CFGEDRX: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgedrx_fail;
        }

        //enable
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para enable fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgedrx_fail;
            }

            if (ucByte > 1)
            {
                AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para enable error. enable = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgedrx_fail;
            }
            if (nbiot_nvGetEdrxEnable() != ucByte)
                nvChanged = TRUE;
            nbiot_nvSetEdrxEnable(ucByte);
            AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxEnable=%d successfully", ucByte);
        }

        if (nbiot_nvGetEdrxEnable() == 1)
        {
            //edrxPtw
            if (uParamCount >= 2)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxPtw fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgedrx_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte > 15)
                    {
                        AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxPtw error. edrxPtw = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cfgedrx_fail;
                    }
                    if (nbiot_nvGetEdrxPtw() != ucByte)
                        nvChanged = TRUE;
                    nbiot_nvSetEdrxPtw(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxPtw=%d successfully", ucByte);
                }
            }

            //edrxValue
            if (uParamCount >= 3)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxValue fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgedrx_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte > 15)
                    {
                        AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxValue error. edrxValue = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cfgedrx_fail;
                    }
                    if (nbiot_nvGetEdrxValue() != ucByte)
                        nvChanged = TRUE;
                    nbiot_nvSetEdrxValue(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CFGEDRX: get para edrxValue=%d successfully", ucByte);
                }
            }
        }

        if (nvChanged)
            CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGEDRX: %d, ptw=%d, edrxVal=%d",
            nbiot_nvGetEdrxEnable(), 
            nbiot_nvGetEdrxPtw(),
            nbiot_nvGetEdrxValue());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGEDRX: enable=[0-1], edrxPtw=[0-15], edrxValue=[0-15]");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cfgedrx_fail:
    AT_TC(g_sw_GPRS, "AT+CFGEDRX: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CPSMS(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 aucBitStr[12] = {0};
    UINT8 aucBitStr2[12] = {0};
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */
    UINT8 nvChanged = FALSE;

    AT_TC(g_sw_GPRS, "AT+CPSMS: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CPSMS: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfgpsm_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CPSMS: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgpsm_fail;
        }
        if (0 == uParamCount || 5 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CPSMS: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfgpsm_fail;
        }

        //mode
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+CPSMS: get para mode fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgpsm_fail;
            }

            if (ucByte > 2)
            {
                AT_TC(g_sw_GPRS, "AT+CPSMS: get para mode error. mode = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfgpsm_fail;
            }

            AT_TC(g_sw_GPRS, "AT+CPSMS: get para mode=%d successfully", ucByte);
            if (ucByte == 0)
            {
                if (nbiot_nvGetPsmEnable() == 1)
                    nvChanged = TRUE;
            }
            else if (ucByte == 1)
            {
                if ((nbiot_nvGetPsmEnable() == 0)
                    || (nbiot_nvGetPsmEnable() == 2))
                    nvChanged = TRUE;
            }
            else if (ucByte == 2)
            {
                if (nbiot_nvGetPsmEnable() == 1)
                    nvChanged = TRUE;

                // 2	Disable the use of PSM and discard all parameters for PSM or, 
                //      if available, reset to the manufacturer specific default values.
                nbiot_nvSetPsmT3412(0x45);
                nbiot_nvSetPsmT3324(0);
                AT_TC(g_sw_GPRS, "AT+CPSMS: reset para, mode=%d, t3412val=0x%02x, t3324Val=0x%02x", 
                    ucByte, nbiot_nvGetPsmT3412(), nbiot_nvGetPsmT3324());
            }
            nbiot_nvSetPsmEnable(ucByte);
        }

        if (nbiot_nvGetPsmEnable() == 1)
        {
            //skip <Requested_GPRS-READY-timer> and <Requested_GPRS-READY-timer>
            uIndex += 2;

            //t3412Val
            if (uParamCount >= 4)
            {
                uIndex++;
                uSize = 12;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, &aucBitStr[0], &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CPSMS: get para periodicalTAU fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgpsm_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (AT_Util_BitStrToByte(&ucByte, &aucBitStr[0]) == FALSE)
                    {
                        AT_TC(g_sw_GPRS, "AT+CPSMS: para periodicalTAU illegal. periodicalTAU = %s", &aucBitStr[0]);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cfgpsm_fail;
                    }
                    if (nbiot_nvGetPsmT3412() != ucByte)
                        nvChanged = TRUE;
                    nbiot_nvSetPsmT3412(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CPSMS: get para t3412Val=0x%02x successfully", ucByte);
                }
            }

            //t3324Val
            if (uParamCount >= 5)
            {
                uIndex++;
                uSize = 12;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, &aucBitStr[0], &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CPSMS: get para ActiveTimer fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfgpsm_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (AT_Util_BitStrToByte(&ucByte, &aucBitStr[0]) == FALSE)
                    {
                        AT_TC(g_sw_GPRS, "AT+CPSMS: para ActiveTimer illegal. ActiveTimer = %s", &aucBitStr[0]);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cfgpsm_fail;
                    }
                    if (nbiot_nvGetPsmT3324() != ucByte)
                        nvChanged = TRUE;
                    nbiot_nvSetPsmT3324(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CPSMS: get para t3324Val=0x%02x successfully", ucByte);
                }
            }
        }

        if (nvChanged)
            CFW_NvParasChangedNotification(NV_CHANGED_PSM);
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_Util_ByteToBitStr(&aucBitStr[0], nbiot_nvGetPsmT3412(), 8);
        AT_Util_ByteToBitStr(&aucBitStr2[0], nbiot_nvGetPsmT3324(), 8);
        
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CPSMS: %d,,,%s,%s",
            nbiot_nvGetPsmEnable(), 
            &aucBitStr[0], &aucBitStr2[0]);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CPSMS: mode=[0-2],,,Requested_Periodic-TAU=\"8bitStringofByte eg. 01000111\", Requested_Active-Time=\"8bitStringofByte eg. 01100101\"");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cfgpsm_fail:
    AT_TC(g_sw_GPRS, "AT+CPSMS: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_CFGHCCP(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT16 usData = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_GPRS, "AT+CFGHCCP: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CFGHCCP: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cfghccp_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CFGHCCP: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfghccp_fail;
        }
        if (0 == uParamCount || 3 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+CFGHCCP: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cfghccp_fail;
        }

        //enable
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para enable fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfghccp_fail;
            }

            if (ucByte > 1)
            {
                AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para enable error. enable = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cfghccp_fail;
            }
            nbiot_nvSetHccpEnable(ucByte);
            AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para hccpEnable=%d successfully", ucByte);
        }

        if (nbiot_nvGetHccpEnable() == 1)
        {
            //profile
            if (uParamCount >= 2)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para profile fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfghccp_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte & 0xf2)
                    {
                        AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para profile=0x%02x, not proper", ucByte);
                        goto cfghccp_fail;
                    }
                    nbiot_nvSetHccpProfile(ucByte);
                    AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para profile=0x%02x successfully", ucByte);
                }
            }

            //maxcid
            if (uParamCount >= 3)
            {
                uIndex++;
                uSize = 2;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &usData, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para maxcid fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cfghccp_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (0 == usData || usData > 16383)
                    {
                        AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para maxcid=%d, not proper", usData);
                        goto cfghccp_fail;
                    }               
                    nbiot_nvSetHccpMaxcid(usData);
                    AT_TC(g_sw_GPRS, "AT+CFGHCCP: get para maxcid=%d successfully", usData);
                }
            }
        }
      
        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGHCCP: %d, profile=0x%02x, maxCid=%d",
            nbiot_nvGetHccpEnable(), 
            nbiot_nvGetHccpProfile(),
            nbiot_nvGetHccpMaxcid());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CFGHCCP: enable=[0-1], profile=[0-15], maxcid=[1-16383]");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cfghccp_fail:
    AT_TC(g_sw_GPRS, "AT+CFGHCCP: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


VOID AT_GPRS_CmdFunc_NASCFG(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_GPRS, "AT+NASCFG: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+NASCFG: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto nascfg_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+NASCFG: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nascfg_fail;
        }
        if (0 == uParamCount || 3 < uParamCount)
        {
            AT_TC(g_sw_GPRS, "AT+NASCFG: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto nascfg_fail;
        }

        //LowPriority
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para LowPriority fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 2)
                {
                    AT_TC(g_sw_GPRS, "AT+NASCFG: get para LowPriority error. LowPriority = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto nascfg_fail;
                }

                if (ucByte == 0)
                {
                    nbiot_nvSetNasSigLowPri(0);
                    nbiot_nvSetNasSigDualPri(0);
                }
                else if (ucByte == 1)
                {
                    nbiot_nvSetNasSigLowPri(1);
                    nbiot_nvSetNasSigDualPri(0);
                }
                else if (ucByte == 2)
                {
                    nbiot_nvSetNasSigLowPri(1);
                    nbiot_nvSetNasSigDualPri(1);
                }
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para LowPriority=%d successfully", ucByte);
            }
        }

        //T3245
        if (uParamCount >= 2)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para T3245 fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+NASCFG: get para T3245 error. T3245 = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto nascfg_fail;
                }
                nbiot_nvSetT3245Behave(ucByte);
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para T3245=%d successfully", ucByte);
            }
        }

        //ExceptionData
        if (uParamCount >= 3)
        {
            uIndex++;
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para ExceptionData fail. iResult = %d", iResult);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto nascfg_fail;
            }

            if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
            {
                if (ucByte > 1)
                {
                    AT_TC(g_sw_GPRS, "AT+NASCFG: get para ExceptionData error. ExceptionData = %d", ucByte);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto nascfg_fail;
                }
                nbiot_nvSetExceptionDataReporting(ucByte);
                AT_TC(g_sw_GPRS, "AT+NASCFG: get para ExceptionData=%d successfully", ucByte);
            }
        }

        nvmWriteStatic();
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+NASCFG: LowPriority=%d, OverrideLowPriority=%d, T3245=%d, ExceptionData=%d",
            nbiot_nvGetNasSigLowPri(), 
            nbiot_nvGetNasSigDualPri(), 
            nbiot_nvGetT3245Behave(),
            nbiot_nvGetExceptionDataReporting());
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);        
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+NASCFG: LowPriority=[0-2], T3245=[0-1], ExceptionData=[0-1]");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

nascfg_fail:
    AT_TC(g_sw_GPRS, "AT+NASCFG: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

extern u16 NL1cRachCELevelQuery(u8 * pucCE_Level);
VOID AT_GPRS_CmdFunc_CRCES(AT_CMD_PARA *pParam)
{
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    UINT8 ceLevel = 0;

    AT_TC(g_sw_GPRS, "AT+CRCES: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+CRCES: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto crces_fail;
    }

    if ((AT_CMD_EXE == pParam->iType)
        ||(AT_CMD_READ == pParam->iType))
    {
        NL1cRachCELevelQuery(&ceLevel);
        if (ceLevel == 0xff)
            ceLevel = 0;    
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CRCES: 3,%d,0", ceLevel);  //<AcT>=3: E-UTRAN (NB-S1 mode)
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

crces_fail:
    AT_TC(g_sw_GPRS, "AT+CRCES: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

extern void sdb_printVersionInfo(s8 *strOutputVersion);
VOID AT_GPRS_CmdFunc_VER(AT_CMD_PARA *pParam)
{
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    INT8 strNbiotVersion[90];

    AT_TC(g_sw_GPRS, "AT+VER: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+VER: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto ver_fail;
    }

    if ((AT_CMD_READ == pParam->iType) || (AT_CMD_TEST == pParam->iType))
    {
        sdb_printVersionInfo(strNbiotVersion);
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+VER: %s", strNbiotVersion);

        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        errCode = ERR_AT_CMS_OPER_NOT_ALLOWED;
        goto ver_fail;
    }

ver_fail:
    AT_TC(g_sw_GPRS, "AT+VER: errCode : %d", errCode);
    AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

 // This function to process the AT+CEREG command. include set,read and test command.
 //
 // Set command
 // +CEREG= [<n>]
 // Possible response(s)
 // OK
 // ERROR
 //
 // Read command
 // +CEREG?
 // Possible response(s)
 // +CEREG: <n>,<stat>[,<lac>,<ci>]
 // +CME ERROR: <err>
 // Test command
 // +CEREG=?
 // Possible response(s)
 // +CGREG: (list of supported <n>s)
 //
 
 VOID AT_GPRS_CmdFunc_CEREG(AT_CMD_PARA *pParam)
 {
	 UINT8 respond[100] = {0};
	 INT32 iResult;
	 UINT8 n = 0;
	 UINT16 uSize;
	 UINT8 uParamCount = 0;
	 UINT32 uErrCode   = ERR_SUCCESS;
	 UINT8 uIndex	   = 0;
	 CFW_NW_STATUS_INFO sStatus;
	 // "CDMA-IS95B  5","LTE 14"
	 UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
 
	 if (NULL == pParam)
	 {
		 AT_TC(g_sw_GPRS, "AT+CEREG:NULL == pParam");
		 uErrCode = ERR_AT_CME_PARAM_INVALID;
		 goto _func_fail;
	 }
 
	 // Process the command. include icommand parameter checking,csw api calling,and responsing...
	 if (AT_CMD_SET == pParam->iType)  // Set command.
	 {
		 // this comman have parameter.
		 if (NULL == pParam->pPara)
		 {
			 AT_TC(g_sw_GPRS, "AT+CEREG:NULL == pParam->pPara");
			 uErrCode = ERR_AT_CME_PARAM_INVALID;
			 goto _func_fail;
		 }
 
		 // Get the count of parameters. This command have 1 parameters.
		 iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
 
		 if (iResult != ERR_SUCCESS || uParamCount != 1)
		 {
			 AT_TC(g_sw_GPRS, "AT+CEREG:Get Param count failed,iResult = 0x%x", iResult);
			 uErrCode = ERR_AT_CME_PARAM_INVALID;
			 goto _func_fail;
		 }
 
		 uSize = SIZEOF(n);
 
		 uIndex = 0;
 
		 iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &n, &uSize);
 
		 if (ERR_SUCCESS != iResult)
		 {
			 AT_TC(g_sw_GPRS, "AT+CEREG:Get n failed,iResult = 0x%x.", iResult);
			 uErrCode = ERR_AT_CME_PARAM_INVALID;
			 goto _func_fail;
		 }
 
		 if (n > 5)
		 {
			 AT_TC(g_sw_GPRS, "AT+CEREG:The Prama n is error,n = 0x%x.", n);
			 uErrCode = ERR_AT_CME_PARAM_INVALID;
			 goto _func_fail;
		 }
 
		 g_uAtCereg = n;
 
		 goto _func_success;
	 }
	 else if (AT_CMD_READ == pParam->iType)  // Read command
	 {
		 uErrCode = CFW_GprsGetstatus(&sStatus, nSim);
 
		 if (uErrCode != ERR_SUCCESS)
		 {
			 uErrCode = ERR_AT_CME_PARAM_INVALID;
			 goto _func_fail;
		 }
 
		 AT_TC(g_sw_GPRS, "nCellId: %d, %d, %d, %d", sStatus.nCellId[0], sStatus.nCellId[1], sStatus.nCellId[2], sStatus.nCellId[3]);
 
		 AT_Cereg_respond(respond, &sStatus,  sStatus.nStatus, TRUE);
		 goto _func_success;
	 }
	 else if (AT_CMD_TEST == pParam->iType)
	 {
		 AT_StrCpy(respond, "+CEREG: (0-5)");
		 goto _func_success;
	 }
	 else
	 {
		 uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
		 goto _func_fail;
	 }
 
	 // ]] yy[mod] at 2008-6-4 for bug ID 8676
 _func_success:
 
	 AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, respond, AT_StrLen(respond), pParam->nDLCI, nSim);
	 return;
 
 _func_fail:
 
	 AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	 return;
 
 }

 UINT8 AT_Cereg_respond(UINT8 *respond, CFW_NW_STATUS_INFO *sStatus, UINT8 stat, BOOL reportN)
 {
	UINT8 ret = 1;
	UINT8 len = 0;
	UINT8 reg = g_uAtCereg;
	UINT8 activeTime[8];
	UINT8 periodicTau[8];
	SUL_UtoBinString(activeTime,sStatus->activeTime);
	SUL_UtoBinString(periodicTau, sStatus->periodicTau);
	 if(reportN == TRUE)
	 {
		 len += AT_Sprintf(respond, "+CEREG: %d, ", g_uAtCereg);
	  if((g_uAtCereg==4)||(g_uAtCereg==5))
	  {
		 reg = 5;
	  }
	  else 
	  {
		 reg = 3;
	  }
	 }
	 else
	 {
		 len += AT_Sprintf(respond, "+CEREG: ");
	 }
	 switch(reg)
	 {
		 case 0:
		  ret = 0;
		  break;
		 case 1:
			 AT_Sprintf(respond + len, "%d", stat);
			 break;
		 case 2:
 CeregLabel2:
			 AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\",%d",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType));
			 break;
		 case 3:
 CeregLabel3:
		  if((stat == 0)||(stat == 3)||(stat==4))
		  {
				 AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\",%d,%d,%d",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type, sStatus->reject_cause);
		  }
		  else
		  {
			  goto CeregLabel2;
		  }
			 break;
		 case 4:
 CeregLabel4:
		  if((sStatus->activeTime != 0xff)&&(sStatus->periodicTau!=0xff))
		  {
			  
		   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,,,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),\
								  activeTime[0],activeTime[1],activeTime[2],activeTime[3],\
								  activeTime[4],activeTime[5],activeTime[6],activeTime[7],\
								  periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],\
								  periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
		  }
		  else if(sStatus->activeTime != 0xff)
			 {
			  
		   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,,,\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),\
								  activeTime[0],activeTime[1],activeTime[2],activeTime[3],\
								  activeTime[4],activeTime[5],activeTime[6],activeTime[7]);
		  }
		  else if(sStatus->periodicTau !=0xff)
		  {
			  
		   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,,,,\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType),\
								  periodicTau[0],periodicTau[1],periodicTau[2],periodicTau[3],\
								  periodicTau[4],periodicTau[5],periodicTau[6],periodicTau[7]);
		  }
		  else
		  {
			  goto CeregLabel2;
		  }
			 break;
		 case 5:
			 if((stat == 0)||(stat == 3)||(stat==4))
			 {
				 if((sStatus->activeTime != 0xff)&&(sStatus->periodicTau!=0xff))
			  {
			  
			   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\",\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type,sStatus->reject_cause,\
								  activeTime[0],activeTime[1],activeTime[2],activeTime[3],\
								  activeTime[4],activeTime[5],activeTime[6],activeTime[7],\
								  periodicTau[0], periodicTau[1], periodicTau[2], periodicTau[3],\
								  periodicTau[4], periodicTau[5], periodicTau[6], periodicTau[7]);
			  }
			  else if(sStatus->activeTime != 0xff)
				 {
				
			   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,%d,%d,\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type,sStatus->reject_cause,\
								  activeTime[0],activeTime[1],activeTime[2],activeTime[3],\
								  activeTime[4],activeTime[5],activeTime[6],activeTime[7]);
			  }
			  else if(sStatus->periodicTau !=0xff)
			  {
			  
			   AT_Sprintf(respond + len, "%d,\"%X%X\",\"%X%X%X%X\", %d,%d,%d,,\"%c%c%c%c%c%c%c%c\"",\
								  stat, sStatus->nAreaCode[0], sStatus->nAreaCode[1],\
								  sStatus->nCellId[0], sStatus->nCellId[1],sStatus->nCellId[2], sStatus->nCellId[3],\
								  AT_Gprs_Mapping_Act_From_PsType(sStatus->PsType), sStatus->cause_type,sStatus->reject_cause,\
								  periodicTau[0],periodicTau[1],periodicTau[2],periodicTau[3],\
								  periodicTau[4],periodicTau[5],periodicTau[6],periodicTau[7]);
			  }
		   else
		   {
			   goto CeregLabel3;
		   }
			 }
		  else 
		  {
			  goto CeregLabel4;
		  }
			 break;
		 default:
		  ret = 2;
		  break;
	 }
	 return ret;
 }

 UINT8  AT_Gprs_Mapping_Act_From_PsType(UINT8 pstype)
{
    switch (pstype)
    {
        case 4:
		return 7;
	 case 1:
	 	return 0;
	 case 2:
	 	return 3;
	 default: return 0;
    }
}



#endif //LTE_NBIOT_SUPPORT

#endif //CFW_GPRS_SUPPORT

