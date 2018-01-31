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



#ifdef CFW_GPRS_SUPPORT
#include "sxs_io.h"
#include <csw.h>
#include "api_msg.h"
#include <cfw_prv.h>

#include "cfw_gprs.h"
#include "cfw_gprs_tool.h"
#include "cfw_gprs_data.h"
#include "string.h"
#include "sockets.h"

// Re-route APS events to MMI for AT
#define GET_APS_TASK        CFW_bal_GetTaskHandle(CSW_TASK_ID_APS)
#define DURATION_BEFORE_DETACH  (120*1000 MILLI_SECOND)

#ifdef LTE_NBIOT_SUPPORT
UINT16 appUlNum = 0;
UINT32 appUlSize = 0;
UINT16 appDlNum = 0;
UINT32 appDlSize = 0;
UINT16 appTtlUlNum = 0;
UINT32 appTtlUlSize = 0;
UINT16 appTtlDlNum = 0;
UINT32 appTtlDlSize = 0;
UINT16 appUlNumDiscard = 0;
UINT32 appUlSizeDiscard = 0;
UINT16 appTtlUlNumDiscard = 0;
UINT32 appTtlUlSizeDiscard = 0;

#define APP_STATISTIC_DL_DATA(size)        {\
        appDlNum++;\
        appDlSize += size;\
        appTtlDlNum++;\
        appTtlDlSize += size;}   

#define APP_STATISTIC_UL_DATA(size)        {\
        appUlNum++;\
        appUlSize += size;\
        appTtlUlNum++;\
        appTtlUlSize += size;} 
CFW_TFT_SET *pTftSetList[CFW_SIM_COUNT][7];
CFW_EQOS     pEQosList[CFW_SIM_COUNT][7];
CFW_TFT_PF  *pTftPfList[CFW_SIM_COUNT][7];
UINT8 AT_GprsSetPCid(UINT8 nCid, UINT8 nPCid, UINT8 nSim);
#endif

#define FLOW_CTRL_HOID   (1<<0)
#define FLOW_CTRL_RELEASE   (1<<1)
#define FLOW_CTRL_INVOID   0XFF


CFW_GPRS_PDPCONT_INFO *PdpContList[CFW_SIM_COUNT][7] = {{NULL}};


UINT8 gprs_flow_ctrl[CFW_SIM_COUNT][8];  //simid, cid
UINT8 gprs_flow_ctrl_G[CFW_SIM_COUNT] = {FLOW_RELEASED,};

CFW_GPRS_QOS pQosList[CFW_SIM_COUNT][7];
CFW_GPRS_QOS pMinQosList[CFW_SIM_COUNT][7];
struct CurrentAppCng CurAppCng[CFW_SIM_COUNT];
UINT32 CFW_GprsRemovePdpCxt(UINT8 nCid, CFW_SIM_ID nSimID);

UINT32 CFW_StopDetachTimer(CFW_SIM_ID nSimID);
UINT32 CFW_StartDetachTimer(CFW_SIM_ID nSimID);
BOOL ispdpactive(CFW_SIM_ID nSimID);
UINT32 gprs_SetGprsSum(INT32 upsum, INT32 downsum, CFW_SIM_ID nSimID );
UINT32 gprs_GetGprsSum(INT32 *upsum, INT32 *downsum, CFW_SIM_ID nSimID );
UINT8 isTestSim(UINT8 nSimID);
extern u32 api_CheckTestSim(u32 nSimID);


CONST UINT8 IPCP_OPTION[19] = {0x80, 0x21, 0x10, 0x01, 0x00, 0x00, 0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00, 0x00, 0x00}; //add by wuys 2006-02-26

#ifndef LTE_NBIOT_SUPPORT
PRIVATE 
#endif
UINT32 CFW_GprsAoProc (HAO hAo, CFW_EV *pEvent);




BOOL Gprs_SetAppCngInit(CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error Gprs_SetAppCngInit()\r\n",0x08100779)));

        return nRet;
    }

    CurAppCng[nSimID].CurAppSet = TRUE;
    CurAppCng[nSimID].CurrentAppTaskHandle =  GET_APS_TASK;

    return nRet;
}

BOOL Gprs_SetAppCngDefault(CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error Gprs_SetAppCngDefault()\r\n",0x0810077a)));

        return nRet;
    }
    CurAppCng[nSimID].CurAppSet = TRUE;
    CurAppCng[nSimID].CurrentAppTaskHandle = GET_APS_TASK;
    return nRet;
}

UINT32 Gprs_SetAppCng(BOOL BeModify, HANDLE TaskHandle, CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error Gprs_SetAppCng()\r\n",0x0810077b)));

        return nRet;
    }

    if((CurAppCng[nSimID].CurAppSet) || (TaskHandle == CurAppCng[nSimID].CurrentAppTaskHandle))
    {
        CurAppCng[nSimID].CurAppSet = BeModify;
        CurAppCng[nSimID].CurrentAppTaskHandle =  TaskHandle;
        return ERR_SUCCESS;
    }
    else
    {
        return ERR_INVALID_HANDLE;// this application don't modify the application aim now.
    }
}

HANDLE  Gprs_GetAppCng(CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error Gprs_GetAppCng()\r\n",0x0810077c)));

        return nRet;
    }

    return CurAppCng[nSimID].CurrentAppTaskHandle;
}


// Only use to test
UINT32 Gprs_MSG_Redirect(CFW_EVENT *pEvent, VOID *pUserData)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("In Gprs_MSG_Redirect, send event: %d \n",0x0810077d)),pEvent->nEventId);
    ASSERT(0);
}

UINT32 DnsServerIP[2][7][2] = {0}; //add wuys 2009-10-26
extern INT32 gprs_sum[CFW_SIM_COUNT][2];
UINT8 GprsCidSavedInApp[CFW_SIM_COUNT];


BOOL IsCidSavedInApp(UINT8 Cid, UINT8 nSimID)
{
    if(0x00 == (GprsCidSavedInApp[nSimID] & (0x01 << Cid)))
        return FALSE;
    else
        return TRUE;
}



UINT32 CFW_GprsInit(VOID)
{

    UINT32 nRet = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsInit !!!!!!!\n",0x0810077f)));
    INT32 gprs_upsum_tmp = 0;
    INT32 gprs_downsum_tmp = 0;

    UINT8 nIndex = 0;
    for(nIndex = 0; nIndex < CFW_SIM_COUNT; nIndex++)
    {
        nRet = CFW_CheckSimId(nIndex);
        if( ERR_SUCCESS != nRet)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsInit()\r\n",0x08100780)));

            return nRet;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("gprs_flow_ctrl_G = %d in CFW_GprsInit()\r\n",0x08100781)),gprs_flow_ctrl_G[nIndex] );

        UINT8 cidIdx = 0;
        for(cidIdx = 0; cidIdx < 7; cidIdx++)
        {
            DnsServerIP[nIndex][cidIdx][0] = 0;
            DnsServerIP[nIndex][cidIdx][1] = 0;
        }
        gprs_GetGprsSum(&gprs_upsum_tmp, &gprs_downsum_tmp, nIndex);
        gprs_sum[nIndex][0] = gprs_upsum_tmp;
        gprs_sum[nIndex][1] = gprs_downsum_tmp;
        GprsCidSavedInApp[nIndex] = 0x00;
        gprs_flow_ctrl_G[nIndex] = FLOW_RELEASED;
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("gprs_flow_ctrl_G = %d in CFW_GprsInit() 2\r\n",0x08100782)),gprs_flow_ctrl_G[nIndex] );

    }

    CFW_RegisterCreateAoProc(API_PDP_CTXACTIVATE_IND, GprsMTAoProc);
    for(nIndex = 0; nIndex < CFW_SIM_COUNT; nIndex++)
    {
        nRet = Gprs_SetAppCngInit(nIndex);
        if( ERR_SUCCESS != nRet)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! Gprs_SetAppCngInit error CFW_GprsInit()\r\n",0x08100783)));
            return nRet;
        }
    }
    CFW_SetEventProc( CFW_SRV_GPRS_EV_RSP, NULL, Gprs_MSG_Redirect );//add by wuys 2008-04-11

    return nRet;
}

#ifdef __MULTI_RAT__
UINT32 CFW_GprsSetReqQosUmts(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSetReqQos()\r\n",0x08100784)));
        return ret;
    }
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQosList[nSimID][nCid - 1].nTrafficClass         = pQos->nTrafficClass;
    pQosList[nSimID][nCid - 1].nMaximumbitrateUL     = pQos->nMaximumbitrateUL;
    pQosList[nSimID][nCid - 1].nMaximumbitrateDL     = pQos->nMaximumbitrateDL;
    pQosList[nSimID][nCid - 1].nGuaranteedbitrateUL = pQos->nGuaranteedbitrateUL;
    pQosList[nSimID][nCid - 1].nGuaranteedbitrateDL = pQos->nGuaranteedbitrateDL;
    pQosList[nSimID][nCid - 1].nDeliveryOrder         = pQos->nDeliveryOrder;
    pQosList[nSimID][nCid - 1].nMaximumSDUSize         = pQos->nMaximumSDUSize;
    pQosList[nSimID][nCid - 1].nSDUErrorRatio         = pQos->nSDUErrorRatio;
    pQosList[nSimID][nCid - 1].nResidualBER         = pQos->nResidualBER;
    pQosList[nSimID][nCid - 1].nDeliveryOfErroneousSDU = pQos->nDeliveryOfErroneousSDU;
    pQosList[nSimID][nCid - 1].nTransferDelay         = pQos->nTransferDelay;
    pQosList[nSimID][nCid - 1].nTrafficHandlingPriority = pQos->nTrafficHandlingPriority;

    return ret;
}

UINT32 CFW_GprsGetReqQosUmts(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsGetReqQos()\r\n",0x08100785)));
        return ret;
    }
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQos->nDelay =  pQosList[nSimID][nCid - 1].nDelay;
    pQos->nMean =  pQosList[nSimID][nCid - 1].nMean;
    pQos->nPeak =  pQosList[nSimID][nCid - 1].nPeak;
    pQos->nPrecedence =  pQosList[nSimID][nCid - 1].nPrecedence;
    pQos->nReliability =  pQosList[nSimID][nCid - 1].nReliability;

    pQos->nTrafficClass = pQosList[nSimID][nCid - 1].nTrafficClass ;
    pQos->nMaximumbitrateUL=   pQosList[nSimID][nCid - 1].nMaximumbitrateUL;
    pQos->nMaximumbitrateDL=   pQosList[nSimID][nCid - 1].nMaximumbitrateDL;
    pQos->nGuaranteedbitrateUL=   pQosList[nSimID][nCid - 1].nGuaranteedbitrateUL;
    pQos->nGuaranteedbitrateDL=   pQosList[nSimID][nCid - 1].nGuaranteedbitrateDL;
    pQos->nDeliveryOrder=   pQosList[nSimID][nCid - 1].nDeliveryOrder;
    pQos->nMaximumSDUSize=   pQosList[nSimID][nCid - 1].nMaximumSDUSize;
    pQos->nSDUErrorRatio=   pQosList[nSimID][nCid - 1].nSDUErrorRatio;
    pQos->nResidualBER=   pQosList[nSimID][nCid - 1].nResidualBER;
    pQos->nDeliveryOfErroneousSDU=   pQosList[nSimID][nCid - 1].nDeliveryOfErroneousSDU;
    pQos->nTransferDelay=   pQosList[nSimID][nCid - 1].nTransferDelay;
    pQos->nTrafficHandlingPriority=   pQosList[nSimID][nCid - 1].nTrafficHandlingPriority;
    return ret;
}
#endif

#ifndef LTE_NBIOT_SUPPORT
UINT8 IPAddress[4];
UINT32 getIPAddress()
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("=====================",0x08100786)));
    CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, IPAddress, 10, 16);
    return (IPAddress[3]<<24|IPAddress[2]<<16|IPAddress[1]<<8|IPAddress[0]);
}
#endif

#ifndef LTE_NBIOT_SUPPORT
PRIVATE
#endif
UINT32 CFW_GprsAoProc (HAO hAo, CFW_EV *pEvent)
{
    if((hAo == 0) || (pEvent == NULL))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_GprsAoProc",0x08100787)));
    UINT32 nUTI = 0;
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    GPRS_SM_INFO* pUserData = CFW_GetAoUserData(hAo);                   // sm state
    UINT32 nAoState  = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nAoState = %d",0x08100788)),nAoState);

    if((UINT32)pEvent == (UINT32)(-1))                          // req to stack
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "EventId = 0xffffffff, nMsgId= %d", pUserData->nMsgId);
    else
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "EventId = %x, nMsgId= %d",pEvent->nEventId, pUserData->nMsgId);

    if (nAoState == CFW_SM_STATE_IDLE)
    {
        if((UINT32)pEvent == (UINT32)(-1))                          // req to stack
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Event comes from Application",0x08100789)));
            if (pUserData->nMsgId == API_PDP_CTXACTIVATE_REQ)           // activate  act = 1;
            {
                CFW_GPRS_PDPCONT_INFO sPdpCXT;
                memset(&sPdpCXT, 0, sizeof(CFW_GPRS_PDPCONT_INFO));
                api_PdpCtxActivateReq_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxActivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxActivateReq_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXACTIVATE_REQ\n",0x0810078a)));
                SUL_MemSet8(pOutMsg, 0, SIZEOF(api_PdpCtxActivateReq_t));
                sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_MAX_LEN);  // the max length of the APN is 100
                sPdpCXT.pPdpAddr = (UINT8 *)CSW_GPRS_MALLOC(THE_PDP_ADDR_MAX_LEN);  // the max length of the PDPADDR is 18
                sPdpCXT.pApnUser = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_USER_MAX_LEN); // add by wuys 2008-02-25
                sPdpCXT.pApnPwd = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_PWD_MAX_LEN); // add by wuys 2008-02-25
                if(CFW_GprsGetPdpCxt(pUserData->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS)
                {
                    //get the context from flash error using the default values

                    //UINT8 nApnNet[5] = {'C', 'M', 'N', 'E', 'T'};         // for internet access // / france websfr
                    //UINT8 nApnNet[6] = {'w', 'e', 'b', 's', 'f', 'r'};         // for internet access // / french websfr
#ifdef LTE_NBIOT_SUPPORT
                    UINT8 nApnWap[5] = {'C', 'T', 'N', 'B'};
#else
                    UINT8 nApnWap[5] = {'C', 'M', 'W', 'A', 'P'};         // for wap  access
#endif
					UINT8 *nApn = NULL;
                    nApn = nApnWap;

                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get PDP context error maybe flash error using the default settings\n",0x0810078b)));
                    sPdpCXT.nPdpType = 0;
                    //APN size is 5 for china and 6 for franch
                    //sPdpCXT.nApnSize = Gprs_APN_Len;
                    //SUL_MemCopy8(sPdpCXT.pApn, nApn, Gprs_APN_Len);
#ifdef LTE_NBIOT_SUPPORT
					sPdpCXT.nApnSize = 4;
					SUL_MemCopy8(sPdpCXT.pApn, nApn, 4);
#else
                    sPdpCXT.nApnSize = 5;
                    SUL_MemCopy8(sPdpCXT.pApn, nApn, 5);
#endif //LTE_NBIOT_SUPPORT

                    sPdpCXT.nPdpAddrSize = 0;
                    SUL_MemSet8(sPdpCXT.pPdpAddr, 0, 4);

                    sPdpCXT.nDComp  = 0;
                    sPdpCXT.nHComp = 0;
#ifdef LTE_NBIOT_SUPPORT
    			    sPdpCXT.PdnType = 1; // IPV4/V6/Non-IP
#endif
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("PDP context config, ApnUserSize: 0x%x; ApnPwdSize:  0x%x \n",0x0810078c)), sPdpCXT.nApnUserSize, sPdpCXT.nApnPwdSize);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, sPdpCXT.pApnUser, THE_APN_USER_MAX_LEN);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, sPdpCXT.pApnPwd, THE_APN_PWD_MAX_LEN);
                SUL_MemSet8(&pUserData->Qos, 0, SIZEOF(CFW_GPRS_QOS));
#ifdef LTE_NBIOT_SUPPORT
                CFW_GprsGetReqQos(pUserData->Cid2Index.nCid, &pUserData->Qos, nSimID);

                pOutMsg->NSapi = pUserData->Cid2Index.nNsapi;
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
		        pOutMsg->nCid = pUserData->Cid2Index.nCid;
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("req QOS: nReliability:%d, nDelay: %d",0x0810078f)), pUserData->Qos.nReliability, pUserData->Qos.nDelay);

                CFW_GprsQos2Api(pUserData->Qos, pOutMsg->QoS, &pOutMsg->QoSLen);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TRACE)|TDB|TNB_ARG(0), TSTXT(TSTR("req QOS to API: ",0x08100790)));
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg->QoS, 11);
#else
                CFW_GprsGetReqQos(pUserData->Cid2Index.nCid, &pUserData->Qos, nSimID);

                pOutMsg->NSapi = pUserData->Cid2Index.nNsapi;
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("req QOS: nReliability:%d, nDelay: %d",0x0810078f)), pUserData->Qos.nReliability, pUserData->Qos.nDelay);
                CFW_GprsQos2Api(pUserData->Qos, pOutMsg->QoS, &pOutMsg->QoSLen);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TRACE)|TDB|TNB_ARG(0), TSTXT(TSTR("req QOS to API: ",0x08100790)));
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg->QoS, 11);
#endif
                CFW_GprsPdpAddr2Api(&sPdpCXT, &pOutMsg->PDPAddLen, pOutMsg->PDPAddress);

                pOutMsg->AccessPointName[0] = sPdpCXT.nApnSize;
#ifdef LTE_NBIOT_SUPPORT
				pOutMsg->PdnType = sPdpCXT.nPdpType;
				pOutMsg->nSlpi = sPdpCXT.nSlpi;
				if(sPdpCXT.nApnSize == 0)
                {
                    pOutMsg->APNLen = 0 ;
                }
                else
                {
                    pOutMsg->APNLen = sPdpCXT.nApnSize + 1 ;
                }
#else
                pOutMsg->APNLen = sPdpCXT.nApnSize + 1 ;
#endif

                // modify by wuys for handle the APN containing ".", 2008-03-05
                //added by wuys 2008-03-05
                UINT8 i;
                UINT8 n = 0;
                for(i = 0; i < sPdpCXT.nApnSize; i++)
                {
                    if( sPdpCXT.pApn[i] == '.')
                    {
                        pOutMsg->AccessPointName[n] = i - n;
                        SUL_MemCopy8(pOutMsg->AccessPointName  + n + 1 , sPdpCXT.pApn + n, i - n);
                        n = i + 1;
                    }
                }
                if (n > 0)
                {
                    pOutMsg->AccessPointName[n] = i - n;
                    SUL_MemCopy8(pOutMsg->AccessPointName  + n + 1 , sPdpCXT.pApn + n, i - n);
                }
                else
                {
                    SUL_MemCopy8(pOutMsg->AccessPointName + 1 , sPdpCXT.pApn, sPdpCXT.nApnSize);
                }
#if 0 //removed by wuys 2008-03-05
                SUL_MemCopy8(pOutMsg->AccessPointName + 1 , sPdpCXT.pApn, sPdpCXT.nApnSize);
#endif
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("APN length: %d, APN: ",0x08100791)), pOutMsg->APNLen);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg->AccessPointName, sPdpCXT.nApnSize + 1 + 4);

                CSW_GPRS_FREE(sPdpCXT.pApn);
                CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
#ifdef LTE_NBIOT_SUPPORT
                pOutMsg->ProtocolCfgOptionLen = CFW_Encode_ePCO(pOutMsg->PdnType, pOutMsg->ProtocolCfgOption, &sPdpCXT);
#else
                // protocol option
                //modified by wuys for test APN anthentication, 2008-02-25
                pOutMsg->ProtocolCfgOption[0] = 0x80;
                pOutMsg->ProtocolCfgOptionLen = 1;
                if((sPdpCXT.nApnUserSize > 0) && (sPdpCXT.nApnPwdSize > 0))
                {
                    pOutMsg->ProtocolCfgOption[1] = 0xc0;
                    pOutMsg->ProtocolCfgOption[2] = 0x23;
                    pOutMsg->ProtocolCfgOption[3] = sPdpCXT.nApnUserSize + sPdpCXT.nApnPwdSize + 2 + 4;
                    pOutMsg->ProtocolCfgOption[4] = 0x01;
                    pOutMsg->ProtocolCfgOption[5] = 0x00;
                    pOutMsg->ProtocolCfgOption[6] = 0x00;
                    pOutMsg->ProtocolCfgOption[7] = sPdpCXT.nApnUserSize + sPdpCXT.nApnPwdSize + 2 + 4;
                    pOutMsg->ProtocolCfgOption[8] = sPdpCXT.nApnUserSize;
                    SUL_MemCopy8(pOutMsg->ProtocolCfgOption + 9 , sPdpCXT.pApnUser, sPdpCXT.nApnUserSize);
                    pOutMsg->ProtocolCfgOption[9 + sPdpCXT.nApnUserSize] = sPdpCXT.nApnPwdSize;
                    SUL_MemCopy8(pOutMsg->ProtocolCfgOption + 9 + sPdpCXT.nApnUserSize + 1 , sPdpCXT.pApnPwd, sPdpCXT.nApnPwdSize);
                    pOutMsg->ProtocolCfgOptionLen = sPdpCXT.nApnUserSize + sPdpCXT.nApnPwdSize + 4 + 4 + 2;
                }
                SUL_MemCopy8(pOutMsg->ProtocolCfgOption + pOutMsg->ProtocolCfgOptionLen , IPCP_OPTION, 19);
                pOutMsg->ProtocolCfgOptionLen += 19;
#endif
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pOutMsg->ProtocolCfgOptionLen: 0x%x  \n",0x08100792)), pOutMsg->ProtocolCfgOptionLen);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg->ProtocolCfgOption, 100);

                CSW_GPRS_FREE(sPdpCXT.pApnUser);
                CSW_GPRS_FREE(sPdpCXT.pApnPwd);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg, 100);

                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_CONNECTING);
            }
            else
            {
            }
        }
#ifdef LTE_NBIOT_SUPPORT
		else if (pEvent->nEventId == API_EPS_CTXACTIVATE_IND)
		{
			api_EpsCtxActivateInd_t  *psInMsg  = (api_EpsCtxActivateInd_t *)pEvent->nParam1;

			if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
			{
				CFW_TFT_PARSE TftParse;
				UINT8  ErrCode = 0;
				UINT8  Index = 0;
				UINT8  nCid = 0;
				
				/* TFT Check */
				pUserData->Tft.uLength = psInMsg->TFTLen;
				SUL_MemCopy8(pUserData->Tft.uTFT, psInMsg->TFT, psInMsg->TFTLen);

				/* ErrCode: 
				   0 : NoError
				   1 : identical packet filter precedence values found, the old packet filters do not belong to the default EPS bearer context
				   2 : identical packet filter precedence values found, the old packet filters belong to the default EPS bearer context
				   Other: Error Cause, send to network
				*/
				TftParse.Ebi		 = psInMsg->Ebi;
				TftParse.LinkEbi	 = psInMsg->LinkedEbi;
				TftParse.EbiDeactNum = 0;
				TftParse.pTftPf 	 = NULL;
				
				ErrCode = CFW_GprsTftCheckInAct(&pUserData->Tft, &TftParse, nSimID);

				if (E_CFW_TFT_ERR_SAME_PRIO_NOT_DEFAULT == ErrCode)
				{
					for (Index = 0; Index < TftParse.EbiDeactNum; Index++)
					{
						nCid = CFW_GprsGetCidFromNsapi(TftParse.EbiDeact[Index], nSimID);
						
						if ( nCid != 0xFF )
						{
							CFW_GprsSecPdpAct(CFW_GPRS_DEACTIVED, nCid, nCid, 0xFFFF, nSimID);
						}
					}

					/* Reset ErrCode */
					ErrCode = E_CFW_TFT_ERR_NO;
				}

				/*
				   Bugzilla �C Bug 1272,jira R8910-366
				   Save Packet Fileter, and in act proc also check 
				 */ 			   
				if(E_CFW_TFT_ERR_NO == ErrCode)
					ErrCode = CFW_GprsSavePFinActProc(&TftParse, nSimID);

				if(E_CFW_TFT_ERR_NO == ErrCode)
				{
					/* Save EPS QoS */
					CFW_GprsApi2EQoS(&pUserData->EQos, psInMsg->EpsQoS, psInMsg->EpsQoSLen);
					CFW_GprsSetCtxEQos(pUserData->Cid2Index.nCid, &pUserData->EQos, nSimID);
					
					pUserData->nActState = CFW_GPRS_ACTIVED;
					
					api_EpsCtxActivateRsp_t *pOutMsg = NULL;
					pOutMsg = (api_EpsCtxActivateRsp_t *)CFW_MemAllocMessageStructure(SIZEOF(api_EpsCtxActivateRsp_t));
					CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "send API_EPS_CTXACTIVATE_RSP\n");

					pUserData->nMsgId = API_EPS_CTXACTIVATE_RSP;
					pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
					
					// send the message to stack
					pUserData->pMsgBody = pOutMsg;
					CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
					CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);

					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_SEC_CTX_ACTIVE_IND, pUserData->Cid2Index.nCid, 0, 0 | (nSimID << 24), 0,pUserData->func);
				}
				else if (E_CFW_TFT_ERR_SAME_PRIO_DEFAULT == ErrCode)
				{					 
					/* Start to disconnection this PDN connection */
					nCid = CFW_GprsGetCidFromNsapi(pUserData->Cid2Index.nLinkedEbi, nSimID);
					
					if ( nCid != 0xFF )
					{
						CFW_GprsAct(CFW_GPRS_DEACTIVED, nCid, 0xFFFF, nSimID);
					}

					CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
					
					CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
				}
				else
				{
					api_EpsCtxActivateRej_t *pOutMsg = NULL;
					pOutMsg = (api_EpsCtxActivateRej_t *)CFW_MemAllocMessageStructure(SIZEOF(api_EpsCtxActivateRej_t));
					CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "send API_EPS_CTXACTIVATE_REJ\n");

					pUserData->nMsgId = API_EPS_CTXACTIVATE_REJ;
					pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
					pOutMsg->Cause	 = ErrCode;
					
					// send the message to stack
					pUserData->pMsgBody = pOutMsg;
					CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
					CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
					
					CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
				}
			}
			else
			{
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
			}
			CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_APNCR_IND, psInMsg->apnrcRate,( pUserData->Cid2Index.nCid << 8) | psInMsg->apnrcAer, nUTI | (nSimID << 24), psInMsg->apnrcUnit,pUserData->func);
		}
#endif

        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" should not Receive the pEvent->nParam1 = 0x%x\n",0x08100793)), pEvent->nParam1);
            CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
        }
    }
    else if(nAoState == CFW_GPRS_STATE_CONNECTING)
    {
        if ((UINT32)pEvent == (UINT32)(-1))
        {
            // in CFW_GPRS_STATE_CONNECTING send req to stack
            if (pUserData->nMsgId == API_PDP_CTXACTIVATE_REJ)
            {
                api_PdpCtxActivateRej_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxActivateRej_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxActivateRej_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send api_PdpCtxActivateRej_t\n",0x08100794)));

                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                pOutMsg->Cause = 0x00;
                pOutMsg->ProtocolCfgOptionLen = 0;
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
            }
            else if (pUserData->nMsgId == API_PDP_CTXDEACTIVATE_REQ) //deactivate  act = 0;
            {
                api_PdpCtxDeactivateReq_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxDeactivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxDeactivateReq_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXDEACTIVATE_REQ 1\n",0x08100795)));

                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                pOutMsg->Cause = 0x24;    // the cause value

                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_DISCONNECTING);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING\n Error :receive pUserData->nMsgId = 0x%x\n",0x08100796)), pUserData->nMsgId);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXACTIVATE_CNF) // actived
        {
            api_PdpCtxActivateCnf_t  *psInMsg  = (api_PdpCtxActivateCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNF data: \n",0x08100797)));
            SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pEvent->nParam1, sizeof(api_PdpCtxActivateCnf_t));

            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CFW_GPRS_PDPCONT_INFO sPdpCXT;

                memset(&sPdpCXT, 0, sizeof(CFW_GPRS_PDPCONT_INFO));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING\nreceive API_PDP_CTXACTIVATE_CNF smindex = 0x%x\n",0x08100798)), psInMsg->SmIndex);
                //CFW_Dump_SXS(CFW_GPRS_TRACE, (UINT8 *)psInMsg, SIZEOF(api_PdpCtxActivateCnf_t));
                pUserData->nActState = CFW_GPRS_ACTIVED;

                CFW_GprsApi2Qos(&pUserData->Qos, psInMsg->QoS, psInMsg->QoSLen);
                CFW_GprsSetReqQos(pUserData->Cid2Index.nCid, &pUserData->Qos, nSimID);
#ifdef LTE_NBIOT_SUPPORT
				CFW_GprsApi2EQoS(&pUserData->EQos, psInMsg->EpsQoS, psInMsg->EpsQoSLen);
				CFW_GprsSetCtxEQos(pUserData->Cid2Index.nCid, &pUserData->EQos, nSimID);

				CFW_GprsSetCidContext(pUserData->Cid2Index.nCid, pUserData->Cid2Index.nLinkedEbi, nSimID);
#endif

                sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_MAX_LEN);  // the max length of the APN is 100
                sPdpCXT.pPdpAddr = (UINT8 *)CSW_GPRS_MALLOC(THE_PDP_ADDR_MAX_LEN);  // the max length of the PDPADDR is 18
                sPdpCXT.pApnUser = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_USER_MAX_LEN); // add by wuys 2008-02-25
                sPdpCXT.pApnPwd = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_PWD_MAX_LEN); // add by wuys 2008-02-25
                memset(sPdpCXT.pApn, 0, THE_APN_MAX_LEN);
                memset(sPdpCXT.pPdpAddr, 0, THE_PDP_ADDR_MAX_LEN);
                memset(sPdpCXT.pApnUser, 0, THE_APN_USER_MAX_LEN);
                memset(sPdpCXT.pApnPwd, 0, THE_APN_PWD_MAX_LEN);

                if(sPdpCXT.pApn != NULL && sPdpCXT.pPdpAddr != NULL && sPdpCXT.pApnUser != NULL && sPdpCXT.pApnPwd != NULL)
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MALloc seccess! \n",0x08100799)));
                if(CFW_GprsGetPdpCxt(pUserData->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS)
                {
                    //get the values from flash error
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetPdpCxt error, maybe the flash error \n",0x0810079a)));
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNF IP address len: 0x%x,address: \n",0x0810079b)), psInMsg->PDPAddLen);
                CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, psInMsg->PDPAddress, psInMsg->PDPAddLen, 16);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("IP Address: %s",0x0810079c),inet_ntoa(*(in_addr*)(psInMsg->PDPAddress+2)));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNF ProtocolCfgOption len: 0x%x,address: \n",0x0810079d)), psInMsg->ProtocolCfgOptionLen);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, psInMsg->ProtocolCfgOption, psInMsg->ProtocolCfgOptionLen);
#ifndef LTE_NBIOT_SUPPORT
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("--------------",0x0810079e)));
                memcpy(IPAddress, psInMsg->PDPAddress+2,4);
                CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, IPAddress, 10, 16);
#endif
                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                if(psInMsg->PDPAddLen > 2) //add by wuys 2008-07-03, for avoiding error address length from stack
                {
                    CFW_GprsPdpApi2Addr(&sPdpCXT, psInMsg->PDPAddLen, psInMsg->PDPAddress);
                }
                else
                {
                    sPdpCXT.nPdpAddrSize = 0;
                    if(sPdpCXT.pPdpAddr != NULL)
                    {
                        CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
			            sPdpCXT.pPdpAddr = NULL;
                    }
                }

#ifndef LTE_NBIOT_SUPPORT
                //add by wuys 2009-10-22
                if(psInMsg->ProtocolCfgOptionLen > 0)
                {
                    UINT8 tmp_i = 0 ;
                    UINT8 tmp_j = 0;
                    UINT8 dnsSize = 0;
                    for(tmp_i = 0; tmp_i < (psInMsg->ProtocolCfgOptionLen - 1); tmp_i++)
                    {
                        if((psInMsg->ProtocolCfgOption[tmp_i] == 0x80) && (psInMsg->ProtocolCfgOption[tmp_i + 1] == 0x21))
                        {
                            for(tmp_j = 0; tmp_j < psInMsg->ProtocolCfgOption[tmp_i + 2]; tmp_j++)
                            {
                                if(psInMsg->ProtocolCfgOption[tmp_i + 3 + tmp_j] == 0x81)
                                {
                                    dnsSize = psInMsg->ProtocolCfgOption[tmp_i + 3 + tmp_j + 1] - 2;
                                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNFsPdpCXT.nDnsAddrSize: %d \n",0x0810079f)), dnsSize);

                                    if(dnsSize == 4)
                                    {
                                        UINT8* p = psInMsg->ProtocolCfgOption + tmp_i + 3 + tmp_j + 2;
                                        DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1][0] = (p[3] <<24) | (p[2] <<16) | (p[1] <<8) |p[0];
                                        p += 6;
                                        DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1][1] = (p[3] <<24) | (p[2] <<16) | (p[1] <<8) |p[0];
                                        CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1], dnsSize<<1, 16);
                                    }
                                    else
                                        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNFsPdpCXT.nDnsAddrSize: memory error \n",0x081007a0)));
                                    tmp_j = tmp_j + 2 + dnsSize - 1;
                                    break;
                                }
                            }
                            tmp_i += (tmp_j - 1 + 3);
                        }
                    }
                }
                //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_PDP_CTXACTIVATE_CNF cid = %d, dns server,address: \n",0x081007a1)),pUserData->Cid2Index.nCid);
                //SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, DnsServerIP[nSimID][pUserData->Cid2Index.nCid-1], 6);
                //SXS_DUMP(CFW_GPRS_TRACE, 0, sPdpCXT.pPdpAddr, sPdpCXT.nPdpAddrSize);
#else
				CFW_Decode_ePCO(psInMsg->PdnType, pUserData->Cid2Index.nCid, psInMsg->ProtocolCfgOptionLen, psInMsg->ProtocolCfgOption);
                sPdpCXT.PdnType = psInMsg->PdnType;
				sPdpCXT.nApnSize = psInMsg->APNLen;
                SUL_MemCopy8(sPdpCXT.pApn, psInMsg->AccessPointName, sPdpCXT.nApnSize);
#endif
                if(CFW_GprsSetPdpCxt(pUserData->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS)
                {
                    // save the value into flash error
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsSetPdpCxt error, maybe the flash error \n",0x081007a2)));
                    pUserData->nPDPAddrSize = psInMsg->PDPAddLen - 2;
                    SUL_MemCopy8(pUserData->nPDPAddr, psInMsg->PDPAddress + 2, psInMsg->PDPAddLen - 2);
                    CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, pUserData->nPDPAddr, pUserData->nPDPAddrSize);
                }
                //SXS_DUMP(CFW_GPRS_TRACE, 0, sPdpCXT.pPdpAddr, sPdpCXT.nPdpAddrSize);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("free    API_PDP_CTXACTIVATE_CNF : \n",0x081007a3)));
                if(sPdpCXT.pApn != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApn);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApn is NULL \n",0x081007a4)));

                if(sPdpCXT.pPdpAddr != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pPdpAddr is NULL \n",0x081007a5)));

                if(sPdpCXT.pApnUser != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApnUser);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApnUser is NULL \n",0x081007a6)));

                if(sPdpCXT.pApnPwd != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApnPwd);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApnPwd is NULL \n",0x081007a7)));

                pUserData->nFlowCtr = gprs_flow_ctrl_G[nSimID];
                if(pUserData->bAttManualAcc)  // MT manual acc
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("post EV_CFW_GPRS_CXT_ACTIVE_RSP to up layer , flow ctrl= %d\n",0x081007a8)), gprs_flow_ctrl_G[nSimID]);
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_ACTIVE_RSP, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), CFW_GPRS_ACTIVED,pUserData->func);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("post EV_CFW_GPRS_ACT_RSP to up layer active successr , flow ctrl= %d\n",0x081007a9)), gprs_flow_ctrl_G[nSimID]);
#ifdef LTE_NBIOT_SUPPORT
					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, sPdpCXT.PdnType, nUTI | (nSimID << 24), CFW_GPRS_ACTIVED,pUserData->func);
#else
					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), CFW_GPRS_ACTIVED,pUserData->func);
				
#endif

                }
#ifdef LTE_NBIOT_SUPPORT
				CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_APNCR_IND, psInMsg->apnrcRate,( pUserData->Cid2Index.nCid << 8) | psInMsg->apnrcAer, nUTI | (nSimID << 24), psInMsg->apnrcUnit,pUserData->func);
#endif

                CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);  // change the state
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXACTIVATE_REJ)
        {
            api_PdpCtxActivateRej_t  *psInMsg = (api_PdpCtxActivateRej_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING: receive API_PDP_CTXACTIVATE_REJ smindex = 0x%x, psInMsg->Cause = 0x%x\n",0x081007ab)), psInMsg->SmIndex, psInMsg->Cause);
                CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *)psInMsg, SIZEOF(api_PdpCtxActivateRej_t));

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                if(pUserData->bAttManualAcc)
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_ACTIVE_RSP, pUserData->Cid2Index.nCid, psInMsg->Cause,  nUTI | (nSimID << 24), 0xF1, pUserData->func);
                else
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, psInMsg->Cause,  nUTI | (nSimID << 24), 0xF1, pUserData->func);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);

                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
                pUserData->Cid2Index.nCid = 0xFF;
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_IND)     //add by wuys for fix bug16960, 2010-05-24
        {
            api_PdpCtxDeactivateInd_t   *psInMsg  = (api_PdpCtxDeactivateInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PDP_CTXDEACTIVATE_IND smindex = 0x%x, cause: 0x%x\n",0x081007ac)), psInMsg->SmIndex, psInMsg->Cause);

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                if(pUserData->bAttManualAcc)
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_ACTIVE_RSP, pUserData->Cid2Index.nCid, psInMsg->Cause,  nUTI | (nSimID << 24), 0xF1, pUserData->func);
                else
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, psInMsg->Cause,  nUTI | (nSimID << 24), 0xF1, pUserData->func);
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0, pUserData->func);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
                pUserData->Cid2Index.nCid = 0xFF;
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXSMINDEX_IND)
        {
            api_PdpCtxSmIndexInd_t *psInMsg = (api_PdpCtxSmIndexInd_t *)pEvent->nParam1;            
#ifdef LTE_NBIOT_SUPPORT
			if (pUserData->Cid2Index.nCid == psInMsg->nCid)
#else
            if (pUserData->Cid2Index.nNsapi == psInMsg->NSapi)
#endif
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING\nreceive API_PDP_CTXSMINDEX_IND = 0x%x, psInMsg->NSapi = 0x%x\n",0x081007ad)), psInMsg->SmIndex, psInMsg->NSapi);
                pUserData->Cid2Index.nSmIndex = psInMsg->SmIndex;
#ifdef LTE_NBIOT_SUPPORT
                pUserData->Cid2Index.nNsapi    = psInMsg->NSapi;
                pUserData->Cid2Index.nLinkedEbi= psInMsg->NSapi;
#endif
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING\n Error :receive eventid = 0x%x\n",0x081007ae)), pEvent->nEventId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    else if(nAoState == CFW_GPRS_STATE_DISCONNECTING)
    {
        if ((UINT32)pEvent == (UINT32)(-1))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_DISCONNECTING\n Error :receive pUserData->nMsgId = 0x%x\n",0x081007af)), pUserData->nMsgId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_CNF)   // deactived
        {
            api_PdpCtxDeactivateCnf_t *psInMsg = (api_PdpCtxDeactivateCnf_t *)pEvent->nParam1;
            if(pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_DISCONNECTING: receive API_PDP_CTXDEACTIVATE_CNF smindex = 0x%x\n",0x081007b0)), psInMsg->SmIndex);
#ifdef LTE_NBIOT_SUPPORT
				if ( PdpContList[nSimID][pUserData->Cid2Index.nCid - 1] != NULL )
#endif
				{
	                if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nPdpAddrSize > 0)
	                {
	                    CSW_GPRS_FREE(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pPdpAddr);
	                }
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nPdpAddrSize = 0;
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pPdpAddr = NULL;
	                if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnUserSize > 0)
	                {
	                    CSW_GPRS_FREE(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApnUser);
	                }
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnUserSize = 0;
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApnUser = NULL;

	                if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnPwdSize > 0)
	                {
	                    CSW_GPRS_FREE(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApnPwd);
	                }
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnPwdSize = 0;
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApnPwd = NULL;

	                if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnSize > 0)
	                {
	                    CSW_GPRS_FREE(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApn);
	                }
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->nApnSize = 0;
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->pApn = NULL;
	                CSW_GPRS_FREE(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]);
	                PdpContList[nSimID][pUserData->Cid2Index.nCid - 1] = NULL;
				}
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                //release nCid when the GPRS state is CFW_GPRS_DEACTIVED add by qidd@20090317
#if 0
                if(DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1])
                {
                    CSW_GPRS_FREE(DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1]);
                    DnsServerIP[nSimID][pUserData->Cid2Index.nCid - 1] = NULL;
                }
#endif
                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsAoProc DEACTIVED UTI: 0x%x \n",0x081007b1)) , nUTI);

#ifdef LTE_NBIOT_SUPPORT
				if(pUserData->bDeactByCfw == TRUE)
				{
					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
				}
				else
				{
					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), CFW_GPRS_DEACTIVED,pUserData->func); //modify by wuys 2008-06-04
				}
#else
#ifdef _QUICK_QUIT_WAP_
                if(0x01 == pUserData->nQQWDMO)
                {
                    pUserData->nQQWDMO = 0x00;
                }
                else
#endif
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), CFW_GPRS_DEACTIVED,pUserData->func); //modify by wuys 2008-06-04
#endif //LTE_NBIOT_SUPPORT
				CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
#if 0   // remove by wuys 2008-06-04         // send the de active msg to aps task
                {
                    COS_EVENT cos_ev;
                    cos_ev.nEventId = EV_INTER_APS_ACT_REQ;
                    cos_ev.nParam1 = pUserData->Cid2Index.nCid;
                    cos_ev.nParam2 = 0;
                    cos_ev.nParam3 = CFW_GPRS_DEACTIVED;
                    BAL_SendTaskEvent(&cos_ev, CSW_TASK_ID_APS);
                }
#endif
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_IND) // deactive ind event:
        {
            api_PdpCtxDeactivateInd_t   *psInMsg  = (api_PdpCtxDeactivateInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_DISCONNECTING: receive api_PdpCtxDeactivateInd_t smindex = 0x%x\n",0x081007b2)), psInMsg->SmIndex);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
#ifdef LTE_NBIOT_SUPPORT
				/* Delete Packet Filter */
				CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);
#endif

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                // CFW_StartDetachTimer(nSimID);
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
#if 0   // remove by wuys 2008-06-04           // send the de active msg to aps task
                {
                    COS_EVENT cos_ev;
                    cos_ev.nEventId = EV_INTER_APS_ACT_REQ;
                    cos_ev.nParam1 = pUserData->Cid2Index.nCid;
                    cos_ev.nParam2 = 0;
                    cos_ev.nParam3 = CFW_GPRS_DEACTIVED;
                    BAL_SendTaskEvent(&cos_ev, CSW_TASK_ID_APS);
                }
#endif
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_DISCONNECTING\n Error :receive eventid = 0x%x\n",0x081007b6)), pEvent->nEventId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    else if(nAoState == CFW_GPRS_STATE_ACTIVE)
    {
        if((UINT32)pEvent == (UINT32)(-1))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" in CFW_GPRS_STATE_ACTIVE send req to stack",0x081007b7)));
            if (pUserData->nMsgId == API_PDP_CTXDEACTIVATE_REQ) //deactivate  act = 0;
            {
                api_PdpCtxDeactivateReq_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxDeactivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxDeactivateReq_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXDEACTIVATE_REQ 2\n",0x081007b8)));
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                pOutMsg->Cause = 0x24;  // the cause value
#ifdef LTE_NBIOT_SUPPORT
				if( pUserData->bDeactByCfw == TRUE )
					COS_Sleep(10);
				
				pOutMsg->TFTLen = pUserData->Tfa.uLength;
				SUL_MemCopy8(pOutMsg->TFT, pUserData->Tfa.uTFT, pUserData->Tfa.uLength);
#endif

                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_DISCONNECTING);
            }
            else if (pUserData->nMsgId == API_PDP_CTXMODIFY_REQ)  // modify
            {
                api_PdpCtxModifyReq_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxModifyReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyReq_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXMODIFY_REQ\n",0x081007b9)));
                //Should be in active state or modify state, copy the data to the msg body
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
				

#ifdef LTE_NBIOT_SUPPORT
		        if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSimID))
				{
					/* Set EPS QOS */
					if(pUserData->ModifyEQos.nQci != 0xFF)
					{
						CFW_GprsEQos2Api(&pUserData->ModifyEQos, pOutMsg->EpsQoS, &pOutMsg->EpsQoSLen);
					}

					/* Set TFT */
					pOutMsg->TFTLen = pUserData->Tfa.uLength;
					SUL_MemCopy8(pOutMsg->TFT, pUserData->Tfa.uTFT, pOutMsg->TFTLen);
				}
				else
#endif
                {
                    CFW_GprsQos2Api(pUserData->ModifyQos, pOutMsg->QoS, &pOutMsg->QoSLen);
                }

                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_MODIFYING);
            }
            else if (pUserData->nMsgId == API_PS_QUEUEDATA_REQ)
            {
                // send event to stack
                // send the message to stack indicate there is one or more data in the queue .
            }
            else if (pUserData->nMsgId == API_PDP_CTXMODIFY_ACC)
            {
                api_PdpCtxModifyAcc_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxModifyAcc_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyAcc_t));

                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXMODIFY_ACC\n",0x081007ba)));
                // copy the data to the msg body
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
            }
#ifdef LTE_NBIOT_SUPPORT
			else if (pUserData->nMsgId == API_EPS_CTXALLOCATE_REQ)
			{
				CFW_TFT_SET *pTftSet; 
				api_EpsCtxAllocateReq_t *pOutMsg = NULL;
				
				pOutMsg = (api_EpsCtxAllocateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_EpsCtxAllocateReq_t));
				CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "send API_EPS_CTXALLOCATE_REQ\n");
				pOutMsg->SmIndex   = pUserData->Cid2Index.nSmIndex;
				pOutMsg->LinkedEbi = pUserData->Cid2Index.nNsapi;

				/* Set EPS QoS */
				CFW_GprsGetCtxEQos(pUserData->nSecActCid, &pUserData->EQos, nSimID);
				CFW_GprsEQos2Api(&pUserData->EQos, pOutMsg->EpsQoS, &pOutMsg->EpsQoSLen);
				
				/* Set TFT */
				CFW_GprsGetCtxTFT(pUserData->nSecActCid, &pTftSet, nSimID);
				CFW_GprsSetTfaForAlloc(&pUserData->Tfa, pTftSet);
				CFW_GprsClrCtxTFT(pUserData->nSecActCid, nSimID);
				
				pOutMsg->TFTLen = pUserData->Tfa.uLength;
				SUL_MemCopy8(pOutMsg->TFT, pUserData->Tfa.uTFT, pOutMsg->TFTLen);
				
				// send the message to stack
				pUserData->pMsgBody = pOutMsg;
				CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
				CFW_SetAoState(hAo, CFW_GPRS_STATE_EPS_ALLOCATING);
			}
#endif

            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE\n Error :receive pUserData->nMsgId = 0x%x\n",0x081007bb)), pUserData->nMsgId);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_IND)  // deactive ind event
        {
            api_PdpCtxDeactivateInd_t   *psInMsg  = (api_PdpCtxDeactivateInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PDP_CTXDEACTIVATE_IND smindex = 0x%x, cause: 0x%x\n",0x081007bc)), psInMsg->SmIndex, psInMsg->Cause);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
#ifdef LTE_NBIOT_SUPPORT
				/* Delete Packet Filter */
				CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);
#endif

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
#if 0   // remove by wuys 2008-06-04            // send the de active msg to aps task
                {
                    COS_EVENT cos_ev;
                    cos_ev.nEventId = EV_INTER_APS_ACT_REQ;
                    cos_ev.nParam1 = pUserData->Cid2Index.nCid;
                    cos_ev.nParam2 = 0;
                    cos_ev.nParam3 = CFW_GPRS_DEACTIVED;
                    BAL_SendTaskEvent(&cos_ev, CSW_TASK_ID_APS);
                }
#endif
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXMODIFY_IND)
        {
            api_PdpCtxModifyInd_t   *psInMsg  = (api_PdpCtxModifyInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PDP_CTXMODIFY_IND smindex = 0x%x\n",0x081007bd)), psInMsg->SmIndex);
                CFW_GetUTI(hAo, &nUTI);
                CFW_GPRS_PDPCONT_INFO sPdpCXT;
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_CONNECTING\nreceive API_PDP_CTXMODIFY_IND smindex = 0x%x\n",0x081007be)), psInMsg->SmIndex);

                CFW_GprsApi2Qos(&pUserData->Qos, psInMsg->QoS, psInMsg->QoSLen);
                CFW_GprsSetReqQos(pUserData->Cid2Index.nCid, &pUserData->Qos, nSimID);

                sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_MAX_LEN);       // the max length of the APN is 100
                sPdpCXT.pPdpAddr = (UINT8 *)CSW_GPRS_MALLOC(THE_PDP_ADDR_MAX_LEN);    // the max length of the PDPADDR is 18
                sPdpCXT.pApnUser = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_USER_MAX_LEN);    // add by wuys 2008-02-25
                sPdpCXT.pApnPwd = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_PWD_MAX_LEN);     // add by wuys 2008-02-25
                memset(sPdpCXT.pApn, 0, THE_APN_MAX_LEN);
                memset(sPdpCXT.pPdpAddr, 0, THE_PDP_ADDR_MAX_LEN);
                memset(sPdpCXT.pApnUser, 0, THE_APN_USER_MAX_LEN);
                memset(sPdpCXT.pApnPwd, 0, THE_APN_PWD_MAX_LEN);

                if(sPdpCXT.pApn != NULL && sPdpCXT.pPdpAddr != NULL && sPdpCXT.pApnUser != NULL && sPdpCXT.pApnPwd != NULL)
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MALloc seccess! \n",0x081007bf)));
                if(CFW_GprsGetPdpCxt(pUserData->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS)
                {
                    //get the values from flash error
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetPdpCxt error, maybe the flash error \n",0x081007c0)));
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("!!!!!API_PDP_CTXMODIFY_IND IP address len: 0x%x,address: \n",0x081007c1)), psInMsg->PDPAddLen);
                SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, psInMsg->PDPAddress, psInMsg->PDPAddLen);

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                if(psInMsg->PDPAddLen > 2) //add by wuys 2008-07-03, for avoiding error address length from stack
                {
                    CFW_GprsPdpApi2Addr(&sPdpCXT, psInMsg->PDPAddLen, psInMsg->PDPAddress);
                }

                //SXS_DUMP(CFW_GPRS_TRACE, 0, sPdpCXT.pPdpAddr, sPdpCXT.nPdpAddrSize);
                if(CFW_GprsSetPdpCxt(pUserData->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS)
                {
                    // save the value into flash error
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsSetPdpCxt error, maybe the flash error \n",0x081007c2)));
                    pUserData->nPDPAddrSize = psInMsg->PDPAddLen - 2;
                    SUL_MemCopy8(pUserData->nPDPAddr, psInMsg->PDPAddress + 2, psInMsg->PDPAddLen - 2);
                    CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, pUserData->nPDPAddr, pUserData->nPDPAddrSize);
                }
                //SXS_DUMP(CFW_GPRS_TRACE, 0, sPdpCXT.pPdpAddr, sPdpCXT.nPdpAddrSize);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("!!!!! free    API_PDP_CTXMODIFY_IND : \n",0x081007c3)));
                if(sPdpCXT.pApn != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApn);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApn is NULL \n",0x081007c4)));

                if(sPdpCXT.pPdpAddr != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pPdpAddr is NULL \n",0x081007c5)));

                if(sPdpCXT.pApnUser != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApnUser);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApnUser is NULL \n",0x081007c6)));

                if(sPdpCXT.pApnPwd != NULL)
                    CSW_GPRS_FREE(sPdpCXT.pApnPwd);
                else
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("sPdpCXT.pApnPwd is NULL \n",0x081007c7)));
#ifdef LTE_NBIOT_SUPPORT
			    if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSimID))
				{
					/* TFT Check */
					if ( psInMsg->TFTLen != 0)
					{
						CFW_TFT_PARSE TftParse;
						UINT8  ErrCode = 0;
						UINT8  Index = 0;
						UINT8  nCid = 0;
				
						pUserData->Tft.uLength = psInMsg->TFTLen;
						SUL_MemCopy8(pUserData->Tft.uTFT, psInMsg->TFT, psInMsg->TFTLen);

						/* ErrCode: 
						   0 : NoError
						   1 : identical packet filter precedence values found, the old packet filters do not belong to the default EPS bearer context
						   2 : identical packet filter precedence values found, the old packet filters belong to the default EPS bearer context
						   Other: Error Cause, send to network
						*/
						TftParse.Ebi		 = pUserData->Cid2Index.nNsapi;
						TftParse.LinkEbi	 = pUserData->Cid2Index.nLinkedEbi;
						TftParse.EbiDeactNum = 0;
						TftParse.pTftPf 	 = NULL;
							
						ErrCode = CFW_GprsTftCheckInMod(&pUserData->Tft, &TftParse, nSimID);

						if (E_CFW_TFT_ERR_SAME_PRIO_NOT_DEFAULT == ErrCode)
						{
							for (Index = 0; Index < TftParse.EbiDeactNum; Index ++)
							{
								nCid = CFW_GprsGetCidFromNsapi(TftParse.EbiDeact[Index], nSimID);
								
								if ( nCid != 0xFF )
								{
									CFW_GprsSecPdpAct(CFW_GPRS_DEACTIVED, nCid, nCid, 0xFFFF, nSimID);
								}
							}
		
							/* Reset ErrCode */
							ErrCode = E_CFW_TFT_ERR_NO;
						}
						
						/*Bugzilla �C Bug 1272,jira R8910-366
						  Save Packet Fileter, and in mod pro also check
						*/						  
						if (E_CFW_TFT_ERR_NO == ErrCode)
							ErrCode = CFW_GprsSavePFinModProc(&TftParse, nSimID);

						if (E_CFW_TFT_ERR_NO == ErrCode)
						{
							/* Save EPS QoS */
							if( psInMsg->EpsQoSLen > 0 )
							{
								CFW_GprsApi2EQoS(&pUserData->EQos, psInMsg->EpsQoS, psInMsg->EpsQoSLen);
								CFW_GprsSetCtxEQos(pUserData->Cid2Index.nCid, &pUserData->EQos, nSimID);
							}
							
							//CFW_GprsCtxModifyAcc(nUTI, pUserData->Cid2Index.nCid, nSimID);
							api_PdpCtxModifyAcc_t *pOutMsg = NULL;
							pOutMsg = (api_PdpCtxModifyAcc_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyAcc_t));
							
							pUserData->nMsgId = API_PDP_CTXMODIFY_ACC;
							pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
							
							// send the message to stack
							pUserData->pMsgBody = pOutMsg;
							CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);

						}
						else if (E_CFW_TFT_ERR_SAME_PRIO_DEFAULT == ErrCode)
						{
							/* Start to disconnection this PDN connection */
							nCid = CFW_GprsGetCidFromNsapi(pUserData->Cid2Index.nLinkedEbi, nSimID);
							
							if ( nCid != 0xFF )
							{
								CFW_GprsAct(CFW_GPRS_DEACTIVED, nCid, 0xFFFF, nSimID);
							}
						}
						else
						{
							api_PdpCtxModifyRej_t *pOutMsg = NULL;
							pOutMsg = (api_PdpCtxModifyRej_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyRej_t));
							CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "send API_PDP_CTXIMODIFY_REJ\n");
							
							// copy the data to the msg body
							pUserData->nMsgId = API_EPS_CTXMODIFY_REJ;
							pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
							pOutMsg->Cause = ErrCode; // reject cause
							// send the message to stack
							pUserData->pMsgBody = pOutMsg;
							CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
						}
						
						CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
					}
					else
					{
						/* Save EPS QoS */
						if( psInMsg->EpsQoSLen > 0 )
						{
							CFW_GprsApi2EQoS(&pUserData->EQos, psInMsg->EpsQoS, psInMsg->EpsQoSLen);
							CFW_GprsSetCtxEQos(pUserData->Cid2Index.nCid, &pUserData->EQos, nSimID);
						}
						
						CFW_GprsCtxModifyAcc(nUTI, pUserData->Cid2Index.nCid, nSimID);
					}
				    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_APNCR_IND, psInMsg->apnrcRate,( pUserData->Cid2Index.nCid << 8) | psInMsg->apnrcAer, nUTI | (nSimID << 24), psInMsg->apnrcUnit,pUserData->func);
				}
				else
#endif
                {
	                //When we received PDP modifying indication from NW, we should send accept message to NW.
	                //CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_MOD_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0, pUserData->func);
	                CFW_GprsCtxModifyAcc(nUTI, pUserData->Cid2Index.nCid, nSimID);
	                //Should not change the status for modifying pdp ctx when receive the IND msg from nw .Remove by lixp  20160103
	                //CFW_SetAoState(hAo, CFW_GPRS_STATE_MODIFYING);
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "warring!!ignore event Cid2Index.nSmIndex:%d, psInMsg->SmIndex=%d", pUserData->Cid2Index.nSmIndex, psInMsg->SmIndex);
                /* GPRS scheduling API_PDP_CTXMODIFY_IND
                 * if have muti active cid, every ao will be called.
                 * so if cid not match, ignore this event.
                 * */
                //CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PSDATA_IND)
        {
            //Receive data from stack, and post them to up layer
            api_PSDataInd_t *psInMsg = (api_PSDataInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nNsapi == psInMsg->Ctrl.NSAPI)
            {
#ifdef __USE_SMD__ //add for 3G test 2015-08-06
#ifdef CHIP_HAS_AP
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PSDATA_IND , send to AP,  cid: %d; Offset: 0x%x\n",0x081007cb)),pUserData->Cid2Index.nCid, psInMsg->Ctrl.SmdOffset);
                pscmd_SendDlPsDataInfo(pUserData->Cid2Index.nCid,psInMsg->Ctrl.SmdOffset,nSimID);
#else
                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                // gprs_sum[nSimID][1] += pData->nDataLength ;
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_DATA_IND, pUserData->Cid2Index.nCid, (UINT32)psInMsg,  nUTI | (nSimID << 24), 0,pUserData->func);           // comsumed the message
#endif
#else
                CFW_GPRS_DATA *pData = NULL;
                UINT16 nDataLen = 0;
                nDataLen = (UINT16)(psInMsg->DataLen - psInMsg->DataOffset);

                pData = (CFW_GPRS_DATA *)CSW_GPRS_MALLOC(nDataLen + OFFSETOF(CFW_GPRS_DATA, pData));
                if(pData == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x081007cc)));
                    return ERR_BUSY_DEVICE;
                }
                pData->nDataLength = nDataLen;
                SUL_MemCopy8(pData->pData, &(psInMsg->UserData[psInMsg->DataOffset]), psInMsg->DataLen - psInMsg->DataOffset);

                //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PSDATA_IND data from stack: psInMsg->DataLen = 0x%x, sInMsg->DataOffset = 0x%x\n",0x081007cd)), psInMsg->DataLen, psInMsg->DataOffset);
                //CFW_Dump_SXS(CFW_GPRS_TRACE, psInMsg->UserData,psInMsg->DataLen);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PSDATA_IND Nsapi = 0x%x nDataLen = 0x%x\n",0x081007ce)), psInMsg->Ctrl.NSAPI, nDataLen);
                if(pData->nDataLength > 100)
                    CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *) pData->pData, 100);
                else
                    CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *) pData->pData, pData->nDataLength);

                //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: data send to uplayer: p = %p\n",0x081007cf)), pData);
                //CFW_Dump_SXS(CFW_GPRS_TRACE, (UINT8 *) pData, (nDataLen + (OFFSETOF(CFW_GPRS_DATA, pData))));
                //CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *) pData->pData, pData->nDataLength);
#if 0
                Gprs_TcpIp(pData, pUserData->Cid2Index.nCid);
                CSW_GPRS_FREE((PVOID)pData);
#else
                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                gprs_sum[nSimID][1] += pData->nDataLength ;
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_DATA_IND, pUserData->Cid2Index.nCid, (UINT32)pData,  nUTI | (nSimID << 24), 0,pUserData->func);           // comsumed the message
#endif
#endif
                // comsumed the message, state not changed
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
#ifdef LTE_NBIOT_SUPPORT
        else if (pEvent->nEventId == API_PSDATA_IND_LTE)
        {
            //Receive data from stack, and post them to up layer
            api_PSDataIndLte_t *psInMsg = (api_PSDataIndLte_t *)pEvent->nParam1;
            T_ComSduLte*     pDlDataNode = psInMsg->pUserData; 
            T_ComSduLte*     pDlDataTail = NULL; 
            u8*                 pDlData = NULL;
/*added by zuohuaxu 20161115, begin*/
            UINT8             nType = 0;
/*added by zuohuaxu 20161115, end*/

            if (pUserData->Cid2Index.nNsapi == psInMsg->nsapi)
            {
                CFW_GPRS_DATA *pData = NULL;
                UINT16 nDataLen = 0;
/*added by zuohuaxu 20161115, begin*/
	         if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1] != NULL)
	         {
		      nType = PdpContList[nSimID][pUserData->Cid2Index.nCid - 1]->PdnType;
	         }
/*added by zuohuaxu 20161115, end*/
#if 0
             if(CfwTestLoopModeB[nSimID] == TRUE)
             {
                while(pDlDataNode!=NULL)
                {
                    pDlDataTail = pDlDataNode;
                    nDataLen = (UINT16)(pDlDataNode->sduLen);
                    pDlData = pDlDataNode->pSduData;

                    TC_SendGprsData(pUserData->Cid2Index.nCid, nSimID, nDataLen, pDlData);

                    // comsumed the message, state not changed
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    
                    pDlDataNode = pDlDataNode->pNext;
                    //Free buffer 
                    tgl00_170FreeDlIpData(pDlDataTail->pSduDataFree);
                    tgl00_168FreeDlNode(pDlDataTail);
                }

             }
#endif
                while(pDlDataNode!=NULL)
                {
                    pDlDataTail = pDlDataNode;
                    nDataLen = (UINT16)(pDlDataNode->sduLen);
                    pDlData = pDlDataNode->pSduData;
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(1), TSTR("dl data len:  %d\n",0x08100127), nDataLen);
                    SXS_DUMP(_MMI, 0, pDlData, nDataLen);
                    pData = (CFW_GPRS_DATA *)CSW_GPRS_MALLOC(nDataLen + OFFSETOF(CFW_GPRS_DATA, pData));
                    if(pData == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x081007cc)));
                        return ERR_BUSY_DEVICE;
                    }
                    pData->nDataLength = nDataLen;
                    SUL_MemCopy8(pData->pData, pDlData, nDataLen);

                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PSDATA_IND_LTE Nsapi = 0x%x nDataLen = 0x%x\n",0x081007ce)), psInMsg->nsapi, nDataLen);
                    if(pData->nDataLength > 100)
                        CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *) pData->pData, 100);
                    else
                        CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, (UINT8 *) pData->pData, pData->nDataLength);


                    //Add by lixp for UTI zero bug at 20090723.
                    CFW_GetUTI(hAo, &nUTI);
                    gprs_sum[nSimID][1] += pData->nDataLength ;
		            APP_STATISTIC_DL_DATA(pData->nDataLength);
					CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_DATA_IND, pUserData->Cid2Index.nCid, (UINT32)pData,  nUTI | (nSimID << 24), 0,NULL); 		  // comsumed the message
                    // comsumed the message, state not changed
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                    pDlDataNode = pDlDataNode->pNext;
                    //Free buffer 
                    tgl00_170FreeDlIpData(pDlDataTail->pSduDataFree);
                    tgl00_168FreeDlNode(pDlDataTail);
                    
                 }
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
#endif //LTE_NBIOT_SUPPORT
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: Error :receive eventid = 0x%x\n",0x081007d0)), pEvent->nEventId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    else if(nAoState == CFW_GPRS_STATE_MODIFYING)
    {
        //when AoState is CFW_GPRS_STATE_MODIFYING, Application send event to stack
        if((UINT32)pEvent == (UINT32)(-1))                          // req to stack
        {
            // Send deactive REQ to stack in CFW_GPRS_STATE_MODIFYING
            if (pUserData->nMsgId == API_PDP_CTXDEACTIVATE_REQ)             //deactivate  act = 0;
            {
                api_PdpCtxDeactivateReq_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxDeactivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxDeactivateReq_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXDEACTIVATE_REQ 3\n",0x081007d1)));
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                pOutMsg->Cause = 0x24;  // the cause value
#ifdef LTE_NBIOT_SUPPORT
				pOutMsg->TFTLen = pUserData->Tfa.uLength;
				SUL_MemCopy8(pOutMsg->TFT, pUserData->Tfa.uTFT, pUserData->Tfa.uLength);
#endif

                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_DISCONNECTING);
            }
            else if (pUserData->nMsgId == API_PDP_CTXMODIFY_ACC)
            {
                api_PdpCtxModifyAcc_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxModifyAcc_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyAcc_t));

                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXMODIFY_ACC\n",0x081007d2)));
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_MODIFYING);
            }
            else if (pUserData->nMsgId == API_PDP_CTXMODIFY_REJ)
            {
                api_PdpCtxModifyRej_t *pOutMsg = NULL;
                pOutMsg = (api_PdpCtxModifyRej_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxModifyRej_t));
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_PDP_CTXMODIFY_REJ\n",0x081007d3)));

                // copy the data to the msg body
                pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
                pOutMsg->Cause = 0x00; // reject cause
                // send the message to stack
                pUserData->pMsgBody = pOutMsg;
                CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_MODIFYING\n Error :receive pUserData->nMsgId = 0x%x\n",0x081007d4)), pUserData->nMsgId);
            }
        }
        else    if(pEvent->nEventId == API_PDP_CTXMODIFY_REJ)
        {
            api_PdpCtxModifyRej_t *psInMsg = (api_PdpCtxModifyRej_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_MODIFYING: receive API_PDP_CTXMODIFY_REJ smindex = 0x%x\n",0x081007d5)), psInMsg->SmIndex);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                // should post to the uplayer : modify error
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_MOD_RSP, pUserData->Cid2Index.nCid, ERR_CME_OPERATION_NOT_ALLOWED,  nUTI | (nSimID << 24), 0xF0, pUserData->func);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXMODIFY_CNF)
        {
            api_PdpCtxModifyCnf_t *psInMsg = (api_PdpCtxModifyCnf_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_MODIFYING: receive API_PDP_CTXMODIFY_CNF smindex = 0x%x\n",0x081007d6)), psInMsg->SmIndex);
                CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                // should post to the uplayer : modify ok
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_MOD_RSP, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0, pUserData->func);
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_IND)
        {
            api_PdpCtxDeactivateInd_t   *psInMsg  = (api_PdpCtxDeactivateInd_t *)pEvent->nParam1;
            if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_MODIFYING: receive API_PDP_CTXDEACTIVATE_IND smindex  0x%x, Cause %x\n",0x081007d7)), psInMsg->SmIndex, psInMsg->Cause);

                CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                    CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
#ifdef LTE_NBIOT_SUPPORT
				/* Delete Packet Filter */
				CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);
#endif

                //Add by lixp for UTI zero bug at 20090723.
                CFW_GetUTI(hAo, &nUTI);
                CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
                CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
#if 0   // remove by wuys 2008-06-04  // send the de active msg to aps task
                {
                    COS_EVENT cos_ev;
                    cos_ev.nEventId = EV_INTER_APS_ACT_REQ;
                    cos_ev.nParam1 = pUserData->Cid2Index.nCid;
                    cos_ev.nParam2 = 0;
                    cos_ev.nParam3 = CFW_GPRS_DEACTIVED;
                    BAL_SendTaskEvent(&cos_ev, CSW_TASK_ID_APS);
                }
#endif
            }
            else
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GPRS_STATE_MODIFYING\n Error :receive eventid = 0x%x\n",0x081007d8)), pEvent->nEventId);
        }
    }
#ifdef LTE_NBIOT_SUPPORT
	else if(nAoState == CFW_GPRS_STATE_EPS_ALLOCATING)
	{
		if ((UINT32)pEvent == (UINT32)(-1))
		{
			// in CFW_GPRS_STATE_EPS_ALLOCATING send req to stack
			if (pUserData->nMsgId == API_PDP_CTXDEACTIVATE_REQ) //deactivate  act = 0;
			{
				api_PdpCtxDeactivateReq_t *pOutMsg = NULL;
				pOutMsg = (api_PdpCtxDeactivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PdpCtxDeactivateReq_t));
				CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "send API_PDP_CTXDEACTIVATE_REQ\n");

				pOutMsg->SmIndex = pUserData->Cid2Index.nSmIndex;
				pOutMsg->Cause = 0x24;	  // the cause value

				// send the message to stack
				pUserData->pMsgBody = pOutMsg;
				CFW_SendSclMessage(pUserData->nMsgId, pUserData->pMsgBody, nSimID);
				CFW_SetAoState(hAo, CFW_GPRS_STATE_DISCONNECTING);
			}
			else
			{
				CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "CFW_GPRS_STATE_CONNECTING\n Error :receive pUserData->nMsgId = 0x%x\n", pUserData->nMsgId);
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
			}
		}
		else if (pEvent->nEventId == API_PDP_CTXDEACTIVATE_IND)
		{
			api_PdpCtxDeactivateInd_t	*psInMsg  = (api_PdpCtxDeactivateInd_t *)pEvent->nParam1;
			if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
			{
				CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(2), "CFW_GPRS_STATE_EPS_ALLOCATING: receive API_PDP_CTXDEACTIVATE_IND smindex	0x%x, Cause %x\n", psInMsg->SmIndex, psInMsg->Cause);

				/* Release Cid */
				if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
					CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);

				if(!IsCidSavedInApp(pUserData->nSecActCid, nSimID))
					CFW_ReleaseCID(pUserData->nSecActCid, nSimID);

				CFW_SetAoState(hAo, CFW_GPRS_STATE_IDLE);
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
				if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1] != NULL)//xuzuohua
				{
					CFW_GprsRemovePdpCxt(pUserData->Cid2Index.nCid, nSimID);
				}

				/* Delete Packet Filter */
				CFW_GprsDelPFInDeactProc(NULL, pUserData->Cid2Index.nNsapi, pUserData->Cid2Index.nLinkedEbi, nSimID);

				//Add by lixp for UTI zero bug at 20090723.
				CFW_GetUTI(hAo, &nUTI);
				CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->nSecActCid, 0,	nUTI | (nSimID << 24), 0xF1,pUserData->func);
				CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0,pUserData->func);
				CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
			}
			else
			{
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
			}
		}
		else if (pEvent->nEventId == API_EPS_CTXALLOCATE_CNF)
		{
			api_EpsCtxAllocateCnf_t *psInMsg = (api_EpsCtxAllocateCnf_t *)pEvent->nParam1;
			if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
			{
				//Add by lixp for UTI zero bug at 20090723.
				CFW_GetUTI(hAo, &nUTI);
				CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->nSecActCid, 0,	nUTI | (nSimID << 24), CFW_GPRS_ACTIVED,pUserData->func);

				CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

				pUserData->nSecActCid = 0xFF;
			}
			else
			{
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
			}
		}
		else if (pEvent->nEventId == API_EPS_CTXALLOCATE_REJ)
		{
			api_EpsCtxAllocateRej_t *psInMsg = (api_EpsCtxAllocateRej_t *)pEvent->nParam1;
			if (pUserData->Cid2Index.nSmIndex == psInMsg->SmIndex)
			{
				CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(2), "CFW_GPRS_STATE_EPS_ALLOCATING: receive API_PDP_CTXACTIVATE_REJ smindex = 0x%x, psInMsg->Cause = 0x%x\n", psInMsg->SmIndex, psInMsg->Cause);

				//Add by lixp for UTI zero bug at 20090723.
				CFW_GetUTI(hAo, &nUTI);
				CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, pUserData->nSecActCid, psInMsg->Cause,  nUTI | (nSimID << 24), 0xF1,pUserData->func);

				CFW_SetAoState(hAo, CFW_GPRS_STATE_ACTIVE);
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);

				if(!IsCidSavedInApp(pUserData->nSecActCid, nSimID))
					CFW_ReleaseCID(pUserData->nSecActCid, nSimID);

				pUserData->nSecActCid = 0xFF;
			}
			else
			{
				CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
			}
		}
		else
		{
			CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "CFW_GPRS_STATE_EPS_ALLOCATING\n Error :receive eventid = 0x%x\n", pEvent->nEventId);
		}
	}
#endif

    else // error state
    {
    }
    return ERR_SUCCESS;
}
//by frank for no tcpip
#if 0
//add by wuys for test gprs 2008-09-05
void Gprs_TcpIp(CFW_GPRS_DATA *pData , UINT32 cid);

void handle_TCP_data(VOID *nEvParam)
{
    CFW_GPRS_DATA *pData = NULL;
    UINT16 nDataLen = 0;
    api_PSDataInd_t *psInMsg = (api_PSDataInd_t *)nEvParam;


    nDataLen = (UINT16)(psInMsg->DataLen - psInMsg->DataOffset);
    pData = (CFW_GPRS_DATA *)CSW_GPRS_MALLOC(nDataLen + OFFSETOF(CFW_GPRS_DATA, pData));
    if(pData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x081007d9)));
        //COS_Sleep(20);//remove by wuys for gprs test 2008-9-03
        //return ERR_BUSY_DEVICE;
    }
    pData->nDataLength = nDataLen;
    SUL_MemCopy8(pData->pData, &(psInMsg->UserData[psInMsg->DataOffset]), psInMsg->DataLen - psInMsg->DataOffset);

    //dump the data for debug

    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GPRS_STATE_ACTIVE: receive API_PSDATA_IND data from stack: psInMsg->DataLen = 0x%x， psInMsg->DataOffset = 0x%x\n",0x081007da)), psInMsg->DataLen, psInMsg->DataOffset);
    //CFW_Dump_SXS(CFW_GPRS_TRACE, psInMsg->UserData,psInMsg->DataLen);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("handle_TCP_data: receive API_PSDATA_IND Nsapi = 0x%x nDataLen = 0x%x\n",0x081007db)), psInMsg->Ctrl.NSAPI, nDataLen);

    Gprs_TcpIp(pData, 1);
    CSW_GPRS_FREE((PVOID)pData);

}
#endif
//add end
// process the MT activation
// register a dynamic ao and post ind message to up layer
// Ind: EV_CFW_GPRS_CXT_ACTIVE_IND
// the parameter of the ind ??? should add more?
HAO GprsMTAoProc(CFW_EV *pEvent)
{
    UINT8  nCID = 0;
    HANDLE hSm;
    GPRS_SM_INFO *proc = NULL;
    api_PdpCtxActivateInd_t  *psInMsg = NULL;
    CFW_GPRS_PDPCONT_INFO sPdpCXT;

    CFW_SIM_ID nSimID = pEvent->nFlag;
    proc = (GPRS_SM_INFO *)CSW_GPRS_MALLOC(SIZEOF(GPRS_SM_INFO)); //  + 80
    if(proc == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8(proc, SIZEOF(GPRS_SM_INFO)); // +80
    psInMsg = (api_PdpCtxActivateInd_t *)pEvent->nParam1;
    //
    // get free cid from active ao modify by qidd @20090317
    if (ERR_SUCCESS != CFW_GetFreeCID(& nCID, nSimID))
    {
        CSW_GPRS_FREE( proc );
        return ERR_CFW_NOT_OPERATOR_ID;
    }
    proc->Cid2Index.nCid = nCID;
    proc->Cid2Index.nNsapi = proc->Cid2Index.nCid + 5;
    proc->Cid2Index.nSmIndex = psInMsg->SmIndex;

    // APN
    sPdpCXT.nApnSize = psInMsg->APNLen;
    sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(sPdpCXT.nApnSize);
    if(sPdpCXT.pApn == NULL)
    {
        CSW_GPRS_FREE( proc );
        return ERR_NO_MORE_MEMORY;
    }
    SUL_MemCopy8(sPdpCXT.pApn, psInMsg->AccessPointName, psInMsg->APNLen);

    // PDP Address
    sPdpCXT.pPdpAddr = (UINT8 *)CSW_GPRS_MALLOC(psInMsg->PDPAddLen - 2);
    if(sPdpCXT.pPdpAddr == NULL)
    {
        CSW_GPRS_FREE( proc );
        CSW_GPRS_FREE( sPdpCXT.pApn );
        return ERR_NO_MORE_MEMORY;
    }


    CFW_GprsPdpApi2Addr(&sPdpCXT, psInMsg->PDPAddLen, psInMsg->PDPAddress);
    if(CFW_GprsSetPdpCxt(proc->Cid2Index.nCid, &sPdpCXT, nSimID) != ERR_SUCCESS) // save the apn and pddress in flash
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_PDP_CTXACTIVATE_IND save the apn ,address in flash error \n",0x081007dc)));
    }
    CSW_GPRS_FREE(sPdpCXT.pApn); // free the apn pointer
    CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
    proc->nActState = CFW_GPRS_DEACTIVED;

    //set the nCid by qidd @20090317
    if (ERR_SUCCESS != CFW_SetCID(proc->Cid2Index.nCid, nSimID))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    hSm = CFW_RegisterAo(CFW_GPRS_SRV_ID, proc, CFW_GprsAoProc, nSimID);
    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CXT_ACTIVE_IND, proc->Cid2Index.nCid, 0, 0 | (nSimID << 24), 0,proc->func);
    CFW_SetAoState(hSm, CFW_GPRS_STATE_IDLE);
    //
    // post the ind message to up layer  ????  // include cid ,uti, and qos, ....
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("GprsMTAoProc: receive API_PDP_CTXACTIVATE_IND \n",0x081007dd)));
    return hSm;

}

//AT+CGPADDR
//get the address of the selected Pdp context of the gprs network.
//[in] nCid [1 - 7]
//[in/out] nLength:
// [in] nLength should be the length that malloc the pdpAdd.
// [out] nLength should be the length of the Pdp address.
//[out]pPdpAdd: Point to the buffer to hold the PDP address
// return: ERR_CFW_INVALID_PARAMETER, ERR_CFW_GPRS_INVALID_CID
// response: None
// remark:
UINT32 get_IPaddrOfNetifbyNum(UINT8 num);
UINT32 CFW_GprsGetPdpAddr(UINT8 nCid, UINT8 *nLength, UINT8 *pPdpAdd, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    CFW_GPRS_PDPCONT_INFO sPdpCXT;
    UINT32  ret = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsGetPdpAddr cid = 0x%x\n",0x081007de)), nCid);
    if((nLength == NULL) || (pPdpAdd == NULL))
        return ERR_CFW_INVALID_PARAMETER;
#ifdef WIFI_SUPPORT
    if(nCid == 0x11)  //0x11 is for wifi
    {
        struct netif *netif_tmp = getEtherNetIf(nCid);
        SUL_MemCopy8(pPdpAdd, (UINT8 *)&(netif_ip4_gw(netif_tmp)->addr), 4);
        return 0;
    }
#endif
    if ((nCid > 7) || (nCid < 1))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsGetPdpAddr()\r\n",0x081007df)));

        return nRet;
    }
    sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(100);  // the max length of the APN is 100
    sPdpCXT.pPdpAddr = (UINT8 *)CSW_GPRS_MALLOC(18);  // the max length of the PDPADDR is 18
    sPdpCXT.pApnUser = (UINT8 *)CSW_GPRS_MALLOC(20); // add by wuys 2008-02-25
    sPdpCXT.pApnPwd = (UINT8 *)CSW_GPRS_MALLOC(20); // add by wuys 2008-02-25

    if((sPdpCXT.pApn == NULL) || (sPdpCXT.pPdpAddr == NULL) || (sPdpCXT.pApnUser == NULL) || (sPdpCXT.pApnPwd == NULL))
    {
        if( NULL != sPdpCXT.pApn )
        {
            CSW_GPRS_FREE(sPdpCXT.pApn);
            sPdpCXT.pApn = NULL;
        }
        if ( NULL != sPdpCXT.pPdpAddr )
        {
            CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
            sPdpCXT.pPdpAddr = NULL;
        }
        if( NULL != sPdpCXT.pApnUser )
        {
            CSW_GPRS_FREE(sPdpCXT.pApnUser);
            sPdpCXT.pApnUser = NULL;
        }
        if( NULL != sPdpCXT.pApnPwd )
        {
            CSW_GPRS_FREE(sPdpCXT.pApnPwd);
            sPdpCXT.pApnPwd = NULL;
        }
        return ERR_NO_MORE_MEMORY;
    }

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            break;
        }
    }
    if(nAoIndex == nAoCount)
    {
        CSW_GPRS_FREE(sPdpCXT.pApn);
        CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
        CSW_GPRS_FREE(sPdpCXT.pApnUser);
        CSW_GPRS_FREE(sPdpCXT.pApnPwd);

        return ERR_CFW_INVALID_PARAMETER;
    }
    ret = CFW_GprsGetPdpCxt(proc->Cid2Index.nCid, &sPdpCXT, nSimID);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsGetPdpCxt = 0x%x, maybe the flash error \n",0x081007e0)), ret);
        *nLength = proc->nPDPAddrSize;
        SUL_MemCopy8(pPdpAdd, proc->nPDPAddr, *nLength);
        CFW_Dump_SXS(CFW_GPRS_TRACE, proc->nPDPAddr, proc->nPDPAddrSize);

    }
    else
    {
        *nLength = sPdpCXT.nPdpAddrSize;
        SUL_MemCopy8(pPdpAdd, sPdpCXT.pPdpAddr, *nLength);
    }

    CSW_GPRS_FREE(sPdpCXT.pApn);
    CSW_GPRS_FREE(sPdpCXT.pPdpAddr);
    CSW_GPRS_FREE(sPdpCXT.pApnUser);
    CSW_GPRS_FREE(sPdpCXT.pApnPwd);
    return ret;  // there is no pdp address according to the cid
}

// get activate state according to the cid
// [in] nCid [1 - 7]
// [out] pState:
// CFW_GPRS_DEACTIVED 0 deactivated
// CFW_GPRS_ACTIVED 1 activated
// return: ERR_CFW_INVALID_PARAMETER, ERR_CFW_GPRS_INVALID_CID
UINT32 CFW_GetGprsActState (UINT8 nCid, UINT8 *pState, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    //    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GetGprsActState cid = 0x%x\n",0x081007e1)), nCid);
    if ((pState == NULL) || (nCid > 7) || (nCid < 1))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GetGprsActState()\r\n",0x081007e2)));

        return nRet;
    }
    // get ao handle according to tha cid
    //
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            *pState = proc->nActState;
            return ERR_SUCCESS;
        }
    }
    *pState = CFW_GPRS_DEACTIVED;
    return ERR_SUCCESS;  // there is no pdp context according to the cid
}


// AT+CGACT
// PDP context activate or deactivate
// nState:
// CFW_GPRS_DEACTIVED 0 deactivated
// CFW_GPRS_ACTIVED 1 activated
// event:EV_CFW_GPRS_ACT_NOTIFY
// parameter1:cid[1 - 7]
// parameter2:0
// type: 0 or 1 to distinguish the active or deactive or 0xF0 to distinguish the rejection.

// return : ERR_CFW_GPRS_HAVE_ACTIVED
// ERR_CFW_UTI_IS_BUSY
// ERR_CFW_GPRS_INVALID_CID
//
UINT32 CFW_GprsAct(UINT8 nState, UINT8 nCid, UINT16 nUTI, CFW_SIM_ID nSimID) 
{
    return CFW_GprsActEX(nState,nCid,nUTI,nSimID,0,NULL);
}

UINT32 CFW_GprsActEX(UINT8 nState, UINT8 nCid, UINT16 nUTI, CFW_SIM_ID nSimID, BOOL SavedInApp, COS_CALLBACK_FUNC_T func)     // [in] nCid  // notify: success or fail
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsAct: GprsCidSavedInApp[0x%x]: 0x%x\r\n",0x081007e3)), nSimID, GprsCidSavedInApp[nSimID]);
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
//    UINT8 nAttstate = 0xFF;
    if(SavedInApp)
        GprsCidSavedInApp[nSimID] |= (0x01 << nCid);

    gprs_SetGprsSum(gprs_sum[nSimID][0], gprs_sum[nSimID][1], nSimID );
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_GprsAct cid = 0x%x, state = 0x%x, nUTI = 0x%x\n",0x081007e4)), nCid, nState, nUTI);
    if((nCid > 7) || (nCid < 1))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CID is INVALID\r\n",0x081007e5)));
        return ERR_CFW_INVALID_PARAMETER;
    }
    if ((nState != CFW_GPRS_DEACTIVED) && (nState != CFW_GPRS_ACTIVED))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !ACTIVED OR DEACTIVE STATUS error\r\n",0x081007e6)));
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! UTI IS BUSY\r\n",0x081007e7)));
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsAct()\r\n",0x081007e8)));

        return nRet;
    }
//    CFW_GetGprsAttState(&nAttstate, nSimID);

#if 0
    //add for test
    HAO s_hAo = 0;              // staitc ao
    NW_SM_INFO *s_proc = NULL;  // static sm
    //add end
    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);
    if((nAttstate == CFW_GPRS_DETACHED) && (
                (API_P_ILLEGAL_USR == s_proc->sGprsStatusInfo.nCause) ||
                (API_P_SRVC_NOT_ALLOWD == s_proc->sGprsStatusInfo.nCause)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !API_NW_NO_SVCE  == s_proc->sGprsStatusInfo.nStatus \n",0x081007e9)) );
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
    else  if(nAttstate == 0xFF)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !API_NW_NO_SVCE  == s_proc->sGprsStatusInfo.nStatus \n",0x081007ea)) );
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }

#else

#if 0

    if(nAttstate == CFW_GPRS_DETACHED)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! nAttstate == CFW_GPRS_DETACHED\r\n",0x081007eb)));
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
#endif
    //add ,do active need check PS status, do deactive need not //2009-11-02
    HAO s_hAo = 0;              // staitc ao
    NW_SM_INFO *s_proc = NULL;  // static sm

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);

    if( (nState == CFW_GPRS_ACTIVED) && (s_proc->sGprsStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! PS status is NO SERVICE\r\n",0x081007ec)));
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }

    //add end


#endif
    // get ao handle according to tha cid
    //
    CFW_SetServiceId(CFW_GPRS_SRV_ID);

    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if (proc->nActState == nState)
            {
                if(nState == CFW_GPRS_ACTIVED)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !HAVE ACTIVED\r\n",0x081007ed)));
                    return ERR_CFW_GPRS_HAVE_ACTIVED; // already actived or deactived should be replaced by macro
                }
                if(nState == CFW_GPRS_DEACTIVED)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !HAVE DEACTIVED\r\n",0x081007ee)));
                    return ERR_CFW_GPRS_HAVE_DEACTIVED; // should be ERR_CFW_GPRS_HAVE_DEACTIVED
                }
            }
            else
            {
                if (nState == CFW_GPRS_DEACTIVED)  // deactive
                {
                    UINT8  nActUTI = 0;
                    // send deactive message to stack
                    proc->nMsgId = API_PDP_CTXDEACTIVATE_REQ;
                    proc->pMsgBody = NULL;
#ifdef _QUICK_QUIT_WAP_
                    proc->nQQWDMO = 0x01;
                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ACT_RSP, nCid, 0, nUTI | (nSimID << 24), CFW_GPRS_DEACTIVED,proc->func);
#endif

                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("send deactive req, ao proc cont : 0x%x\n",0x081007ef)), nUTI);
#ifdef LTE_NBIOT_SUPPORT
					/* nUTI == 0xFFFF, This deactivate Request is start by CFW self */
					proc->bDeactByCfw = FALSE;
					if( nUTI == 0xFFFF )
					{
						CFW_GetFreeUTI(0, &nUTI);
						proc->bDeactByCfw = TRUE;
					}
					/* Delete Packet Filter */
					CFW_GprsDelPFInDeactProc(NULL, proc->Cid2Index.nNsapi, proc->Cid2Index.nLinkedEbi, nSimID);
#endif

                    CFW_GetUsingUTI(hAo, &nActUTI);
                    CFW_RelaseUsingUTI(0, nActUTI);
                    CFW_SetUTI(hAo, nUTI, 0);

                    nRet = CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_act  CFW_SetAoProcCode : 0x%x\n",0x081007f0)), nRet);

                    CSW_ASSERT(nRet == ERR_SUCCESS);

                    return ERR_SUCCESS;
                }
                else // should not be CFW_GPRS_ACTIVED ? maybe the MT exist.
                {
                    //remove by wuys 2010-04-29 for patching a QQ bug
                    /*
                      proc->nMsgId = API_PDP_CTXACTIVATE_REQ;
                      proc->pMsgBody = NULL;
                      proc->bAttManualAcc = TRUE;
                      proc->nFlowCtr = FLOW_RELEASED; //  2 uplink congestion or low memory disappear
                      //         proc->nFlowCtr = 1; //  1 uplink congestion or low memory disappear
                      CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("should not be CFW_GPRS_ACTIVED,  maybe the MT exist\n",0x081007f1)));
                      CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
                    */
                    return ERR_SUCCESS;
                }

            }
        }
    }
    // should be active here
    if (nState == CFW_GPRS_ACTIVED)   // attach
    {
        proc = (GPRS_SM_INFO *)CSW_GPRS_MALLOC(SIZEOF(GPRS_SM_INFO));

        if (proc == NULL)   // malloc error
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("malloc exception\n",0x081007f2)));
            return ERR_CME_MEMORY_FULL;  // should be updated by macro
        }

        // Clear the proc structure, ensure that there is not existing un-intialized member.
        SUL_ZeroMemory32(proc, SIZEOF(GPRS_SM_INFO));

        //get the context according to the ncid
        //should copy the context to msgbody
        //set nCid when the GPRS state is CFW_GPRS_ACTIVED add by qidd@20090317
        if (ERR_SUCCESS != CFW_SetCID(nCid, nSimID))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !SET CID INVALID\r\n",0x081007f3)));
            return ERR_CFW_INVALID_PARAMETER;
        }
        proc->func = func;
        proc->nMsgId = API_PDP_CTXACTIVATE_REQ;
        proc->pMsgBody = NULL;
        proc->Cid2Index.nCid = nCid;
        proc->Cid2Index.nNsapi = nCid + 4;
        proc->Cid2Index.nSmIndex = 0x0;
        proc->nActState = CFW_GPRS_DEACTIVED;  // ???  // shoud find the ao according to the cid???
        proc->bAttManualAcc = FALSE;
        proc->nFlowCtr = FLOW_RELEASED;    //   2 uplink congestion or low memory disappear
        //                proc->nFlowCtr = 1;    //   1 uplink congestion or low memory disappear

        proc->nFlowCtr =    gprs_flow_ctrl_G[nSimID] ;
        gprs_flow_ctrl[nSimID][proc->Cid2Index.nCid] = FLOW_RELEASED;

        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("register ao and continue CFW_SetUTI : 0x%x\n",0x081007f4)), nUTI);
        hAo = CFW_RegisterAo(CFW_GPRS_SRV_ID, proc, CFW_GprsAoProc, nSimID);
        CFW_SetUTI(hAo, nUTI, 0);
        nRet = CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_act  CFW_SetAoProcCode : 0x%x\n",0x081007f5)), nRet);

        CSW_ASSERT(nRet == ERR_SUCCESS);
        CFW_StopDetachTimer(nSimID);

    }
    else   // detach
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("warring! if(nState == CFW_GPRS_ACTIVED)\r\n",0x081007f6)));
        // error; should not be here
    }

    return ERR_SUCCESS;
}

//
// MT
// Manual acceptance of a network request for PDP context activation 'A'
// parameter1: 0
// parameter2: 0
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
// event: EV_CFW_GPRS_CXT_ACTIVE_IND_NOTIFY
// parameter1: cid [1 - 7]
// parameter2: 0
// UTI:uti
// type: 0 or 0xF0 to distinguish the confirm or reject the request.
UINT32 CFW_GprsManualAcc(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    UINT8 nAttstate = 0xFF;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsManualAcc cid = 0x%x, nUTI = 0x%x\n",0x081007f8)), nCid, nUTI);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        return ERR_CFW_UTI_IS_BUSY;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsManualAcc()\r\n",0x081007f9)));

        return nRet;
    }
    CFW_GetGprsAttState(&nAttstate, nSimID);
    if(nAttstate == CFW_GPRS_DETACHED)
    {
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
    CFW_SetServiceId(CFW_GPRS_SRV_ID);

    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);

    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if(proc->nActState == CFW_GPRS_ACTIVED)
            {
                return ERR_CFW_GPRS_HAVE_ACTIVED;
            }
            proc->nMsgId = API_PDP_CTXACTIVATE_REQ;
            proc->pMsgBody = NULL;

            proc->bAttManualAcc = TRUE;
            //SUL_MemCopy8(&proc->ModifyQos, Qos, SIZEOF(CFW_GPRS_QOS));

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;
}

// ATH
// Manual rejection of a network request for PDP context activation 'H'
// parameter1: None
// parameter2: None
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
// response: None
UINT32 CFW_GprsManualRej(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsManualRej cid = 0x%x, nUTI = 0x%x\n",0x081007fa)), nCid, nUTI);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        return ERR_CFW_UTI_IS_BUSY;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsManualRej()\r\n",0x081007fb)));

        return nRet;
    }
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if(proc->nActState == CFW_GPRS_ACTIVED)
            {
                return ERR_CFW_GPRS_HAVE_ACTIVED;
            }
            proc->nMsgId = API_PDP_CTXACTIVATE_REJ;
            proc->pMsgBody = NULL;

            //SUL_MemCopy8(&proc->ModifyQos, Qos, SIZEOF(CFW_GPRS_QOS));

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;   }

}

// modify needed
// [in]UTI
// [in]nCid
// [in]Qos: point the qos set the Qos
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
// ERR_CFW_INVALID_PARAMETER
// response: EV_CFW_GPRS_MOD_NOTIFY
// parameter1: Cid
// parameter1: 0
// UTI: uti
// type: 0
UINT32 CFW_GprsCtxModify(UINT16 nUTI, UINT8 nCid, CFW_GPRS_QOS *Qos, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;    //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    // add by wuys 2009-03-18, for fix the bug 11403
    UINT8 nActState = 0xFF;
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsCtxModify()\r\n",0x081007fc)));

        return nRet;
    }
    CFW_GetGprsActState(nCid, &nActState, nSimID);
    if(nActState != CFW_GPRS_ACTIVED)
        return ERR_CME_OPERATION_NOT_ALLOWED;
    //add end
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsCtxModify cid = 0x%x, nUTI = 0x%x\n",0x081007fd)), nCid, nUTI);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        return ERR_CFW_UTI_IS_BUSY;
    }
    if (Qos == NULL)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    if( nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsCtxModify()\r\n",0x081007fe)));
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            proc->nMsgId = API_PDP_CTXMODIFY_REQ;
            proc->pMsgBody = NULL;

            SUL_MemCopy8(&proc->ModifyQos, Qos, SIZEOF(CFW_GPRS_QOS));

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;
}


// no response
// [in]UTI
// [in]nCid [1 - 7]
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
UINT32 CFW_GprsCtxModifyAcc(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;    //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsCtxModifyAcc cid = 0x%x, nUTI = 0x%x\n",0x081007ff)), nCid, nUTI);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_IsFreeUTI ??\r\n",0x08100800)));
        //Let's send to NW.
        //  return ERR_CFW_UTI_IS_BUSY;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsCtxModifyAcc()\r\n",0x08100801)));

        return nRet;
    }
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GetAoHandle(nAoIndex\r\n",0x08100802)));
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("proc->nMsgId = API_PDP_CTXMODIFY_ACC\r\n",0x08100803)));
            proc->nMsgId = API_PDP_CTXMODIFY_ACC;
            proc->pMsgBody = NULL;

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;
}

// [in]UTI
// [in]nCid [1 - 7]
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
// response: none
UINT32 CFW_GprsCtxModifyRej(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsCtxModifyRej cid = 0x%x, nUTI = 0x%x\n",0x08100804)), nCid, nUTI);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if (CFW_IsFreeUTI(nUTI, CFW_GPRS_SRV_ID) != ERR_SUCCESS)
    {
        return ERR_CFW_UTI_IS_BUSY;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsCtxModifyRej()\r\n",0x08100805)));

        return nRet;
    }
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            proc->nMsgId = API_PDP_CTXMODIFY_REJ;
            proc->pMsgBody = NULL;
            //SUL_MemCopy8(&proc->ModifyQos, Qos, SIZEOF(CFW_GPRS_QOS));

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;
}


extern  UINT8 nQueueid[];
// nCid [1 - 7]
// send data   //UINT8 nCid, UINT16 nDataLength, UINT8 *pData
// event: None
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_INVALID_PARAMETER
extern BOOL VDS_CacheNodeIsLess(VOID);
struct store_packet
{
    UINT8 cid;
    UINT8 pad[3];
    CFW_SIM_ID simid;
    struct store_packet *next;
    CFW_GPRS_DATA *pGprsData;
};

struct store_packet *store_packet_head = NULL;
struct store_packet *store_packet_end = NULL;
UINT8 store_packet_cnt = 0;
UINT32 do_store_packet(UINT16 len, UINT8 *pData, UINT8 cid, CFW_SIM_ID simid)
{
    if(store_packet_cnt > 50)
        return 0xff;

    struct store_packet *new_packet = NULL;
    new_packet = CSW_Malloc(sizeof(struct store_packet));
    if(new_packet == NULL)
        return 0xff;

    new_packet->pGprsData = CSW_Malloc(sizeof(CFW_GPRS_DATA) + len);
    if(NULL != new_packet->pGprsData)
    {
        new_packet->next = NULL;
        new_packet->pGprsData->nDataLength = len;
        new_packet->cid = cid;
        new_packet->simid = simid;
        SUL_MemCopy8(new_packet->pGprsData->pData, pData, len);

        if(NULL == store_packet_end)
        {
            store_packet_head = new_packet;
            store_packet_end = new_packet;
        }
        else
        {
            store_packet_end->next = new_packet;
            store_packet_end = new_packet;
        }
    }
    else
    {
        CSW_Free(new_packet);
        return 0xff;
    }

    store_packet_cnt++;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("do_store_packet, store_packet_cnt: %d \r\n",0x08100806)), store_packet_cnt );
    return 0;
}
/*
#define FLOW_CTRL_HOID   1
#define FLOW_CTRL_RELEASE   0
#define FLOW_CTRL_INVOID   0XFF

UINT8 gprs_flow_ctrl[4][8];  //simid, cid
*/

UINT32 do_send_stored_packet(UINT8 nCID, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("do_send_stored_packet begin, store_packet_cnt: %d \r\n",0x08100807)), store_packet_cnt );

    struct store_packet *pLinked = store_packet_head;
    UINT32 ret = ERR_SUCCESS;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "========== nSimID = %d ==========",nSimID);
    while((pLinked != NULL) && (pLinked->simid == nSimID))
    {
        if(gprs_flow_ctrl_G[nSimID] == FLOW_CONTROL_ENABLED)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "========== Flow control enable again! ==========");
            store_packet_head = pLinked;
            break;
        }
#ifdef LTE_NBIOT_SUPPORT
        ret = CFW_GprsSendData(pLinked->cid, pLinked->pGprsData, 0,0,pLinked->simid);
#else
        ret = CFW_GprsSendData(pLinked->cid, pLinked->pGprsData, pLinked->simid);
#endif
        //Release memery
        CSW_Free(pLinked->pGprsData);
        CSW_Free(pLinked);

        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "========== pLinked->cid = %d, pLinked->simid = %d ==========", pLinked->cid, pLinked->simid);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "========== store_packet_cnt = %d ==========",store_packet_cnt);
        store_packet_cnt--;

        pLinked = pLinked->next;
        if(NULL == pLinked)
        {
            store_packet_head = NULL;
            store_packet_end = NULL;
//          gprs_flow_ctrl_G[nSimID] = FLOW_RELEASED;
            break;
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), "do_send_stored_packet end, remain %d packages",store_packet_cnt);
    return 0;
}

BOOL CFW_GprsSendDataAvaliable(UINT8 nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsSendDataAvaliable = %d",0x08100808)),  gprs_flow_ctrl_G[nSimID] == FLOW_RELEASED);
    return (gprs_flow_ctrl_G[nSimID] == FLOW_RELEASED);
}
#ifdef LTE_NBIOT_SUPPORT
extern UINT8 filterSwitch;
typedef struct IpSimpleFilterInfo_Tag
{
    UINT8 protocol;
    UINT16 transport_offset;
}IpSimpleFilterInfo;

// 0 ��ʾIP���治���ˣ���Ҫ�жϴ����
// 1��ʾIP���治���ˣ�����Ҫ�жϴ���㣬��������������
// 2��ʾIP������ˣ�����Ҫ�жϴ����

static UINT8 CFW_SimpleFilterIpv4Analysis(IpSimpleFilterInfo *ip_info, UINT8 *data, UINT16 len)
{
    UINT8 ret = 0;
    UINT8 localfilter = filterSwitch;
    ip_info->protocol = data[9];
    ip_info->transport_offset = (data[0] & 0x0F) << 2;
    if(ip_info->protocol == 1)   //icmp
    {
        if(localfilter & 0x08)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("expected ipv4 packet: ping packet or other icmp packet",0x0810080b)));
            ret = 1;
        }
	 else
	 {
	     ret = 2;
	 }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

// 0 ��ʾIP���治���ˣ���Ҫ�жϴ����
// 1��ʾIP���治���ˣ�����Ҫ�жϴ���㣬��������������
// 2��ʾIP������ˣ�����Ҫ�жϴ����

static UINT8 CFW_SimpleFilterIpv6Analysis(IpSimpleFilterInfo *ip_info, UINT8 *data, UINT16 len)
{
    UINT8 ret = 0;
    u32 v_pos = 0;
    u8  v_nextHead = 0;
    v_pos = 40;
    v_nextHead = data[6];
    while (v_nextHead != 0x3a)
    {
        if ((v_nextHead == 0) || (v_nextHead == 0x2b) || (v_nextHead == 0x3c))
        {
            v_nextHead = data[v_pos];
            v_pos += (data[v_pos + 1] + 1) << 3;
        }
        else if (v_nextHead == 0x2c)
        {
            v_nextHead = data[v_pos];
            v_pos += 8;
        }
        else if (v_nextHead == 0x33)
        {
            v_nextHead = data[v_pos];
            v_pos += data[v_pos + 1] << 2;
        }
        else if (v_nextHead == 0x29)
        {
            v_nextHead = data[v_pos + 6];
            v_pos += 40;
        }
        else//ESP OR other can't recongnised extend header
        {
            break;
        }
    }
    ip_info->protocol = v_nextHead;
    ip_info->transport_offset = v_pos;
    if(ip_info->protocol == 0x3a)  //icmpv6 need pass always
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("expected icmpv6 packet",0x0810080b)));
        ret = 1;
    }
    return ret;
}

// 0 ��ʾIP���治���ˣ���Ҫ�жϴ����
// 1��ʾIP���治���ˣ�����Ҫ�жϴ���㣬��������������
// 2��ʾIP������ˣ�����Ҫ�жϴ����

static UINT8 CFW_SimpleFilterIpAnalysis(IpSimpleFilterInfo *ip_info, UINT8 *data, UINT16 len)
{
    UINT8 ret = 0;
    if(data)
    {
        if((data[0] & 0xF0) == 0x40)
        {
            ret = CFW_SimpleFilterIpv4Analysis(ip_info, data, len);
        }
	 else if((data[0] & 0xF0) == 0x60)
	 {
	     ret = CFW_SimpleFilterIpv6Analysis(ip_info, data, len);
	 }
    }
    else
    {
        ret = 2;
    }
    return ret;
}

static BOOL CFW_SimpleFilterTransportLayerAnalysis(IpSimpleFilterInfo *ip_info, UINT8 *data, UINT16 len)
{
    BOOL ret = TRUE;
    UINT8 localfilter = filterSwitch;
    UINT16 idx = ip_info->transport_offset;
    UINT16 srcPort = 0;
    UINT16 dstPort = 0;
    UINT8 *tranport_ptr = &data[idx];
    if((ip_info->protocol == 17) || (ip_info->protocol == 6))
    {
        srcPort = (tranport_ptr[0] << 8) | tranport_ptr[1];
        dstPort = (tranport_ptr[2] << 8) | tranport_ptr[3];
        if(localfilter & 0x01)   //iperf 5001~5031
        {
            if(((dstPort >5000)&&(dstPort<5032)) ||((ip_info->protocol == 6)&&((srcPort >5000)&&(srcPort<5032))))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("expected iperf packet, srcPort/dstPort: %d/%d",0x0810080b)), srcPort, dstPort);
                ret = FALSE;
            }
        }
	 if(localfilter & 0x02)   //internet
	 {
	     if((dstPort == 53) || ((ip_info->protocol == 6)&&((dstPort == 80) || (dstPort == 8000) || (dstPort == 8080) || (dstPort == 443))))
	     {
	         CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("expected http or dns packet, dstPort: %d",0x0810080b)), dstPort);
	         ret = FALSE;
	     }
	 }
	 if(localfilter & 0x04)  //ftp
	 {
	     if((dstPort == 20) || (dstPort == 21) || (dstPort == 22) || (dstPort == 69))
	     {
	         CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("expected ftp packet, dstPort: %d",0x0810080b)), dstPort);
	         ret = FALSE;
	     }
	 }
	 #if 0
	 if(localfilter & 0x08)
	 {
	     if(dstPort == 53)  //dns maybe udp or tcp, mostly used udp payload
	     {
	         CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("expected dns packet for ping, dstPort: %d",0x0810080b)),dstPort);
	         ret = FALSE;
	     }
	 }
	 #endif
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("just pass for these packets, protocol: %d",0x0810080b)), ip_info->protocol);
        ret = FALSE;
    }
    return ret;
}

//AT+IPFLT���� ���˹���
//bit 1: iperf dest port : 5001~5031
//bit 2: internet http packets and dns packets
//bit 3: ftp
//bit 4: ping packets

//TRUE ���˴����а�
//FALSE ������

static BOOL CFW_IpSimpleFilterRegular(UINT8 *data, UINT16 len)
{
    BOOL  ret = TRUE;
    UINT8 ipresult = 0;
    IpSimpleFilterInfo ip_info = {0, 0};
	
    //cfw_nbiot_printData(0, len, data);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(1), TSTR("ul data len:  %d\n",0x08100127), len);
    SXS_DUMP(_MMI, 0, data, len);
    if(!filterSwitch)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("expected ip packet from app and no filter regular",0x0810080b)));
        return FALSE;
    }
    ipresult = CFW_SimpleFilterIpAnalysis(&ip_info, data, len);
    if(ipresult == 0)
    {
        ret = CFW_SimpleFilterTransportLayerAnalysis(&ip_info, data, len);
    }
    else if(ipresult == 1)
    {
        ret = FALSE;
    }
    else if(ipresult == 2)
    {
        ret = TRUE;
    }
    return ret;
}


UINT32 CFW_GprsSendData(UINT8 nCid, CFW_GPRS_DATA *pGprsData, UINT8 nasRai, UINT16 type_of_user_data,CFW_SIM_ID nSimID)
#else
UINT32 CFW_GprsSendData(UINT8 nCid, CFW_GPRS_DATA *pGprsData, CFW_SIM_ID nSimID)
#endif
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    UINT8 nActState = CFW_GPRS_DEACTIVED;
    UINT8 flow_ctrl_tmp = FLOW_RELEASED;
    //UINT8 nQueueid;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsSendData() begin, SimID:%d, cid: %d \r\n",0x08100809)), nSimID, nCid );
    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsSendData cid = 0x%x, \n",0x0810080a)), nCid);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    if ((pGprsData == NULL) || (pGprsData->pData == NULL))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
#ifdef LTE_NBIOT_SUPPORT
    APP_STATISTIC_UL_DATA(pGprsData->nDataLength);
#endif
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSendData()\r\n",0x0810080b)));

        return nRet;
    }
    CFW_GetGprsActState(nCid, &nActState, nSimID);
    if(nActState != CFW_GPRS_ACTIVED)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("The pdp is not actived\n",0x0810080c)));
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    if( nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSendData()\r\n",0x0810080d)));

        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsSendData: send data to stack:\n",0x0810080e)));

    if(pGprsData->nDataLength > 100)
        CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, pGprsData->pData, 100);
    else
        CFW_Dump_SXS(CFW_GPRS_TRACE_DATA, pGprsData->pData, pGprsData->nDataLength);
#ifdef LTE_NBIOT_SUPPORT
	if(CFW_IpSimpleFilterRegular(pGprsData->pData, pGprsData->nDataLength))
	{
		CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("unexpected packet, just free it",0x0810080b)));
		return ERR_SUCCESS;
	}
#endif
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nAoCount = 0x%x:\n",0x0810080f)), nAoCount);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if (proc->nActState == CFW_GPRS_ACTIVED)
            {
                flow_ctrl_tmp = gprs_flow_ctrl_G[nSimID];
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
                if((VDS_CacheNodeIsLess())&& (gprs_flow_ctrl_G[nSimID]  ==FLOW_RELEASED))
#else
                if(gprs_flow_ctrl_G[nSimID] == FLOW_RELEASED)
#endif
                {
                    gprs_sum[nSimID][0] += pGprsData->nDataLength;
#ifdef LTE_NBIOT_SUPPORT
                    if(CFW_GprsGetPsType(nSimID) == 4)
                    {
                        CFW_NbiotSendData(proc->Cid2Index.nNsapi,pGprsData->nDataLength, pGprsData->pData, \
							nasRai, type_of_user_data, nSimID, 1);
                    }
		      else
		      {
#endif  
                        CFW_SendSclData(proc->Cid2Index.nNsapi, nQueueid[nSimID], (pQosList[nSimID][nCid - 1].nReliability < 3)?API_PDP_ACK:API_PDP_UNACK , \
							pGprsData->nDataLength, pGprsData->pData, nSimID);
#ifdef LTE_NBIOT_SUPPORT
		      }
#endif
                }



                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsSendData: receive the FlowCtr = %d  from stack, discard the data req\n",0x08100810)), flow_ctrl_tmp);
                    do_store_packet(pGprsData->nDataLength, pGprsData->pData, nCid, nSimID);
                    //return ERR_CME_OPERATION_NOT_SUPPORTED; // flow control
                }
                return ERR_SUCCESS;
            }
            else
            {
                return ERR_CME_OPERATION_NOT_ALLOWED; // should not send the data in the deactived state
            }

        }

    }
    return ERR_CFW_GPRS_INVALID_CID; // the cid error ,there is no context according to the cid

}
#ifdef __USE_SMD__


//#define CMN_SET_SIM_ID(EventId, SimId)      ((EventId) | ((SimId) << 29))
UINT32 CFW_SendULPsDataInfo(
    UINT8 nNsapi,
    UINT8 nMode,
    UINT32 pDataOffset,
    CFW_SIM_ID nSimID
)
{
    // For internal use
    //#define sxr_PopIn(Data, Queue) sxr_QueueLast(Data, Queue)
    //#define sxr_PopOut(Queue)      sxr_QueueRmvFirst(Queue)

    //Msg_t           *psMsg     = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SendULPsDataInfo: send UL data info!",0x08100811)));

    UINT32 Signal[4] = {0, };

    Signal[0] = CMN_SET_SIM_ID(API_PSDATA_REQ, nSimID);
    Signal[1] = nNsapi|(nMode<<8);
    Signal[2] = pDataOffset;
    sxr_Send ((void *)Signal, API_MBX, SXR_SEND_EVT);

    return ERR_SUCCESS;
}
//#define CMN_SET_SIM_ID(EventId, SimId)      ((EventId) | ((SimId) << 29))
UINT32 CFW_SendULPsDataInfoEx(
    UINT8 nCid,
    UINT32 pDataOffset,
    CFW_SIM_ID nSimID
)
{
    // For internal use
    //#define sxr_PopIn(Data, Queue) sxr_QueueLast(Data, Queue)
    //#define sxr_PopOut(Queue)      sxr_QueueRmvFirst(Queue)

    //Msg_t           *psMsg     = NULL;

    UINT8 nMode;

    UINT32 Signal[4] = {0, };

    Signal[0] = CMN_SET_SIM_ID(API_PSDATA_REQ, nSimID);
    if(pQosList[nSimID][nCid - 1].nReliability < 3)
        nMode = API_PDP_ACK;
    else
        nMode = API_PDP_UNACK;

    Signal[1] = (nCid+4)|(nMode<<8);
    Signal[2] = pDataOffset;
    sxr_Send ((void *)Signal, API_MBX, SXR_SEND_EVT);

    return ERR_SUCCESS;
}

/*
UINT32 CFW_SendDLPsDataInfo(
    UINT8 Cid,
    UINT32 pDataOffset
)
{
        PSCMD_RECV_MSG_T  tmp ;
    tmp.cid = Cid;
    tmp.offset = pDataOffset;

    hal_ApCommSendPsCmd(&tmp, sizeof(PSCMD_RECV_MSG_T));
    //CFW_SendULPsDataInfo(tmp->cid+4; tmp->offset);

    return ERR_SUCCESS;
}
*/

UINT32 CFW_PsSendData(UINT8 nCid, UINT32 pDataOffset, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    UINT8 nActState = CFW_GPRS_DEACTIVED;
    UINT8 flow_ctrl_tmp = FLOW_RELEASED;
    //UINT8 nQueueid;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_GprsSendData() begin, SimID:%d; cid: %d ; Offset: 0x%x\r\n",0x08100812)), nSimID, nCid,pDataOffset );
    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsSendData cid = 0x%x, \n",0x08100813)), nCid);
    if((nCid > 7) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSendData()\r\n",0x08100814)));

        return nRet;
    }
    CFW_GetGprsActState(nCid, &nActState, nSimID);
    if(nActState != CFW_GPRS_ACTIVED)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("The pdp is not actived\n",0x08100815)));
        /* release uplink memory in ring buffer */
        smd_free_ul_ipdata_buf(pDataOffset);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    if( nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSendData()\r\n",0x08100816)));

        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsSendData: send data to stack:\n",0x08100817)));

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nAoCount = 0x%x:\n",0x08100818)), nAoCount);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if (proc->nActState == CFW_GPRS_ACTIVED)
            {
                flow_ctrl_tmp = gprs_flow_ctrl_G[nSimID];
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
                if((VDS_CacheNodeIsLess())&& (gprs_flow_ctrl_G[nSimID]  ==FLOW_RELEASED))
#else
                if(gprs_flow_ctrl_G[nSimID]  ==FLOW_RELEASED)
#endif
                {

                    if(pQosList[nSimID][nCid - 1].nReliability < 3)
                    {
                        //   CFW_SendSclData(proc->Cid2Index.nNsapi, nQueueid[nSimID], API_PDP_ACK , pGprsData->nDataLength, pGprsData->pData, nSimID);
                        CFW_SendULPsDataInfo(proc->Cid2Index.nNsapi,  API_PDP_ACK,pDataOffset,nSimID);
                    }
                    else
                    {
                        // CFW_SendSclData(proc->Cid2Index.nNsapi, nQueueid[nSimID], API_PDP_UNACK , pGprsData->nDataLength, pGprsData->pData, nSimID);
                        CFW_SendULPsDataInfo(proc->Cid2Index.nNsapi, API_PDP_UNACK, pDataOffset,nSimID);
                    }
                    //    CFW_SendULPsDataInfo(tmp->cid+4; tmp->offset, tmp->simId);

                    //CFW_SendULPsDataInfo(proc->Cid2Index.nNsapi,  pDataOffset,nSimID);
                }



                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsSendData: receive the FlowCtr = %d  from stack, discard the data req\n",0x08100819)), flow_ctrl_tmp);
                    //return ERR_CME_OPERATION_NOT_SUPPORTED; // flow control
                }
                return ERR_SUCCESS;
            }
            else
            {
                return ERR_CME_OPERATION_NOT_ALLOWED; // should not send the data in the deactived state
            }

        }

    }
    return ERR_CFW_GPRS_INVALID_CID; // the cid error ,there is no context according to the cid

}

#endif
//=============================================================================================
//synchronization function
//AT+CGDCONT
//[in]nCid[1 - 7], [in]pPdpCont
//
//=============================================================================================
UINT32 CFW_GprsSetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_GprsSetPdpCxt, cid: 0x%x, pPdpCont: 0x%x, simid: 0x%x\n",0x0810081a)), nCid, pPdpCont, nSimID);

    //#if TMP_GPRS_CONTEXT
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > 7) || (nCid < 1) || (pPdpCont == NULL) )
        return ERR_CFW_INVALID_PARAMETER;

#ifndef LTE_NBIOT_SUPPORT
    if((pPdpCont->nPdpType > CFW_GPRS_PDP_TYPE_PPP)
#else
    if((pPdpCont->nPdpType > CFW_GPRS_PDP_TYPE_NONIP)
#endif
            || (pPdpCont->nDComp > CFW_GPRS_PDP_D_COMP_V44)
            || (pPdpCont->nHComp > CFW_GPRS_PDP_H_COMP_RFC3095))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSetPdpCxt()\r\n",0x0810081b)));

        return nRet;
    }
    CFW_GPRS_PDPCONT_INFO *new_pdp = CSW_GPRS_MALLOC(sizeof(CFW_GPRS_PDPCONT_INFO));
    memset(new_pdp, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO));
    new_pdp->nApnSize = pPdpCont->nApnSize;
    if(new_pdp->nApnSize > 0)
    {
        if(new_pdp->nApnSize >THE_APN_MAX_LEN)
        {
            if(new_pdp != NULL)
                CSW_GPRS_FREE(new_pdp);
            return ERR_CFG_PARAM_OUT_RANGR;
        }
        new_pdp->pApn = CSW_GPRS_MALLOC(pPdpCont->nApnSize);
        memcpy(new_pdp->pApn, pPdpCont->pApn, pPdpCont->nApnSize);
    }
    new_pdp->nApnPwdSize = pPdpCont->nApnPwdSize;
    if(new_pdp->nApnPwdSize > 0)
    {
        if(new_pdp->nApnPwdSize >THE_APN_PWD_MAX_LEN)
        {
            if(new_pdp->pApn != NULL)
                CSW_GPRS_FREE(new_pdp->pApn);
            if(new_pdp != NULL)
                CSW_GPRS_FREE(new_pdp);
            return ERR_CFG_PARAM_OUT_RANGR;
        }
        new_pdp->pApnPwd = CSW_GPRS_MALLOC(pPdpCont->nApnPwdSize);
        memcpy(new_pdp->pApnPwd, pPdpCont->pApnPwd, pPdpCont->nApnPwdSize);
    }
    new_pdp->nApnUserSize = pPdpCont->nApnUserSize;
    if(new_pdp->nApnUserSize > 0)
    {
        if(new_pdp->nApnUserSize > THE_APN_USER_MAX_LEN)
        {
            if(new_pdp->pApn != NULL)
                CSW_GPRS_FREE(new_pdp->pApn);
            if(new_pdp->pApnPwd != NULL)
                CSW_GPRS_FREE(new_pdp->pApnPwd);
            if(new_pdp != NULL)
                CSW_GPRS_FREE(new_pdp);
            return ERR_CFG_PARAM_OUT_RANGR;
        }
        new_pdp->pApnUser = CSW_GPRS_MALLOC(pPdpCont->nApnUserSize);
        memcpy(new_pdp->pApnUser, pPdpCont->pApnUser, pPdpCont->nApnUserSize);
    }
    new_pdp->nPdpAddrSize = pPdpCont->nPdpAddrSize;

    if(new_pdp->nPdpAddrSize > 0)
    {
        if(new_pdp->nApnUserSize > THE_PDP_ADDR_MAX_LEN)
        {
            if(new_pdp->pApn != NULL)
                CSW_GPRS_FREE(new_pdp->pApn);
            if(new_pdp->pApnPwd != NULL)
                CSW_GPRS_FREE(new_pdp->pApnPwd);
            if(new_pdp->pApnUser != NULL)
                CSW_GPRS_FREE(new_pdp->pApnUser);
            if(new_pdp != NULL)
                CSW_GPRS_FREE(new_pdp);
            return ERR_CFG_PARAM_OUT_RANGR;
        }
        new_pdp->pPdpAddr = CSW_GPRS_MALLOC(THE_PDP_ADDR_MAX_LEN);
        memcpy(new_pdp->pPdpAddr, pPdpCont->pPdpAddr, pPdpCont->nPdpAddrSize);
    }
    else
    {
        new_pdp->pPdpAddr = CSW_GPRS_MALLOC(THE_PDP_ADDR_MAX_LEN);
        memset(new_pdp->pPdpAddr, 0, THE_PDP_ADDR_MAX_LEN);
    }

    new_pdp->nDComp = pPdpCont->nDComp;
    new_pdp->nHComp = pPdpCont->nDComp;
    new_pdp->nPdpType = pPdpCont->nPdpType;
#ifdef LTE_NBIOT_SUPPORT
    new_pdp->PdnType = pPdpCont->PdnType;
    new_pdp->nSlpi = pPdpCont->nSlpi;
#endif
    if(PdpContList[nSimID][nCid - 1] != NULL)
    {
        CFW_GprsRemovePdpCxt(nCid, nSimID);
    }
    PdpContList[nSimID][nCid - 1] = new_pdp;
    UINT8 n = 0;
    for(; n < 7; n++)
    {
        if(PdpContList[nSimID][n] != NULL)
        {
            if( PdpContList[nSimID][n]->nApnPwdSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnPwd != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd:\n",0x0810081c)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnPwd, PdpContList[nSimID][n]->nApnPwdSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd: NULL\n",0x0810081d)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnSize > 0)
            {
                if(PdpContList[nSimID][n]->pApn != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn:\n",0x0810081e)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApn, PdpContList[nSimID][n]->nApnSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn: NULL\n",0x0810081f)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnUserSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnUser != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser:\n",0x08100820)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnUser, PdpContList[nSimID][n]->nApnUserSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser: NULL\n",0x08100821)), n + 1);
                }
            }
            if( PdpContList[nSimID][n]->nPdpAddrSize > 0)
            {
                if(PdpContList[nSimID][n]->pPdpAddr != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr:\n",0x08100822)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pPdpAddr, PdpContList[nSimID][n]->nPdpAddrSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr: NULL\n",0x08100823)), n + 1);
                }
            }

        }
    }
    return ret;

}

//=============================================================================================
//[in]nCid[1 - 7], [out]pPdpCont
//=============================================================================================
UINT32 CFW_GprsGetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetPdpCxt\n",0x08100824)));

    UINT8 n = 0;
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsGetPdpCxt()\r\n",0x08100825)));

        return nRet;
    }

    for(; n < 7; n++)
    {
        if(PdpContList[nSimID][n] != NULL)
        {
            if( PdpContList[nSimID][n]->nApnPwdSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnPwd != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd:\n",0x08100826)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnPwd, PdpContList[nSimID][n]->nApnPwdSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd: NULL\n",0x08100827)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnSize > 0)
            {
                if(PdpContList[nSimID][n]->pApn != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn:\n",0x08100828)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApn, PdpContList[nSimID][n]->nApnSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn: NULL\n",0x08100829)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnUserSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnUser != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser:\n",0x0810082a)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnUser, PdpContList[nSimID][n]->nApnUserSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser: NULL\n",0x0810082b)), n + 1);
                }
            }
            if( PdpContList[nSimID][n]->nPdpAddrSize > 0)
            {
                if(PdpContList[nSimID][n]->pPdpAddr != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr:\n",0x0810082c)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pPdpAddr, PdpContList[nSimID][n]->nPdpAddrSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr: NULL\n",0x0810082d)), n + 1);
                }
            }

        }
    }
    //add end
    // #if TMP_GPRS_CONTEXT
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > 7) || (nCid < 1) || (pPdpCont == NULL) )
        return ERR_CFW_INVALID_PARAMETER;
    if(NULL == PdpContList[nSimID][nCid - 1])
        return ERR_NOT_SUPPORT;
    pPdpCont->nApnSize =  PdpContList[nSimID][nCid - 1]->nApnSize;
    if(pPdpCont->nApnSize > 0)
        memcpy(pPdpCont->pApn,  PdpContList[nSimID][nCid - 1]->pApn, pPdpCont->nApnSize);

    pPdpCont->nApnUserSize =  PdpContList[nSimID][nCid - 1]->nApnUserSize;
    if(pPdpCont->nApnUserSize > 0)
        memcpy(pPdpCont->pApnUser,  PdpContList[nSimID][nCid - 1]->pApnUser, pPdpCont->nApnUserSize);

    pPdpCont->nApnPwdSize =  PdpContList[nSimID][nCid - 1]->nApnPwdSize;
    if(pPdpCont->pApnPwd > 0)
        memcpy(pPdpCont->pApnPwd,  PdpContList[nSimID][nCid - 1]->pApnPwd, pPdpCont->nApnPwdSize);

    pPdpCont->nPdpAddrSize =  PdpContList[nSimID][nCid - 1]->nPdpAddrSize;
    if( PdpContList[nSimID][nCid - 1]->nPdpAddrSize > 0)
    {
        memcpy(pPdpCont->pPdpAddr,  PdpContList[nSimID][nCid - 1]->pPdpAddr, pPdpCont->nPdpAddrSize);
    }
    else
    {
        memset(pPdpCont->pPdpAddr, 0, 18);
    }

    pPdpCont->nDComp =  PdpContList[nSimID][nCid - 1]->nDComp;
    pPdpCont->nHComp =  PdpContList[nSimID][nCid - 1]->nHComp;
    pPdpCont->nPdpType =  PdpContList[nSimID][nCid - 1]->nPdpType;

#ifdef LTE_NBIOT_SUPPORT
    pPdpCont->PdnType = PdpContList[nSimID][nCid - 1]->PdnType;
#endif
    return ret;

}

UINT32 CFW_GprsRemovePdpCxt(UINT8 nCid, CFW_SIM_ID nSimID)
{
    UINT8 n = 0;
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsRemovePdpCxt()\r\n",0x0810082e)));

        return nRet;
    }

    for(; n < 7; n++)
    {
        if(PdpContList[nSimID][n] != NULL)
        {
            if( PdpContList[nSimID][n]->nApnPwdSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnPwd != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd:\n",0x0810082f)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnPwd, PdpContList[nSimID][n]->nApnPwdSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnPwd: NULL\n",0x08100830)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnSize > 0)
            {
                if(PdpContList[nSimID][n]->pApn != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn:\n",0x08100831)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApn, PdpContList[nSimID][n]->nApnSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApn: NULL\n",0x08100832)), n + 1);
                }
            }

            if( PdpContList[nSimID][n]->nApnUserSize > 0)
            {
                if(PdpContList[nSimID][n]->pApnUser != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser:\n",0x08100833)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pApnUser, PdpContList[nSimID][n]->nApnUserSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pApnUser: NULL\n",0x08100834)), n + 1);
                }
            }
            if( PdpContList[nSimID][n]->nPdpAddrSize > 0)
            {
                if(PdpContList[nSimID][n]->pPdpAddr != NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr:\n",0x08100835)), n + 1);
                    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, PdpContList[nSimID][n]->pPdpAddr, PdpContList[nSimID][n]->nPdpAddrSize);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("PdpCont %d: pPdpaddr: NULL\n",0x08100836)), n + 1);
                }
            }

        }
    }

    // #if TMP_GPRS_CONTEXT
    UINT32  ret = ERR_SUCCESS;
    if((nCid > 7) || (nCid < 1)  )
        return ERR_CFW_INVALID_PARAMETER;
    if(NULL == PdpContList[nSimID][nCid - 1])
        return ERR_NOT_SUPPORT;

    if(PdpContList[nSimID][nCid - 1]->pApn != NULL)
        CSW_GPRS_FREE(PdpContList[nSimID][nCid - 1]->pApn);
    if(PdpContList[nSimID][nCid - 1]->pApnPwd != NULL)
        CSW_GPRS_FREE(PdpContList[nSimID][nCid - 1]->pApnPwd);
    if(PdpContList[nSimID][nCid - 1]->pApnUser != NULL)
        CSW_GPRS_FREE(PdpContList[nSimID][nCid - 1]->pApnUser);
    if(PdpContList[nSimID][nCid - 1]->pPdpAddr != NULL)
        CSW_GPRS_FREE(PdpContList[nSimID][nCid - 1]->pPdpAddr);
    /*
          if(DnsServerIP[nSimID][nCid - 1]!=NULL)
              {
                CSW_GPRS_FREE(DnsServerIP[nSimID][nCid - 1]);
              DnsServerIP[nSimID][nCid - 1]=NULL;
              }
    */
    CSW_GPRS_FREE(PdpContList[nSimID][nCid - 1]);

    PdpContList[nSimID][nCid - 1] = NULL;


    return ret;


}

UINT32 CFW_GprsSetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSetReqQos()\r\n",0x08100837)));

        return ret;
    }

    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
        return ERR_CFW_INVALID_PARAMETER;
    if((pQos->nDelay > 7) || (pQos->nMean > 31) || (pQos->nPeak > 15)
            || (pQos->nPrecedence > 7) || (pQos->nReliability > 7))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQosList[nSimID][nCid - 1].nDelay = pQos->nDelay;
    pQosList[nSimID][nCid - 1].nMean = pQos->nMean;
    pQosList[nSimID][nCid - 1].nPeak = pQos->nPeak ;
    pQosList[nSimID][nCid - 1].nPrecedence = pQos->nPrecedence;
    pQosList[nSimID][nCid - 1].nReliability = pQos->nReliability;

    return ret;
}

UINT32 CFW_GprsGetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsGetReqQos()\r\n",0x08100838)));

        return ret;
    }

    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
        return ERR_CFW_INVALID_PARAMETER;

    pQos->nDelay =  pQosList[nSimID][nCid - 1].nDelay;
    pQos->nMean =  pQosList[nSimID][nCid - 1].nMean;
    pQos->nPeak =  pQosList[nSimID][nCid - 1].nPeak;
    pQos->nPrecedence =  pQosList[nSimID][nCid - 1].nPrecedence;
    pQos->nReliability =  pQosList[nSimID][nCid - 1].nReliability;

    return ret;
}

UINT32 CFW_GprsSetMinQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsSetMinQos()\r\n",0x08100839)));

        return ret;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TRACE)|TDB|TNB_ARG(5) , TSTXT(TSTR("CFW_GprsSetMinQos = 0x%x , 0x%x ,0x%x ,0x%x ,0x%x \n",0x0810083a)), pQos->nDelay, pQos->nMean, pQos->nPeak, pQos->nPrecedence, pQos->nReliability);

    if ((nCid > 7) || (nCid < 1) || (pQos == NULL))
        return ERR_CFW_INVALID_PARAMETER;
    if ((pQos->nDelay > 4) || (pQos->nMean > 31) || (pQos->nPeak > 9)
            || (pQos->nPrecedence > 3) || (pQos->nReliability > 5))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pMinQosList[nSimID][nCid - 1].nDelay        = pQos->nDelay;
    pMinQosList[nSimID][nCid - 1].nMean         = pQos->nMean;
    pMinQosList[nSimID][nCid - 1].nPeak         = pQos->nPeak;
    pMinQosList[nSimID][nCid - 1].nPrecedence   = pQos->nPrecedence;
    pMinQosList[nSimID][nCid - 1].nReliability  = pQos->nReliability;

    return ret;
}

UINT32 CFW_GprsGetMInQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsGetMInQos()\r\n",0x0810083b)));

        return ret;
    }

    if(pQos == NULL)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((nCid > 7) || (nCid < 1) || (pQos == NULL))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    pQos->nPrecedence = pMinQosList[nSimID][nCid - 1].nPrecedence;
    pQos->nDelay      = pMinQosList[nSimID][nCid - 1].nDelay;
    pQos->nMean       = pMinQosList[nSimID][nCid - 1].nMean;
    pQos->nPeak       = pMinQosList[nSimID][nCid - 1].nPeak;
    pQos->nReliability = pMinQosList[nSimID][nCid - 1].nReliability;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TRACE)|TDB|TNB_ARG(5) , TSTXT(TSTR("CFW_GprsSetMinQos = 0x%x , 0x%x ,0x%x ,0x%x ,0x%x \n",0x0810083c)), pQos->nDelay, pQos->nMean, pQos->nPeak, pQos->nPrecedence, pQos->nReliability);


    return ret;
}

#ifndef LTE_NBIOT_SUPPORT
static 
#endif
UINT32 storecid[CFW_SIM_COUNT][8];
#define MAX_CID_NUM   0x07

//The CFW_GetFreeCID function find the empty position to save the CID.
// [out]pCID point to UINT8 type to retrieve the free CID.
// return: If the function succeeds, the return value is ERR_SUCCESS, if the function fails, the error code ERR_CFW_NOT_OPERATOR_ID may be returned
UINT32 CFW_GetFreeCID(UINT8 *pCID, CFW_SIM_ID nSimID)
{
    UINT8  n = 0, i = 0;
    UINT32 temp = 0x00;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GetFreeCID()\r\n",0x0810083d)));

        return ret;
    }

    for(i = 0; i < 8; i++)
    {
        for(n = 1; n < 33; n++)
        {
            temp = storecid[nSimID][i] & (1 << (n - 1));
            if (0 == temp)
            {
                *pCID = (i * 32 + n);
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("!!!!!!! CFW_GetFreeCID() 0x%d\r\n",0x0810083e)), *pCID);

                if( *pCID > MAX_CID_NUM )
                    return ERR_CFW_NOT_OPERATOR_ID;
                else
                    return ERR_SUCCESS;
            }
        }
    }

    return ERR_CFW_NOT_OPERATOR_ID;
}


//The CFW_ReleaseCID function will release the CID when the state of GPRS attachment is CFW_GPRS_DEACTIVATED.
//[in] nCID
// return: If the function succeeds, the return value is ERR_SUCCESS, if the function fails, the error code ERR_CFW_INVALID_PARAMETER may be returned
UINT32 CFW_ReleaseCID(UINT8 nCID, CFW_SIM_ID nSimID)
{
    UINT8 i = 0, n = 0;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_ReleaseCID()\r\n",0x0810083f)));

        return ret;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("!!!!!!! CFW_ReleaseCID() 0x%d\r\n",0x08100840)), nCID);

    if((0 != nCID) && (MAX_CID_NUM >= nCID))
    {
        i = nCID / 32;
        n = nCID - i * 32;
        if (0 == (storecid[nSimID][i] & (1 << (n - 1))))
        {
            return ERR_CFW_INVALID_PARAMETER;
        }
        else
        {
            storecid[nSimID][i] = storecid[nSimID][i] & (~(1 << (n - 1)));
            GprsCidSavedInApp[nSimID] &= ~(0x01 << nCID);
#ifndef LTE_NBIOT_SUPPORT
            if (!isTestSim(nSimID))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID), TSTXT("!!!api_CheckTestSim is %d, not test sim"), api_CheckTestSim(nSimID));
                CFW_StartDetachTimer(nSimID);
            }
#endif
            return ERR_SUCCESS;
        }
    }

    return ERR_CFW_INVALID_PARAMETER;
}


//The CFW_SetCID function will set the CID when the state of GPRS attachment is CFW_GPRS_ACTIVATED.
//[in] nCID
// return: If the function succeeds, the return value is ERR_SUCCESS, if the function fails, the error code ERR_CFW_INVALID_PARAMETER may be returned
UINT32 CFW_SetCID(UINT8 nCID, CFW_SIM_ID nSimID)
{
    UINT8 i = 0, n = 0;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SetCID()\r\n",0x08100841)));

        return ret;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("!!!!!!! CFW_SetCID() 0x%d\r\n",0x08100842)), nCID);

    if((0 != nCID) && (MAX_CID_NUM >= nCID))
    {
        i = nCID / 32;
        n = nCID - i * 32;
        if (0 != (storecid[nSimID][i] & (1 << (n - 1))))
        {
            return ERR_CFW_INVALID_PARAMETER;
        }
        else
        {
            storecid[nSimID][i] = storecid[nSimID][i] | (1 << (n - 1));
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_INVALID_PARAMETER;
}


//add wuys 2009-10-22

#ifdef WIFI_SUPPORT

extern UINT32 wifiDNSserverIP ;

#endif
UINT32 CFW_getDnsServerbyPdp(UINT8 nCid, UINT8 nSimID )
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_getDnsServerbyPdp, cid : %d, simid: %d\n",0x08100843)), nCid, nSimID);
    //  SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, DnsServerIP[nSimID][nCid-1], 6);
    UINT8 *tmp;
    UINT32 serip = 0x00;
    //CFW_GPRS_PDPCONT_INFO  *tmp_pdp;
    //tmp_pdp = PdpContList[nSimID][nCid-1];
#ifdef WIFI_SUPPORT
    if(nCid == 0x11)
    {
        ip_addr_t *serverip;
        serverip = wifi_dns_getserver(0);
        serip = ip_addr_get_ip4_u32(serverip);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_getDnsServerbyPdp, wifiDNSserverIP : 0x%x\n",0x08100844)), serip);
        return serip;

    }
    else
#endif
    {
        tmp = DnsServerIP[nSimID][nCid - 1];
    }
    if(tmp == NULL)
        return serip;
    serip = tmp[3] << 24 | tmp[2] << 16 | tmp[1] << 8 | tmp[0];
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_getDnsServerbyPdp, serip 0x%x\n",0x08100845)), serip);

    // udp_connect(resolv_pcb, &serverIP, DNS_SERVER_PORT);
    return serip;
}

UINT32* getDNSServer(UINT8 nCid, UINT8 nSimID)
{
    if((nCid > 7) ||(nSimID >= CFW_SIM_COUNT))
        return 0;
    return DnsServerIP[nSimID][nCid - 1];
}
extern UINT32 CFW_TcpipInetAddr(const INT8 *cp);

UINT32 CFW_SetDnsServerbyPdp(UINT8 nCid, UINT8 nSimID, UINT8 *Pro_DnsIp, UINT8 *Sec_DnsIp)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SetDnsServerbyPdp, cid : %d, simid: %d\n",0x08100846)), nCid, nSimID);
    //  SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, DnsServerIP[nSimID][nCid-1], 6);
    //    UINT8* tmp;
    //UINT32 serip= 0x00;
    //CFW_GPRS_PDPCONT_INFO  *tmp_pdp;
    //tmp_pdp = PdpContList[nSimID][nCid-1];


    DnsServerIP[nSimID][nCid - 1][0] = (UINT8 *)CFW_TcpipInetAddr(Pro_DnsIp);
    DnsServerIP[nSimID][nCid - 1][1] = (UINT8 *)CFW_TcpipInetAddr(Sec_DnsIp);
    return (UINT32)DnsServerIP[nSimID][nCid - 1];
    //if(tmp==NULL)
    //  return serip;
    //serip = tmp[3]<<24| tmp[2]<<16| tmp[1]<<8| tmp[0];
    //   CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_getDnsServerbyPdp, serip 0x%x\n",0x08100847)),serip);

    // udp_connect(resolv_pcb, &serverIP, DNS_SERVER_PORT);
    //    return serip;
}

//add end

//add wuys 2009-10-22
/*
extern INT8 *CFW_TcpipInet_ntoa(in_addr addr);
VOID CFW_GetDnsIP(UINT8 cid, INT8** DnsIP)
{
in_addr iip;
UINT8 tmp_ip[4]= {0x00,0x00,0x00,0x00};
            memcpy(tmp_ip, PdpContList[cid-1]->pDnsAddr, PdpContList[cid-1]->nDnsAddrSize);
            iip.s_addr = *((UINT32*) tmp_ip);
            *DnsIP=CFW_TcpipInet_ntoa(iip);

}
*/
//add end

//add by wuys 2010-05-17, remove detach process from MMI to CSW


#if 0

U32 gprs_Detach (
#ifdef __MMI_DUAL_SIM__
    CFW_SIM_ID nSimId
#endif
)

{
    U32      rCode = 0;

    nwap_uti = FREE_UTI ;
#ifdef __MMI_DUAL_SIM__
    rCode == CFW_GprsAtt( CFW_GPRS_DETACHED, nwap_uti, nSimId);
#else
    rCode == CFW_GprsAtt( CFW_GPRS_DETACHED, nwap_uti);
#endif

    mmi_trace (1, "JDD_LOG: gprs_Detach : Exit[%X]", rCode) ;
    return rCode ;
}
void gprs_detach_stop_timer(
#ifdef __MMI_DUAL_SIM__
    CFW_SIM_ID nSimID_detach
#endif /* __MMI_DUAL_SIM__ */
)
{
#ifdef __MMI_DUAL_SIM__
    mmi_trace(TRUE, "JDD_LOG: Func: %s nSimID_detach=%d", __FUNCTION__, nSimID_detach);
    if (nSimID_detach == CFW_SIM_0)
    {
        StopTimer(GPRS_DETACH_DELAY_TIMER);
    }
    else if (nSimID_detach == CFW_SIM_1)
    {
        StopTimer(GPRS_DETACH_DELAY_TIMER_SIM1);
    }

#else  /* No Defined __MMI_DUAL_SIM__ */
    StopTimer(GPRS_DETACH_DELAY_TIMER);
#endif /* __MMI_DUAL_SIM__ */
}
void gprs_detach_timer_callback()
{
#ifdef __MMI_DUAL_SIM__
    gprs_Detach(CFW_SIM_0);
#else
    gprs_Detach();
#endif
}
#ifdef __MMI_DUAL_SIM__
void gprs_detach_timer_callback_sim1()
{
    gprs_Detach(CFW_SIM_1);
}
#endif
void gprs_detach_start_timer(
#ifdef __MMI_DUAL_SIM__
    CFW_SIM_ID nSimID
#endif /* __MMI_DUAL_SIM__ */
)
{
    U32 delay_time = 2 * 60 * 1000; // 2m
#ifdef __MMI_DUAL_SIM__
    if (nSimID == CFW_SIM_0)
    {
        StartTimer(GPRS_DETACH_DELAY_TIMER, delay_time, gprs_detach_timer_callback);
    }
    else if (nSimID == CFW_SIM_1)
    {
        StartTimer(GPRS_DETACH_DELAY_TIMER_SIM1, delay_time, gprs_detach_timer_callback_sim1);
    }
#else  /* No Defined __MMI_DUAL_SIM__ */
    StartTimer(GPRS_DETACH_DELAY_TIMER, delay_time, gprs_detach_timer_callback);
#endif /* __MMI_DUAL_SIM__ */

}




#endif



BOOL start_gprsDetach_timer[CFW_SIM_COUNT] = {FALSE,};
BOOL CFW_isPdpActiveAvailable(CFW_SIM_ID nSimID)
{
    if(storecid[nSimID][0] == 0x00)
        return FALSE;
    else
        return TRUE;
}
UINT32 CFW_StartDetachTimer(CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_StartDetachTimer begin, active pdp: 0x%x,\n",0x08100848)), storecid[nSimID][0] );

    if(storecid[nSimID][0] == 0x00)
    {
        BOOL ret;
        ret = COS_SetTimerEX(0xFFFFFFFF, CFW_START_GPRSDETACH_TIMER_ID + ((UINT8)nSimID),
                             COS_TIMER_MODE_SINGLE, DURATION_BEFORE_DETACH);
        if(ret)
        {
            start_gprsDetach_timer[nSimID] = TRUE;
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_StartDetachTimer success, nSimID: %d\n",0x08100849)), nSimID);

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_StartDetachTimer fail\n",0x0810084a)));

        }
    }

    return storecid[nSimID][0];

}


UINT32 CFW_StopDetachTimer(CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_StopDetachTimer begin, start detach timer: %d, simid: %d\n",0x0810084b)), start_gprsDetach_timer[nSimID], nSimID);

    if(start_gprsDetach_timer[nSimID])
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_StopDetachTimer COS_KillTimerEX\n",0x0810084c)));

        COS_KillTimerEX(0xFFFFFFFF, CFW_START_GPRSDETACH_TIMER_ID + ((UINT8) nSimID));
        start_gprsDetach_timer[nSimID] = FALSE;
    }

    return ERR_SUCCESS;

}
BOOL ispdpactive(CFW_SIM_ID nSimID)
{
    //   CFW_SIM_ID tmpnSimID;
    bool ret = FALSE;

    UINT8 sim_num = 0;
    for(sim_num = 0; (CFW_SIM_0 + sim_num) < CFW_SIM_COUNT; sim_num++)
        if(((CFW_SIM_0 + sim_num) != nSimID) && (storecid[sim_num][0] != 0x00))
            ret = TRUE;

    return ret;
}





#ifdef LTE_NBIOT_SUPPORT
extern void tgl00_170FreeDlIpData(u8 *po);
extern void tgl00_168FreeDlNode(T_ComSduLte *pt);


CFW_EQOS     pEQosList[CFW_SIM_COUNT][7];
UINT16 nonIpMtu = 1600;

const pcoContainer pcoCtn[] = 
	{{PCO_TYPE_IPCP, (0x8021)},
	  {PCO_TYPE_IPV6DNS,(0x0003)},
	  {PCO_TYPE_IPV4DNS, (0x000d)},
	  {PCO_TYPE_NONIP_MTU, (0x0015)}};

static PCO_TYPE_ENUM_T CFW_Find_PcoType_From_Id(u16 id)
{
    UINT8       cnt = sizeof(pcoCtn)/sizeof(pcoContainer);
    UINT8       i = 0;
    for(i=0; i < cnt; i++)
    {
        if(id == pcoCtn[i].ContId)
	     return pcoCtn[i].type;
    }
    return PCO_TYPE_MAX;
}

static VOID CFW_Decode_ePCO(UINT8 pdnType,UINT8 cid, UINT16 pcoLen, UINT8 *ePCO)
{
    UINT8 i = 0, j = 0;
    UINT8 dnsSize;
    ip_addr_t              dns_server;

    UINT8 ipv4_dns_parse = 0;
    UINT8 ipv6_dns_parse = 0;
    u16 containerId = 0;
    if((pcoLen == 0) || (ePCO == NULL))
        return;
    for(i = 1; i < pcoLen; )   //skip ext 80 byte
    {
        containerId = (ePCO[i] << 8) | ePCO[i+1];
	 CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_Decode_ePCO: i = %d, containerId = 0x%04x",0x0810079f)),i, containerId);
	 i += 2;
	 switch(CFW_Find_PcoType_From_Id(containerId))
	 {
	     case PCO_TYPE_IPCP:
	     {
		  CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("PCO_TYPE_IPCP \n",0x081007a0)));
	         UINT8   ipcp_len = ePCO[i++];
	         if((ipv4_dns_parse == 0)&&((ePCO[i] == 0x03) || (ePCO[i] == 0x02)))    //ncp_config_nak or ack
	         {
                    ipv4_dns_parse = 1;
                    if((ePCO[i+4] == 0x81) || (ePCO[i+4] == 0x83))
                    {
                       //lwip dns server
                         ip_addr_set_zero_ip4(&dns_server);
	                  IP_ADDR4(&dns_server, ePCO[i+6], ePCO[i+7], ePCO[i+8], ePCO[i+9]);
	                  dns_setserver((pdnType == 1)?0:1, &dns_server);
                         ip_addr_debug_print(IP_DEBUG, &dns_server);
		          //ppp dns server
	                    dnsSize = ePCO[i+5]-2;
		                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nDnsAddrSize: %d \n",0x0810079f)), dnsSize);
                        SUL_MemCopy8(DnsServerIP[0][cid - 1], &ePCO[i+6], dnsSize );
                        
                     }      
	          }
		   i += ipcp_len;
	         break;
	     }
#if  LWIP_IPV4 && LWIP_IPV6
	     case PCO_TYPE_IPV6DNS:
	     {
		  CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("PCO_TYPE_IPV6DNS \n",0x081007a0)));
		   UINT8 ipv6dnsLen = ePCO[i++];
		   if((ipv6_dns_parse == 0)||(pdnType == 2))
		   {
		   ipv6_dns_parse = 1;
		   ip_addr_set_zero_ip6(&dns_server);
                 IP6_ADDR_PART((ip_2_ip6(&dns_server)),0, ePCO[i], ePCO[i+1], ePCO[i+2], ePCO[i+3]);
		   IP6_ADDR_PART((ip_2_ip6(&dns_server)),1, ePCO[i+4], ePCO[i+5], ePCO[i+6], ePCO[i+7]);
		   IP6_ADDR_PART((ip_2_ip6(&dns_server)),2, ePCO[i+8], ePCO[i+9], ePCO[i+10], ePCO[i+11]);
		   IP6_ADDR_PART((ip_2_ip6(&dns_server)),3, ePCO[i+12], ePCO[i+13], ePCO[i+14], ePCO[i+15]);
                 dns_setserver(j++, &dns_server);
		   ip_addr_debug_print(IP_DEBUG, &dns_server);
		   }
                 i += ipv6dnsLen;
	          break;
	     }
#endif
	     case PCO_TYPE_IPV4DNS:
	     {
		  CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("PCO_TYPE_IPV4DNS \n",0x081007a0)));
		  UINT8 ipv4dnsLen = ePCO[i++];
		 
		  if(ipv4_dns_parse = 0)
		  {
		      ipv4_dns_parse = 1;
		      //lwip dns server
		       ip_addr_set_zero_ip4(&dns_server);
		      IP_ADDR4(&dns_server, ePCO[i], ePCO[i+1], ePCO[i+2], ePCO[i+3]);
		      dns_setserver((pdnType == 1)?0:1, &dns_server);
                    ip_addr_debug_print(IP_DEBUG, &dns_server);
		      //ppp dns server
		      CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nDnsAddrSize: %d \n",0x0810079f)), ipv4dnsLen);

              SUL_MemCopy8(DnsServerIP[0][cid - 1], &ePCO[i], dnsSize );
		  }
		  i += ipv4dnsLen;
	         break;
	     }
	     case PCO_TYPE_NONIP_MTU:
	     {
		  CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("PCO_TYPE_NONIP_MTU \n",0x081007a0)));
		  UINT8 mtuLen = ePCO[i++];
		  nonIpMtu = ePCO[i]<<8|ePCO[i+1];
		  //SUL_MemCopy8(&nonIpMtu, ePCO[i], mtuLen);   how to attention little-endian issue?
		  i += mtuLen;
	         break;
	     }
	     //case PCO_TYPE_MAX:
	     default:
	     {
		  CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("unspported pco type parse\n",0x081007a0)));
	         UINT8 dft_len = ePCO[i++];
		  i += dft_len;
		  //unsupport-container, maybe should support in the future   reserved
	         break;
	     }
	 }
    }
}


static UINT16 CFW_Encode_ePCO(UINT8 pdnType, UINT8 *ePCO, CFW_GPRS_PDPCONT_INFO *pdpCtx)
{
    UINT16 len = 0;
    if(ePCO == NULL)
        return 0;
    ePCO[len++] = 0x80;  //ePCO context format conflict btw 24008 and 24301
    
    if((pdpCtx) && (pdpCtx->nApnUserSize > 0) && (pdpCtx->nApnPwdSize > 0))
    {
        ePCO[len++] = 0xc0;
        ePCO[len++] = 0x23;
        ePCO[len++] = pdpCtx->nApnUserSize + pdpCtx->nApnPwdSize + 2 + 4;
        ePCO[len++] = 0x01;
        ePCO[len++] = 0x00;
        ePCO[len++] = 0x00;
        ePCO[len++] = pdpCtx->nApnUserSize + pdpCtx->nApnPwdSize + 2 + 4;
        ePCO[len++] = pdpCtx->nApnUserSize;
        SUL_MemCopy8(&ePCO[len] , pdpCtx->pApnUser, pdpCtx->nApnUserSize);
        len += pdpCtx->nApnUserSize;
        ePCO[len++] = pdpCtx->nApnPwdSize;
        SUL_MemCopy8(&ePCO[len], pdpCtx->pApnPwd, pdpCtx->nApnPwdSize);
        len += pdpCtx->nApnPwdSize;
    }
	
    if(pdnType == 1)
    {
	    SUL_MemCopy8(ePCO +len , IPCP_OPTION, 19);
	    len += 19;
	    //encode 000d dns server IPv4 Address
	    //ePCO[len++] = 0x00;
	    //ePCO[len++] = 0x0d;
	    //ePCO[len++] = 0x00; //length
    }
    else if(pdnType == 2)  //ipv6
    {
        ePCO[len++] = 0x00;   //0003H   DNS Server IPV6 Address Request
	 ePCO[len++] = 0x03;
	 ePCO[len++] = 0x00;
    }
    else if(pdnType == 3)  //ipv4v6
    {
        SUL_MemCopy8(ePCO +len , IPCP_OPTION, 19);
	 len += 19;
	 //encode 000d dns server IPv4 Address
	 ePCO[len++] = 0x00;
	 ePCO[len++] = 0x0d;
	 ePCO[len++] = 0x00; //length
	 ePCO[len++] = 0x00;   //0003H   DNS Server IPV6 Address Request
	 ePCO[len++] = 0x03;
	 ePCO[len++] = 0x00;
    }
    else if(pdnType == 5)
    {
        ePCO[len++] = 0x00;  //0015H  Non-IP Link MTU Request
        ePCO[len++] = 0x15;
	 ePCO[len++] = 0x00; //length
    }
    //0016H APN rate control support indicator is encoded by nas
    return len;
}


extern u8 nbiot_nvGetDefaultPdnType(void);

UINT32 CFW_GprsSendDefaultCtx(UINT8 uCid,CFW_EQOS nEqos,UINT8 nSimID )
{
	CFW_GPRS_PDPCONT_INFO sPdpCXT;
	memset(&sPdpCXT, 0, sizeof(CFW_GPRS_PDPCONT_INFO));
	api_DefaultPdnCtxConfigReq_t *pOutMsg = NULL;
	pOutMsg = (api_DefaultPdnCtxConfigReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_DefaultPdnCtxConfigReq_t));
	//GPRS_SM_INFO* pUserData  = NULL;
	//pUserData = (GPRS_SM_INFO *)CFW_MemAllocMessageStructure(SIZEOF(GPRS_SM_INFO));
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_DEFAULT_PDN_CTXCONFIG_REQ\n",0x0810078a)));
	SUL_MemSet8(pOutMsg, 0, SIZEOF(api_DefaultPdnCtxConfigReq_t));
	sPdpCXT.pApn = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_MAX_LEN); 
	//sPdpCXT.pApnUser = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_USER_MAX_LEN);
	//sPdpCXT.pApnPwd = (UINT8 *)CSW_GPRS_MALLOC(THE_APN_PWD_MAX_LEN);
	UINT8 nApnWap[5] = {'N', 'B', 'N', 'E', 'T'}; 
	UINT8 *nApn = NULL;
	nApn = nApnWap;
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get Default PDN CTX CONFIG\n",0x0810078b)));
	sPdpCXT.nPdpType = 0;
	sPdpCXT.nApnSize = 5;
	SUL_MemCopy8(sPdpCXT.pApn, nApn, 5);
	sPdpCXT.nPdpAddrSize = 0;
	sPdpCXT.nDComp  = 0;
	sPdpCXT.nHComp = 0;
	sPdpCXT.PdnType = 0; 
    /*
    pUserData->Qos.nGuaranteedbitrateDL = nEqos.nDlGbr;	
	pUserData->Qos.nGuaranteedbitrateUL = nEqos.nUlGbr;
	pUserData->Qos.nMaximumbitrateDL = nEqos.nDlMbr;
	pUserData->Qos.nMaximumbitrateUL = nEqos.nUlMbr;
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("PDP context config, ApnUserSize: 0x%x; ApnPwdSize:  0x%x \n",0x0810078c)), sPdpCXT.nApnUserSize, sPdpCXT.nApnPwdSize);

*/
	pOutMsg->APNLen = sPdpCXT.nApnSize + 1 ;
	pOutMsg->AccessPointName[0] = sPdpCXT.nApnSize;

	UINT8 i;
	UINT8 n = 0;
	for(i = 0; i < sPdpCXT.nApnSize; i++)
	{
	if( sPdpCXT.pApn[i] == '.')
	{
	    pOutMsg->AccessPointName[n] = i - n;
	    SUL_MemCopy8(pOutMsg->AccessPointName  + n + 1 , sPdpCXT.pApn + n, i - n);
	    n = i + 1;
	}
	}
	if (n > 0)
	{
	pOutMsg->AccessPointName[n] = i - n;
	SUL_MemCopy8(pOutMsg->AccessPointName  + n + 1 , sPdpCXT.pApn + n, i - n);
	}
	else
	{
	SUL_MemCopy8(pOutMsg->AccessPointName + 1 , sPdpCXT.pApn, sPdpCXT.nApnSize);
	}
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("APN length: %d, APN: ",0x08100791)), pOutMsg->APNLen);
	CSW_GPRS_FREE(sPdpCXT.pApn);
    //pOutMsg->PdnType = nbiot_nvGetDefaultPdnType();
	   
    pOutMsg->ProtocolCfgOptionLen = CFW_Encode_ePCO(pOutMsg->PdnType, pOutMsg->ProtocolCfgOption, &sPdpCXT);
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pOutMsg->ProtocolCfgOptionLen: 0x%x  \n",0x08100792)), pOutMsg->ProtocolCfgOptionLen);
	SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg->ProtocolCfgOption, 100);

	SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pOutMsg, 100);
	CFW_SendSclMessage(API_DEFAULT_PDN_CTXCONFIG_REQ, pOutMsg, nSimID);
	return ERR_SUCCESS;
	//CSW_Free(pUserData); // stack will free
}

UINT32 CFW_GprsGetCtxEQos(UINT8 nCid, CFW_EQOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQos->nQci  =   pEQosList[nSimID][nCid - 1].nQci;
    pQos->nUlMbr=   pEQosList[nSimID][nCid - 1].nUlMbr;
    pQos->nDlMbr=   pEQosList[nSimID][nCid - 1].nDlMbr;
    pQos->nUlGbr=   pEQosList[nSimID][nCid - 1].nUlGbr;
    pQos->nDlGbr=   pEQosList[nSimID][nCid - 1].nDlGbr;

    return ret;
}

UINT32 CFW_GprsSetCtxEQos(UINT8 nCid, CFW_EQOS* pQos,UINT8 nSimID )
{
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    
    pEQosList[nSimID][nCid - 1].nQci   = pQos->nQci;
    pEQosList[nSimID][nCid - 1].nUlMbr = pQos->nUlMbr;
    pEQosList[nSimID][nCid - 1].nDlMbr = pQos->nDlMbr;
    pEQosList[nSimID][nCid - 1].nUlGbr = pQos->nUlGbr;
    pEQosList[nSimID][nCid - 1].nDlGbr = pQos->nDlGbr;
    
    return ERR_SUCCESS;
}


UINT32 CFW_GprsClrCtxTFT(UINT8 nCid, UINT8 nSimID)
{
    VOID       * pFree;
    CFW_TFT_SET* pTftSet   = pTftSetList[nSimID][nCid - 1];
    
    if( (nCid > CFW_PDPCID_MAX) || (nCid < 1) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    while(pTftSet != NULL)
    {
        pFree   = pTftSet;
        pTftSet = pTftSet->pNext;
        CSW_GPRS_FREE(pFree);
    }
        
    pTftSetList[nSimID][nCid - 1] = NULL;
    
    return ERR_SUCCESS;
}




UINT32 CFW_GprsSetCtxTFT(UINT8 nCid, CFW_TFT_SET* pTft,UINT8 nSimID )
{
    CFW_TFT_SET* pTftSet   = pTftSetList[nSimID][nCid - 1];
    CFW_TFT_SET* pTftSetIn = NULL; 
    
    if( (nCid > CFW_PDPCID_MAX) || (nCid < 1) || (pTft == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    /* PF ID Not Exist, Clear All */
    if ( (pTft->Pf_BitMap & 0x8000) != 0x8000 )
    {
        CID2INDEX* pCid2Index = NULL;
        
        CFW_GprsClrCtxTFT(nCid, nSimID);

        pCid2Index = CFW_GprsGetCid2IndexInfo(nCid, nSimID);

        /* Can't Clear All PF for a Dedicated Bearer */
        if ( pCid2Index != NULL && pCid2Index->nNsapi != pCid2Index->nLinkedEbi )
            return ERR_CFW_INVALID_PARAMETER;
        else        
            return ERR_SUCCESS;
    }

    /* In PF Compnent, IPv6 and IPv6 information can't exist both,
       Invalid PF Parameter
    */
    if ( (pTft->Pf_BitMap & 0x3000) == 0x3000 
      || (pTft->Pf_BitMap & 0x3000) == 0x0018 
      || (pTft->Pf_ID > 16 || pTft->Pf_ID < 1)
      || (pTft->Pf_Direct > 3 && (pTftSet->Pf_BitMap & 0x0020)) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    pTftSetIn = CSW_GPRS_MALLOC(sizeof(CFW_TFT_SET));

    /* In TFT, PF Identity 0-15, in AT PF Identity 1-16 */
    pTft->Pf_ID -= 1;
    
    SUL_MemCopy8(pTftSetIn, pTft, sizeof(CFW_TFT_SET));
    
    pTftSetIn->pNext = pTftSetList[nSimID][nCid - 1];
    pTftSetList[nSimID][nCid - 1] = pTftSetIn;
    
    return ERR_SUCCESS;
}



UINT32 CFW_GprsGetCtxTFT(UINT8 nCid, CFW_TFT_SET **pTft, UINT8 nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > CFW_PDPCID_MAX) || (nCid < 1) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    *pTft = pTftSetList[nSimID][nCid - 1];
    
    return ret;
}

// TRACE TFT
UINT32 CFW_GprsTftTrace(CFW_TFT_PF *pTftPf)
{
    CFW_PF_CPNT * pPfCpnt = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0),     "===========TFT Trace Start========");

    while( pTftPf != NULL )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(4), "PF: EBI=%d, Dir=%d, ID=%d, Pri=%d",
                  pTftPf->uEBI, pTftPf->uPF_Dir, pTftPf->uPF_ID, pTftPf->uPF_Pri);
        pPfCpnt = pTftPf->Cpnt;
        while( pPfCpnt != NULL )
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "PF Component Type = 0x%x", pPfCpnt->uCpntType);
            SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, pPfCpnt->uCpntCont, pPfCpnt->uCpntLen);
            pPfCpnt = pPfCpnt->pNext;
        }
        pTftPf = pTftPf->pNext;
    }
    
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0),     "===========TFT Trace End==========");
    
    return ERR_SUCCESS;
}


// Delete the Packet Fileter indiated by Nsapi/EBI, and Create a TFT IE
void CFW_GprsDelPFInDeactProc(CFW_TFT* pTft, UINT8 Ebi, UINT8 LinkEbi, CFW_SIM_ID nSimID)
{
    CFW_TFT_PF* pTftPf     = NULL;
    CFW_TFT_PF* pTftPfPrev = NULL;
    UINT8       nDelPfNum  = 0;

    /* Clear TFT Content */
    if ( pTft != NULL )
        SUL_MemSet8(pTft, 0x00, sizeof(CFW_TFT));

    /* Invalid Parameter */
    if( Ebi < 5 || Ebi > 15 || LinkEbi < 5 || LinkEbi > 15)
        return ;
    
    pTftPfPrev = pTftPf = pTftPfList[nSimID][LinkEbi-5];

    /* Empty Packet Filter */
    if ( pTftPf == NULL )
        return ;

    while ( pTftPf != NULL )
    {
        CFW_TFT_PF* pTftPfFree = pTftPf;
        
        /* if Ebi == LinkEbi, Delete all PF */
        if ( pTftPf->uEBI == Ebi || Ebi == LinkEbi )
        {
            if ( pTftPf == pTftPfList[nSimID][LinkEbi-5] )
            {
                pTftPfList[nSimID][LinkEbi-5] = pTftPf->pNext;
                pTftPfPrev = pTftPf->pNext;
                pTftPf = pTftPf->pNext;
            }
            else
            {
                pTftPfPrev->pNext = pTftPf->pNext;
                pTftPf = pTftPf->pNext;
            }

            /* Set Delete PF Identitiers */
            if ( pTft != NULL )
            {
                pTft->uTFT[nDelPfNum+1] = pTftPfFree->uPF_ID;
                nDelPfNum ++;
            }
            
            /* Free This PF */
            CFW_GprsTftPfFree(pTftPfFree);            
        }
        else
        {
            /* Check Next PF */
            pTftPfPrev = pTftPf;
            pTftPf = pTftPf->pNext;   
        }
    }

    /* Set TFT operation code and Number of packet filters*/
    if ( pTft != NULL )
    {
        /* 1 0 1 0 Delete packet filters from existing TFT & E bit */
        pTft->uTFT[0] = (0xA0 | nDelPfNum);
        pTft->uLength = nDelPfNum + 1;
    }
    
}

// Save the Packet Fileter in Activate Procedure indiated by Nsapi/EBI
UINT8 CFW_GprsSavePFinActProc(CFW_TFT_PARSE *pTftParse, UINT8 nSimID)
{
    CFW_TFT_PF *existPfHead = NULL;
    CFW_TFT_PF *insertPf    = NULL;
    CFW_TFT_PF *nextPf      = NULL;
    CFW_TFT_PF *newTftPf    = NULL;
    UINT8 Ebi     = pTftParse->Ebi;
    UINT8 LinkEbi = pTftParse->LinkEbi;

    if(((LinkEbi > 15)||(LinkEbi < 5))
        ||((Ebi > 15)||(Ebi < 5)))
        return E_CFW_TFT_ERR_UNDEFINE;  

    existPfHead = pTftPfList[nSimID][LinkEbi - 5];
    newTftPf    = pTftParse->pTftPf;

    /* 
    c) Semantic errors in packet filters:
    2) When the resulting TFT does not contain any packet filter which applicable 
        for the uplink direction.
        The UE shall reject the activation request with ESM cause #44 "semantic errors 
        in packet filter(s)"
    */
    if(!(CFW_GprsTftHasUpDir(newTftPf)
        ||(CFW_GprsTftHasUpDir(existPfHead))))
    {
        CFW_GprsTftPfFree(newTftPf);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsSavePFinActProc error: no up dir\n");
        return E_CFW_TFT_ERR_PF_SEMA;
    }
    
    /* 
    d) Syntactical errors in packet filters
    1) When the TFT operation = "Create a new TFT" and two or more packet filters
       in the resultant TFT would have identical packet filter identifiers.

       In cases 1 and 3 the UE shall reject the activation request with 
       ESM cause #45 "syntactical errors in packet filter(s)".
    */
    if(CFW_GprsTftPfHasSameIDInAct(newTftPf,existPfHead,Ebi))
    {
        CFW_GprsTftPfFree(newTftPf);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsSavePFinActProc error: same pf id\n");
        return E_CFW_TFT_ERR_PF_SYNT;       
    }   

    nextPf = newTftPf;
    
    while(nextPf != NULL)
    {
        insertPf = nextPf;
        nextPf = nextPf->pNext;
        
        existPfHead = CFW_GprsTftInsertPf(&existPfHead,insertPf);
    }

    pTftPfList[nSimID][LinkEbi - 5] = existPfHead;

    CFW_GprsTftTrace(existPfHead);
    
    return E_CFW_TFT_ERR_NO;
}

// Save the Packet Fileter in Modify Procedure indiated by Nsapi/EBI
UINT8 CFW_GprsSavePFinModProc(CFW_TFT_PARSE *pTftParse, UINT8 nSimID)
{
    CFW_TFT_PF   *existTftPf   = NULL;
    CFW_TFT_PF   *newTftPf     = NULL;
    CFW_TFT_PF   *nextPf       = NULL;
    CFW_TFT_PF   *insertPf     = NULL;
    CFW_TFT_PF   *origTftClone = NULL;    
    UINT8 errorCode = E_CFW_TFT_ERR_NO;

    UINT8 tftOp   = pTftParse->TftOpCode;
    UINT8 Ebi     = pTftParse->Ebi;
    UINT8 LinkEbi = pTftParse->LinkEbi;

    if(((LinkEbi > 15)||(LinkEbi < 5))
        ||((Ebi > 15)||(Ebi < 5)))
        return E_CFW_TFT_ERR_UNDEFINE;  

    existTftPf  = pTftPfList[nSimID][LinkEbi - 5];
    newTftPf    = pTftParse->pTftPf;

    /* Clone Existed TFT */
    origTftClone = CFW_GprsTftClone(existTftPf);
    
    if(E_TFT_OP_CREATE == tftOp)    
    {
        /*
        a) Semantic errors in TFT operations
        1) TFT operation = "Create a new TFT" when there is already 
            an existing TFT for the EPS bearer context.

            the UE shall further process the new activation request and,
            if it was processed successfully, delete the old TFT."
        */  
        origTftClone = CFW_GprsTftDeleteSameEbi(&origTftClone,Ebi); 
        
        origTftClone = CFW_GprsTftFindAndDelSameID(newTftPf,origTftClone,&origTftClone,Ebi);

        /* Insert PF */
        nextPf = newTftPf;
        
        while(nextPf != NULL)
        {
            insertPf = nextPf;
            nextPf = nextPf->pNext;
            
            origTftClone = CFW_GprsTftInsertPf(&origTftClone,insertPf);
        }
    }
    
    if((E_TFT_OP_ADD == tftOp)
        ||(E_TFT_OP_REPLACE == tftOp))  
    {
        origTftClone = CFW_GprsTftFindAndDelSameID(newTftPf,origTftClone,&origTftClone,Ebi);
        
        /* Insert PF */
        nextPf = newTftPf;
        
        while(nextPf != NULL)
        {
            insertPf = nextPf;
            nextPf = nextPf->pNext;
            
            origTftClone = CFW_GprsTftInsertPf(&origTftClone,insertPf);
        }
    }

    if(E_TFT_OP_DEL_PF == tftOp)
    {
        origTftClone = CFW_GprsTftFindAndDelSameID(newTftPf,origTftClone,&origTftClone,Ebi);

        /* 3) TFT operation = "Delete packet filters from existing TFT" when it would 
              render the TFT empty.

              In case 3, if the packet filters belong to a dedicated EPS bearer context, 
              the UE shall process the new deletion request and, if no error according 
              to items b, c, and d was detected, the UE shall reject the modification 
              request with ESM cause #41 "semantic error in the TFT operation".
        */
        if( (LinkEbi != Ebi) && (FALSE == CFW_GprsTftHasEbi(origTftClone, Ebi)) )
        {
            CFW_GprsTftFreeAll(&origTftClone);
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsSavePFinModProc error: after delete pf,pf is empty\n");
            return E_CFW_TFT_ERR_OP_SEMA;
        }        
    }
    
    if(E_TFT_OP_DELETE == tftOp)
    {
        origTftClone = CFW_GprsTftDeleteSameEbi(&origTftClone,Ebi); 
    } 
    
    /*
    d)  Syntactical errors in packet filters:
    2)  When the TFT operation = "Create a new TFT" and two or more packet filters in 
        all TFTs associated with this PDN connection would have identical packet 
        filter precedence values.

        Same precedence values in the new TFT, Reject with #45 "syntactical errors in packet filter(s)".
    */
    if(CFW_GprsNewTftHasSamePriority(origTftClone))
    {
        CFW_GprsTftFreeAll(&origTftClone);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsSavePFinModProc error: same priority error\n");
        errorCode = E_CFW_TFT_ERR_PF_SYNT;
        return errorCode;       
    }

    /*
    c) Semantic errors in packet filters
    2) When the resulting TFT, which is assigned to a dedicated EPS bearer context,
       does not contain any packet filter applicable for the uplink direction among
       the packet filters created on request from the network

       The UE shall reject the modification request with
       ESM cause #44 "semantic errors in packet filter(s)"
    */    
    if(!(CFW_GprsTftHasUpDir(newTftPf)
        ||(CFW_GprsTftHasUpDir(origTftClone)))
        && (LinkEbi != Ebi))
    {
        CFW_GprsTftFreeAll(&origTftClone);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsSavePFinModProc error: no up dir\n");
        return E_CFW_TFT_ERR_PF_SEMA;
    }

    /*  Success */
    pTftPfList[nSimID][LinkEbi - 5] = origTftClone;

    CFW_GprsTftFreeAll(&existTftPf);

    CFW_GprsTftTrace(origTftClone);

    return E_CFW_TFT_ERR_NO;        
}


// Packet Fileter Check in Activate Procedure indiated by Nsapi/EBI
UINT8 CFW_GprsTftCheckInAct(CFW_TFT* pTft, CFW_TFT_PARSE *pTftParse, CFW_SIM_ID nSimID)
{
    TFT_HEAD_BIT *head;
    CFW_TFT_PF   *newTftPfHead = NULL;    
    UINT8 errorCode = E_CFW_TFT_ERR_NO;
    UINT8 Ebi       = pTftParse->Ebi;
    UINT8 LinkEbi   = pTftParse->LinkEbi;
    UINT8 flagArr[16] = {0};

    if((NULL == pTft)
        ||(nSimID >= CFW_SIM_COUNT)
        ||((LinkEbi > 15)||(LinkEbi < 5))
        ||((Ebi > 15)||(Ebi < 5)))
        return E_CFW_TFT_ERR_UNDEFINE;

    /*
    b) Syntactical errors in TFT operations
    2) mismatch conter.
    */
    if(pTft->uLength < 1)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: ulength = 0\n");
        return E_CFW_TFT_ERR_OP_SYNT;
    }
    head = (TFT_HEAD_BIT *)pTft->uTFT;

/*
    a)  Semantic errors in TFT operations:
    1)  When the TFT operation is an operation other than "Create a new TFT"
        The UE shall reject the activation request with ESM cause #41 "semantic 
        error in the TFT operation".
*/
    if(E_TFT_OP_CREATE != head->tftOp)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: not Create type\n");
        return E_CFW_TFT_ERR_OP_SEMA;
    }

/*
    b)  Syntactical errors in TFT operations:
    1)  When the TFT operation = "Create a new TFT" and the packet filter list 
        in the TFT IE is empty.
        The UE shall reject the activation request with ESM cause #42 "syntactical 
        error in the TFT operation".
*/
    if((0 == head->numOfPf)
        ||(pTft->uLength < 4))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: pf is empty\n");    
        return E_CFW_TFT_ERR_OP_SYNT;
    }

    errorCode = CFW_GprsTftMsgParseWithComponent(pTft,Ebi,&newTftPfHead);

    if(errorCode != E_CFW_TFT_ERR_NO)
    {
        CFW_GprsTftFreeAll(&newTftPfHead);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: pf Component error\n");
        return errorCode;
    }

    /*
    d)  Syntactical errors in packet filters:
    1)  When the TFT operation = "Create a new TFT" and two or more packet filters in 
        the resultant TFT would have identical packet filter identifiers.
    
        In cases 1 and 3 the UE shall reject the activation request with ESM cause 
    #45 "syntactical errors in packet filter(s)".
    */
    if(CFW_GprsTftPfHasSameID(newTftPfHead,flagArr))
    {
        CFW_GprsTftFreeAll(&newTftPfHead);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: same pf id error\n");
        errorCode = E_CFW_TFT_ERR_PF_SYNT;
        return errorCode;       
    }

    /*
    d)  Syntactical errors in packet filters:
    2)  When the TFT operation = "Create a new TFT" and two or more packet filters in 
        all TFTs associated with this PDN connection would have identical packet 
        filter precedence values.

        Same precedence values in the new TFT, Reject with #45 "syntactical errors in packet filter(s)".
    */
    if(CFW_GprsNewTftHasSamePriority(newTftPfHead))
    {
        CFW_GprsTftFreeAll(&newTftPfHead);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInAct error: same priority error\n");
        errorCode = E_CFW_TFT_ERR_PF_SYNT;
        return errorCode;       
    }

    /*
    d)  Syntactical errors in packet filters:
        In case 2, if the old packet filters do not belong to the default EPS bearer 
        context, the UE shall not diagnose an error, shall further process the new 
        activation request and, if it was processed successfully, shall delete the 
        old packet filters which have identical filter precedence values. Furthermore, 
        by means of explicit peer-to-peer signalling between the network and the UE, 
        the UE shall perform a UE requested bearer resource modification procedure 
        to deactivate the EPS bearer context(s) for which it has deleted the packet 
        filters.
        In case 2, if one or more old packet filters belong to the default EPS bearer 
        context, the UE shall release the relevant PDN connection. If the relevant 
        PDN connection is the last one that the UE has and EMM-REGISTERED without 
        PDN connection is not supported by the UE or the MME, the UE shall detach 
        and re-attach to the network.
    
    */
    errorCode = CFW_GprsTftFindOldSamePrio(newTftPfHead,
                                           pTftPfList[nSimID][LinkEbi-5],
                                           pTftParse);

    if(E_CFW_TFT_ERR_SAME_PRIO_DEFAULT == errorCode)
    {
        CFW_GprsTftFreeAll(&newTftPfHead);
        //errorCode = E_CFW_TFT_ERR_PF_SYNT;
        return errorCode;       
    }

    pTftParse->TftOpCode = head->tftOp;
    pTftParse->pTftPf    = newTftPfHead;
    
    return errorCode;
}


UINT8 CFW_GprsTftCheckInMod(CFW_TFT* pTft, CFW_TFT_PARSE *pTftParse, CFW_SIM_ID nSimID)                                  
{
    TFT_HEAD_BIT *tftHead;
    CFW_TFT_PF   *newTftPfHead = NULL;    
    UINT8 errorCode = E_CFW_TFT_ERR_NO;
    UINT8 Ebi       = pTftParse->Ebi;
    UINT8 LinkEbi   = pTftParse->LinkEbi;
    UINT8 flagArr[16] = {0};

    if((NULL == pTft)
        ||((LinkEbi > 15)||(LinkEbi < 5))
        ||((Ebi > 15)||(Ebi < 5)))
        return E_CFW_TFT_ERR_UNDEFINE;

    /*
    b) Syntactical errors in TFT operations
    2) mismatch conter.
    */
    if(pTft->uLength < 1)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: length is zero\n");
        return E_CFW_TFT_ERR_OP_SYNT;  
    }

    tftHead = (TFT_HEAD_BIT *)pTft->uTFT;

    /*
    a) Semantic errors in TFT operations
    4) TFT operation = "Delete existing TFT" for a dedicated EPS bearer context
    
       In case 4 the UE shall reject the modification request with
       ESM cause #41 "semantic error in the TFT operation"
    */
    if((E_TFT_OP_DELETE == tftHead->tftOp)
        &&(LinkEbi != Ebi))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: delete a dedicate\n");
        return E_CFW_TFT_ERR_OP_SEMA;
    }

    /*
    b) Syntactical errors in TFT operations
    1) When the TFT operation = "Create a new TFT", "Add packet filters in existing TFT",
       "Replace packet filters in existing TFT" or "Delete packet filters from existing TFT" 
       and the packet filter list in the TFT IE is empty
    
       UE shall reject the modification request with 
       ESM cause #42 "syntactical error in the TFT operation"
    */

    if((E_TFT_OP_CREATE == tftHead->tftOp)
        ||(E_TFT_OP_ADD == tftHead->tftOp)
        ||(E_TFT_OP_REPLACE == tftHead->tftOp))
    {
        if((pTft->uLength < 5)
            ||(tftHead->numOfPf == 0))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: pf is empty\n");
            return E_CFW_TFT_ERR_OP_SYNT;
        }
    }
        
    if(E_TFT_OP_DEL_PF == tftHead->tftOp)
    {
        if((pTft->uLength < 2)
            ||(tftHead->numOfPf == 0))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: delete Pf, pf is empty\n");
            return E_CFW_TFT_ERR_OP_SYNT;
        }
    }


    /*
    b) Syntactical errors in TFT operations
    2) TFT operation = "Delete existing TFT" or "No TFT operation" with
       a non-empty packet filter list in the TFT IE
    
       UE shall reject the modification request with 
       ESM cause #42 "syntactical error in the TFT operation"
    */
    if((E_TFT_OP_DELETE == tftHead->tftOp)
        ||(E_TFT_OP_NO_OP == tftHead->tftOp))
    {
        if(tftHead->numOfPf != 0)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: pf is not empty\n");
            return E_CFW_TFT_ERR_OP_SYNT;
        }
    }

    switch(tftHead->tftOp)
    {
        case E_TFT_OP_CREATE:
        case E_TFT_OP_ADD:
        case E_TFT_OP_REPLACE:
            errorCode = CFW_GprsTftMsgParseWithComponent(pTft,Ebi,&newTftPfHead);
            break;
        case E_TFT_OP_DEL_PF:
            errorCode = CFW_GprsTftDelPfMsgParse(pTft,Ebi,&newTftPfHead);
            break;
        case E_TFT_OP_NO_OP:
            break;
        case E_TFT_OP_DELETE:
            break;
        default:
            break;
    }

    if(errorCode != E_CFW_TFT_ERR_NO)
    {
        CFW_GprsTftFreeAll(&newTftPfHead);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: pf Component error\n");
        return errorCode;
    }
    
    /*
    d) Syntactical errors in packet filters
    1) When the TFT operation = "Create a new TFT", "Add packet filters to existing TFT",
       and two or more packet filters in the resultant TFT would
       have identical packet filter identifiers
    
       if two or more packet filters with identical packet filter identifiers are
       contained in the new request, the UE shall reject the modification request with
       ESM cause #45 "syntactical errors in packet filter(s)"
    */  
    if((E_TFT_OP_CREATE == tftHead->tftOp)
        ||(E_TFT_OP_ADD == tftHead->tftOp)
        ||(E_TFT_OP_DEL_PF == tftHead->tftOp))
    {
        if(CFW_GprsTftPfHasSameID(newTftPfHead,flagArr))
        {
            CFW_GprsTftFreeAll(&newTftPfHead);
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: same pf id error\n");
            errorCode = E_CFW_TFT_ERR_PF_SYNT;
            return errorCode;       
        }
    }

    /*
    d) Syntactical errors in packet filters
    2) When the TFT operation = "Create a new TFT", "Add packet filters to existing TFT"
       or "Replace packet filters in existing TFT", and two or more packet filters among
       all TFTs associated with this PDN connection would have identical packet filter precedence values
    
       UE shall reject the modification request with 
       ESM cause #45 "syntactical errors in packet filter(s)"
    */  
    if((E_TFT_OP_CREATE == tftHead->tftOp)
        ||(E_TFT_OP_ADD == tftHead->tftOp)
        ||(E_TFT_OP_REPLACE == tftHead->tftOp))
    {
        if(CFW_GprsNewTftHasSamePriority(newTftPfHead))
        {
            CFW_GprsTftFreeAll(&newTftPfHead);
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "CFW_GprsTftCheckInMod error: same priority error\n");
            errorCode = E_CFW_TFT_ERR_PF_SYNT;
            return errorCode;       
        }
    }

    /*
    d)  Syntactical errors in packet filters:
    2)  In case 2, if the old packet filters do not belong to the default EPS bearer 
        context, the UE shall not diagnose an error, shall further process the new 
        activation request and, if it was processed successfully, shall delete the 
        old packet filters which have identical filter precedence values. Furthermore, 
        by means of explicit peer-to-peer signalling between the network and the UE, 
        the UE shall perform a UE requested bearer resource modification procedure 
        to deactivate the EPS bearer context(s) for which it has deleted the packet 
        filters.
        In case 2, if one or more old packet filters belong to the default EPS bearer 
        context, the UE shall release the relevant PDN connection. If the relevant 
        PDN connection is the last one that the UE has and EMM-REGISTERED without 
        PDN connection is not supported by the UE or the MME, the UE shall detach 
        and re-attach to the network.
    */
    if((E_TFT_OP_CREATE == tftHead->tftOp)
        ||(E_TFT_OP_ADD == tftHead->tftOp)
        ||(E_TFT_OP_REPLACE == tftHead->tftOp))
    {   
        errorCode = CFW_GprsTftFindOldSamePrio(newTftPfHead,
                                               pTftPfList[nSimID][LinkEbi-5],
                                               pTftParse);
        
        if(E_CFW_TFT_ERR_SAME_PRIO_DEFAULT == errorCode)
        {
            CFW_GprsTftFreeAll(&newTftPfHead);
            //errorCode = E_CFW_TFT_ERR_PF_SYNT;
            return errorCode;       
        }   
    }

    pTftParse->TftOpCode = tftHead->tftOp;
    pTftParse->pTftPf    = newTftPfHead;
    return errorCode;   
}


// Packet Matching use Component
UINT8 CFW_GprsPfCpntMaching(CFW_PF_CPNT* pCpnt, UINT16 nDataSize, UINT8 *pData)
{
    UINT8  Ret       = TRUE;
    UINT8  HdrLen    = 0;
    UINT8  ProtId    = 0;
    UINT8  ToS_TC    = 0;
    UINT16 Port      = 0;
    UINT16 PortMin   = 0;
    UINT16 PortMax   = 0;
    UINT32 nValue1   = 0;
    UINT32 nValue2   = 0;
    UINT8  uMask[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if ( pCpnt == NULL )
        return FALSE;
    
    while ( pCpnt != NULL )
    {
        switch ( pCpnt->uCpntType )
        {
        case IPV4_REMOTE_ADDRESS_TYPE:
            if ( (pData[0] & 0xF0) != 0x40) /* Version NOT IPV4 */
            {
                Ret = FALSE;
            }
            else
            {
                if (FALSE == CFW_GprsAddrCompare(pCpnt->uCpntCont, pData + 16, pCpnt->uCpntCont + 4, 32)) 
                {
                    Ret = FALSE;
                }
            }
            break;

        case IPV4_LOCAL_ADDRESS_TYPE:
            if ( (pData[0] & 0xF0) != 0x40) /* Version NOT IPV4 */
            {
                Ret = FALSE;
            }
            else
            {
                if (FALSE == CFW_GprsAddrCompare(pCpnt->uCpntCont, pData + 12, pCpnt->uCpntCont + 4, 32)) 
                {
                    Ret = FALSE;
                }
            }
            break;

        case IPV6_REMOTE_ADDRESS_TYPE:
            if ( (pData[0] & 0xF0) != 0x60) /* Version NOT IPV6 */
            {
                Ret = FALSE;
            }
            else
            {
                if (FALSE == CFW_GprsAddrCompare(pCpnt->uCpntCont, pData + 24, pCpnt->uCpntCont + 16, 128)) 
                {
                    Ret = FALSE;
                }
            }
            break;

        case IPV6_REMOTE_ADDRESS_PREFIX_LENGTH_TYPE:
            if ( (pData[0] & 0xF0) != 0x60) /* Version NOT IPV6 */
            {
                Ret = FALSE;
            }
            else
            {
                if (FALSE == CFW_GprsAddrCompare(pCpnt->uCpntCont, pData + 24, uMask, pCpnt->uCpntCont[16])) 
                {
                    Ret = FALSE;
                }
            }
            break;

        case IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE:
            if ( (pData[0] & 0xF0) != 0x60) /* Version NOT IPV6 */
            {
                Ret = FALSE;
            }
            else
            {
                if (FALSE == CFW_GprsAddrCompare(pCpnt->uCpntCont, pData + 8, uMask, pCpnt->uCpntCont[16])) 
                {
                    Ret = FALSE;
                }
            }
            break;

        case PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                if ( pData[9] != pCpnt->uCpntCont[0] )
                    Ret = FALSE;
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                if ( pData[6] != pCpnt->uCpntCont[0] )
                    Ret = FALSE;
            }
            else
            {
                Ret = FALSE;
            }
            break;

        case SINGLE_LOCAL_PORT_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                HdrLen = (pData[0] & 0x0F)*4;
                ProtId = pData[9];
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                HdrLen = 40;
                ProtId = pData[6];
            }
            else
            {
                Ret = FALSE;
            }

            if( Ret == TRUE && (ProtId == CFW_TCPIP_IPPROTO_TCP 
                || ProtId == CFW_TCPIP_IPPROTO_UDP) )
            {
                Port    = (pData[HdrLen] << 8) | pData[HdrLen + 1];
                PortMin = (pCpnt->uCpntCont[0] << 8) | pCpnt->uCpntCont[1];

                if ( Port !=  PortMin)
                    Ret = FALSE;
            }
            break;

        case LOCAL_PORT_RANGE_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                HdrLen = (pData[0] & 0x0F)*4;
                ProtId = pData[9];
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                HdrLen = 40;
                ProtId = pData[6];
            }
            else
            {
                Ret = FALSE;
            }

            if( Ret == TRUE && (ProtId == CFW_TCPIP_IPPROTO_TCP 
                || ProtId == CFW_TCPIP_IPPROTO_UDP) )
            {
                Port    = (pData[HdrLen] << 8) | pData[HdrLen + 1];
                PortMin = (pCpnt->uCpntCont[0] << 8) | pCpnt->uCpntCont[1];
                PortMax = (pCpnt->uCpntCont[2] << 8) | pCpnt->uCpntCont[3];

                if ( Port <  PortMin || Port > PortMax)
                    Ret = FALSE;
            }
            break;

        case SINGLE_REMOTE_PORT_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                HdrLen = (pData[0] & 0x0F)*4;
                ProtId = pData[9];
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                HdrLen = 40;
                ProtId = pData[6];
            }
            else
            {
                Ret = FALSE;
            }

            if( Ret == TRUE && (ProtId == CFW_TCPIP_IPPROTO_TCP 
                || ProtId == CFW_TCPIP_IPPROTO_UDP) )
            {
                Port    = (pData[HdrLen + 2] << 8) | pData[HdrLen + 3];
                PortMin = (pCpnt->uCpntCont[0] << 8) | pCpnt->uCpntCont[1];

                if ( Port !=  PortMin)
                    Ret = FALSE;
            }
            break;

        case REMOTE_PORT_RANGE_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                HdrLen = (pData[0] & 0x0F)*4;
                ProtId = pData[9];
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                HdrLen = 40;
                ProtId = pData[6];
            }
            else
            {
                Ret = FALSE;
            }

            if( Ret == TRUE && (ProtId == CFW_TCPIP_IPPROTO_TCP 
                || ProtId == CFW_TCPIP_IPPROTO_UDP) )
            {
                Port    = (pData[HdrLen + 2] << 8) | pData[HdrLen + 3];
                PortMin = (pCpnt->uCpntCont[0] << 8) | pCpnt->uCpntCont[1];
                PortMax = (pCpnt->uCpntCont[2] << 8) | pCpnt->uCpntCont[3];

                if ( Port <  PortMin || Port > PortMax)
                    Ret = FALSE;
            }
            break;

        case SECURITY_PARAMETER_INDEX_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                HdrLen = (pData[0] & 0x0F)*4;
                ProtId = pData[9];

                if (ProtId == 51) /*AH*/
                    HdrLen += 4;
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                HdrLen = 40;
                ProtId = pData[6];
                
                if (ProtId == 51) /*AH*/
                    HdrLen += 4;
            }
            else
            {
                Ret = FALSE;
            }

            /* AH and ESP */
            if( Ret == TRUE && (ProtId == 50 || ProtId == 51) )
            {
                nValue1 = (pData[HdrLen + 0] << 24) | (pData[HdrLen + 1] << 16) | (pData[HdrLen + 2] << 8) | (pData[HdrLen + 3]);
                nValue2 = (pCpnt->uCpntCont[0] << 24) | (pCpnt->uCpntCont[1] << 16) | (pCpnt->uCpntCont[2] << 8) | (pCpnt->uCpntCont[3]);

                if ( nValue1 != nValue2)
                    Ret = FALSE;
            }
            break;

        case TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE:
            if ( (pData[0] & 0xF0) == 0x40) /* Version IPV4 */
            {
                ToS_TC = pData[1];
            }
            else if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                ToS_TC = ((pData[0]&0x0F)<<4)|((pData[1] & 0xF0)>>4);
            }
            else
            {
                Ret = FALSE;
            }

            if ( Ret == TRUE )
            {
                if ( (ToS_TC & pCpnt->uCpntCont[1]) != (pCpnt->uCpntCont[0] & pCpnt->uCpntCont[1]) )
                    Ret = FALSE;
            }
            break;

        case FLOW_LABEL_TYPE:
            if ( (pData[0] & 0xF0) == 0x60) /* Version IPV6 */
            {
                nValue1 = ((pData[1]&0x0F)<<16) | (pData[2]<<8) | pData[3];
                nValue2 = ((pCpnt->uCpntCont[0]&0x0F)<<16) | (pCpnt->uCpntCont[1]<<8) | pCpnt->uCpntCont[2];

                if ( (nValue1&0xFFFFF) != (nValue2&0xFFFFF) )
                    Ret = FALSE;
            }
            else
            {
                Ret = FALSE;
            }
            break;

        default:
            break;
        }

        if ( Ret == TRUE )
        {
            pCpnt = pCpnt->pNext;
        }
        else
        {
            break;
        }
    }

    return Ret;
}


// Packe Filter Matching, Find the currect EBI
UINT8 CFW_GprsPfMatching(CFW_SIM_ID nSimID, UINT8 LinkEbi, UINT16 nDataSize, UINT8 *pData)
{
    CFW_TFT_PF* pTftPf = NULL; 

    /* Invalid Parameter */
    if( LinkEbi < 5 || LinkEbi > 15)
        return 0xFF;

    pTftPf = pTftPfList[nSimID][LinkEbi-5];

    /* if no PF existed, use LinkEbi */
    if ( pTftPf == NULL )
        return LinkEbi;

    while (pTftPf != NULL)
    {
        if ((pTftPf->uPF_Dir & 0x02)
            &&( TRUE ==  CFW_GprsPfCpntMaching( pTftPf->Cpnt, nDataSize, pData) ))
        {
            return pTftPf->uEBI;
        }
        else
        {
            /* if Linked EBI found and not match, ignore Linked EBI */
            if ( pTftPf->uEBI == LinkEbi )
                LinkEbi = 0xFF;
                
            pTftPf = pTftPf->pNext;
        }
    }

    /* No Matching PF Found, use LinkEbi */
    return LinkEbi;
}

UINT8 CFW_GprsSetTfaContent(UINT8 *Content, CFW_TFT_SET* pTftSet)
{
    UINT8 Pos = 0;

    if ( pTftSet->Pf_BitMap & 0x2000 )
    {
        Content[Pos++] = IPV4_REMOTE_ADDRESS_TYPE;
        SUL_MemCopy8(Content + Pos, pTftSet->Pf_RmtAddrIPv4, 8);
        Pos += 8;
    }
    else if ( pTftSet->Pf_BitMap & 0x1000 )
    {
        Content[Pos++] = IPV6_REMOTE_ADDRESS_TYPE;
        SUL_MemCopy8(Content + Pos, pTftSet->Pf_RmtAddrIPv6, 32);
        Pos += 32;
    }

    if ( pTftSet->Pf_BitMap & 0x0800 )
    {
        Content[Pos++] = PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE;
        Content[Pos++] = pTftSet->Pf_PortNum_NH;
    }
    
    if ( pTftSet->Pf_BitMap & 0x0400 )
    {
        Content[Pos++] = LOCAL_PORT_RANGE_TYPE;
        Content[Pos++] = (UINT8)(pTftSet->Pf_LocalPortRangeMin>>8);
        Content[Pos++] = (UINT8)(pTftSet->Pf_LocalPortRangeMin&0xFF);
        Content[Pos++] = (UINT8)(pTftSet->Pf_LocalPortRangeMax>>8);
        Content[Pos++] = (UINT8)(pTftSet->Pf_LocalPortRangeMax&0xFF);
    }
    
    if ( pTftSet->Pf_BitMap & 0x0200 )
    {
        Content[Pos++] = REMOTE_PORT_RANGE_TYPE;
        Content[Pos++] = (UINT8)(pTftSet->Pf_RemotPortRangeMin>>8);
        Content[Pos++] = (UINT8)(pTftSet->Pf_RemotPortRangeMin&0xFF);
        Content[Pos++] = (UINT8)(pTftSet->Pf_RemotPortRangeMax>>8);
        Content[Pos++] = (UINT8)(pTftSet->Pf_RemotPortRangeMax&0xFF);
    }
    
    if ( pTftSet->Pf_BitMap & 0x0100 )
    {
        Content[Pos++] = SECURITY_PARAMETER_INDEX_TYPE;
        Content[Pos++] = (UINT8)((pTftSet->Pf_SPI>>24)&0xFF);
        Content[Pos++] = (UINT8)((pTftSet->Pf_SPI>>16)&0xFF);
        Content[Pos++] = (UINT8)((pTftSet->Pf_SPI>>8)&0xFF);
        Content[Pos++] = (UINT8)((pTftSet->Pf_SPI)&0xFF);
    }
    
    if ( pTftSet->Pf_BitMap & 0x0080 )
    {
        Content[Pos++] = TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE;
        Content[Pos++] = pTftSet->Pf_TOS_TC[0];
        Content[Pos++] = pTftSet->Pf_TOS_TC[1];
    }
    
    if ( pTftSet->Pf_BitMap & 0x0040 )
    {
        Content[Pos++] = FLOW_LABEL_TYPE;
        Content[Pos++] = (UINT8)((pTftSet->Pf_FlowLabel>>16)&0x0F);
        Content[Pos++] = (UINT8)((pTftSet->Pf_FlowLabel>>8)&0xFF);
        Content[Pos++] = (UINT8)((pTftSet->Pf_FlowLabel)&0xFF);
    }
    
    if ( pTftSet->Pf_BitMap & 0x0010 )
    {
        Content[Pos++] = IPV4_LOCAL_ADDRESS_TYPE;
        SUL_MemCopy8(Content + Pos, pTftSet->Pf_LocalAddrIPv4, 8);
        Pos += 8;
    }
    else if ( pTftSet->Pf_BitMap & 0x0008 )
    {
        Content[Pos++] = IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE;
        SUL_MemCopy8(Content + Pos, pTftSet->Pf_LocalAddrIPv6, 16);
        Pos += 16;
        Content[Pos++] = CFW_GprsGetIPv6Prefix(pTftSet->Pf_LocalAddrIPv6 + 16);
    }

    return Pos;    
}


// Set TFA For Allocation Procedure
VOID CFW_GprsSetTfaForAlloc(CFW_TFT *pTfa, CFW_TFT_SET *pTftSet)
{
    UINT8 Pos   = 1;
    UINT8 PfNum = 0;
    UINT8 Pf_ID, Pf_PreIdx, Pf_Dir;
    UINT8 Length;

    /* Set TFT Operation Code */
    pTfa->uTFT[0] = 0x20; /*0 0 1 Create new TFT */

    while ( pTftSet != NULL && pTftSet->Pf_BitMap != 0 && PfNum < 16 )
    {
        Pf_ID     = pTftSet->Pf_ID;
        Pf_PreIdx = pTftSet->Pf_EvaPreIdx;
        PfNum ++ ;

        /* Direction  */
        if ( pTftSet->Pf_BitMap & 0x0020 ) 
            Pf_Dir = pTftSet->Pf_Direct;
        else
            Pf_Dir = 3;
        
        pTfa->uTFT[Pos++] = ( (Pf_Dir << 4) | (Pf_ID & 0x0F) );
        pTfa->uTFT[Pos++] = Pf_PreIdx;

        Length = CFW_GprsSetTfaContent(&pTfa->uTFT[Pos+1], pTftSet);

        pTfa->uTFT[Pos++] = Length;
        Pos += Length;

        pTftSet = pTftSet->pNext;
    }
    
    pTfa->uLength  = Pos;
    pTfa->uTFT[0] |= PfNum;
}

// Set TFA For Modify Procedure
VOID CFW_GprsSetTfaForMod(CFW_TFT *pTfa, CFW_TFT_SET *pTftSet, UINT8 LinkEbi, UINT8 Ebi, UINT8 nSimID)
{
    UINT8       Pos   = 1;
    UINT8       PfNum = 0;
    UINT8       Pf_ID, Pf_PreIdx, Pf_Dir;
    UINT8       Length;
    UINT8       bReplace = FALSE;
    CFW_TFT_PF* pTftPf = pTftPfList[nSimID][LinkEbi-5];

    if((nSimID >= CFW_SIM_COUNT) ||((LinkEbi > 15)||(LinkEbi < 5)) 
        || ((Ebi > 15)||(Ebi < 5)))    
    {
        pTfa->uLength = 1;
        pTfa->uTFT[0] = 0xC0;
        return ;
    }

    /* Find Same PF ID */
	if(pTftPf == NULL)
	{
	    bReplace = TRUE;
	}
	else
	{
	    while ( pTftPf != NULL )
	    {
	        if ( pTftPf->uPF_ID == pTftSet->Pf_ID )
	        {
	            bReplace = TRUE;
	            break;
	        }
	        else
	        {
	            pTftPf = pTftPf->pNext;
	        }
	    }
	}

    /* Only ID and Pre Ideneity */
    if ( pTftSet->Pf_BitMap == 0xC000)
    {
        pTfa->uTFT[0] =0xA0; /* 1 0 1 Delete packet filters from existing TFT */
        while ( pTftSet != NULL )
        {
            pTfa->uTFT[Pos++] = (pTftSet->Pf_ID & 0x0F);
            PfNum ++ ;
        }     
    }
    else
    {
        if ( bReplace == TRUE )
            pTfa->uTFT[0] =0x80; /* 1 0 0 Replace packet filters in existing TFT */
        else
            pTfa->uTFT[0] =0x60; /* 0 1 1 Add packet filters to existing TFT */
        
        while ( pTftSet != NULL && pTftSet->Pf_BitMap != 0 && PfNum < 16 )
        {
            Pf_ID     = pTftSet->Pf_ID;
            Pf_PreIdx = pTftSet->Pf_EvaPreIdx;
            PfNum ++ ;

            /* Direction  */
            if ( pTftSet->Pf_BitMap & 0x0020 ) 
                Pf_Dir = pTftSet->Pf_Direct;
            else
                Pf_Dir = 3;
            
            pTfa->uTFT[Pos++] = ( (Pf_Dir << 4) | (Pf_ID & 0x0F) );
            pTfa->uTFT[Pos++] = Pf_PreIdx;

            Length = CFW_GprsSetTfaContent(&pTfa->uTFT[Pos+1], pTftSet);

            pTfa->uTFT[Pos++] = Length;
            Pos += Length;

            pTftSet = pTftSet->pNext;
        }

    }

    pTfa->uLength  = Pos;
    pTfa->uTFT[0] |= PfNum;    
}

VOID CFW_NbiotSendData( UINT8 linkeEbi, UINT16 length, VOID* pData , UINT8 rai, UINT16 type_of_user_data, UINT8 nSimID, UINT8 vDataNum)
{

    UINT8 nSelectedEbi = 0;

    if ( linkeEbi != 0 )
    {
        /* TFT Matching */
        nSelectedEbi = CFW_GprsPfMatching(nSimID, linkeEbi, length, pData);

        if ( nSelectedEbi == 0xFF )
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "No EBI Found in TFT Match\n");
            return ;
        }
        else
        {
            /* Send to Protocol Stack */
            CFW_SendPduData(nSelectedEbi,  length, pData, rai, type_of_user_data,nSimID, vDataNum);
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "Invalid linkeEbi\n");
    }
}


// Used By TC in Test Loop Mode B, Request to Loop Data Back
UINT8 CFW_GprsSendTcData( UINT8 ebi, UINT8 nSimID, UINT16 length, VOID* pData )
{
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    UINT8 nSelectedEbi = 0;
    GPRS_SM_INFO *proc = NULL;      //  sm info
    HAO hAo = 0;                  //  ao
    UINT8 ret = 0;
    
    /* Find the ebi  */
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);

    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nNsapi == ebi && proc->nActState == CFW_GPRS_ACTIVED)
        {
            /* Get Linked EBI */
            nSelectedEbi = proc->Cid2Index.nLinkedEbi;
             CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), \
                "CFW_GprsSendTcData, 1 ebi:%d", nSelectedEbi);

            break;
        }
    }
    if ( nSelectedEbi != 0 )
    {
        /* TFT Matching */
        ret= CFW_GprsPfMatching(nSimID, nSelectedEbi, length, pData);
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), \
                "CFW_GprsSendTcData, 2 ebi:%d", ret);
        return ret;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), \
                "CFW_GprsSendTcData, 3 ebi return 0xff");
        return 0xFF;
    }

}


VOID* CFW_GprsGetCid2IndexInfo(UINT8 nCid, UINT8 nSimID)
{
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    HAO   hHAO = 0;
    GPRS_SM_INFO *pProc = NULL;
        
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hHAO = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        pProc = CFW_GetAoUserData(hHAO);
        
        if ( nCid == pProc->Cid2Index.nCid)
        {
            return &pProc->Cid2Index;
        }
    }
    
    return NULL;
}


UINT8 CFW_GprsGetCidFromNsapi(UINT8 Nsapi, UINT8 nSimID)
{
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    HAO   hHAO = 0;
    GPRS_SM_INFO *pProc = NULL;
        
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hHAO = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        pProc = CFW_GetAoUserData(hHAO);
        
        if ( Nsapi == pProc->Cid2Index.nNsapi )
        {
            return pProc->Cid2Index.nCid;
        }
    }
    
    return 0xFF;
}


UINT8 CFW_GprsSetCidContext(UINT8 PCid, UINT8 LinkEBI, UINT8 nSimID)
{
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    HAO   hHAO = 0;
    GPRS_SM_INFO *pProc = NULL;
        
    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hHAO = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        pProc = CFW_GetAoUserData(hHAO);
        
        if ( LinkEBI == pProc->Cid2Index.nLinkedEbi 
          && PCid != pProc->Cid2Index.nCid )
        {
            AT_GprsSetPCid(pProc->Cid2Index.nCid, PCid, nSimID);
        }
    }
    
    return 0xFF;
}


UINT32 CFW_GprsSetQosCtx(UINT8 nCid, CFW_EQOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQosList[nSimID][nCid - 1].nMaximumbitrateUL = pQos->nUlMbr;
    pQosList[nSimID][nCid - 1].nMaximumbitrateDL = pQos->nDlMbr;
    pQosList[nSimID][nCid - 1].nGuaranteedbitrateUL = pQos->nUlGbr ;
    pQosList[nSimID][nCid - 1].nGuaranteedbitrateDL = pQos->nDlGbr;
    return ret;
}

UINT32 CFW_GprsGetQosCtx(UINT8 nCid, CFW_EQOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    if( (nCid > 7) || (nCid < 1) || (pQos == NULL) )
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    pQos->nUlMbr=   pQosList[nSimID][nCid - 1].nMaximumbitrateUL;
    pQos->nDlMbr=   pQosList[nSimID][nCid - 1].nMaximumbitrateDL;
    pQos->nUlGbr=   pQosList[nSimID][nCid - 1].nGuaranteedbitrateUL;
    pQos->nDlGbr=   pQosList[nSimID][nCid - 1].nGuaranteedbitrateDL;
    return ret;
}


VOID CFW_NvParasChangedNotification(UINT8  ChangedValue)
{
      api_NvParasChangeInd_t *pOutMsg = NULL;
      pOutMsg = (api_NvParasChangeInd_t *)CFW_MemAllocMessageStructure(SIZEOF(api_NvParasChangeInd_t));
      if(pOutMsg != NULL)
      {
          pOutMsg->changedValue = ChangedValue;
          CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("NAS NV Paras changedValue: %d\n",0x0810084b)), ChangedValue);
          CFW_SendSclMessage(API_NV_CHANGE_IND, pOutMsg, 0);
      }
}


void CFW_NBIOT_ReleaseDlData(CFW_EV *pEvent)
{
    api_PSDataIndLte_t *psInMsg = (api_PSDataIndLte_t *)pEvent->nParam1;
    T_ComSduLte*     pDlDataNode = psInMsg->pUserData; 
    T_ComSduLte*     pDlDataFree = pDlDataNode; 
    while(pDlDataFree)
    {
        pDlDataNode = pDlDataNode->pNext;
        tgl00_170FreeDlIpData(pDlDataFree->pSduDataFree);
        tgl00_168FreeDlNode(pDlDataFree);
	 pDlDataFree = pDlDataNode;
    }
}

//
UINT8 CFW_GprsGetPdpIpv4Addr(UINT8 nCid, UINT8 *nLength, UINT8 *ipv4Addr, CFW_SIM_ID nSimID)
{
    UINT8  pdpAddr[18] = {0};
    if(ERR_SUCCESS == CFW_GprsGetPdpAddr(nCid, nLength, pdpAddr, nSimID))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("addrLen: %d \n",0x081007e0)), *nLength);
        CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, pdpAddr, *nLength, 16);
        if(ipv4Addr)
        {
            if(*nLength == 4)
            {
                SUL_MemCopy8(ipv4Addr, pdpAddr, 4);
	         //*nLength = 4;
	         return 0;
            }
	     else if(*nLength == 12)
	     {
	         SUL_MemCopy8(ipv4Addr, pdpAddr+8, 4);
	         *nLength = 4;
	          return 0;
	     }
	     else return 1;
        }
        else return 1;
    }
    else return 1;
}


UINT8 CFW_GprsGetPdpIpv6IfId(UINT8 nCid, UINT8 *nLength, UINT8 *if_id, CFW_SIM_ID nSimID)
{
    UINT8  pdpAddr[18] = {0};
    CFW_GprsGetPdpAddr(nCid, nLength, pdpAddr, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("addrLen: %d \n",0x081007e0)), *nLength);
    CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, pdpAddr, *nLength, 16);
    if((if_id)&&(*nLength >= 8))
    {
        SUL_MemCopy8(if_id, pdpAddr, 8);
	 *nLength = 8;
	 return 0;
    }
    else return 1;
}


const ip6_addr_t *CFW_GprsGetPdpIpv6AddressFromDest(UINT8 nCid, const ip6_addr_t *dest, UINT8 nSim)
{
    ip6_addr_t *ip6 = NULL;
    struct netif * inp = getGprsNetIf(nSim, nCid);
    if(inp)
    {
        ip6 = ip_2_ip6(ip6_select_source_address(inp, dest));
	 ip6_addr_debug_print(SOCKETS_DEBUG, ip6);
    }
    return ip6;
}


const ip6_addr_t *CFW_GprsGetPdpIpv6Address(UINT8 nCid, UINT8 nSim)
{
    ip6_addr_t *ip6 = NULL;
    struct netif * inp = getGprsNetIf(nSim,nCid);
    if(inp)
    {
        ip6 =  ip6_try_to_select_source_address(inp);
	 ip6_addr_debug_print(SOCKETS_DEBUG, ip6);
    }
    return ip6;
}



UINT32 CFW_GetFreeCID_(UINT8 *pCID, CFW_SIM_ID nSimID);
//UINT8  tgl00_14CheckTestSim(void);

// process the EPS MT activation
// register a dynamic ao and post ind message to up layer
// Ind: EV_CFW_EPS_CXT_ACTIVE_IND
// the parameter of the ind ??? should add more?
HAO EpsMTAoProc(CFW_EV *pEvent)
{
    UINT8  nCID = 0;
    UINT8  nPCID = 0;
    HANDLE hSm;
    GPRS_SM_INFO *proc = NULL;
    api_EpsCtxActivateInd_t  *psInMsg = NULL;
    UINT32 nRet = ERR_SUCCESS;

    CFW_SIM_ID nSimID = pEvent->nFlag;
    proc = (GPRS_SM_INFO *)CSW_GPRS_MALLOC(SIZEOF(GPRS_SM_INFO)); //  + 80
    if(proc == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8(proc, SIZEOF(GPRS_SM_INFO)); // +80
    psInMsg = (api_EpsCtxActivateInd_t *)pEvent->nParam1;

    // Get nCid From 
    if ( psInMsg->actType == 1/*ACTIVED_BY_UE_ALLOCATION*/)
    {
        UINT8 nAoCount = 0;
        UINT8 nAoIndex = 0;
        HAO   hAo = 0;
        GPRS_SM_INFO *pUserData = NULL;
        
        CFW_SetServiceId(CFW_GPRS_SRV_ID);
        nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
        
        for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
        {
            hAo = CFW_GetAoHandle(nAoIndex, CFW_GPRS_STATE_EPS_ALLOCATING, CFW_GPRS_SRV_ID, nSimID);
            if( hAo !=  NULL )
            {
                pUserData = CFW_GetAoUserData(hAo);
                nCID = pUserData->nSecActCid;
                break;
            }
        }

        if ( nCID == 0 )
        {
            CSW_GPRS_FREE( proc );
            return ERR_CFW_NOT_OPERATOR_ID;
        }
    }
    // Get free cid from active ao modify, Except CID 1, CID Reserved for Default EPS Bearer
    else if (ERR_SUCCESS == CFW_GetFreeCID_(& nCID, nSimID))
    {
        if (ERR_SUCCESS != CFW_SetCID(nCID, nSimID))
        {
            CSW_GPRS_FREE( proc );
            return ERR_CFW_INVALID_PARAMETER;
        }
        
        nPCID = CFW_GprsGetCidFromNsapi(psInMsg->LinkedEbi, nSimID);
        
        if ( nPCID != 0xFF )
            AT_GprsSetPCid(nCID, nPCID, nSimID);
    }
    else
    {
        CSW_GPRS_FREE( proc );
        return ERR_CFW_NOT_OPERATOR_ID;
    }

    proc->Cid2Index.nCid       = nCID;
    proc->Cid2Index.nNsapi     = psInMsg->Ebi;
    proc->Cid2Index.nSmIndex   = psInMsg->SmIndex;
    proc->Cid2Index.nLinkedEbi = psInMsg->LinkedEbi;

    proc->nActState = CFW_GPRS_DEACTIVED;

    hSm = CFW_RegisterAo(CFW_GPRS_SRV_ID, proc, CFW_GprsAoProc, nSimID);

    CFW_SetAoState(hSm, CFW_GPRS_STATE_IDLE);
    
    return hSm;
}

// AT+CGACT
// Sec PDP context activate or deactivate
// nState:
// CFW_GPRS_DEACTIVED 0 deactivated
// CFW_GPRS_ACTIVED 1 activated
// event:EV_CFW_GPRS_ACT_NOTIFY
// parameter1:cid[1 - CFW_PDPCID_MAX]
// parameter2:0
// type: 0 or 1 to distinguish the active or deactive or 0xF0 to distinguish the rejection.

// return : ERR_CFW_GPRS_HAVE_ACTIVED
// ERR_CFW_UTI_IS_BUSY
// ERR_CFW_GPRS_INVALID_CID
UINT32 CFW_GprsSecPdpAct(UINT8 nState, UINT8 nCid, UINT8 nPCid, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL, *pCidProc = NULL;      //  sm info
    HAO hAo = 0, pCidhAo = 0;                         //  ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    if((nCid > CFW_PDPCID_MAX) || (nCid < 1))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR !CID is INVALID\r\n");
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((nPCid > CFW_PDPCID_MAX) || (nPCid < 1))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR !PCID is INVALID\r\n");
        return ERR_CFW_INVALID_PARAMETER;
    }    
    if ((nState != CFW_GPRS_DEACTIVED) && (nState != CFW_GPRS_ACTIVED))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR !ACTIVED OR DEACTIVE STATUS error\r\n");
        return ERR_CFW_INVALID_PARAMETER;
    }
    if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSimID))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! Sec PDP Act not allowed in GSM\r\n");
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
    
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! SIM id error CFW_GprsSecPdpAct()\r\n");

        return nRet;
    }

    //add ,do active need check PS status, do deactive need not //2009-11-02
    HAO s_hAo = 0;              // staitc ao
    NW_SM_INFO *s_proc = NULL;  // static sm

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);

    if( (nState == CFW_GPRS_ACTIVED) && (s_proc->sGprsStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! PS status is NO SERVICE\r\n");
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }

    // get ao handle according to tha cid
    //
    CFW_SetServiceId(CFW_GPRS_SRV_ID);

    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            if (proc->nActState == nState)
            {
                if(nState == CFW_GPRS_ACTIVED)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR !HAVE ACTIVED\r\n");
                    return ERR_CFW_GPRS_HAVE_ACTIVED; // already actived or deactived should be replaced by macro
                }
                if(nState == CFW_GPRS_DEACTIVED)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR !HAVE DEACTIVED\r\n");
                    return ERR_CFW_GPRS_HAVE_DEACTIVED; // should be ERR_CFW_GPRS_HAVE_DEACTIVED
                }
            }
            else
            {
                if (nState == CFW_GPRS_DEACTIVED)  // deactive
                {
                    UINT8  nActUTI = 0;
                    // send deactive message to stack
                    proc->nMsgId = API_PDP_CTXDEACTIVATE_REQ;
                    proc->pMsgBody = NULL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "send deactive req, ao proc cont : 0x%x\n", nUTI);

                    /* nUTI == 0xFFFF, This deactivate Request is start by CFW self */
                    proc->bDeactByCfw = FALSE;
                    if( nUTI == 0xFFFF )
                    {
                        CFW_GetFreeUTI(0, &nUTI);
                        proc->bDeactByCfw = TRUE;
                    }

                    CFW_GetUsingUTI(hAo, &nActUTI);
                    CFW_RelaseUsingUTI(0, nActUTI);
                    CFW_SetUTI(hAo, nUTI, 0);

                    /* Delete Packet Filter */
                    CFW_GprsDelPFInDeactProc(&proc->Tfa, proc->Cid2Index.nNsapi, proc->Cid2Index.nLinkedEbi, nSimID);

                    nRet = CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
                    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "CFW_act  CFW_SetAoProcCode : 0x%x\n", nRet);

                    CSW_ASSERT(nRet == ERR_SUCCESS);

                    return ERR_SUCCESS;
                }
                else // should not be CFW_GPRS_ACTIVED ? maybe the MT exist.
                {
                    return ERR_SUCCESS;
                }

            }
        }

        /* Save PCid's HAO and PROC */
        if (proc->Cid2Index.nCid == nPCid)
        {
            if (nState == CFW_GPRS_ACTIVED && proc->nActState == CFW_GPRS_DEACTIVED)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! PCid not Actived\r\n");
                return ERR_CME_OPERATION_NOT_ALLOWED;
            }

            /* Save HAO and Proc */
            pCidhAo  = hAo ;
            pCidProc = proc;
        }
    }

    // should be active here
    if (nState == CFW_GPRS_ACTIVED)   // Active
    {
        CFW_TFT_SET* pTftSet = NULL;
        
        if (pCidProc == NULL || pCidhAo == 0)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! PCid not Actived\r\n");
            return ERR_CME_OPERATION_NOT_ALLOWED;
        }

        CFW_GprsGetCtxTFT(nCid, &pTftSet, nSimID);

        //set nCid when the GPRS state is CFW_GPRS_ACTIVED add by qidd@20090317
        if (ERR_SUCCESS != CFW_SetCID(nCid, nSimID))
        {
            CFW_GprsClrCtxTFT(nCid, nSimID);
            return ERR_CFW_INVALID_PARAMETER;
        }

        /* PF Not Define, or Packet Filter Compnent Empty, Error */
        if ( pTftSet == NULL || pTftSet->Pf_BitMap == 0xC000)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! TFT Error When CGACT\r\n");
            CFW_GprsClrCtxTFT(nCid, nSimID);
            return ERR_CME_OPERATION_NOT_ALLOWED;
        }

        UINT8  nActUTI = 0;
        
        /* Bugzilla �C Bug 1805 */
        // send Allocate message to stack
        pCidProc->nMsgId = API_EPS_CTXALLOCATE_REQ;
        pCidProc->pMsgBody = NULL;
        pCidProc->nSecActCid = nCid;
        
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(1), "send allocate req, ao proc cont : 0x%x\n", nUTI);
        
        CFW_GetUsingUTI(pCidhAo, &nActUTI);
        CFW_RelaseUsingUTI(0, nActUTI);
        CFW_SetUTI(pCidhAo, nUTI, 0);
        
        nRet = CFW_SetAoProcCode(pCidhAo, CFW_AO_PROC_CODE_CONTINUE);
        
        CSW_ASSERT(nRet == ERR_SUCCESS);
        
        return ERR_SUCCESS;

    }
    else   // Deactive
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "warring! if(nState == CFW_GPRS_ACTIVED)\r\n");
        // error; should not be here
    }

    return ERR_SUCCESS;
}

// modify needed
// [in]UTI
// [in]nCid
// [in]Qos: point the qos set the Qos
// return: ERR_CFW_GPRS_INVALID_CID
// ERR_CFW_UTI_IS_BUSY
// ERR_CFW_INVALID_PARAMETER
// response: EV_CFW_GPRS_MOD_NOTIFY
// parameter1: Cid
// parameter1: 0
// UTI: uti
// type: 0
UINT32 CFW_GprsCtxEpsModify(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID)
{
    GPRS_SM_INFO *proc = NULL;    //  sm info
    HAO hAo  = 0;                  //  ao
    CFW_EQOS     EQoS; 
    CFW_TFT_SET *pTftSet; 
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;
    UINT8 nActState = 0xFF;
    UINT8  nActUTI = 0;
    UINT32 nRet = ERR_SUCCESS;
    
    nRet = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != nRet)
    {
        //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(0), "ERROR ! SIM id error CFW_GprsCtxModify()\r\n",0x081007fe);

        return nRet;
    }
    CFW_GetGprsActState(nCid, &nActState, nSimID);
    if(nActState != CFW_GPRS_ACTIVED)
        return ERR_CME_OPERATION_NOT_ALLOWED;
    //add end
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TNB_ARG(2), "CFW_GprsCtxModify cid = 0x%x, nUTI = 0x%x\n", nCid, nUTI);
    if((nCid > CFW_PDPCID_MAX) || (nCid < 1))
        return ERR_CFW_INVALID_PARAMETER;

    if( nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_GprsCtxModify()\r\n",0x081007fe)));
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);
        proc = CFW_GetAoUserData(hAo);
        if (proc->Cid2Index.nCid == nCid)
        {
            proc->nMsgId = API_PDP_CTXMODIFY_REQ;
            proc->pMsgBody = NULL;

            CFW_GetUsingUTI(hAo, &nActUTI);
            CFW_RelaseUsingUTI(0, nActUTI);
            CFW_SetUTI(hAo, nUTI, 0);

            CFW_GprsGetCtxEQos(nCid, &EQoS, nSimID);

            if( 0 != SUL_MemCompare(&EQoS, &proc->EQos, SIZEOF(CFW_EQOS)) )
                SUL_MemCopy8(&proc->ModifyEQos, &EQoS, SIZEOF(CFW_EQOS));
            else
                proc->ModifyEQos.nQci = 0xFF;

            CFW_GprsGetCtxTFT(nCid, &pTftSet, nSimID);

            if ( pTftSet == NULL )
            {
                /* no TFT operation */
                proc->Tfa.uLength = 1;
                proc->Tfa.uTFT[0] = 0xC0;
            }
            else
            {
                CFW_GprsSetTfaForMod(&proc->Tfa, pTftSet, proc->Cid2Index.nLinkedEbi, proc->Cid2Index.nNsapi, nSimID);
            }

            CFW_GprsClrCtxTFT(nCid, nSimID);

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE); // trigger the aom
            return ERR_SUCCESS;
        }
    }
    return ERR_CFW_GPRS_INVALID_CID;   // there is no context according to the cid;
}

UINT32 CFW_GetFreeCID_(UINT8 *pCID, CFW_SIM_ID nSimID)
{
    UINT8  n = 0, i = 0;
    UINT32 temp = 0x00;
    UINT32 ret = ERR_SUCCESS;
    UINT32 StartCid = 1;
    
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        return ret;
    }

#if 0
    /* In test Env, Cid Start with 5 */
    if (TRUE == tgl00_14CheckTestSim())
    {
        StartCid = 5;
    }
#endif

    for(i = 0; i < 8; i++)
    {
        for(n = 1; n < 33; n++)
        {
            temp = storecid[nSimID][i] & (1 << (n - 1));
            if (0 == temp && StartCid < (i * 32 + n))
            {
                *pCID = (i * 32 + n);

                if( *pCID > MAX_CID_NUM )
                    return ERR_CFW_NOT_OPERATOR_ID;
                else
                    return ERR_SUCCESS;
            }
        }
    }

    return ERR_CFW_NOT_OPERATOR_ID;
}


#endif //LTE_NBIOT_SUPPORT


#endif

