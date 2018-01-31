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
// #include <scl.h>
#include "api_msg.h"
#include "cmn_defs.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"
#include "itf_api.h"
#include "hal_uart.h"
// #include "cfw_cfg.h"
#include "cfw_gprs_data.h"

#if (CFW_NW_DUMP_ENABLE==0) && (CFW_NW_SRV_ENABLE==1)
//#define  TEST_SPECIAL_PLMN
#include "cfw_nw_data.h"
#include "cfw_nw_tool.h"

#ifdef AT_MODULE_SUPPORT_OTA
extern S8 gHomePLMNNum[NUMBER_OF_SIM][6+1];
extern U8 gPLMN[NUMBER_OF_SIM][6+1];
#endif

extern BOOL sxs_ProtoStackAvail (void);
extern BOOL gSatFreshComm[];

extern  CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;
extern UINT8 gprs_flow_ctrl_G[CFW_SIM_COUNT] ;  //simid, cid

extern UINT32 CFW_StopDetachTimer(CFW_SIM_ID nSimID);
PUBLIC UINT32 CFW_StackInit(CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwStop(BOOL bPowerOff, UINT16 nUTI, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwImeiConf(api_ImeiInd_t *pImei, NW_SM_INFO *pSmInfo, UINT32 nUTI, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwQualReportConf(api_QualReportCnf_t *pQualReport, NW_SM_INFO *pSmInfo, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwListConf(api_NwListInd_t *pListInd, NW_SM_INFO *pSmInfo, UINT32 nUTI, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwCsStatusConf(api_NwCsRegStatusInd_t *pCsRegStatus, NW_SM_INFO *pSmInfo, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwPsStatusConf(api_NwPsRegStatusInd_t *pPsRegStatus, NW_SM_INFO *pSmInfo, UINT32 nUTI,
                                  CFW_SIM_ID nSimID);
UINT32 CFW_NW_SendDetectMBSMessage(UINT8 nMode, CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwStartStack(UINT8 nBand,
#ifdef __MULTI_RAT__
                                UINT8 nUBand,
#endif
#ifdef CSW_USER_DBS
                                UINT8 nStrtFlag,
#endif
                                CFW_SIM_ID nSimID);

#ifdef CFW_GPRS_SUPPORT
PRIVATE VOID CFW_GprsDeactiveAll(CFW_SIM_ID nSimID);
#endif
extern CFW_SIM_OC_STATUS SIMOCStatus[];
UINT8 CFW_NWGetRat(CFW_SIM_ID nSimID);
extern VOID SendRefreshSclMsg(UINT8 nSAT_Status, CFW_SIM_ID nSimID);


extern UINT32 CFW_CfgNwGetCompanyName (UINT8 pOperatorId[6],UINT8 **Company);

extern UINT8 CFW_GetAutoCallCmd(CFW_SIM_ID nSimID);
PRIVATE UINT32 CFW_NwAoProc(HAO hAo, CFW_EV *pEvent);
PRIVATE HAO NetWorkTimerProc(CFW_EV *pEvent);
PRIVATE VOID CFW_IMSItoHomePlmn(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen);
UINT32 CFW_ChangeRoamByPLMNName(api_NwCsRegStatusInd_t *pCsRegStatus, UINT8 *nOperatorId, CFW_SIM_ID nSimID);
#define NW_UTI_SIM NW_UTI_MIN + 0
UINT32 CFW_SetQualReport(BOOL bStart, CFW_SIM_ID nSimID)
{
    api_QualReportReq_t *pOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SetQualReport %d\n",0x0810084d)), bStart);

    pOutMsg              = (api_QualReportReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_QualReportReq_t));
    pOutMsg->StartReport = bStart;

    CFW_SendSclMessage(API_QUAL_REPORT_REQ, pOutMsg, nSimID);

    return ERR_SUCCESS;
}
PRIVATE UINT32 CFW_SetFMSIMOpen(HAO hAo, CFW_EV *pEvent)
{
    NW_FM_INFO *pFMInfo;
    UINT32 nUTI = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen \n",0x0810084e)));

    if ((hAo == 0) || (pEvent == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen  ao or pEvent ERROR!!! \n",0x0810084f)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);

    CFW_GetUTI(hAo, &nUTI);

    pFMInfo = CFW_GetAoUserData(hAo);

    switch (pFMInfo->n_CurrStatus)
    {
        case CFW_SM_NW_STATE_IDLE:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen CFW_SM_NW_STATE_IDLE \n",0x08100850)));

            if ((UINT32)pEvent == 0xFFFFFFFF)
            {

                if (pFMInfo->nTryCount >= 0x01)
                {
                    CFW_SIM_STATUS sSimStatus = CFW_SIM_STATUS_END;

                    sSimStatus = CFW_GetSimStatus(nSimID);

                    if (CFW_SIM_ABSENT == sSimStatus)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SetFlightModeProc NO SIM  simid %d\n",0x08100851)), nSimID);

                        // CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, ERR_CME_SIM_NOT_INSERTED, 0, nUTI|(nSimID<<24), 0xF0);
                        // CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        // hAo = HNULL;
                        // CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                        // return ERR_SUCCESS;
                    }
                    else
                    {
                        //            CFW_STORELIST_INFO* pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        CFW_SIM_ID s = CFW_SIM_0;

                        for (; s < CFW_SIM_COUNT; s++)
                        {
                            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                        }
                        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                        if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM send SIM Open ERROR!!!! \n",0x08100852)));
                        }
                    }
                }
            }
            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        }
        break;

        case CFW_SM_NW_STATE_SEARCH:
        {
            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen ERROR!!!! \n",0x08100853)));
        }
        break;
        default:
            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen state ERROR!!! \n",0x08100854)));
            break;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFMSIMOpen end \n",0x08100855)));

    return ERR_SUCCESS;
}

UINT32 CFW_NW_SendPLMNList(CFW_StoredPlmnList *plmnlist, UINT8 nSimID)
{
    SIM_CHV_PARAM *pG_Chv_Param;
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
    {

        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NW_SendPLMNList, nSimID=%d\n",0x08100856)), nSimID);

        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));
        if (pSimInfo == NULL)
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NW_SendPLMNList, Malloc ERROR\n",0x08100857)));

        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim
        SUL_MemCopy8((void *)&pSimInfo->p_StoreList[nSimID], (void *)plmnlist, sizeof(CFW_StoredPlmnList));
        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
        return CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SetCommSIMOpen(
    CFW_SIM_ID nSimID
)
{
    HAO hAo = 0;

    NW_FM_INFO *pFMBinary = NULL;
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetCommSIMOpen() start\r\n",0x08100858)));
    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SetCommSIMOpen()\r\n",0x08100859)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SetCommSIMOpen);
        return ERR_CFW_INVALID_PARAMETER;
    }
    pFMBinary = (NW_FM_INFO *)CSW_SIM_MALLOC(SIZEOF(NW_FM_INFO));
    if (pFMBinary == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_SetCommSIMOpen() data error\r\n",0x0810085a)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SetCommSIMOpen);
        return ERR_NO_MORE_MEMORY;
    }
    hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pFMBinary, CFW_SetFMSIMOpen, nSimID);
    CFW_SetUTI(hAo, 0x03 + nSimID, 1);
    SUL_ZeroMemory32(pFMBinary, SIZEOF(NW_FM_INFO));
    CFW_SetServiceId(CFW_APP_SRV_ID);

    pFMBinary->n_CurrStatus = CFW_SM_NW_STATE_IDLE;
    pFMBinary->n_PrevStatus = CFW_SM_NW_STATE_IDLE;
    pFMBinary->nTryCount    = 0x01;
    pFMBinary->nMode        = 0;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetCommSIMOpen() end\r\n",0x0810085b)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SetCommSIMOpen);
    return ret;
}
#ifdef CHIP_HAS_AP
extern UINT8 gChangeRatComm[CFW_SIM_COUNT] ;
#endif
UINT32 CFW_SetFlightModeProc(HAO hAo, CFW_EV *pEvent)
{
    NW_FM_INFO *pFMInfo;
    UINT32 nRet = 0;
    CFW_EV ev;
    UINT32 nUTI = 0x00;
    CFW_SIM_ID nSimID;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SetFlightModeProc);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc()\r\n",0x0810085c)));
    if ((hAo == 0) || (pEvent == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc  ao or pEvent ERROR!!! \n",0x0810085d)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    nSimID = CFW_GetSimCardID(hAo);
    CFW_GetUTI(hAo, &nUTI);
    pFMInfo = CFW_GetAoUserData(hAo);

    switch (pFMInfo->n_CurrStatus)
    {
        case CFW_SM_NW_STATE_IDLE:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc CFW_SM_NW_STATE_IDLE \n",0x0810085e)));
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                CFW_SIM_STATUS sSimStatus = CFW_SIM_STATUS_END;
                sSimStatus = CFW_GetSimStatus(nSimID);

                SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
                ev.nParam1 = 0xff;
                pEvent     = &ev;
#if 0
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3),
                          TSTXT
                          (TSTR("CFW_SetFlightModeProc if ((UINT32)pEvent == 0xFFFFFFFF) sSimStatus: %d,nSimID : %d,pFMInfo->nTryCount:%d\n",0x081013d2)),
                          sSimStatus, nSimID, pFMInfo->nTryCount);
                if (CFW_SIM_ABSENT == sSimStatus)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc 3333  \n",0x0810085f)));

                    CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, ERR_CME_SIM_NOT_INSERTED, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    hAo = HNULL;
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                    return ERR_SUCCESS;
                }
#endif
                if (pFMInfo->nTryCount >= 0x01)
                {
                    if (CFW_ENABLE_COMM == pFMInfo->nMode)
                    {
#ifndef LTE_NBIOT_SUPPORT
                        nRet = CFW_StackInit(nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SetFlightModePro call CFW_StackInit, ret=0x%x\n",0x08100860)), nRet);
                        if (ERR_SUCCESS != nRet)
                        {
                            CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, nRet, pFMInfo->nStorageFlag, nUTI | (nSimID << 24), 0xF0);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Set FM enable ERROR! \n",0x08100861)));
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            hAo = HNULL;
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                            return ERR_SUCCESS;
                        }
                        else
#endif
                        {

#ifdef CHIP_HAS_AP
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTXT(TSTR("CFW_SetFlightModePro, enable success nMode=%d, gSatFreshComm[%d] = 0x%x,ACLBStep:%d\n",0x08100862)),
                                      pFMInfo->nMode, nSimID, gSatFreshComm[nSimID],_GetACLBStep());
#else
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_SetFlightModePro, enable success nMode=%d, gSatFreshComm[%d] = 0x%x\n",0x08100863)),
                                      pFMInfo->nMode, nSimID, gSatFreshComm[nSimID]);

#endif
#ifdef HAL_FORCE_LPS_OS_ALIGN
                            hal_LpsForceOsTimAlignEnable(TRUE);
#endif
                            if((2 == (gSatFreshComm[nSimID] & 0x0F))
#ifdef CHIP_HAS_AP
                                    ||( (2 == _GetACLBStep())&&(0 == nSimID) )
#endif
                              )
                            {
                                //in this case,we need waitting CS status IND.
                                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("please look CS staus function!\n",0x08100864)));
                            }
#ifdef CHIP_HAS_AP
                            else if( 2 == gChangeRatComm[nSimID])
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT("FM change rat succes!\n"));
                                gChangeRatComm[nSimID] = 0;
                                CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pFMInfo->nMode, 0xfe, nUTI | (nSimID << 24),0x00);
                            }
#endif
                            else
                            {
                                CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pFMInfo->nMode, pFMInfo->nStorageFlag, nUTI | (nSimID << 24),0x00);
                            }
#if 0
                            if( (2 != (gSatFreshComm[nSimID] & 0x0F)))
                            {
                                CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pFMInfo->nMode, pFMInfo->nStorageFlag, nUTI | (nSimID << 24),0x00);
                                // CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, 2, 0, CFW_NW_IND_UTI | (nSimID << 24), 2);
                            }
#endif

                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "=============Check Sim Status(nSimID=%d)===========", nSimID);
                            SIM_CHV_PARAM *pG_Chv_Param;
                            CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), "nPin1Remain = %x", pG_Chv_Param->nPin1Remain);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), "nPuk1Remain = %x", pG_Chv_Param->nPuk1Remain);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), "nPin1Status = %x", pG_Chv_Param->nPin1Status);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), "nPuk1Status = %x", pG_Chv_Param->nPuk1Status);

                            if(pG_Chv_Param->nPin1Status != 7)
                            {
                                if(pG_Chv_Param->nPin1Remain == 0)
                                {
                                    CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), "The SIM card (%d) has been BLOCKED, so skip SIM OPEN");
                                    if (0 == pFMInfo->nStorageFlag)
                                    {
                                        CFW_CfgNwSetFM(pFMInfo->nMode, nSimID);
                                    }

                                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                                    return ERR_SUCCESS;
                                }
                            }

                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "Send SIM_OPEN_REQ to stack!");
                            CFW_SetCommSIMOpen(nSimID);
                            if (0 == pFMInfo->nStorageFlag)
                            {
                                CFW_CfgNwSetFM(pFMInfo->nMode, nSimID); // only use dual sim
                            }
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM enable  Success \n",0x08100865)));
#ifdef LTE_NBIOT_SUPPORT
				            CFW_StackInit(nSimID);
#endif
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            hAo = HNULL;
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                            return ERR_SUCCESS;
                        }

                    }
                    else
                    {
                        NW_SM_INFO *s_proc = NULL;  // static sm info
                        HAO s_hAo = 0;  // static handle

                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM disable\n",0x08100866)));
                        if (CFW_SIM_ABSENT == sSimStatus)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc NO SIM  \n",0x08100867)));
                            // CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, ERR_CME_SIM_NOT_INSERTED, 0, nUTI|(nSimID<<24), 0xF0);
                            // CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            // hAo = HNULL;
                            // CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
                            // return ERR_SUCCESS;
                        }
                        else
                        {
 #ifdef LTE_NBIOT_SUPPORT
                            if(CFW_NWGetRat(nSimID) == CFW_RAT_NBIOT_ONLY)  //nbiot send  close sim before register_req
                             {
                                if(ERR_SUCCESS !=  CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID))
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM send SIM Close ERROR!!!! \n",0x08100868)));
                                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                                    hAo = HNULL;
                                }
                            }
#endif
                        }
#ifdef LTE_NBIOT_SUPPORT
						nRet = CFW_NwDeRegister(CFW_APP_UTI_MIN + (nUTI>>8),1, nSimID);
#else
                        nRet = CFW_NwDeRegister(CFW_APP_UTI_MIN + (nUTI>>8),FALSE, nSimID);
#endif
                        s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
                        s_proc            = CFW_GetAoUserData(s_hAo);
                        s_proc->nStpCause = STOP_CAUSE_FM;
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SetFlightModePro call CFW_NwDeRegister, ret=0x%x\n",0x08100869)), nRet);
                        if (ERR_SUCCESS != nRet)
                        {
                            CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, nRet, pFMInfo->nStorageFlag, nUTI | (nSimID << 24), 0xF0);
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM disable Error!\n",0x0810086a)));
                            CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                            hAo = HNULL;
                        }
#if 0
                        // Modify for case 34.123-9.1
#ifndef __MULTI_RAT__
                        CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
#endif
#endif
                    }

                    pFMInfo->n_PrevStatus = pFMInfo->n_CurrStatus;
                    pFMInfo->n_CurrStatus = CFW_SM_NW_STATE_SEARCH;

                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pEvent->nParam1, pFMInfo->nStorageFlag, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM Option  Failure! \n",0x0810086b)));
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    hAo = HNULL;
                }
                // CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("==> !!! if ((UINT32)pEvent == 0xFFFFFFFF)\r\n",0x0810086c)));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        break;

        case CFW_SM_NW_STATE_SEARCH:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc CFW_SM_NW_STATE_SEARCH \n",0x0810086d)));
            if (pEvent->nEventId == EV_CFW_NW_DEREGISTER_RSP)
            {
#ifdef LTE_NBIOT_SUPPORT
                if(CFW_NWGetRat(nSimID) != CFW_RAT_NBIOT_ONLY)
#endif
                {
                    if (ERR_SUCCESS !=  CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM send SIM Close ERROR!!!! \n",0x0810086e)));
                        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                        hAo = HNULL;
                    }
                }
                if (0x00 == pEvent->nType)
                {
                    HAO s_hAo = 0;  // staitc ao
                    NW_SM_INFO *s_proc = NULL;  // static sm
                    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
                    s_proc                         = CFW_GetAoUserData(s_hAo);
                    s_proc->bDetached              = FALSE;
                    s_proc->nAttach                = 0xFF;
                    s_proc->sGprsStatusInfo.nCause = 0;
                    s_proc->nGprsAttState          = CFW_GPRS_DETACHED;
#ifdef HAL_FORCE_LPS_OS_ALIGN
                    CFW_COMM_MODE sOtherSimMode = 0;
                    CFW_SIM_ID nOtherSimID = CFW_SIM_0;
                    if( CFW_SIM_0 == nSimID )
                    {
                        nOtherSimID = CFW_SIM_1;
                    }
                    else if( CFW_SIM_1 == nSimID )
                    {
                        nOtherSimID = CFW_SIM_0;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR! get sim ID error!!!\n",0x0810086f)));
                    }
                    CFW_GetComm(&sOtherSimMode,nOtherSimID);
                    if( CFW_DISABLE_COMM == sOtherSimMode )
                    {
                        hal_LpsForceOsTimAlignEnable(FALSE);
                    }
#endif
                    if (0 == pFMInfo->nStorageFlag)
                    {
                        CFW_CfgNwSetFM(pFMInfo->nMode, nSimID);
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SetFlightModeProc EV_CFW_NW_DEREGISTER_RSP gSatFreshComm[%d] = 0x%x\n",0x08100870)), nSimID, gSatFreshComm[nSimID]);
#ifdef LTE_NBIOT_SUPPORT
                    if(CFW_NWGetRat(nSimID) != CFW_RAT_NBIOT_ONLY)
                    {
#endif
                        CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, API_NW_NO_SVCE, 0x00,CFW_NW_IND_UTI | (nSimID << 24), 2);
#ifdef LTE_NBIOT_SUPPORT
                    }
		            else
		            {
		                CFW_PostNotifyEvent(EV_CFW_GPRS_STATUS_IND, API_NW_NO_SVCE, 0,nUTI | (nSimID << 24), 2);
		            }
#endif
                    CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, API_NW_NO_SVCE, 0x00,GENERATE_SHELL_UTI() | (nSimID << 24), 2);
                    if( 0x01 == (gSatFreshComm[nSimID] & 0x0F) )
                    {
                        //CFW_SimInit(1, nSimID );
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("0x01 == (gSatFreshComm[nSimID] & 0x0F)  \n",0x08100871)));
                        if(0x20 != (gSatFreshComm[nSimID] & 0xF0))
                        {
                            UINT8 nRUti = 0x00;
                            gSatFreshComm[nSimID] = ((gSatFreshComm[nSimID] & 0xF0) + 0x02);
                            CFW_GetFreeUTI(0, (UINT8 *)&nRUti);
                            CFW_SetComm(CFW_ENABLE_COMM, 1, nRUti, nSimID);
                        }
                        else
                        {
                            CFW_GetFreeUTI(0, (UINT8 *)(&nUTI));
                            if(g_cfw_sim_status[nSimID].UsimFlag)
                                CFW_SimPatchEX(API_USIM_EF_LOCI, APP_UTI_SHELL, nSimID);
                            else
                                CFW_SimPatchEX(API_SIM_EF_LOCI, APP_UTI_SHELL, nSimID);
                            gSatFreshComm[nSimID] = 2 + (0xF0 & gSatFreshComm[nSimID]);
                        }

                    }
#ifdef CHIP_HAS_AP
                    else if( 1 == gChangeRatComm[nSimID])
                    {
                        UINT8 nRUti = 0x00;
                        CFW_GetFreeUTI(0, (UINT8 *)&nRUti);
                        CFW_SetComm(CFW_ENABLE_COMM, 1, nRUti, nSimID);
                        gChangeRatComm[nSimID] = 2;
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT("FM change Rat to nomal mode!\n"));
                    }
#endif
                    else
                    {
                        CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pEvent->nParam1, pFMInfo->nStorageFlag, nUTI | (nSimID << 24), 0x00);
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM disable Success!\n",0x08100872)));
                    }
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, pEvent->nParam1, pFMInfo->nStorageFlag, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("FM EV_CFW_NW_DEREGISTER_RSP Error!\n",0x08100873)));
                }
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                hAo = HNULL;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SetFlightModePro Fair pEvent->nType : 0x%x, eventID = 0x%x \n",0x08100874)), pEvent->nType,pEvent->nEventId);
            }
        }
        break;

        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetFlightModeProc error! \n",0x08100875)));
            break;

    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
    return ERR_SUCCESS;
}

CFW_COMM_MODE gCOMMMode[CFW_SIM_COUNT] = {CFW_DISABLE_COMM,CFW_DISABLE_COMM};
UINT32 CFW_SetCommEX(CFW_COMM_MODE nMode,CFW_SIM_ID nSimID)
{
    gCOMMMode[nSimID] = nMode;
    return ERR_SUCCESS;
}

UINT32 CFW_GetCommEX(CFW_COMM_MODE *pMode, CFW_SIM_ID nSimID)
{
    *pMode = gCOMMMode[nSimID];
    return ERR_SUCCESS;
}

UINT32 CFW_SetComm(CFW_COMM_MODE nMode, UINT8 nStorageFlag, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    NW_FM_INFO *pFMBinary = NULL;
    // CFW_SIM_STATUS sSimStatus= CFW_SIM_STATUS_END;
    UINT32 ret = ERR_SUCCESS;
    // UINT32 nAoStatus=0;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_SetComm() start, nMode=%d, nStorageFlag=%d, nSimID=%d\r\n",0x08100876)),
              nMode, nStorageFlag, nSimID);

    CFW_SetCommEX(nMode,nSimID);
#ifdef _TTCN_TEST_
    return ret;
#endif
    if (!sxs_ProtoStackAvail())
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ProtoStack unavailable\r\n",0x08100877)));
        nMode = CFW_DISABLE_COMM;
    }

    CSW_PROFILE_FUNCTION_ENTER(CFW_SetComm);

    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SetComm()\r\n",0x08100878)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (nMode > CFW_ENABLE_COMM)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! nMode error CFW_SetComm()\r\n",0x08100879)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
        return ERR_CFW_INVALID_PARAMETER;
    }

#if 0
    hAo        = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    nAoStatus  = CFW_GetAoState(hAo);
    sSimStatus = CFW_GetSimStatus(nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SetComm() nAoStatuse=0x%x,sSimStatus = 0x%x\r\n",0x0810087a)), nAoStatus, sSimStatus);

    if (nAoStatus == CFW_SM_NW_STATE_STOP)
    {
        if (nMode == CFW_DISABLE_COMM)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetComm()11, return ERR_CME_OPERATION_NOT_ALLOWED\n",0x0810087b)));
            CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
            return ERR_CME_OPERATION_NOT_ALLOWED;
        }
    }
    else
    {
        if (nMode == CFW_ENABLE_COMM)
        {
            if (!((CFW_SIM_ABSENT == sSimStatus) && (nAoStatus == CFW_SM_NW_STATE_IDLE)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetComm()22, return ERR_CME_OPERATION_NOT_ALLOWED\n",0x0810087c)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
                return ERR_CME_OPERATION_NOT_ALLOWED;
            }

        }

    }

    hAo = 0;
#endif
    pFMBinary = (NW_FM_INFO *)CSW_SIM_MALLOC(SIZEOF(NW_FM_INFO));
    if (pFMBinary == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! data error CFW_SetComm()\r\n",0x0810087d)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
        return ERR_NO_MORE_MEMORY;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SetComm() www , simid: 0x%x , pFMBinary: 0x%x\n",0x0810087e)), nSimID, pFMBinary);

    hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pFMBinary, CFW_SetFlightModeProc, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SetComm(), hAo: 0x%x \n",0x0810087f)), hAo);

    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pFMBinary, SIZEOF(NW_FM_INFO));
    CFW_SetServiceId(CFW_APP_SRV_ID);

    pFMBinary->n_CurrStatus = CFW_SM_NW_STATE_IDLE;
    pFMBinary->n_PrevStatus = CFW_SM_NW_STATE_IDLE;
    pFMBinary->nTryCount    = 0x01;
    pFMBinary->nMode        = nMode;
    pFMBinary->nStorageFlag = nStorageFlag;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_SetComm() end\r\n",0x08100880)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SetComm);
    return ret;
}


UINT32 CFW_GetComm(CFW_COMM_MODE *pMode, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GetComm() nSimID : %d\r\n",0x08100881)), nSimID);
#ifdef __MODEM_NO_AP_
    if (!nSimID)
    {
        *pMode = CFW_ENABLE_COMM;
        return ERR_SUCCESS;
    }
    else
#endif
    {
        UINT32 nRet = ERR_SUCCESS;
        nRet = CFW_CfgNwGetFM(pMode, nSimID);
        CFW_SetCommEX(*pMode,nSimID);
        return nRet;
    }
}

typedef struct _NW_CS_INFO
{
    BOOL nFrstNCvFlag;
    BOOL nTimeFlag;
    UINT32 nDelayTime;
    api_NwCsRegStatusInd_t nCSInfo;
} NW_CS_DELAY_INFO;

NW_CS_DELAY_INFO gCSDelayInfo[CFW_SIM_COUNT];
PUBLIC UINT32 CFW_NW_SetCSDelayTime(UINT32 ntime, CFW_SIM_ID nSimID)
{
    gCSDelayInfo[nSimID].nDelayTime = ntime;
    return ERR_SUCCESS;
}

PRIVATE UINT32 CFW_NW_GetCSDelayTime(CFW_SIM_ID nSimID)
{
    return gCSDelayInfo[nSimID].nDelayTime;
}


#if defined(SIMCARD_HOT_DETECT)
UINT32 CFW_NwResetSim(
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    UINT16 nUTI = 0;
    NW_SM_INFO* s_proc = NULL;  // static sm info
    HAO s_hAo = 0;  // static handle
    CFW_COMM_MODE bFM;


    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwResetSim start\n",0x08100882)));
    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !s_hAo == 0 \n",0x08100883)));
        return ERR_CFW_INVALID_PARAMETER;   // AOM error
    }

    CFW_GetComm((CFW_COMM_MODE*)&bFM, nSimID);
    if (CFW_ENABLE_COMM == bFM)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_ENABLE_COMM, start stack, nSimID=%d\n",0x08100884)), nSimID);
        s_proc                    = CFW_GetAoUserData(s_hAo);
        //s_proc->nCsStatusIndCount = 0;
        s_proc->bChangeBand = FALSE;
        s_proc->nStpCause   = STOP_CAUSE_HOT_DETECT;
        CFW_NwStop(FALSE, nUTI, nSimID);
    }
    else if ((CFW_DISABLE_COMM == bFM) || (CFW_CHECK_COMM == bFM))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_DISABLE_COMM, don't start stack, nSimID=%d\n",0x08100885)), nSimID);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("FlightMode State Error!, bfm=%d\n",0x08100886)), bFM);
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwResetSim end\n",0x08100887)));

    return ret;
}
#endif


// When System start, this function will be called one time.
// Typically, used this function to register AO proc,
// don't take a long time to process in this function.
// If you don't care this function, return ERR_SUCCESS.
//
extern UINT8 CFW_GetAutoCallCmd(CFW_SIM_ID nSimID);

UINT32 CFW_NwInit(VOID)
{
    UINT32 ret = ERR_SUCCESS;

    NW_SM_INFO *s_proc = NULL;  // static sm
    HAO s_hAo = 0;  // static ao
    UINT8 bFM;
    UINT8 bDetectMBS=0;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwInit start\n",0x08100888)));
    CFW_SIM_ID nSimID;
#ifdef LTE_NBIOT_SUPPORT
    for (nSimID = CFW_SIM_0; nSimID < 1; nSimID++)
#else
    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
#endif
    {
        s_proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));

        // Clear the proc structure, ensure that there is not existing un-intialized member.
        SUL_ZeroMemory8(s_proc, SIZEOF(NW_SM_INFO));

        CFW_CfgNwGetNetWorkMode(&s_proc->nNetMode, nSimID);

        s_proc->nNetModeEx             = CFW_NW_AUTOMATIC_MODE;
        s_proc->bStaticAO              = TRUE;
        s_proc->bReselNW               = FALSE;
        s_proc->bDetached              = FALSE;
        s_proc->bRoaming               = FALSE;
        s_proc->bNetAvailable          = FALSE;
        s_proc->nAttach                = 0xFF;
        s_proc->nCsStatusIndCount      = 0;
        s_proc->bTimerneed             = FALSE;
        s_proc->bTimeDeleted           = FALSE;
        s_proc->sGprsStatusInfo.nCause = 0;
        s_proc->nGprsAttState          = CFW_GPRS_DETACHED;  // 0xFF;
        s_proc->sStatusInfo.nCsStatusChange = TRUE;

        CFW_CfgNwGetFrequencyBand(&s_proc->nBand, nSimID);
#ifdef __MULTI_RAT__
        CFW_CfgNwGetFrequencyUMTSBand(&s_proc->nUBand, nSimID);
#endif
        s_hAo = CFW_RegisterAo(CFW_NW_SRV_ID, s_proc, CFW_NwAoProc, nSimID);

        CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_STOP);
        CFW_SetAoProcCode(s_hAo, CFW_AO_PROC_CODE_MULTIPLE);

#ifdef CHIP_HAS_AP
        // Modem should not start stack unless AP requests to start it
        if ((0xFF != CFW_GetAutoCallCmd(nSimID)) && (CFW_SIM_0 == nSimID))
        {
            CFW_CfgNwSetFM(CFW_ENABLE_COMM, nSimID);
        }
        else
        {
            CFW_CfgNwSetFM(CFW_DISABLE_COMM, nSimID);
        }
#endif // CHIP_HAS_AP

		CFW_CfgNwGetDetectMBS(&bDetectMBS);
        if(1== bDetectMBS)
		{
			CFW_NW_SendDetectMBSMessage(bDetectMBS,  nSimID);
		}
        CFW_GetComm((CFW_COMM_MODE *)&bFM, nSimID);
#ifdef DISABLE_SECOND_SIM
        if(DEFAULT_SIM_SLOT != nSimID)
            bFM = CFW_DISABLE_COMM;
#endif
        if (CFW_ENABLE_COMM == bFM)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_ENABLE_COMM, start stack, nSimID=%d\n",0x08100889)), nSimID);
#ifndef LTE_NBIOT_SUPPORT
            CFW_StackInit(nSimID);
#endif
        }
        else if ((CFW_DISABLE_COMM == bFM) || (CFW_CHECK_COMM == bFM))
        {
#ifndef CHIP_HAS_AP
#ifdef HAL_FORCE_LPS_OS_ALIGN
            CFW_COMM_MODE sOtherSimMode = 0;
            CFW_SIM_ID nOtherSimID = CFW_SIM_0;
            if( CFW_SIM_0 == nSimID )
            {
                nOtherSimID = CFW_SIM_1;
            }
            else if( CFW_SIM_1 == nSimID )
            {
                nOtherSimID = CFW_SIM_0;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR! get sim ID error!!!\n",0x0810088a)));
            }
            CFW_GetComm(&sOtherSimMode,nOtherSimID);
            if( CFW_DISABLE_COMM == sOtherSimMode )
            {
                hal_LpsForceOsTimAlignEnable(FALSE);
            }
#endif
#endif
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_DISABLE_COMM, don't start stack, nSimID=%d\n",0x0810088b)), nSimID);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("FlightMode State Error!, bfm=%d\n",0x0810088c)), bFM);
        }
        gCSDelayInfo[CFW_SIM_0 + nSimID].nTimeFlag = TRUE;
        gCSDelayInfo[CFW_SIM_0 + nSimID].nFrstNCvFlag = TRUE;
        CFW_NW_SetCSDelayTime(3 * 1000 MILLI_SECOND, CFW_SIM_0 + nSimID);

    }

    CFW_RegisterCreateAoProc(EV_TIMER, NetWorkTimerProc); // timer
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwInit end\n",0x0810088d)));

    return ret;
}


// used to register to the plmn fail, then auto register
PRIVATE HAO NetWorkTimerProc(CFW_EV *pEvent)
{
    // 1000 ms will get prefer plmn from sim and register to the plmn , manual.
    NW_SM_INFO *s_proc = NULL;  // sm info
    HAO s_hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(NetWorkTimerProc);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("NetWorkTimerProc  start\n",0x0810088e)));

    UINT32 nTimerID = *(UINT32 *)(pEvent->nParam1);

#ifdef CFW_GPRS_SUPPORT
    UINT8 uti;
#endif


    //Add by wcf, 2015/1/19, process polling timer
    //Process Sim reset Polling timer
#ifdef _SIM_HOT_PLUG_

    extern UINT8 g_nPollingTimer[CFW_SIM_COUNT] ;
    extern UINT8 g_bTimerStatus[CFW_SIM_COUNT] ;

    if(nTimerID == PRV_CFW_SIM_POLLING_TIMER_SIM0_ID)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NW---SIM Card Removed -sim0--Time out g_bTimerStatus = %d",0x0810088f), g_bTimerStatus[CFW_SIM_0]);
        if(g_bTimerStatus[CFW_SIM_0] == TRUE)
        {
            CFW_SimInit(1, CFW_SIM_0);
            BOOL result = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM0_ID , \
                                         COS_TIMER_MODE_SINGLE, g_nPollingTimer[CFW_SIM_0] * 1000 MILLI_SECOND);
            if (result == FALSE)
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("---Start timer Fail---",0x08100890));
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NW---Polling timer start simID = %d ---",0x08100891), CFW_SIM_0);
        }
    }

    else if(nTimerID == PRV_CFW_SIM_POLLING_TIMER_SIM1_ID)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NW---SIM Card Removed -sim1--Time out g_bTimerStatus = %d",0x08100892), g_bTimerStatus[CFW_SIM_1]);
        if(   g_bTimerStatus[CFW_SIM_1] == TRUE)
        {
            CFW_SimInit(1, CFW_SIM_1);
            BOOL result = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM1_ID, \
                                         COS_TIMER_MODE_SINGLE, g_nPollingTimer[CFW_SIM_1] * 1000 MILLI_SECOND);
            if (result == FALSE)
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("---Start timer Fail---",0x08100893));
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NW---Polling timer start simID = %d ---",0x08100894), CFW_SIM_1);
        }
    }
    else
#endif      //#ifdef _SIM_HOT_PLUG_
        //End, Process polling timer

#ifdef _TCPIP_SYNC_IO_
    extern HANDLE tcpip_syn_sem[];
    extern UINT32 tcpip_syn_status[];
	if((nTimerID >= PRV_CFW_TCPIP_SYNC_IO_SOCKET0_ID) && (nTimerID <= PRV_CFW_TCPIP_SYNC_IO_SOCKET3_ID))
	{
	    UINT8 id = nTimerID - PRV_CFW_TCPIP_SYNC_IO_SOCKET0_ID;
	    tcpip_syn_status[id] = id << 16 | PRV_CFW_TCPIP_SYNC_IO_SOCKET0_ID;
	    TCPIP_ReleaseSemaphore(id, FALSE);
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL) ,  "====== TCPIP sync timer overtime=%d", nTimerID);
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL) ,  "====== tcpip_syn_status[0] =%d", tcpip_syn_status[id]);
	    return;
	}
	else
#endif

        if((nTimerID >= PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID) && (nTimerID <= PRV_CFW_STK_TM_ID8_TIMER_SIM1_ID))
        {
            _SATTimerProc(nTimerID);
            return -1;
        }
        else if ((nTimerID >= PRV_CFW_AUTO_CALL_TIMER_SIM0_ID) && (nTimerID <= PRV_CFW_AUTO_CALL_TIMER_SIM1_ID))
        {
            _AutoCallTimerProc(nTimerID - PRV_CFW_AUTO_CALL_TIMER_SIM0_ID);
            return -1;
        }
    switch (nTimerID)
    {
        case PRV_CFW_NW_SIM0_TIMER_ID:
            s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, CFW_SIM_0);
            if (s_hAo == 0)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("s_hAo == 0  ERROR CFW_SIM_0\n",0x08100895)));
                CSW_PROFILE_FUNCTION_EXIT(NetWorkTimerProc);
                return - 1;
            }
            s_proc             = CFW_GetAoUserData(s_hAo);
            s_proc->bTimerneed = FALSE;

            // get pref plmn from sim
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get pref plmn from sim CFW_SIM_0 \n",0x08100896)));

            CFW_SimGetPrefOperatorList(NW_UTI_SIM, CFW_SIM_0);
            break;

        case PRV_CFW_NW_SIM1_TIMER_ID:
            s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, CFW_SIM_1);
            if (s_hAo == 0)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("s_hAo == 0  ERROR CFW_SIM_1\n",0x08100897)));
                CSW_PROFILE_FUNCTION_EXIT(NetWorkTimerProc);
                return - 1;
            }
            s_proc             = CFW_GetAoUserData(s_hAo);
            s_proc->bTimerneed = FALSE;

            // get pref plmn from sim
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get pref plmn from sim  CFW_SIM_1\n",0x08100898)));

            CFW_SimGetPrefOperatorList(NW_UTI_SIM, CFW_SIM_1);
            break;
        case PRV_CFW_NW_SIM2_TIMER_ID:
            s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, CFW_SIM_1 + 1);
            if (s_hAo == 0)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("s_hAo == 0  ERROR CFW_SIM_1\n",0x08100899)));
                CSW_PROFILE_FUNCTION_EXIT(NetWorkTimerProc);
                return - 1;
            }
            s_proc             = CFW_GetAoUserData(s_hAo);
            s_proc->bTimerneed = FALSE;

            // get pref plmn from sim
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get pref plmn from sim  CFW_SIM_1\n",0x0810089a)));

            CFW_SimGetPrefOperatorList(NW_UTI_SIM, CFW_SIM_1 + 1);
            break;
        case PRV_CFW_NW_SIM3_TIMER_ID:
            s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, CFW_SIM_1 + 2);
            if (s_hAo == 0)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("s_hAo == 0  ERROR CFW_SIM_1\n",0x0810089b)));
                CSW_PROFILE_FUNCTION_EXIT(NetWorkTimerProc);
                return - 1;
            }
            s_proc             = CFW_GetAoUserData(s_hAo);
            s_proc->bTimerneed = FALSE;

            // get pref plmn from sim
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get pref plmn from sim  CFW_SIM_1\n",0x0810089c)));

            CFW_SimGetPrefOperatorList(NW_UTI_SIM, CFW_SIM_1 + 2);
            break;
        case PRV_CFW_NW_CS_DELAY_SIM0_TIMER_ID:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("   DELAY TIME  SIM 0  \n",0x0810089d));
            gCSDelayInfo[CFW_SIM_0].nTimeFlag = FALSE;
            CFW_BalSendMessage(CFW_MBX, API_NW_CSREGSTATUS_IND, (VOID *)&gCSDelayInfo[CFW_SIM_0].nCSInfo,
                               sizeof(api_NwCsRegStatusInd_t), CFW_SIM_0);
            break;

        case PRV_CFW_NW_CS_DELAY_SIM1_TIMER_ID:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("DELAY TIME  SIM 1  \n",0x0810089e));
            gCSDelayInfo[CFW_SIM_1].nTimeFlag = FALSE;
            CFW_BalSendMessage(CFW_MBX, API_NW_CSREGSTATUS_IND, (VOID *)&gCSDelayInfo[CFW_SIM_1].nCSInfo,
                               sizeof(api_NwCsRegStatusInd_t), CFW_SIM_1);
            break;
        case PRV_CFW_NW_CS_DELAY_SIM2_TIMER_ID:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("DELAY TIME  SIM 1  \n",0x0810089f));
            gCSDelayInfo[CFW_SIM_1 + 1].nTimeFlag = FALSE;
            CFW_BalSendMessage(CFW_MBX, API_NW_CSREGSTATUS_IND, (VOID *)&gCSDelayInfo[CFW_SIM_1 + 1].nCSInfo,
                               sizeof(api_NwCsRegStatusInd_t), CFW_SIM_1 + 1);
            break;
        case PRV_CFW_NW_CS_DELAY_SIM3_TIMER_ID:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("DELAY TIME  SIM 1  \n",0x081008a0));
            gCSDelayInfo[CFW_SIM_1 + 2].nTimeFlag = FALSE;
            CFW_BalSendMessage(CFW_MBX, API_NW_CSREGSTATUS_IND, (VOID *)&gCSDelayInfo[CFW_SIM_1 + 2].nCSInfo,
                               sizeof(api_NwCsRegStatusInd_t), CFW_SIM_1 + 2);
            break;
#ifdef CFW_GPRS_SUPPORT

        // add by wuys 2010-05-17
        case CFW_START_GPRSDETACH_TIMER_ID:

            CFW_GetFreeUTI(0, &uti);
            CFW_GprsAtt(CFW_GPRS_DETACHED, uti, CFW_SIM_0);
            break;

        // add by wuys 2010-05-17
        case CFW_START_GPRSDETACH_TIMER_ID_SIM1:

            CFW_GetFreeUTI(0, &uti);
            CFW_GprsAtt(CFW_GPRS_DETACHED, uti, CFW_SIM_1);

            break;
        case CFW_START_GPRSDETACH_TIMER_ID_SIM2:

            CFW_GetFreeUTI(0, &uti);
            CFW_GprsAtt(CFW_GPRS_DETACHED, uti, (CFW_SIM_1 + 1));

            break;
        case CFW_START_GPRSDETACH_TIMER_ID_SIM3:

            CFW_GetFreeUTI(0, &uti);
            CFW_GprsAtt(CFW_GPRS_DETACHED, uti, (CFW_SIM_1 + 2));

            break;
#endif
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("NetWorkTimerProc pEvent->nParam1 EEROR!!! pEvent->nParam1: %d\n",0x081008a1)),
                      pEvent->nParam1);
            break;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("NetWorkTimerProc end \n",0x081008a2)));
    CSW_PROFILE_FUNCTION_EXIT(NetWorkTimerProc);

    return 1;
}

// start the stack
extern BOOL g_auto_calib ;
extern BOOL _GetACLBStatus();

PUBLIC UINT32 CFW_StackInit(
    CFW_SIM_ID nSimID
)
{
    UINT8 nBand = 0;
#ifdef CSW_USER_DBS
    UINT8 nStrtFlag = 0xff;
#endif
#ifdef __MULTI_RAT__
    UINT8 nUBand = 0;
#endif
    UINT8 bDetectMBS=0;
    // get 1 Full functionality.
    // 2 Disable transmit RF circuits only.
    // 3 Disable receive RF circuits only.
    // 4 Disable both transmit and receive RF circuits

    // and
    // get band of the stack
    // then start the stack if needed.
    // for test
    // nBand = API_GSM_900P | API_DCS_1800 | API_GSM_900E;


    // get the frequency band from auto call setting in ram
    if (CFW_GetAutoCallFreqBand(&nBand))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Currently status is producing Band 0x%x\n",0x081008a3)), nBand);
    }
    else
    {
        UINT32 nRet = CFW_CfgNwGetFrequencyBand(&nBand, nSimID);

        if (ERR_SUCCESS != nRet)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Error CFW_CfgNwGetFrequencyBand Failed Ret:0x%x \n",0x081008a4)), nRet);
        }
#ifdef __MULTI_RAT__
        nRet = CFW_CfgNwGetFrequencyUMTSBand(&nUBand, nSimID);
        if (ERR_SUCCESS != nRet)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Error CFW_CfgNwGetFrequencyUMTSBand Failed Ret:0x%x \n",0x081008a4)), nRet);
        }
#endif
    }

    if(_GetACLBStatus() == TRUE)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_StackInit Band is 0,for auto calib.\n",0x081008a5)));
#ifdef CSW_USER_DBS
        CFW_NwStartStack(0, nStrtFlag, nSimID);
#else
#ifdef __MULTI_RAT__
        CFW_NwStartStack(0, 0, nSimID);
#else
        CFW_NwStartStack(0, nSimID);
#endif
#endif
    }
    else
    {
#ifdef CSW_USER_DBS
        CFW_NwStartStack(nBand, nStrtFlag, nSimID);
#else
#ifdef CHIP_HAS_AP
//        nBand = 236;
#ifdef __MULTI_RAT__
        CFW_NwStartStack(nBand, nUBand, nSimID);
#else
        CFW_NwStartStack(nBand, nSimID);
#endif
#else
#ifdef __MULTI_RAT__
        CFW_NwStartStack(nBand, nUBand, nSimID);
#else
        CFW_NwStartStack(nBand, nSimID);
#endif
#endif
#endif
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_StackInit Band:0x%x \n",0x081008a6)), nBand);

    }

	CFW_CfgNwGetDetectMBS(&bDetectMBS );
	if(1== bDetectMBS)
	{
		CFW_NW_SendDetectMBSMessage(bDetectMBS,  nSimID);
	}
	else
	{
		CFW_NW_SendDetectMBSMessage(0,  nSimID);
	}
    return ERR_SUCCESS;
}

// add a timer, if cs state is full service, we will delay some seconds when we receive no service. by Frank start.
BOOL ispdpactive(CFW_SIM_ID nSimID);
UINT32 CFW_NWDelay(HAO hAo, CFW_EV *pEvent)
{
    VOID *nEvParam  = (VOID *)pEvent->nParam1;
    UINT32 nEventId = pEvent->nEventId;
    UINT32 nCSTime  = 0;

    NW_SM_INFO *proc = NULL;

    proc = CFW_GetAoUserData(hAo);

    CFW_SIM_ID nSimID = CFW_SIM_END;

    nSimID = CFW_GetSimCardID(hAo);

    if (API_NW_CSREGSTATUS_IND == nEventId)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR(" CSREGSTATUS  status: %d; SIMID : %d\n",0x081008a7), ((api_NwCsRegStatusInd_t *)nEvParam)->Status,
                  nSimID);
    }
    nCSTime = CFW_NW_GetCSDelayTime(nSimID);

    if ((nCSTime > 0) && (API_NW_CSREGSTATUS_IND == nEventId) && gCSDelayInfo[nSimID].nTimeFlag)
    {
        api_NwCsRegStatusInd_t *p = (api_NwCsRegStatusInd_t *)nEvParam;
        switch (p->Status)
        {
            case API_NW_NO_SVCE:
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CSREGSTATUS NO_SVCE  \n",0x081008a8));
                SUL_MemCopy8((void *) & (gCSDelayInfo[nSimID].nCSInfo), (const void *)nEvParam, SIZEOF(api_NwCsRegStatusInd_t));
#ifdef CFW_GPRS_SUPPORT
                if ((API_NW_FULL_SVCE == proc->sStatusInfo.nStatus) && gCSDelayInfo[nSimID].nFrstNCvFlag && (ispdpactive(nSimID)))
#else
                if ((API_NW_FULL_SVCE == proc->sStatusInfo.nStatus) && gCSDelayInfo[nSimID].nFrstNCvFlag)
#endif
                {

                    gCSDelayInfo[nSimID].nFrstNCvFlag = FALSE;
                    BOOL POSSIBLY_UNUSED bRet =
                        COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_NW_CS_DELAY_SIM0_TIMER_ID + nSimID, COS_TIMER_MODE_SINGLE, nCSTime);
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CSREGSTATUS NO_SVCE FrstNCvFlag bRet %d  \n",0x081008a9), bRet);
                    return ERR_SUCCESS;
                }

                // return ERR_SUCCESS;
                break;
            case API_NW_LTD_SVCE:
                SUL_MemCopy8((void *) & (gCSDelayInfo[nSimID].nCSInfo), (const void *)nEvParam, SIZEOF(api_NwCsRegStatusInd_t));
                break;
            case API_NW_FULL_SVCE:
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CSREGSTATUS FULL_SVCE  \n",0x081008aa));
                if (FALSE == gCSDelayInfo[nSimID].nFrstNCvFlag)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CSREGSTATUS  Kill Timer   \n",0x081008ab));
                    COS_KillTimerEX(0xFFFFFFFF, PRV_CFW_NW_CS_DELAY_SIM0_TIMER_ID + nSimID);
                }
                gCSDelayInfo[nSimID].nFrstNCvFlag = TRUE;
                break;
            default:
                break;
        }
    }
    else if ((nCSTime > 0) && (API_NW_CSREGSTATUS_IND == nEventId) && (!gCSDelayInfo[nSimID].nTimeFlag))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CSREGSTATUS !gCSDelayInfo[nSimID].nTimeFlag  \n",0x081008ac));

        gCSDelayInfo[nSimID].nFrstNCvFlag = TRUE;
        gCSDelayInfo[nSimID].nTimeFlag    = TRUE;
    }
    if (API_NW_CSREGSTATUS_IND == nEventId)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CSREGSTATUS  22222  status: %d; SIMID : %d\n",0x081008ad), ((api_NwCsRegStatusInd_t *)nEvParam)->Status,
                  nSimID);
    }
    return ERR_CONTINUE;

}

//
// NW AO process entry.
// There are some action will done in this function:
// 1> ??
// 2> ??
//
extern void *memcpy(void *, const void *, size_t);



/*
Number of spare bits in last octet (octet 3, bits 1 to 3)

2   1
0   0   1       bit 8 is spare and set to "0" in octet n
0   1   0       bits 7 and 8 are spare and set to "0" in octet n
0   1   1       bits 6 to 8(inclusive) are spare and set to "0" in octet n
1   0   0       bits 5 to 8(inclusive) are spare and set to "0" in octet n
1   0   1       bits 4 to 8(inclusive) are spare and set to "0" in octet n
1   1   0       bits 3 to 8(inclusive) are spare and set to "0" in octet n
1   1   1       bits 2 to 8(inclusive) are spare and set to "0" in octet n
0   0   0       this field carries no information about the number of spare bits in octet n

*/
UINT8 _GetNWINOSBILO(UINT8 n)
{
    return (n & 7);
}

/*
Add CI (octet 3, bit 4)

0               The MS should not add the letters for the Country's Initials to the text string
1               The MS should add the letters for the Country's Initials and a separator
                (e.g. a space) to the text string

*/

UINT8 _GetNWIACI(UINT8 n)
{
    return ((n >> 3) & 0x1);
}


/*
Coding Scheme (octet 3, bits 5-7)

0   0   0       Cell Broadcast data coding scheme, GSM default alphabet, language unspecified, defined in 3GPP TS 23.038 [8b]
0   0   1       UCS2 (16 bit) [72]
0   1   0
to  reserved
1   1   1

Text String (octet 4 to octet n, inclusive)
Encoded according to the Coding Scheme defined by octet 3, bits 5-7

*/

UINT8 _GetNWIDCS(UINT8 n)
{
    return ((n >> 4) & 7);
}



/**
 *  Function Description:
 *      Get the Network long name string, long name string length, Coding Scheme
 *  @param1
 *      pNwkNames: input network indication string
 *  @param2
 *      ppLongName: Output the long name string
 *  @param3
 *      pShortNameLen: Output the long name string length
 *  @param4
 *      pDCS: Output the long name string Coding Scheme,0->ASCII 1->UCS2
 *
 *  @return  ERR_SUCCESS | ERR_NO_MORE_MEMORY
 */

UINT32 CFW_GetNWILongName(UINT8 *pNwkNames,UINT8 ** ppLongName,UINT8 * pLongNameLen,UINT8 * pDCS)
{
    *pDCS = _GetNWIDCS(pNwkNames[2]);
    if(0x00 == *pDCS)
    {
        *ppLongName = (UINT8 *)CSW_NW_MALLOC(pNwkNames[0]*8/7 + 4);
        if(!(*ppLongName))
            return ERR_NO_MORE_MEMORY;
        SUL_MemSet8(*ppLongName,0x00,pNwkNames[0]*8/7 + 4);

        *pLongNameLen = SUL_Decode7Bit(&(pNwkNames[3]),*ppLongName,pNwkNames[0]);
    }
    else
    {
        *ppLongName = (UINT8 *)CSW_NW_MALLOC(pNwkNames[0] + 4);
        if(!*ppLongName)
            return ERR_NO_MORE_MEMORY;
        SUL_ZeroMemory8(*ppLongName,pNwkNames[0] + 4);

        *pLongNameLen = pNwkNames[0];
        SUL_MemCopy8(*ppLongName,&(pNwkNames[3]),pNwkNames[0]);
    }
    return ERR_SUCCESS;

}

/**
 *  Function Description:
 *      Get the Network short name string, short name string length, Coding Scheme
 *  @param1
 *      pNwkNames: input network indication string
 *  @param2
 *      ppShortName: Output the short name string
 *  @param3
 *      pShortNameLen: Output the short name string length
 *  @param4
 *      pDCS: Output the short name string Coding Scheme,0->ASCII 1->UCS2
 *
 *  @return  ERR_SUCCESS | ERR_NO_MORE_MEMORY
 */

UINT32 CFW_GetNWIShortName(UINT8 *pNwkNames,UINT8 ** ppShortName,UINT8 * pShortNameLen,UINT8 * pDCS)
{

    /*
    0d long name length include dcs
    08 short name length include dcs
    85 special byte;
    ce 24 55 0b 3a 36 9b 20 28 b3 e9 04
    80 special byte;
    c7 66 13 04 65 36 9d
    */
    *pDCS = _GetNWIDCS(pNwkNames[2 + pNwkNames[0]]);
    *pShortNameLen = pNwkNames[1];

    if(0x00 == *pDCS)
    {
        *ppShortName = (UINT8 *)CSW_NW_MALLOC(pNwkNames[1]*8/7 + 4);
        if(!*ppShortName)
            return ERR_NO_MORE_MEMORY;
        SUL_MemSet8(*ppShortName,0x00,pNwkNames[1]*8/7 + 4);

        *pShortNameLen = SUL_Decode7Bit(&(pNwkNames[(pNwkNames[0])+3]),*ppShortName,pNwkNames[1]);
    }
    else
    {
        *ppShortName = (UINT8 *)CSW_NW_MALLOC(pNwkNames[1] + 4);
        if(!*ppShortName)
            return ERR_NO_MORE_MEMORY;
        SUL_ZeroMemory8(*ppShortName,pNwkNames[1] + 4);

        *pShortNameLen = pNwkNames[1];
        SUL_MemCopy8(*ppShortName,&(pNwkNames[(pNwkNames[0])+3]),pNwkNames[1]);
    }
    return ERR_SUCCESS;
}


UINT32 _SendNetWorkInfo2Uplayer( api_NwkInfoInd_t *nEvParam, CFW_SIM_ID nSimID)
{
    CFW_NW_NETWORK_INFO *pNetWorkInfo = NULL;
    api_NwkInfoInd_t *psInMsg = (api_NwkInfoInd_t *)nEvParam;
    if(!nEvParam)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("nEvParam NULL\n",0x081008ae)));
        return ERR_INVALID_PARAMETER;
    }

    pNetWorkInfo = (CFW_NW_NETWORK_INFO *)CSW_NW_MALLOC(SIZEOF(CFW_NW_NETWORK_INFO));
    if(!pNetWorkInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("_SendNetWorkInfo2Uplayer Malloc failed\n",0x081008af)));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pNetWorkInfo,SIZEOF(CFW_NW_NETWORK_INFO));
    SUL_MemCopy8(pNetWorkInfo, psInMsg, SIZEOF(CFW_NW_NETWORK_INFO));
    /*
    typedef struct _CFW_NW_NETWORK_INFO
    {
    UINT8 nLocalTimeZoneLen;
    UINT8 nLocalTimeZone;
    UINT8 nUniversalTimeZoneLen;
    UINT8 nUniversalTimeZone[7];
    UINT8 nDaylightSavingTimeLen;
    UINT8 nDaylightSavingTime;
    UINT8 nNwkFullNameLen;
    UINT8 nNwkShortNameLen;
    UINT8 nNwkNames[247];
    }CFW_NW_NETWORK_INFO;
    */
    CSW_TC_MEMBLOCK(CFW_NW_TS_ID | C_DETAIL, (UINT8 *)pNetWorkInfo, 100, 16);
    pNetWorkInfo->nUniversalTimeZone[0] = (pNetWorkInfo->nUniversalTimeZone[0] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[0] & 0x0F) << 4);
    pNetWorkInfo->nUniversalTimeZone[1] = (pNetWorkInfo->nUniversalTimeZone[1] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[1] & 0x0F) << 4);
    pNetWorkInfo->nUniversalTimeZone[2] = (pNetWorkInfo->nUniversalTimeZone[2] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[2] & 0x0F) << 4);
    pNetWorkInfo->nUniversalTimeZone[3] = (pNetWorkInfo->nUniversalTimeZone[3] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[3] & 0x0F) << 4);
    pNetWorkInfo->nUniversalTimeZone[4] = (pNetWorkInfo->nUniversalTimeZone[4] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[4] & 0x0F) << 4);
    pNetWorkInfo->nUniversalTimeZone[5] = (pNetWorkInfo->nUniversalTimeZone[5] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[5] & 0x0F) << 4);
    //pNetWorkInfo->nUniversalTimeZone[6] = (pNetWorkInfo->nUniversalTimeZone[6] >> 4) + ((pNetWorkInfo->nUniversalTimeZone[6] & 0x0F) << 4);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("_SendNetWorkInfo2Uplayer API_NWKINFO_IND  data:....\n",0x081008b0)));
    CSW_TC_MEMBLOCK(CFW_NW_TS_ID | C_DETAIL, (UINT8 *)pNetWorkInfo, 100, 16);
    // send the network info to up layer
    //
    if(pNetWorkInfo->nUniversalTimeZone[0] ||
            pNetWorkInfo->nUniversalTimeZone[1] ||
            pNetWorkInfo->nUniversalTimeZone[2] ||
            pNetWorkInfo->nUniversalTimeZone[3] ||
            pNetWorkInfo->nUniversalTimeZone[4] ||
            pNetWorkInfo->nUniversalTimeZone[5] ||
            pNetWorkInfo->nUniversalTimeZone[6])
    {

        CFW_PostNotifyEvent(EV_CFW_NW_NETWORKINFO_IND, (UINT32)pNetWorkInfo, 0, CFW_NW_IND_UTI | (nSimID << 24), 0x00);
    }
    else
    {
        if(pNetWorkInfo != NULL)
        {
            CSW_NW_FREE(pNetWorkInfo);
        }
    }
    return ERR_SUCCESS;
}

CFW_LOCK_BCCH gbLockBCCH = {FALSE, LOCK_BCCH_START};

UINT32 _ProcessLockBCCHNWRsp(CFW_SIM_ID nSimID)
{
    if(LOCK_BCCH_PROCESS == gbLockBCCH.bLockBCCHStatus)
    {
        CFW_SetComm(CFW_ENABLE_COMM, 1, APP_UTI_SHELL, nSimID);
        gbLockBCCH.bLockBCCHStatus = LOCK_BCCH_END;
    }
    else if(LOCK_BCCH_END == gbLockBCCH.bLockBCCHStatus)
    {
        gbLockBCCH.bLockBCCHStatus = LOCK_BCCH_START;
    }

    return ERR_SUCCESS;
}


UINT32 CFW_NwSetLockBCCH(BOOL bLock, UINT16 nUTI, CFW_SIM_ID nSimID )
{
    UINT8 bFM = 0x00;

    gbLockBCCH.bLockBCCH = bLock;
    gbLockBCCH.bLockBCCHStatus = LOCK_BCCH_PROCESS;

    CFW_GetComm((CFW_COMM_MODE *)&bFM, nSimID);
    if (CFW_ENABLE_COMM == bFM)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_ENABLE_COMM, start stack, nSimID=%d\n",0x081008b1)), nSimID);
        CFW_SetComm(CFW_DISABLE_COMM, 1, APP_UTI_SHELL, nSimID);

    }
    else if ((CFW_DISABLE_COMM == bFM) || (CFW_CHECK_COMM == bFM))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bFM = CFW_DISABLE_COMM, don't start stack, nSimID=%d\n",0x081008b2)), nSimID);
    }

    return ERR_SUCCESS;
}

BOOL CFW_NwGetLockBCCH()
{
    return gbLockBCCH.bLockBCCH;
}

PRIVATE VOID processFlowControl(HAO hAo, CFW_EV *pEvent, CFW_SIM_ID nSimID)
{
    api_FlowCtrInd_t *psInMsg = (api_FlowCtrInd_t *)pEvent->nParam1;

    if(pEvent->nEventId  == API_SYS_CTR_IND)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL), "EventID = API_SYS_CTR_IND, psInMsg->status = %d", psInMsg->status);
        gprs_flow_ctrl_G[nSimID] = psInMsg->status;

        if(FLOW_CONTROL_DISABLED == psInMsg->status)
        {
            UINT32 nUTI;
            NW_SM_INFO *proc = CFW_GetAoUserData(hAo);
            hal_UartSetRts(AT_DEFAULT_UART, TRUE);            
	     CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL), "Disable UartFlowControl ");
            CFW_GetUTI(hAo, &nUTI);
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL), "Send EV_CFW_GPRS_CTRL_RELEASE_IND ...");
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_CTRL_RELEASE_IND, psInMsg->status, nSimID ,  nUTI | (nSimID << 24), 0,proc->func);            // comsumed the message
        }
 	else
 	{
 	 	hal_UartSetRts(AT_DEFAULT_UART, FALSE);	     
		CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL), "Enable UartFlowControl");
 	}

        //else if(FLOW_CONTROL_ENABLED == psInMsg->status)
        //{
        //    gprs_flow_ctrl_G[nSimID] = FLOW_CONTROL_ENABLED;
        //}
    }
    else if(pEvent->nEventId == API_PS_ULCONGEST_IND)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL), "EventID = API_PS_ULCONGEST_IND, psInMsg->status = %d", psInMsg->status);
    }

#ifdef __USE_SMD__
    if(psInMsg->status == DISABLE_SERIAL_RX) //1: disable serial RX; 2:release serial RX
    {
        hal_ApCommPS_FlowCtrl(TRUE);
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("hal_ApCommPS_FlowCtrl %d\n",0x081008fd)), 1);

    }
    else if(psInMsg->status == ENABLE_SERIAL_RX)
    {
        hal_ApCommPS_FlowCtrl(FALSE);
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("hal_ApCommPS_FlowCtrl %d\n",0x081008fe)), 0);

    }
#endif
}

PRIVATE UINT32 CFW_NwAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 ret    = ERR_SUCCESS;
    UINT32 nState = 0;  // sm state

    NW_SM_INFO *proc = NULL;
    UINT32 nEventId = 0;  // message id from stack
    VOID *nEvParam  = NULL;
    UINT32 nUTI     = 0;

    HAO s_hAo = 0;  // staitc ao
    NW_SM_INFO *s_proc = NULL;  // static sm
    UINT32 s_nState = 0;  // static state

    CFW_EV ev;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwAoProc);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_NwAoProc start\n",0x081008b3)));

    CFW_SIM_ID nSimID = CFW_SIM_END;

    nSimID = CFW_GetSimCardID(hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_NwAoProc after get sim id\n",0x081008b4)));

    // verify the parameter
    if ((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_NwAoProc get message from AT \n",0x081008b5)));
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent     = &ev;
    }

    if ((hAo == 0) || (pEvent == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc (hAo == 0) || (pEvent == NULL) ERROR!!\n",0x081008b6)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwAoProc);
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        nEvParam = (VOID *)pEvent->nParam1;

        // CFW_SetAoState(hAo)
        proc     = CFW_GetAoUserData(hAo);
        nState   = CFW_GetAoState(hAo);
        nEventId = pEvent->nEventId;
        CFW_GetUTI(hAo, &nUTI);

        // get static information

        s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

        s_proc   = CFW_GetAoUserData(s_hAo);
        s_nState = CFW_GetAoState(s_hAo);
    }
    
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTXT(TSTR("CFW_NwAoProc nSIMID = 0x%x state = 0x%x, s_state = 0x%x eventId = 0x%x nUTI = 0x%x\n",0x081008b7)),
              nSimID, nState, s_nState, nEventId, nUTI);
    // for sim proc
    if (pEvent->nEventId == EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP)
    {
        // nEvParam the data
        if (pEvent->nType == 0)
        {
            UINT8 plmn[6];

            // success
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP    PLMNs value=%x!, plmn num = 0x%x\n",0x081008b8),
                      pEvent->nParam1, pEvent->nParam2);
            SUL_MemCopy8(plmn, (VOID *)pEvent->nParam1, 6);
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("auto register to:  \n",0x081008b9));

            // CFW_Dump(CFW_NW_TS_ID, (UINT8 *)pEvent->nParam1, 6 );

            CFW_NwSetRegistration(plmn, CFW_NW_MANUAL_MODE, nUTI, nSimID);

            s_proc->bReselNW = FALSE;

            // free the param1
            CSW_GPRS_FREE(pEvent->nParam1);
        }
        else
        {
            // fail
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP    fail \n",0x081008ba));

        }
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwAoProc);
        return ERR_SUCCESS;
    }
    if (pEvent->nEventId == API_NW_STORELIST_IND)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Store Monitor List for Sim %d\n",0x081008bb)), nSimID);
        CFW_StoredPlmnList *pStoreList = (CFW_StoredPlmnList *)nEvParam;
        CFW_CfgSetStoredPlmnList(pStoreList, nSimID);
#ifdef CHIP_HAS_AP
        CFW_PostNotifyEvent(EV_CFW_NW_STORELIST_IND, 0, 0, nUTI | (nSimID << 24), 0x0);
#endif
        return ERR_SUCCESS;
    }
    ret = CFW_NWDelay(hAo, pEvent);
    if (ERR_CONTINUE != ret)
    {
        // CFW_SIM_ID nSimID = CFW_SIM_END;
        UINT32 nCSTime  = 0;
        //VOID *nEvParam  = (VOID *)pEvent->nParam1;
        if(nEvParam!=NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwAoProc ERR_CONTINUE! status = %d \n",0x081008bc)),((api_NwCsRegStatusInd_t *)nEvParam)->Status);
        }
        nSimID = CFW_GetSimCardID(hAo);
        nCSTime = CFW_NW_GetCSDelayTime(nSimID);
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_NwAoProc ERR_CONTINUE! nSimID = 0x%x nCSTime = 0x%x, TimeFlag = %d \n",0x081008bd)),
                  nSimID, nCSTime, gCSDelayInfo[nSimID].nTimeFlag);
        return ret;
    }

    //
    // network sm state
    if (nState == CFW_SM_STATE_IDLE)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_SM_STATE_IDLE\n",0x081008be)));
        if (pEvent->nParam1 == 0xff)  // req to stack
        {
            if (proc->nMsgId == API_NW_LIST_REQ)
            {
                if (s_nState == CFW_SM_NW_STATE_IDLE)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_LIST_REQ to stack\n",0x081008bf)));
                    CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                    CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_IDLE);
                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("list req should be called in idle mode\n",0x081008c0)));
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED, 0,
                                        nUTI | (nSimID << 24), 0xF0);

                }

            }
            else if (proc->nMsgId == API_QUAL_REPORT_REQ)
            {
                CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_QUAL_REPORT_REQ to stack\n",0x081008c1)));

                // s_proc->nCSQUTI = nUTI;
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                // CFW_SetAoState(hAo, s_nState);
            }
            else if (proc->nMsgId == API_IMEI_REQ)
            {
                CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_IMEI_REQ to stack\n",0x081008c2)));

                CFW_SetAoState(hAo, s_nState);
            }
            else if ((proc->nMsgId == API_NW_SEL_REQ) || (proc->nMsgId == API_NW_RESEL_REQ))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("API_NW_SEL_REQ?API_NW_RESEL_REQ  0x%x s_nState  = 0x%x\n",0x081008c3)), proc->nMsgId, s_nState);
                if ((s_nState == CFW_SM_NW_STATE_IDLE) || (s_nState == CFW_SM_NW_STATE_SEARCH))
                {
                    if (s_proc->nNetMode != s_proc->nNetModeEx)
                    {
                        api_NwSelModeReq_t *pSelMode =
                            (api_NwSelModeReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_NwSelModeReq_t));
                        pSelMode->Mode = s_proc->nNetMode == API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE;

                        CFW_SendSclMessage(API_NW_SELMODE_REQ, pSelMode, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("send API_NW_SELMODE_REQ to stack first mode = 0x%x\n",0x081008c4)), pSelMode->Mode);
                        s_proc->nNetModeEx = s_proc->nNetMode;

                        CFW_CfgNwSetNetWorkMode(s_proc->nNetMode, nSimID);
                    }
                    if (proc->nMsgId == API_NW_SEL_REQ)
                    {
                        COS_Sleep(7000);
                        //hal_TimDelay(10*16384);
                        api_NwSelReq_t *pSelPlmn = NULL;
                        pSelPlmn = (api_NwSelReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_NwSelReq_t));
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("CFW_NwSetRegistration API_NW_SEL_REQ PLMN = %x%x%x%x%x%x \n",0x081008c5)),
                                  s_proc->nAttOperatorId[0], s_proc->nAttOperatorId[1], s_proc->nAttOperatorId[2], s_proc->nAttOperatorId[3], s_proc->nAttOperatorId[4], s_proc->nAttOperatorId[5]);
                        plmnArray2Api(s_proc->nAttOperatorId, pSelPlmn->PlmnId);  // covert to api format
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_NwSetRegistration end API_NW_SEL_REQ PLMN = %x %x %x \n",0x081008c6)),
                                  pSelPlmn->PlmnId[0], pSelPlmn->PlmnId[1], pSelPlmn->PlmnId[2]);
                        proc->pMsgBody = pSelPlmn;

                        CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_SEL_REQ to stack\n",0x081008c7)));
                    }
                    else
                    {
                        CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_RESEL_REQ to stack\n",0x081008c8)));
                    }

                    CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_SEARCH);  // set the static state
                    s_proc->nCOPSUTI = nUTI;  // used for sel or resel
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo); // un register the AO
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" sel or resel should be called in idle mode\n",0x081008c9)));
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                    CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP, ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED, 0,
                                        nUTI | (nSimID << 24), 0xF0);
                }
            }
            else if (proc->nMsgId == API_START_REQ)
            {
                // send the req to stack
                if ((s_nState == CFW_SM_NW_STATE_STOP) || (s_nState == CFW_SM_NW_STATE_IDLE))
                {
                    api_StartReq_t *pOutMsg = NULL;
                    pOutMsg = (api_StartReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_StartReq_t));

                    SUL_MemSet8(pOutMsg, 0xff, SIZEOF(api_StartReq_t));
					pOutMsg->Band = proc->nBand;
//#ifdef __MULTI_RAT__
 #ifdef LTE_NBIOT_SUPPORT
                    pOutMsg->Rat = CFW_NWGetRat(nSimID);
                    pOutMsg->bOnlyAtt = proc->bOnlyAtt;
#endif //LTE_NBIOT_SUPPORT
#ifdef ONLY_MMI_SUPPORT
                    pOutMsg->bLockArf = FALSE;
#else
                    pOutMsg->bLockArf = CFW_NwGetLockBCCH();
#endif
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Lock BCCH %d\n",0x081008ca)), pOutMsg->bLockArf);
                    proc->pMsgBody = pOutMsg;
#ifdef CSW_USER_DBS
                    // china mobile
                    if(0xFF != CFW_GetAutoCallCmd(nSimID))
                    {
                        pOutMsg->Plmn[0] = 0x00;
                        pOutMsg->Plmn[1] = 0x00;
                        pOutMsg->Plmn[2] = 0x00;
                    }
                    else
                    {
                        if( 0 == nSimID )// china mobile
                        {
                            pOutMsg->Plmn[0] = 0x64;
                            pOutMsg->Plmn[1] = 0xf0;
                            pOutMsg->Plmn[2] = 0x00;
                        }
                        else if(  1 == nSimID ) //china unicom
                        {
                            pOutMsg->Plmn[0] = 0x64;
                            pOutMsg->Plmn[1] = 0xf0;
                            pOutMsg->Plmn[2] = 0x10;
                        }
                        else
                        {
                            pOutMsg->Plmn[0] = 0xff;
                            pOutMsg->Plmn[1] = 0xff;
                            pOutMsg->Plmn[2] = 0xff;
                        }
                    }
#endif

                    CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_START_REQ to Stack\n",0x081008cb)));
                    if (s_proc->bDetached || s_proc->bChangeBand) // whether detached first
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc if (s_proc->bDetached || s_proc->bChangeBand)\n",0x081008cc)));
                    }
                    else  // and need send quality report req to stack too
                    {
                        api_QualReportReq_t *pReqMsg = NULL;

                        pReqMsg              = (api_QualReportReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_QualReportReq_t));
                        pReqMsg->StartReport = TRUE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc !!!!! if (s_proc->bDetached || s_proc->bChangeBand)\n",0x081008cd)));

                        CFW_SendSclMessage(API_QUAL_REPORT_REQ, pReqMsg, nSimID);
                    }

                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                    CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_SEARCH);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" start stack req should be called in stop state\n",0x081008ce)));
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                }
            }
            else if (proc->nMsgId == API_STOP_REQ)
            {
                CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_STOP_REQ to stack\n",0x081008cf)));
                if (!s_proc->bChangeBand) // just detach not change band
                {
                    s_proc->bDetached     = TRUE; // flag for detach
                    s_proc->bNetAvailable = FALSE;  // HAMEINA SETCOMM
                }
                else
                {
                    s_proc->nBandUTI = nUTI;
                }

                // set the static ao state
                CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_STOP);

                CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);

            }
            else if (proc->nMsgId == API_NW_PSATTACH_REQ)
            {
                // ps attach
                // should check the state of the gprs att state

                //Remvoe state check for GCF test
                //if (s_nState == CFW_SM_NW_STATE_IDLE)
                {
                    UINT32 gprs_ret = ERR_SUCCESS;

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_PSATTACH_REQ to stack\n",0x081008d0)));
                    api_NwPsAttachReq_t   *pOutMsg = NULL;
                    pOutMsg = (api_NwPsAttachReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_NwPsAttachReq_t  ));
                    SUL_MemSet8(pOutMsg, 0, SIZEOF(api_NwPsAttachReq_t  ));
                    *pOutMsg = proc->AttachType;
                    proc->pMsgBody = pOutMsg;

                    gprs_ret = CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);
                    if (gprs_ret == ERR_SUCCESS)
                    {
                        s_proc->nGprsAttState = CFW_GPRS_ATTACHED;
                    }
                    CFW_GetUTI(hAo, &nUTI);

                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                    s_proc->nAttach    = 1;
                    s_proc->nAttachUTI = nUTI;

                }

            }
            else if (proc->nMsgId == API_NW_PSDETACH_REQ)
            {
                // ps detach
                // should check the state of the gprs att state
                if (s_nState == CFW_SM_NW_STATE_IDLE)
                {
                    UINT32 gprs_ret = ERR_SUCCESS;

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_PSDETACH_REQ to stack\n",0x081008d2)));
                    api_NwPsDetachReq_t   *pOutMsg = NULL;
                    pOutMsg = (api_NwPsDetachReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_NwPsDetachReq_t  ));
                    SUL_MemSet8(pOutMsg, 0, SIZEOF(api_NwPsDetachReq_t  ));
                    *pOutMsg = proc->DetachType;
                    proc->pMsgBody = pOutMsg;
                    gprs_ret = CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);
                    if (gprs_ret == ERR_SUCCESS)
                    {
                        s_proc->nGprsAttState = CFW_GPRS_DETACHED;
                    }

                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                    s_proc->nAttach = 0;
                }
                else
                {
                    COS_CALLBACK_FUNC_T func = proc->func;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("pa detach should be called the detach in idle mode\n",0x081008d3)));
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                    CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ATT_RSP, ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED, 0,
                                        nUTI | (nSimID << 24), 0xF0, func);
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" The req not supported proc->nMsgId = 0x%x\n",0x081008d4)), proc->nMsgId);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

            }

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" should not Receive the pEvent->nParam1 = 0x%x\n",0x081008d5)), pEvent->nParam1);

            // CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            // not req
        }

        // CFW_StackInit();
    }
    else if (nState == CFW_SM_NW_STATE_STOP)  // Stop
    {
        /* event will be handle both in Static AO and Normal AO */
        if (nEventId == API_NW_PSREGSTATUS_IND)
        {
            api_NwPsRegStatusInd_t *psInMsg = (api_NwPsRegStatusInd_t *)nEvParam;
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "get API_NW_PSREGSTATUS_IND from stack Status: 0x%x ,Cause: 0x%x, staticAO=%d \n", psInMsg->Status, psInMsg->Cause, proc->bStaticAO);

            nUTI = s_proc->nAttachUTI;
            s_proc->sGprsStatusInfo.nCause = psInMsg->Cause;
            CFW_NwPsStatusConf(psInMsg, proc, nUTI, nSimID);
        }
        else
        {
            // static AO
            if (proc->bStaticAO)
            {
                if((pEvent->nEventId == API_SYS_CTR_IND) || (pEvent->nEventId == API_PS_ULCONGEST_IND))
                {
#ifdef CFW_GPRS_SUPPORT
                    processFlowControl(hAo, pEvent, nSimID);
#endif
                }
                else if (nEventId == API_NW_CSREGSTATUS_IND)
                {
                    api_NwCsRegStatusInd_t *p = (api_NwCsRegStatusInd_t *)nEvParam;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("receive API_NW_CSREGSTATUS_IND at stop state\n",0x081008db)));

                    CFW_NwCsStatusConf(p, proc, nSimID);

                    // Fix bug 16947 and 16888

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwAoProc p->Status %d\n",0x081008dc)), p->Status);

                    if (p->Status == API_NW_NO_SVCE)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc if (p->Status == API_NW_NO_SVCE)\n",0x081008dd)));

                        CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc !!!!   if (p->Status == API_NW_NO_SVCE)\n",0x081008de)));

                        CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                        CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_IDLE);
                    }
                }
#ifdef LTE_NBIOT_SUPPORT
	            else if(pEvent->nEventId == API_NW_ERRC_CONNSTATUS_IND)
	            {
	                api_NwErrcConnStatusInd_t *psInMsg = (api_NwErrcConnStatusInd_t *)nEvParam;
					s_proc->sGprsStatusInfo.ConnMode = psInMsg->ConnMode;
					CFW_PostNotifyEvent(EV_CFW_ERRC_CONNSTATUS_IND, 0, 0,
			                                nUTI | (nSimID << 24), psInMsg->ConnMode);
	     }
#endif
                else if (nEventId == API_QUAL_REPORT_CNF)
                {
                    // Added by Qiyan
                    api_QualReportCnf_t *p = (api_QualReportCnf_t *)nEvParam;
                    CSW_TRACE(_CSW|TLEVEL(32)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_QUAL_REPORT_CNF from stack\n",0x081008df)));

                    CFW_NwQualReportConf(p, proc, nSimID);
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("receive API_QUAL_REPORT_CNF at stop state\n",0x081008e0)));
                }
                else if (nEventId == API_NWKINFO_IND)
                {
                    _SendNetWorkInfo2Uplayer(nEvParam, nSimID);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc warring unknow eventid \n",0x081008e1)));

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                }

            }
            else
            {
                if (nEventId == API_STOP_CNF)
                {
                    // confirm the stop of the stack

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_STOP_CNF from stack\n",0x081008e2)));
                    s_proc->sStatusInfo.nStatus    = CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING;
                    s_proc->nNetModeEx             = CFW_NW_AUTOMATIC_MODE;
                    s_proc->bStaticAO              = TRUE;
                    s_proc->bReselNW               = FALSE;
                    s_proc->bDetached              = FALSE;
                    s_proc->bRoaming               = FALSE;
                    s_proc->bNetAvailable          = FALSE;
                    s_proc->nAttach                = 0xFF;
                    s_proc->nCsStatusIndCount      = 0;
                    s_proc->bTimerneed             = FALSE;
                    s_proc->bTimeDeleted           = FALSE;
                    s_proc->sGprsStatusInfo.nCause = 0;
                    s_proc->nGprsAttState          = CFW_GPRS_DETACHED;  // 0xFF;

                    // lai, cellid =0
                    SUL_ZeroMemory8(s_proc->sStatusInfo.nAreaCode, 5);
                    SUL_ZeroMemory8(s_proc->sStatusInfo.nCellId, 2);
                    //SUL_ZeroMemory8(s_proc->nOperatorId, 6);
                    s_proc->Rssi = 99;
                    s_proc->Ber  = 99;

                    CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_STOP);

                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);

                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                    // any others???
#if defined(SIMCARD_HOT_DETECT)
                    if (STOP_CAUSE_HOT_DETECT == s_proc->nStpCause)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc  nStpCause is SIMCARD Plugin or Plugout. \n",0x081008e3)));
                        s_proc->nStpCause = STOP_CAUSE_NOMAL;
                        CFW_CfgNwGetFrequencyBand(&s_proc->nBand, nSimID);
#ifdef CSW_USER_DBS
                        CFW_NwStartStack(s_proc->nBand,0xff, nSimID);
#else
#ifdef __MULTI_RAT__
                        CFW_CfgNwGetFrequencyBand(&s_proc->nUBand, nSimID);
                        CFW_NwStartStack(s_proc->nBand, s_proc->nUBand, nSimID);
#else
                        CFW_NwStartStack(s_proc->nBand, nSimID);
#endif
#endif

                    }
                    else
#endif
                        if (STOP_CAUSE_FB == s_proc->nStpCause)
                        {
#ifdef CSW_USER_DBS
                            UINT8 nStrtFlag = 0xff;
#endif
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc  nStpCause is Frequency Band\n",0x081008e4)));
                            s_proc->nStpCause = STOP_CAUSE_NOMAL;

#ifdef CSW_USER_DBS
                            CFW_NwStartStack(s_proc->nBand, nStrtFlag, nSimID);
#else
#ifdef __MULTI_RAT__
                            CFW_NwStartStack(s_proc->nBand, s_proc->nUBand, nSimID);
#else
                            CFW_NwStartStack(s_proc->nBand, nSimID);
#endif
#endif
                        }
                        else if (STOP_CAUSE_POWEROFF == s_proc->nStpCause)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL),
                                    "CFW_NwAoProc  nStpCause PowerOFF\n");
                            s_proc->nStpCause = STOP_CAUSE_NOMAL;
#ifdef LTE_NBIOT_SUPPORT
                            CFW_GprsDeactiveAll(nSimID);
#endif  
                            s_proc->bPowerOff = TRUE;
                        }

                        else if (STOP_CAUSE_FM == s_proc->nStpCause)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc  nStpCause is Flight Mode\n",0x081008e5)));
                            s_proc->nStpCause = STOP_CAUSE_NOMAL;
#ifdef LTE_NBIOT_SUPPORT
			   CFW_GprsDeactiveAll(nSimID);
#endif  
                            CFW_PostNotifyEvent(EV_CFW_NW_DEREGISTER_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                            CFW_PostNotifyEvent(EV_CFW_NW_DEREGISTER_RSP, 0, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0);
                        }
#ifdef CSW_USER_DBS
                        else if(STOP_CAUSE_DBS  == s_proc->nStpCause)
                        {
                            UINT8 nStrtFlag = 0xff;
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwAoProc _DBS nOperatorId[4]:0x%x \n",0x081008e6)), s_proc->nOperatorId[4]);

                            //s_proc->nStpCause = STOP_CAUSE_NOMAL;
                            if(1 == s_proc->nOperatorId[4])
                            {
                                nStrtFlag = 1;
                            }
                            else
                            {
                                nStrtFlag = 0;
                            }


                            //start stack use other plmn
                            //get plmn
                            CFW_NwStartStack(s_proc->nBand, nStrtFlag, nSimID);
                        }
#endif
#ifdef LTE_NBIOT_SUPPORT
                   else if(STOP_CAUSE_NOMAL ==  s_proc->nStpCause)
                   {
			           //s_proc->sGprsStatusInfo.nCurrRat = 0;   这个变量没有用，可以用这个代替全局变量
			           s_proc->sGprsStatusInfo.nStatus = CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING;
			           s_proc->sGprsStatusInfo.PsType = 0;
                       CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("at+cgatt=0 cnf from mmc",0x081008e5)));
			           CFW_GprsDeactiveAll(nSimID);
                        //notify cereg:0
                       CFW_PostNotifyEvent(EV_CFW_GPRS_ATT_RSP, 0, 0, nUTI | (nSimID << 24), CFW_GPRS_DETACHED);
                   }
 #endif
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc nStpCause is ERROR\n",0x081008e7)));
                        }

                    SUL_ZeroMemory8(s_proc->nOperatorId, 6);

                }
                else if (nEventId == API_IMEI_IND)
                {
                    // state not change
                    api_ImeiInd_t *p = (api_ImeiInd_t *)nEvParam;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_IMEI_IND from stack\n",0x081008e8)));

                    CFW_NwImeiConf(p, proc, nUTI, nSimID);
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                    // un register ao
                }
                else if (nEventId == API_NWKINFO_IND)
                {
                    _SendNetWorkInfo2Uplayer(nEvParam, nSimID);

                }
                else if (nEventId == API_NW_CSREGSTATUS_IND)
                {
                    api_NwCsRegStatusInd_t *p = (api_NwCsRegStatusInd_t *)nEvParam;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("receive API_NW_CSREGSTATUS_IND at CFW_SM_NW_STATE_STOP state\n",0x081008e9)));

                    CFW_NwCsStatusConf(p, proc, nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwAoProc p->Status %d\n",0x081008ea)), p->Status);

                    if (p->Status == API_NW_NO_SVCE)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc if (p->Status == API_NW_NO_SVCE)\n",0x081008eb)));

                        CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc !!!!   if (p->Status == API_NW_NO_SVCE)\n",0x081008ec)));

                        CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                        CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_IDLE);
                    }
                }
                else  // error
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("not support event line %d 0x%x\n",0x081008ed)), __LINE__,nEventId);
                    CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                }
            }
        }
    }
    else if (nState == CFW_SM_NW_STATE_SEARCH)  // Search
    {
        // static AO
        if (proc->bStaticAO)
        {
            if (nEventId == API_NW_CSREGSTATUS_IND)
            {
                api_NwCsRegStatusInd_t *p = (api_NwCsRegStatusInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_CSREGSTATUS_IND from stack\n",0x081008ee)));

                CFW_NwCsStatusConf(p, proc, nSimID);
#ifdef CSW_USER_DBS
                if (STOP_CAUSE_DBS == s_proc->nStpCause)
                {
                    CFW_TSM_FUNCTION_SELECT tSelecFUN;
                    UINT16 nUTI = 0;

                    s_proc->nStpCause = STOP_CAUSE_NOMAL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_CSREGSTATUS_IND STOP_CAUSE_FB == s_proc->nStpCause\n",0x081008ef)));

                    tSelecFUN.nNeighborCell  = 1;
                    tSelecFUN.nServingCell    = 1;
                    CFW_GetFreeUTI(0, &nUTI);
                    CFW_EmodOutfieldTestStart(&tSelecFUN, nUTI, 0);

                }
#endif
                if (p->Status == API_NW_NO_SVCE)
                {
                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);
                }
                else
                {
                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                }
            }
            else if (nEventId == API_NW_PSREGSTATUS_IND)
            {
                // used for furture
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_PSREGSTATUS_IND from stack\n",0x081008f0)));
                
                //
                //Deal with when CS is doing search network,PS register full service firstly.
                //
                api_NwPsRegStatusInd_t *psInMsg = (api_NwPsRegStatusInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "Status: 0x%x ,Cause:0x%x, staticAO:%d \n", psInMsg->Status, psInMsg->Cause, proc->bStaticAO);

                nUTI = s_proc->nAttachUTI;
                s_proc->sGprsStatusInfo.nCause = psInMsg->Cause;
                CFW_NwPsStatusConf(psInMsg, proc, nUTI, nSimID);
#ifdef LTE_NBIOT_SUPPORT
                if (psInMsg->Status != API_NW_NO_SVCE)
				{
				    CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
				}
#endif
            }
						// for cscon ind
#ifdef LTE_NBIOT_SUPPORT
			 else if(pEvent->nEventId == API_NW_ERRC_CONNSTATUS_IND)
			 {
				 api_NwErrcConnStatusInd_t *psInMsg = (api_NwErrcConnStatusInd_t *)nEvParam;
			  s_proc->sGprsStatusInfo.ConnMode = psInMsg->ConnMode;
			  CFW_PostNotifyEvent(EV_CFW_ERRC_CONNSTATUS_IND, 0, 0,
									nUTI | (nSimID << 24), psInMsg->ConnMode);
	
			 }
#endif

            else if((pEvent->nEventId == API_SYS_CTR_IND) || (pEvent->nEventId == API_PS_ULCONGEST_IND))
            {
#ifdef CFW_GPRS_SUPPORT
                processFlowControl(hAo, pEvent, nSimID);
#endif

            }
            else if (nEventId == API_QUAL_REPORT_CNF)
            {
                // state not change
                api_QualReportCnf_t *p = (api_QualReportCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_QUAL_REPORT_CNF from stack\n",0x081008f4)));

                CFW_NwQualReportConf(p, proc, nSimID);
            }
            else if (nEventId == API_NWKINFO_IND)
            {
                _SendNetWorkInfo2Uplayer(nEvParam, nSimID);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("not support event line %d 0x%x\n",0x081008f5)), __LINE__,nEventId);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
        }
        else
        {
            if (nEventId == API_IMEI_IND)
            {
                // state not change
                api_ImeiInd_t *p = (api_ImeiInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_IMEI_IND from stack\n",0x081008f6)));

                CFW_NwImeiConf(p, proc, nUTI, nSimID);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                // un register ao
            }
            else if (nEventId == API_NWKINFO_IND)
            {
                _SendNetWorkInfo2Uplayer(nEvParam, nSimID);
            }
            else  // error
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("not support event line %d 0x%x\n",0x081008f7)), __LINE__,nEventId);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
        }

    }
    else if (nState == CFW_SM_NW_STATE_IDLE)  // Idle
    {
        if (pEvent->nParam1 == 0xff)  // req to stack
        {
            if (proc->nMsgId == API_NW_LISTABORT_REQ)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send API_NW_LISTABORT_REQ to stack\n",0x081008f8)));

                CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_IDLE);
                CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
            }
        }

        // static AO
        if (proc->bStaticAO)
        {
            if (nEventId == API_NW_CSREGSTATUS_IND)
            {
                // store reg status
                // if the URC is on, response to AT, and store the message
                // else, store the message , not response to AT
                //
                api_NwCsRegStatusInd_t *p = (api_NwCsRegStatusInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_CSREGSTATUS_IND from stack\n",0x081008f9)));
#ifdef CSW_USER_DBS
                if (STOP_CAUSE_DBS == s_proc->nStpCause)
                {
                    CFW_TSM_FUNCTION_SELECT tSelecFUN;
                    UINT16 nUTI = 0;

                    s_proc->nStpCause = STOP_CAUSE_NOMAL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_CSREGSTATUS_IND STOP_CAUSE_FB == s_proc->nStpCause\n",0x081008fa)));

                    tSelecFUN.nNeighborCell  = 1;
                    tSelecFUN.nServingCell    = 1;
                    CFW_GetFreeUTI(0, &nUTI);
                    CFW_EmodOutfieldTestStart(&tSelecFUN, nUTI, 0);

                }
#endif
                CFW_NwCsStatusConf(p, proc, nSimID);
                if (p->Status == API_NW_NO_SVCE)
                {
                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);
                }
                else
                {
                    CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                }
            }
            else if (nEventId == API_NW_PSREGSTATUS_IND)
            {
                // used for future
                api_NwPsRegStatusInd_t *psInMsg = (api_NwPsRegStatusInd_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2),
                          TSTXT(TSTR("get API_NW_PSREGSTATUS_IND from stack psInMsg->Status: 0x%x ,psInMsg->Cause: 0x%x \n",0x081008fb)),
                          psInMsg->Status, psInMsg->Cause);

                // Add by lixp for UTI zero bug at 20090723.

                nUTI                           = s_proc->nAttachUTI;
                s_proc->sGprsStatusInfo.nCause = psInMsg->Cause;
                CFW_NwPsStatusConf(psInMsg, proc, nUTI, nSimID);
            }
			// for cscon ind
#ifdef LTE_NBIOT_SUPPORT
			 else if(pEvent->nEventId == API_NW_ERRC_CONNSTATUS_IND)
			 {
				 api_NwErrcConnStatusInd_t *psInMsg = (api_NwErrcConnStatusInd_t *)nEvParam;
			  s_proc->sGprsStatusInfo.ConnMode = psInMsg->ConnMode;
			  CFW_PostNotifyEvent(EV_CFW_ERRC_CONNSTATUS_IND, 0, 0,
									nUTI | (nSimID << 24), psInMsg->ConnMode);
	
			 }
#endif

            else if((pEvent->nEventId == API_SYS_CTR_IND) || (pEvent->nEventId == API_PS_ULCONGEST_IND))
            {
#ifdef CFW_GPRS_SUPPORT
                processFlowControl(hAo, pEvent, nSimID);
#endif
            }
            else if (nEventId == API_QUAL_REPORT_CNF)
            {
                // state not change
                // used by system, and store the message , and the message will be used by AT.
                api_QualReportCnf_t *p = (api_QualReportCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_QUAL_REPORT_CNF from stack\n",0x081008ff)));

                CFW_NwQualReportConf(p, proc, nSimID);
            }
            else if (nEventId == API_NWKINFO_IND)
            {
                _SendNetWorkInfo2Uplayer(nEvParam, nSimID);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc warring unknow eventid  11111\n",0x08100900)));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        else
        {
            if (nEventId == API_NW_LIST_IND)
            {
                // state not change
                // notify AT: EV_CFW_NW_GET_AVAIL_OPERATOR_RSP
                // with parameter: CFW_NW_OPERATOR_INFO list, and the number of operators
                api_NwListInd_t *p = (api_NwListInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_NW_LIST_IND from stack\n",0x08100901)));
                SUL_MemCopy8(proc->nOperatorId, s_proc->nOperatorId, 6);

                CFW_NwListConf(p, proc, nUTI, nSimID);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
            else if (nEventId == API_NW_LISTABORT_IND)
            {
                api_NwListAbortInd_t *p = (api_NwListAbortInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("get API_NW_LISTABORT_IND from stack cause: 0x%x\n",0x08100902)), p->Cause);
                if (API_NORMAL_LIST_ABORT == p->Cause)
                {
                    CFW_PostNotifyEvent(EV_CFW_NW_ABORT_LIST_OPERATORS_RSP, 0, 0, nUTI | (nSimID << 24), 0x00);
                }
                else if (API_ABORM_LIST_ABORT == p->Cause)  // we receive this event when we have send list REQ and have a call to come in.we will don't receive list IND,but we will receive abort IND and cause is 1.
                {
//#define AUTO_RELIST
#define AUTO_RELIST_MAX_NUM 2
#ifdef AUTO_RELIST
                    if(AUTO_RELIST_MAX_NUM > proc->nReListNum)
                    {
                        proc->nReListNum++;
                        if (s_nState == CFW_SM_NW_STATE_IDLE)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("resend API_NW_LIST_REQ to stack\n",0x081008bf)));
                            CFW_SendSclMessage(proc->nMsgId, proc->pMsgBody, nSimID);

                            CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_IDLE);
                            CFW_SetAoState(hAo, CFW_SM_NW_STATE_IDLE);
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("list req should be called in idle mode\n",0x081008c0)));
                            CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
                            CFW_PostNotifyEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED, 0,
                                                nUTI | (nSimID << 24), 0xF0);

                        }
                        return ERR_SUCCESS;
                    }
#endif

                    CFW_PostNotifyEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, (UINT32)NULL, 0, nUTI | (nSimID << 24), 0xF0);
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_NW_ABORT_LIST_OPERATORS_RSP, 0, 0, nUTI | (nSimID << 24), 0xF0);
                }

                // response to the list abort
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

            }
            else if (nEventId == API_IMEI_IND)
            {
                // state not change
                // notify AT: EV_CFW_NW_GET_IMEI_ID_RSP
                // with parameter: Pointer to the buffer to hold the IMEI number in ASCII string format
                // and the buffer length
                api_ImeiInd_t *p = (api_ImeiInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_IMEI_IND from stack\n",0x08100903)));

                CFW_NwImeiConf(p, proc, nUTI, nSimID);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
            else if (nEventId == API_NWKINFO_IND)
            {
                _SendNetWorkInfo2Uplayer(nEvParam, nSimID);
            }
            else if (nEventId == API_STOP_CNF)
            {
                // confirm the stop of the stack

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get API_STOP_CNF from stack\n",0x08100904)));
                s_proc->sStatusInfo.nStatus    = CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING;
                s_proc->nNetModeEx             = CFW_NW_AUTOMATIC_MODE;
                s_proc->bStaticAO              = TRUE;
                s_proc->bReselNW               = FALSE;
                s_proc->bDetached              = FALSE;
                s_proc->bRoaming               = FALSE;
                s_proc->bNetAvailable          = FALSE;
                s_proc->nAttach                = 0xFF;
                s_proc->nCsStatusIndCount      = 0;
                s_proc->bTimerneed             = FALSE;
                s_proc->bTimeDeleted           = FALSE;
                s_proc->sGprsStatusInfo.nCause = 0;
                s_proc->nGprsAttState          = CFW_GPRS_DETACHED;  // 0xFF;

                // lai, cellid =0
                SUL_ZeroMemory8(s_proc->sStatusInfo.nAreaCode, 5);
                SUL_ZeroMemory8(s_proc->sStatusInfo.nCellId, 2);
                //SUL_ZeroMemory8(s_proc->nOperatorId, 6);
                s_proc->Rssi = 99;
                s_proc->Ber  = 99;

                CFW_SetAoState(s_hAo, CFW_SM_NW_STATE_STOP);

                CFW_SetAoState(hAo, CFW_SM_NW_STATE_STOP);

                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);

                // any others???
                if (STOP_CAUSE_FB == s_proc->nStpCause)
                {
#ifdef CSW_USER_DBS
                    UINT8 nStrtFlag = 0xff;
#endif
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc  nStpCause is Frequency Band\n",0x08100905)));
                    s_proc->nStpCause = STOP_CAUSE_NOMAL;

#ifdef CSW_USER_DBS
                    CFW_NwStartStack(s_proc->nBand, nStrtFlag, nSimID);
#else
#ifdef __MULTI_RAT__
                    CFW_NwStartStack(s_proc->nBand, s_proc->nUBand, nSimID);
#else
                    CFW_NwStartStack(s_proc->nBand, nSimID);
#endif
#endif

                }
                else if (STOP_CAUSE_FM == s_proc->nStpCause)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc  nStpCause is Flight Mode\n",0x08100906)));
                    s_proc->nStpCause = STOP_CAUSE_NOMAL;
                    CFW_PostNotifyEvent(EV_CFW_NW_DEREGISTER_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                    CFW_PostNotifyEvent(EV_CFW_NW_DEREGISTER_RSP, 0, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0);
                }
#ifdef CSW_USER_DBS
                else if(STOP_CAUSE_DBS  == s_proc->nStpCause)
                {
                    UINT8 nStrtFlag = 0xff;
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwAoProc _DBS nOperatorId[4]:0x%x \n",0x08100907)), s_proc->nOperatorId[4]);

                    if(1 == s_proc->nOperatorId[4])
                    {
                        nStrtFlag = 1;
                    }
                    else
                    {
                        nStrtFlag = 0;
                    }


                    //start stack use other plmn
                    //get plmn
                    CFW_NwStartStack(s_proc->nBand, nStrtFlag, nSimID);
                }
#endif
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc nStpCause is ERROR\n",0x08100908)));
                }

                SUL_ZeroMemory8(s_proc->nOperatorId, 6);

            }
            else  // error
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAoProc warring unknow eventid  22222\n",0x08100909)));

                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }

    }
    else  // state error
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("In CFW_NwAoProc state error\n",0x0810090a)));
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_NwAoProc);
    return ret;
}

// ////////////////////////////////////////////////////////////////////////
//
// CM Interface by MMI
//

//
// get available operators, send API_NW_LIST to stack
// and the response : API_NW_LIST_IND
//
UINT32 CFW_NwGetAvailableOperators(UINT16 nUTI
                                   , CFW_SIM_ID nSimID
                                  )
{
    // should not be called in stop state
    // state not change
    NW_SM_INFO *proc = NULL;

    // UINT32 nState    = 0;       // sm state
    HAO hAo   = 0;  // Handle
    HAO s_hAo = 0;  // static handle

    NW_SM_INFO *s_proc = NULL;  // static sm info
    CSW_PROFILE_FUNCTION_ENTER(CFW_NwGetAvailableOperators);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwGetAvailableOperators start nUTI = 0x%x\n",0x0810090b)), nUTI);

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !! get sim id error!\n",0x0810090c)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetAvailableOperators);
        return ret;
    }

#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !! UTI BUSY!\n",0x0810090d)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetAvailableOperators);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("malloc exception\n",0x0810090e)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetAvailableOperators);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory8(proc, SIZEOF(NW_SM_INFO));

    // save the selected mode to Ao (static AO)
    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! s_hAo == 0 \n",0x0810090f)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetAvailableOperators);
        return - 1; // AOM error
    }

    s_proc = CFW_GetAoUserData(s_hAo);

    if (s_proc->bTimerneed)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("kill the network timer\n",0x08100910)));

        s_proc->bTimerneed = FALSE;
        proc->bTimeDeleted = TRUE;

        // kill the timer
        UINT32 nTimerId = 0;

        if (CFW_SIM_0 == nSimID)
            nTimerId = PRV_CFW_NW_SIM0_TIMER_ID;
        else if (CFW_SIM_1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM1_TIMER_ID;
        else if (CFW_SIM_1 + 1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM2_TIMER_ID;
        else if (CFW_SIM_1 + 2 == nSimID)
            nTimerId = PRV_CFW_NW_SIM3_TIMER_ID;
        COS_KillTimerEX(0xFFFFFFFF, nTimerId);
    }

    proc->nMsgId    = API_NW_LIST_REQ;
    proc->pMsgBody  = NULL;
    proc->bStaticAO = FALSE;

    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetAvailableOperators end\n",0x08100911)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetAvailableOperators);
    return ERR_SUCCESS;
}

UINT32 CFW_NwGetCurrentOperator(UINT8 OperatorId[6], UINT8 *pMode
                                , CFW_SIM_ID nSimID
                               )
{
    // state not change
    NW_SM_INFO *s_proc = NULL;
    HAO s_hAo = 0;  // Handle

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetCurrentOperator start\n",0x08100912)));

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwGetCurrentOperator);

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetCurrentOperator SIM ERROR!\n",0x08100913)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetCurrentOperator);
        return ret;
    }

    if ((pMode == NULL) || (OperatorId == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetCurrentOperator input ERROR!\n",0x08100914)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetCurrentOperator);
        return ERR_CFW_INVALID_PARAMETER;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetCurrentOperator);
        return - 1; // AOM error
    }

    s_proc = CFW_GetAoUserData(s_hAo);  // get info from AO
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("CFW_NwGetCurrentOperator PLMN = %x%x%x%x%x mode = 0x%x\n",0x08100915)),
              s_proc->nOperatorId[0],
              s_proc->nOperatorId[1],
              s_proc->nOperatorId[2],
              s_proc->nOperatorId[3],
              s_proc->nOperatorId[4],
              s_proc->nNetMode == API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE);

    SUL_MemCopy8(OperatorId, s_proc->nOperatorId, 6);
    *pMode = s_proc->nNetMode == API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE;
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetCurrentOperator end\n",0x08100916)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetCurrentOperator);
    return ERR_SUCCESS;
}

//
// get Imei , send API_IMEI_REQ to stack with no parameter
// response API_IMEI_CNF with parameter : IMEI
UINT32 CFW_NwGetImei(UINT16 nUTI
                     , CFW_SIM_ID nSimID
                    )
{
    // state not change
    NW_SM_INFO *proc = NULL;
    HAO hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwGetImei);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwGetImei nUTI = 0x%x\n",0x08100917)), nUTI);

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetImei SIM ERROR!\n",0x08100918)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetImei);
        return ret;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("UTI busy\n",0x08100919)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetImei);
        return ERR_CFW_UTI_IS_BUSY;
    }

    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("malloc exception\n",0x0810091a)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetImei);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));

    proc->nMsgId    = API_IMEI_REQ;
    proc->pMsgBody  = NULL;
    proc->bStaticAO = FALSE;

    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetImei end\n",0x0810091b)));

    // CFW_SendSclMessage( API_IMEI_REQ, NULL);
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetImei);
    return ERR_SUCCESS;
}

//
// get signal quality
// notify AT: EV_CFW_NW_GET_SIGNAL_QUALITY_RSP with parameter: rssi , ber
UINT32 SRVAPI CFW_NwGetSignalQuality(UINT8 *pSignalLevel, UINT8 *pBitError
                                     , CFW_SIM_ID nSimID
                                    )
{
    // state not change
    NW_SM_INFO *s_proc = NULL;  // sm info
    HAO s_hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwGetSignalQuality);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR! CFW_CheckSimId start\n",0x0810091c)));
    /*
      UINT8 ret;
      ret = CFW_CheckSimId(nSimID);
      if (ERR_SUCCESS != ret)
      {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_CheckSimId SIM ERROR!\n",0x0810091d)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetSignalQuality);
        return ret;
      }
      */

    if ((pSignalLevel == NULL) || (pBitError == NULL))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetSignalQuality);
        return ERR_CFW_INVALID_PARAMETER;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR! s_hAo == 0 \n",0x0810091e)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetSignalQuality);
        return - 1; // AOM error
    }

    s_proc = CFW_GetAoUserData(s_hAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_NwGetSignalQuality end rssi = %d, ber = %d \n",0x0810091f)), s_proc->Rssi, s_proc->Ber);
    *pSignalLevel = s_proc->Rssi;
    *pBitError    = s_proc->Ber;

    CSW_PROFILE_FUNCTION_EXIT(CFW_NwGetSignalQuality);
    return ERR_SUCCESS;
}

//
// get the reg status from the stored message
//
UINT32 SRVAPI CFW_NwGetStatus(CFW_NW_STATUS_INFO *pStatusInfo
                              , CFW_SIM_ID nSimID
                             )
{
    NW_SM_INFO *s_proc = NULL;  // sm info
    HAO s_hAo = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwGetStatus start\n",0x08100920)));

#if 0
    UINT8 ret;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwGetStatus SIM ERROR!\n",0x08100921)));
        return ret;
    }
#endif

    if (pStatusInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwGetStatus SIM input error!\n",0x08100922)));
        return ERR_CFW_INVALID_PARAMETER;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("s_hAo == 0 \n",0x08100923)));
        return - 1; // AOM error
    }

    s_proc = CFW_GetAoUserData(s_hAo);

    SUL_MemCopy8(pStatusInfo, &s_proc->sStatusInfo, SIZEOF(CFW_NW_STATUS_INFO));

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), TSTXT("CFW_NwGetStatus CurrRat = 0x%x, nSimID=%d\n"), pStatusInfo->nCurrRat, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwGetStatus status = 0x%x\n",0x08100924)), pStatusInfo->nStatus);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTXT(TSTR("CFW_NwGetStatus end LAC = 0x%x, 0x%x, cell id = 0x%x, 0x%x\n",0x08100925)),
              pStatusInfo->nAreaCode[3], pStatusInfo->nAreaCode[4], pStatusInfo->nCellId[0], pStatusInfo->nCellId[1]);
#ifdef __MULTI_RAT__
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_NwGetStatus CurrRat = 0x%x nCellId[2,3] 0x%x,  0x%x\n",0x08100926)), pStatusInfo->nCurrRat, pStatusInfo->nCellId[2], pStatusInfo->nCellId[3]);
#endif
    return ERR_SUCCESS;

}

//
// set the select mode and attempt to register the selected operator.
// if the mode=0, the operatorId should be ignored.
// if the mode=1, the operatorId should be valid.
// if the mode=4, the operatorId should be valid, and if the attempt failed, as if mode=0 will be entered.
UINT32 CFW_NwSetRegistration(UINT8 nOperatorId[6], UINT8 nMode, UINT16 nUTI
                             , CFW_SIM_ID nSimID
                            )
{
    UINT32 s_nState = 0;  // static sm state
    HAO hAo         = 0;  // Handle
    HAO s_hAo       = 0;  // static handle

    NW_SM_INFO *proc = NULL;  // sm info
    NW_SM_INFO *s_proc = NULL;  // static sm info

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwSetRegistration);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwSetRegistration start\n",0x08100927)));

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwSetRegistration SIM ERROR!\n",0x08100928)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return ret;
    }

    // validate parameters
    if ((nMode != CFW_NW_AUTOMATIC_MODE) && (nMode != CFW_NW_MANUAL_MODE) && (nMode != CFW_NW_MANUAL_AUTOMATIC_MODE))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !nMode is not correct \n",0x08100929)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_NwSetRegistration mode = %d nUTI = 0x%x\n",0x0810092a)), nMode, nUTI);

#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !UTI_IS_BUSY \n",0x0810092b)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !malloc exception\n",0x0810092c)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));

    // save the selected mode to Ao (static AO)

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !s_hAo == 0 \n",0x0810092d)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return - 1; // AOM error
    }

    s_proc   = CFW_GetAoUserData(s_hAo);
    s_nState = CFW_GetAoState(s_hAo);

    s_proc->nNetMode = nMode;
    s_proc->bReselNW = TRUE;
    CFW_SetNWTimerOutFlag(TRUE, nSimID);

    // s_proc->nCOPSUTI = nUTI;
    s_proc->nReselUTI         = nUTI;
    s_proc->nCsStatusIndCount = 0;

    // if time need, delete the timer, else do nothing
    if (s_proc->bTimerneed)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("kill the network timer\n",0x0810092e)));
        s_proc->bTimerneed = FALSE;

        // kill the timer
        UINT32 nTimerId = 0;

        if (CFW_SIM_0 == nSimID)
            nTimerId = PRV_CFW_NW_SIM0_TIMER_ID;
        else if (CFW_SIM_1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM1_TIMER_ID;
        else if (CFW_SIM_1 + 1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM2_TIMER_ID;
        else if (CFW_SIM_1 + 2 == nSimID)
            nTimerId = PRV_CFW_NW_SIM3_TIMER_ID;
        COS_KillTimerEX(0xFFFFFFFF, nTimerId);
    }
    if (nMode != CFW_NW_AUTOMATIC_MODE)
    {
        SUL_MemCopy8(s_proc->nAttOperatorId, nOperatorId, 6);
    }

    if (s_nState == CFW_SM_NW_STATE_STOP)
    {
//        COS_Sleep(2000);
#ifdef CSW_USER_DBS
        UINT8 nStrtFlag = 0xff;
#endif
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("in stop state\n",0x0810092f)));

#ifdef CSW_USER_DBS
        CFW_NwStartStack(s_proc->nBand, nStrtFlag, nSimID);
#else
#ifdef __MULTI_RAT__
        CFW_NwStartStack(s_proc->nBand, s_proc->nUBand, nSimID);
#else
        CFW_NwStartStack(s_proc->nBand, nSimID);
#endif
#endif

        CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
        return ERR_SUCCESS;
    }
    else
    {
	if( API_NW_NO_SVCE == s_proc->sStatusInfo.nStatus)
            return ERR_CME_NO_NETWORK_SERVICE;
    }

    if (nMode == CFW_NW_AUTOMATIC_MODE) // automatic mode
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwSetRegistration automatic mode\n",0x08100930)));

        // should use AOM
        proc->nMsgId    = API_NW_RESEL_REQ;
        proc->pMsgBody  = NULL;
        proc->bStaticAO = FALSE;

        hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

        CFW_SetUTI(hAo, nUTI, 0);

        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom
    }
    else  // manual mode or automatic/manual mode
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("CFW_NwSetRegistration manual mode PLMN = %x%x%x%x%x%x \n",0x08100931)),
                  nOperatorId[0], nOperatorId[1], nOperatorId[2], nOperatorId[3], nOperatorId[4], nOperatorId[5]);
        // should use AOM
        proc->nMsgId    = API_NW_SEL_REQ;
        proc->pMsgBody  = NULL;
        proc->bStaticAO = FALSE;

        hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

        CFW_SetUTI(hAo, nUTI, 0);

        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwSetRegistration end\n",0x08100932)));

    // store the mode in flash
    // Todo use Mode;
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwSetRegistration);
    return ERR_SUCCESS;
}

//
// used by AT, cops=2,
// send API_STOP_REQ to stack with parameter api_StopReq: False
// response API_STOP_CNF
UINT32 CFW_NwDeRegister(UINT16 nUTI,
#ifdef LTE_NBIOT_SUPPORT
						UINT8 detachType,
#else
                        BOOL bPowerOff, 
#endif
						CFW_SIM_ID nSimID
                       )
{
    UINT32 ret = 0;
    NW_SM_INFO *s_proc = NULL;  // static sm
    HAO s_hAo = 0;            // staitc ao

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwDeRegister start\n",0x08100933)));
    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("DeRegister INVALID PARMETER!\n",0x08100934)));
        return ERR_CFW_INVALID_PARAMETER;
    }
    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);
    if((s_hAo == NULL) || (s_proc == NULL))
    {
        return ERR_INVALID_HANDLE;
    }
    s_proc->nStpCause = STOP_CAUSE_FM;

#ifdef LTE_NBIOT_SUPPORT

    ret = CFW_NwStop(detachType, nUTI, nSimID);

#else
    ret = CFW_NwStop(bPowerOff, nUTI, nSimID);

#endif
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwDeRegister end\n",0x08100935)));
    return ret;
}

//Delay 5 second for nw deattch
#define POWER_OFF_DELAY_TIME 5
UINT32 CFW_NwDeRegisterEX()
{
    UINT32 ret = 0;
    NW_SM_INFO *s_proc = NULL;  // static sm
    HAO s_hAo = 0;            // staitc ao

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), 
            TSTXT(TSTR("CFW_NwDeRegister start\n",0x08100933)));
    UINT8 sim_index = 0;
#ifndef LTE_NBIOT_SUPPORT
    for(sim_index = 0; sim_index < CFW_SIM_COUNT; sim_index++)
#endif
    {
        CFW_COMM_MODE nFM = CFW_ENABLE_COMM;
        if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, sim_index))
        {
            return ERR_CFG_FAILURE;
        }
        if (CFW_ENABLE_COMM != nFM)
        {
            continue; 
        }

        s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, sim_index);
        s_proc = CFW_GetAoUserData(s_hAo);
        if((s_hAo == NULL) || (s_proc == NULL))
        {
            return ERR_INVALID_HANDLE;
        }
        s_proc->nStpCause = STOP_CAUSE_POWEROFF;

        UINT8 nUTI = 0x00;
        CFW_GetFreeUTI(0,&nUTI);

#ifdef LTE_NBIOT_SUPPORT
        ret = CFW_NwStop(detachType, nUTI, sim_index);
#else
        ret = CFW_NwStop(TRUE, nUTI,sim_index);

#endif
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwDeRegister end\n",0x08100935)));

        UINT8 i = 0x00;
        for(i;i < POWER_OFF_DELAY_TIME*10;i++)
        {
            if(TRUE == s_proc->bPowerOff)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "PowerOff simcard[%d] deattch success\n", sim_index);
                break;
            }
            sxr_Sleep(16384 / 10);
        }
        if(i == POWER_OFF_DELAY_TIME*10)
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "PowerOff simcard[%d] did not get deattch message\n", sim_index);
        //return ERR_WAIT_EVENT;
    }
    return ERR_SUCCESS; 
}

// used by system

//
// start the stack
//
#ifdef LTE_NBIOT_SUPPORT
UINT8 gCFWNWRat[CFW_SIM_COUNT] = {CFW_RAT_NBIOT_ONLY, CFW_RAT_NBIOT_ONLY};
#else
UINT8 gCFWNWRat[CFW_SIM_COUNT] = {CFW_RAT_GSM_ONLY, CFW_RAT_GSM_ONLY};
#endif

UINT32 CFW_NWSetRat(UINT8 nRat, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "CFW_NWSetRat Rat[%d]=%d\n", nSimID, nRat);
    gCFWNWRat[nSimID] = nRat;
    return ERR_SUCCESS;
}

UINT8 CFW_NWGetRat(CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "CFW_NWGetRat Rat[%d]=%d\n", nSimID, gCFWNWRat[nSimID]);
    return gCFWNWRat[nSimID];
}
#ifdef __MULTI_RAT__

UINT8 gCFWSTACKRat[CFW_SIM_COUNT] = {CFW_RAT_UMTS_ONLY, CFW_RAT_GSM_ONLY};
UINT32 CFW_NWSetStackRat(UINT8 nRat, CFW_SIM_ID nSimID)
{
    gCFWSTACKRat[nSimID] = nRat;
    return ERR_SUCCESS;
}
UINT8 CFW_NWGetStackRat(CFW_SIM_ID nSimID)
{
    return gCFWSTACKRat[nSimID];
}
#endif


PRIVATE UINT32 CFW_NwStartStack(
    UINT8 nBand
#ifdef __MULTI_RAT__
    , UINT8 nUBand
#endif
#ifdef CSW_USER_DBS
    , UINT8 nStrtFlag
#endif
    , CFW_SIM_ID nSimID
)
{
    // state change to search
    HAO hAo = 0;  // Handle

    NW_SM_INFO *proc = NULL;  // sm info

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwStartStack);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwStartStack start \n",0x08100936)));

    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !malloc exception\n",0x08100937)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwStartStack);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));
    proc->nBand = nBand;

    // save the nBand in sm information;;

    proc->nMsgId    = API_START_REQ;
    proc->pMsgBody  = NULL;
    proc->bStaticAO = FALSE;
#ifdef CSW_USER_DBS
    proc->nStrtFlag  = nStrtFlag;
#endif
#ifdef __MULTI_RAT__
    proc->nUBand = nUBand;
#endif
    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom

    // CFW_SendSclMessage(API_START_REQ, nTempBand);
    // need to send network mode to stack
    // api_selmode_req ---> api
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwStartStack end \n",0x08100938)));

    // CFW_SetAoNextState(hAo, CFW_SM_NW_STATE_SEARCH);   //set the state to search
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwStartStack);
    return ERR_SUCCESS;
}

//
// stop the stack , power Off
//
UINT32 CFW_NwStopStack(
    CFW_SIM_ID nSimID
)
{

    UINT32 ret = 0;
#ifdef CSW_USER_DBS
    HAO s_hAo = 0;              // staitc ao
    NW_SM_INFO *s_proc = NULL;  // static sm
#endif
    CSW_PROFILE_FUNCTION_ENTER(CFW_NwStopStack);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwStopStack start \n",0x08100939)) );

#ifdef CSW_USER_DBS
    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);
    if((s_hAo == NULL) || (s_proc == NULL))
    {
        return ERR_INVALID_HANDLE;
    }
    s_proc->nStpCause = STOP_CAUSE_DBS;
#endif

#if 0
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwStopStack SIM ERROR!\n",0x0810093a)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwStopStack);
        return ret;
    }
#endif
    ret = CFW_NwStop(FALSE, 0, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwStopStack end \n",0x0810093b)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwStopStack);
    return ret;
}

// used by stop and detach
PRIVATE UINT32 CFW_NwStop(
#ifdef LTE_NBIOT_SUPPORT
                          UINT8 detachType,
#else
                          BOOL bPowerOff, 
#endif
						  UINT16 nUTI
                          , CFW_SIM_ID nSimID
                         )
{
    // state  change to stop
    HAO hAo = 0;  // Handle

    api_StopReq_t *pOutMsg = NULL;

    NW_SM_INFO *proc = NULL;

    pOutMsg           = (api_StopReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_StopReq_t));
#ifdef LTE_NBIOT_SUPPORT
    pOutMsg->detachType = detachType;  // power off or Not
#else
    pOutMsg->PowerOff = bPowerOff;  // power off or Not
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TNB_ARG(2), TSTXT("CFW_NwStop nUTI = 0x%x,PowerOff=%d\n"), nUTI,bPowerOff);
#endif

    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR malloc exception\n",0x0810093d)));
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));

    proc->nMsgId    = API_STOP_REQ;
    proc->pMsgBody  = pOutMsg;
    proc->bStaticAO = FALSE;

    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom
    // use AOM
    // CFW_SendSclMessage( API_STOP_REQ, pOutMsg);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwStop end \n",0x0810093e)));

    // chage the state to stop
    return ERR_SUCCESS;
}

PRIVATE UINT8 getsum(UINT8 input)
{
    UINT8 temp = input % 10;

    if (input / 10)
    {
        temp += input / 10;
    }
    return temp;
}

PRIVATE UINT8 getCD(UINT8 *pData)
{
    UINT8 i   = 0;
    UINT8 j   = 0;
    UINT8 sum = 0;
    UINT8 IMEI[14];

    SUL_MemCopy8(IMEI, pData, 14);
    for (i = 0; i < 14 / 2; i++)
    {
        IMEI[13 - 2 * i] = IMEI[13 - 2 * i] * 2;
    }

    for (j = 0; j < 14; j++)
    {
        sum += getsum(IMEI[j]);
    }

    // SXS_TRACE(CFW_NW_TS_ID_SXS, "sum = %d, CD = 0xd\n", sum, (10 - sum%10));

    return (10 - sum % 10);

}

//
PRIVATE UINT32 CFW_NwImeiConf(api_ImeiInd_t *pImei, NW_SM_INFO *pSmInfo, UINT32 nUTI
                              , CFW_SIM_ID nSimID
                             )
{
    UINT8 *pOutMsg = (UINT8 *)CSW_NW_MALLOC(15);

    SUL_MemCopy8(pOutMsg, pImei->Imei, API_IMEI_SIZE);
    pOutMsg[14] = getCD(pOutMsg);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwImeiConf \n",0x0810093f)));

    // CFW_Dump(CFW_NW_TS_ID, pOutMsg, 15);

    CFW_PostNotifyEvent(EV_CFW_NW_GET_IMEI_RSP, (UINT32)pOutMsg, 15, nUTI | (nSimID << 24), 0);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwImeiConf end\n",0x08100940)));
    return ERR_SUCCESS;
}

//
// transform the rssi and rxqual from stack to AT+CGSN standard
//
// <rssi>:
// 0 -113 dBm or less
// 1 -111 dBm
// 2...30  -109... -53 dBm
// 31  -51 dBm or greater
// 99  not known or not detectable
// <ber> (in percent):
// 0...7 as RXQUAL values in the table in GSM 05.08 [20] subclause 8.2.4
// 99  not known or not detectable

//
PRIVATE UINT32 CFW_NwQualReportConf(api_QualReportCnf_t *pQualReport, NW_SM_INFO *pSmInfo
                                    , CFW_SIM_ID nSimID
                                   )
{
    NW_SM_INFO *proc = pSmInfo;
    UINT8 nRssi = pQualReport->Rssi;
    CFW_IND_EVENT_INFO sIndEventInfo;
    UINT16 nInd = 0;

    UINT8 nRssiEx = 0;
    UINT8 nBerEx  = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwQualReportConf);

    // BOOL bChanged = FALSE;
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwQualReportConf start \n",0x08100941)));

    nRssiEx = proc->Rssi;
    nBerEx  = proc->Ber;
    CFW_CfgGetIndicatorEventInfo(&sIndEventInfo, nSimID);
    CFW_CfgGetIndicatorEvent(&nInd, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwQualReportConf, nSimID=%d\n",0x08100942)), nSimID);
    // nInd = CFW_CFG_IND_SQ_EVENT|CFW_CFG_IND_BIT_ERR_EVENT;  //just for test

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("From stack : Rssi = %d RxQual = %d nInd = 0x%x\n",0x08100943)), pQualReport->Rssi,
              pQualReport->RxQual, nInd);

    if (nRssi > 113)
    {
        proc->Rssi = 0;
    }
    else if ((nRssi <= 113) && (nRssi >= 51))
    {
        proc->Rssi = (UINT8)(31 - ((nRssi - 51) / 2));
    }
    else if (nRssi < 51)
    {
        proc->Rssi = 31;
    }
    else
    {
        proc->Rssi = 99;
    }

    // need some transfer
    // proc->Rssi = pQualReport->Rssi;
    // if cc not working ber = 99, else = real value

    if (sIndEventInfo.call == 0)
    {
        proc->Ber = 99;
    }
    else
    {
        proc->Ber = pQualReport->RxQual;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("After transform Rssi = %d RxQual = %d\n",0x08100944)), proc->Rssi, proc->Ber);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TNB_ARG(1), TSTXT("After transform nCsStatusChange = %d\n"), proc->sStatusInfo.nCsStatusChange);
    if ((nInd & CFW_CFG_IND_SQ_EVENT) || (nInd & CFW_CFG_IND_BIT_ERR_EVENT))
    {
        if ((nInd & CFW_CFG_IND_SQ_EVENT) && (nInd & CFW_CFG_IND_BIT_ERR_EVENT))
        {
            if ((proc->Rssi != nRssiEx) || (proc->Ber != nBerEx)|| (proc->sStatusInfo.nCsStatusChange == TRUE))
            {
                UINT8 uSignal;

                if (!proc->Rssi)
                    uSignal = 0;
                else if (proc->Rssi == 1)
                    uSignal = 1;
                else if (proc->Rssi == 31)
                    uSignal = 5;
                else
                    uSignal        = (UINT8)(2 + (proc->Rssi - 1) / 10);
                sIndEventInfo.sq = uSignal;

                sIndEventInfo.bit_err = proc->Ber;
                nRssiEx               = proc->Rssi;
                nBerEx                = proc->Ber;

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("the signal quality change 11 and bit error change\n",0x08100945)));

                CFW_CfgSetIndicatorEventInfo(&sIndEventInfo, nSimID); // for ind
                CFW_PostNotifyEvent(EV_CFW_NW_SIGNAL_QUALITY_IND, (UINT32)nRssiEx, (UINT32)nBerEx,
                                    CFW_NW_IND_UTI | (nSimID << 24), 0);

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("rssi = %d ber = %d\n",0x08100946)), nRssiEx, nBerEx);

            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "Warning! nRssiEx=%d,nBerEx = %d,nCsStatusChange=%d\n", nRssiEx, nBerEx, proc->sStatusInfo.nCsStatusChange);
            }
        }
        else if (nInd & CFW_CFG_IND_SQ_EVENT)
        {
            if ((proc->Rssi != nRssiEx)||(proc->sStatusInfo.nCsStatusChange == TRUE))
            {
                UINT8 uSignal;

                if (!proc->Rssi)
                    uSignal = 0;
                else if (proc->Rssi == 1)
                    uSignal = 1;
                else if (proc->Rssi == 31)
                    uSignal = 5;
                else
                    uSignal        = (UINT8)(2 + (proc->Rssi - 1) / 10);
                sIndEventInfo.sq = uSignal;

                nRssiEx = proc->Rssi;
                nBerEx  = 0;

                // CFW_CfgSetIndicatorEvent(CFW_CFG_IND_SQ_EVENT);
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("the signal quality change22\n",0x08100947)));

                CFW_CfgSetIndicatorEventInfo(&sIndEventInfo, nSimID); // for ind
                CFW_PostNotifyEvent(EV_CFW_NW_SIGNAL_QUALITY_IND, (UINT32)nRssiEx, (UINT32)nBerEx,
                                    CFW_NW_IND_UTI | (nSimID << 24), 0);


                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("rssi = %d ber = %d\n",0x08100948)), nRssiEx, nBerEx);

            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "Warning! CFW_CFG_IND_SQ_EVENT,nRssiEx=%d,nCsStatusChange=%d\n", nRssiEx, proc->sStatusInfo.nCsStatusChange);
            }
        }
        else if (nInd & CFW_CFG_IND_BIT_ERR_EVENT)
        {
            if ((proc->Ber != nBerEx)||(proc->sStatusInfo.nCsStatusChange == TRUE))
            {
                sIndEventInfo.bit_err = proc->Ber;

                nRssiEx = 0;
                nBerEx  = proc->Ber;

                // CFW_CfgSetIndicatorEvent(CFW_CFG_IND_BIT_ERR_EVENT);
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("the signal bit error change in 33 call progress \n",0x08100949)));

                CFW_CfgSetIndicatorEventInfo(&sIndEventInfo, nSimID); // for ind
                CFW_PostNotifyEvent(EV_CFW_NW_SIGNAL_QUALITY_IND, (UINT32)nRssiEx, (UINT32)nBerEx,
                                    CFW_NW_IND_UTI | (nSimID << 24), 0);

                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("rssi = %d ber = %d\n",0x0810094a)), nRssiEx, nBerEx);
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "Warning! CFW_CFG_IND_BIT_ERR_EVENT, nBerEx=%d,nCsStatusChange=%d\n", nBerEx, proc->sStatusInfo.nCsStatusChange);
            }
        }
        else  // should not come here
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "ERROR!! CFW_NwQualReportConf should never come here");
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwQualReportConf, return success\n",0x0810094b)));

    // EV_CFW_NW_SIGNAL_LEVEL_IND
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwQualReportConf);
    return ERR_SUCCESS;
}

//
PRIVATE UINT32 CFW_NwListConf(api_NwListInd_t *pListInd, NW_SM_INFO *pSmInfo, UINT32 nUTI
                              , CFW_SIM_ID nSimID
                             )
{
    UINT8 nListNum = 0;

    CFW_NW_OPERATOR_INFO *pOperatorList = NULL;
    CFW_NW_OPERATOR_INFO *pOperatorListTemp = NULL;
    UINT8 nIndex = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwListConf);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwListConf start \n",0x0810094c)));

    nListNum          = pListInd->PlmnNb;
    pOperatorList     = (CFW_NW_OPERATOR_INFO *)CSW_NW_MALLOC((UINT16)(SIZEOF(CFW_NW_OPERATOR_INFO) * nListNum));
    pOperatorListTemp = pOperatorList;

    for (nIndex = 0; nIndex < nListNum; nIndex++)
    {
        PlmnApi2Array(&pListInd->PlmnL[nIndex * 3], pOperatorList->nOperatorId);
        if (pListInd->Restr[nIndex] == API_PLMN_FBD)  // forbidden
        {
            pOperatorList->nStatus = CFW_NW_OPERATOR_STATUS_FORBIDDEN;
        }
        else if (pListInd->Restr[nIndex] == API_PLMN_OK)
        {
            if (SUL_MemCompare(pOperatorList->nOperatorId, pSmInfo->nOperatorId, 6) == 0) // the current selected
            {
                pOperatorList->nStatus = CFW_NW_OPERATOR_STATUS_CURRENT;
            }
            else  // available
            {
                pOperatorList->nStatus = CFW_NW_OPERATOR_STATUS_AVAILABLE;
            }
        }
        else  // unknow
        {
            pOperatorList->nStatus = CFW_NW_OPERATOR_STATUS_UNKNOW;
        }

        // CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwListConf \n",0x0810094d)) );
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("CFW_NwListConf index = 0x%x, satus = 0x%x ,PLMN = %x%x%x%x\n",0x0810094e)),
                  nIndex,
                  pOperatorList->nStatus,
                  pOperatorList->nOperatorId[0],
                  pOperatorList->nOperatorId[1],
                  pOperatorList->nOperatorId[2],
                  pOperatorList->nOperatorId[3]);
        pOperatorList++;
    }

    CFW_PostNotifyEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, (UINT32)pOperatorListTemp, nListNum, nUTI | (nSimID << 24), 0);

    if (pSmInfo->bTimeDeleted)  // if register fail and list the plmn, delete the timer, then restart the timer
    {
        BOOL ret;

        UINT32 nTimerId = 0;

        if (CFW_SIM_0 == nSimID)
            nTimerId = PRV_CFW_NW_SIM0_TIMER_ID;
        else if (CFW_SIM_1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM1_TIMER_ID;
        else if (CFW_SIM_1 + 1 == nSimID)
            nTimerId = PRV_CFW_NW_SIM2_TIMER_ID;
        else if (CFW_SIM_1 + 2 == nSimID)
            nTimerId = PRV_CFW_NW_SIM3_TIMER_ID;
        ret = COS_SetTimerEX(0xFFFFFFFF, nTimerId, COS_TIMER_MODE_SINGLE, 5 * 1000 MILLI_SECOND);
        pSmInfo->bTimerneed = TRUE;

        if (ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("list conf , nw timer start success\n",0x0810094f)));
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("nw timer start fail\n",0x08100950)));
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwListConf end \n",0x08100951)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwListConf);
    return ERR_SUCCESS;
}

PRIVATE UINT32 CFW_NwPsStatusConf(api_NwPsRegStatusInd_t *pPsRegStatus, NW_SM_INFO *pSmInfo, UINT32 nUTI
                                  , CFW_SIM_ID nSimID
                                 )
{
    UINT32 nAreaCellId = 0;
    UINT8 nStatusURC   = 0xff;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwPsStatusConf);

    CFW_CfgGetNwStatus(&nStatusURC, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("PS status = 0x%x, RAI = %x%x%x, CI = 0x%x%x\n",0x08100952)),
              pPsRegStatus->Status,
              pPsRegStatus->RAI[0],
              pPsRegStatus->RAI[1],
              pPsRegStatus->RAI[2],
              pPsRegStatus->CellId[0], pPsRegStatus->CellId[1]);

    //Add by Lixp for auto attch PS by stack default at 20150520

    if(pPsRegStatus->Status == API_NW_FULL_SVCE)
    {
        pSmInfo->nGprsAttState = CFW_GPRS_ATTACHED;
#ifdef LTE_NBIOT_SUPPORT //seems no influence
		pSmInfo->bNetAvailable = TRUE;
#endif
    }
#if 0
    else if (0xFF == pSmInfo->nAttach)
    {
        pSmInfo->nGprsAttState = CFW_GPRS_DETACHED;
#ifdef LTE_NBIOT_SUPPORT //seems no influence
	 pSmInfo->bNetAvailable = FALSE;
        //CFW_StopDetachTimer(nSimID);
#endif
    }
#endif
    // add by wuys 2010-03-16 for handling detach originated by network
#ifdef __MULTI_RAT__
    UINT8 CurrentRat = pPsRegStatus->NwType;
#endif
    //pSmInfo->sGprsStatusInfo.PsType = pPsRegStatus->NwType;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_NwPsStatusConf , cause: 0x%x ,NwType: 0x%x\n",0x08100953)), pPsRegStatus->Cause,pPsRegStatus->NwType);

    if ((pPsRegStatus->Status == API_NW_NO_SVCE) && (pPsRegStatus->Cause == API_P_NWK_DET))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwPsStatusConf  no service, cause: %x \n",0x08100954)), pPsRegStatus->Cause);

        pSmInfo->nGprsAttState = CFW_GPRS_DETACHED;
    }
    if( API_NW_FULL_SVCE == pPsRegStatus->Status && pPsRegStatus->Roaming != 0
            &&  API_NW_FULL_SVCE == pSmInfo->sStatusInfo.nStatus )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwPsStatusConf Change Roaming is 0\n",0x08100955)));
        pPsRegStatus->Roaming = 0;
    }
    // add end
    GprsStatusApi2At(pPsRegStatus->Status, pPsRegStatus->Roaming, &pSmInfo->sGprsStatusInfo.nStatus
	#ifdef LTE_NBIOT_SUPPORT
	,nSimID
	#endif
	);
    pSmInfo->sGprsStatusInfo.PsType = pPsRegStatus->NwType;//add for dif gprs or edge by wuys 2013-07-19
#ifdef LTE_NBIOT_SUPPORT
    if(pSmInfo->sGprsStatusInfo.PsType == 4)
    {
        CFW_NWSetRat(CFW_RAT_NBIOT_ONLY,nSimID);
        
    }
    else if((pSmInfo->sGprsStatusInfo.PsType == 1) || (pSmInfo->sGprsStatusInfo.PsType == 2))
    {
        CFW_NWSetRat(CFW_RAT_GSM_ONLY,nSimID);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("simID: [%d] unkown psType for nbiot&gsm dual-stack\n",0x08100955)), nSimID);
    }
#endif
    SUL_MemCopy8(pSmInfo->sGprsStatusInfo.nAreaCode, &pPsRegStatus->RAI[3], 2);
#ifdef LTE_NBIOT_SUPPORT
	SUL_MemCopy8(pSmInfo->sGprsStatusInfo.nCellId, pPsRegStatus->CellId, 4);
       if(CFW_NWGetRat(nSimID) == CFW_RAT_NBIOT_ONLY)
       {
           pSmInfo->sGprsStatusInfo.cause_type = pPsRegStatus->cause_type;
	    pSmInfo->sGprsStatusInfo.reject_cause = pPsRegStatus->Cause;
	    pSmInfo->sGprsStatusInfo.activeTime = pPsRegStatus->activeTime;
	    pSmInfo->sGprsStatusInfo.periodicTau = pPsRegStatus->periodicTau;
	    pSmInfo->sGprsStatusInfo.nwSupportedCiot = pPsRegStatus->nwSupportedCiot;
	    pSmInfo->sGprsStatusInfo.edrxEnable = pPsRegStatus->edrxEnable;
	    pSmInfo->sGprsStatusInfo.edrxPtw = pPsRegStatus->edrxPtw;
	    pSmInfo->sGprsStatusInfo.edrxValue = pPsRegStatus->edrxValue;
	    if(pSmInfo->sGprsStatusInfo.edrxEnable == 1)
	    {
	        CFW_PostNotifyEvent(EV_CFW_GPRS_EDRX_IND, (UINT32)pSmInfo->sGprsStatusInfo.edrxValue, pSmInfo->sGprsStatusInfo.edrxPtw,
                                nUTI | (nSimID << 24), 0);
	    }
	    CFW_PostNotifyEvent(EV_CFW_GPRS_CCIOTOPT_IND, (UINT32)pPsRegStatus->nwSupportedCiot, 0,
                                nUTI | (nSimID << 24), 0);

          if (pSmInfo->bReselNW) // attach to another plmn
         {
             pSmInfo->bReselNW = FALSE;

             if(FALSE == CFW_GetNWTimerOutFlag(nSimID))
             {
                 CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Maybe high layer has timer out and no long wait the message\n",0x0810096d)));
                 return ERR_SUCCESS;
             }
             else
             {
                 CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pCsRegStatus->Status %d\n",0x0810096e)), pPsRegStatus->Status);
                 CFW_SetNWTimerOutFlag(FALSE, nSimID);
             }

             if (pPsRegStatus->Status == API_NW_FULL_SVCE)
             {
                 UINT8 *pPLMN = (UINT8 *)CSW_NW_MALLOC(6);

                 SUL_MemCopy8(pPLMN, pSmInfo->nOperatorId, 6);
                 CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send  EV_CFW_NW_SET_REGISTRATION_RSP to ATL\n",0x0810096f)));

                 CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP,
                                (UINT32)pPLMN,
                                (UINT32)(pSmInfo->nNetMode ==
                                         API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE),
                                pSmInfo->nCOPSUTI | (nSimID << 24), 0);
            }
            else if (pPsRegStatus->Status == API_NW_LTD_SVCE)
            {

                 if (pSmInfo->nNetMode == CFW_NW_MANUAL_AUTOMATIC_MODE)
                 {
                     BOOL ret;

                    UINT32 nTimerId = 0;

                    if (CFW_SIM_0 == nSimID)
                        nTimerId = PRV_CFW_NW_SIM0_TIMER_ID;
                    else if (CFW_SIM_1 == nSimID)
                        nTimerId = PRV_CFW_NW_SIM1_TIMER_ID;
                    else if ((CFW_SIM_1 + 1) == nSimID)
                        nTimerId = PRV_CFW_NW_SIM2_TIMER_ID;
                    else if ((CFW_SIM_1 + 2) == nSimID)
                        nTimerId = PRV_CFW_NW_SIM3_TIMER_ID;
                    ret = COS_SetTimerEX(0xFFFFFFFF, nTimerId, COS_TIMER_MODE_SINGLE, 5 * 1000 MILLI_SECOND);
                    pSmInfo->bTimerneed = TRUE;

                    if (ret)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("register fail,  CFW_NW_MANUAL_AUTOMATIC_MODE mode, nw timer start success\n",0x08100970)));
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("nw timer start fail\n",0x08100971)));
                    }

                }
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("try to attach to another plmn fail limited service\n",0x08100972)));

                CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP, ERR_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY, 0,
                                    pSmInfo->nCOPSUTI | (nSimID << 24), 0xF0);

            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("try to attach to another plmn fail no service\n",0x08100973)));

                CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP, ERR_CME_NO_NETWORK_SERVICE, 0,
                                    pSmInfo->nCOPSUTI | (nSimID << 24), 0xF0);
            }
        }
	    
       }
#else
    SUL_MemCopy8(pSmInfo->sGprsStatusInfo.nCellId, pPsRegStatus->CellId, 2);
#endif
    nAreaCellId =
        (((pSmInfo->sGprsStatusInfo.nAreaCode[0] << 8) | pSmInfo->sGprsStatusInfo.nAreaCode[1]) << 16) | ((pSmInfo->
                sGprsStatusInfo.
                nCellId[0] << 8)
                | pSmInfo->
                sGprsStatusInfo.
                nCellId[1]);

    // ....need to do more...
    // if doing the attach post the rsp to uplayer
    if (pSmInfo->nAttach == 1)  // doing the attach
    {
        pSmInfo->nAttach = 0xFF;
        if (pPsRegStatus->Status == API_NW_FULL_SVCE) // attach success
        {
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ATT_RSP, 0, 0, nUTI | (nSimID << 24), 1,pSmInfo->func);
        }
        else  // attach failure
        {
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ATT_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xF1,pSmInfo->func);
        }
    }
    else if (pSmInfo->nAttach == 0) // doing the detach
    {
        pSmInfo->nAttach = 0xFF;
        if (pPsRegStatus->Status == API_NW_NO_SVCE) // dettach success
        {
            // need deactive the actived pdp context
            // need to do un register gprs AO s
#ifdef CFW_GPRS_SUPPORT
//            CFW_GprsDeactiveAll(nSimID);
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ATT_RSP, 0, 0, nUTI | (nSimID << 24), 0,pSmInfo->func);
#endif
        }
        else  // dettach failure
        {
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_ATT_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xF0,pSmInfo->func);
        }
    }
    else  // nothing action
    {
        // post the status ind to uplayer according to the URC, need to do
        if (nStatusURC == 1 || nStatusURC == 2)
        {
            CFW_PostGPRSNotifyEvent(EV_CFW_GPRS_STATUS_IND, (UINT32)pSmInfo->sGprsStatusInfo.nStatus, nAreaCellId,
                                nUTI | (nSimID << 24), 2,pSmInfo->func);
        }

    }
    pSmInfo->func = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwPsStatusConf end \n",0x08100956)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwPsStatusConf);
    return ERR_SUCCESS;
}

PRIVATE UINT8 gNWTimerOutFlag[CFW_SIM_COUNT] = {FALSE, FALSE,};

BOOL CFW_GetNWTimerOutFlag(CFW_SIM_ID nSimID)
{
    return gNWTimerOutFlag[nSimID];
}

VOID CFW_SetNWTimerOutFlag(BOOL bClear, CFW_SIM_ID nSimID)
{
    gNWTimerOutFlag[nSimID] = bClear;
}

// store the status info and send the info to at according to the URC
PRIVATE UINT32 CFW_NwCsStatusConf(api_NwCsRegStatusInd_t *pCsRegStatus, NW_SM_INFO *pSmInfo
                                  , CFW_SIM_ID nSimID
                                 )
{
    UINT32 nRet = ERR_SUCCESS;
    CFW_IND_EVENT_INFO sIndEventInfo;
    UINT8 nStatusURC = 0xff;

    BOOL bNetAvailableEx; // net work available
    BOOL bRoamingEx;  // roaming ?

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwCsStatusConf);
    CFW_CfgGetIndicatorEventInfo(&sIndEventInfo, nSimID);

    // SUL_MemCopy8(pSmInfo->sStatusInfo.nAreaCode, &pCsRegStatus->LAI[0], 3);
    SUL_MemCopy8(pSmInfo->sStatusInfo.nCellId, pCsRegStatus->CellId, 2);
    PlmnApi2Array(pCsRegStatus->LAI, pSmInfo->nOperatorId);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("cs status = 0x%x, cs roaming = 0x%x ,pCsRegStatus->LAI = %x%x,cellid %x%x",0x08100957)),
              pCsRegStatus->Status,
              pCsRegStatus->Roaming,
              pCsRegStatus->LAI[0],
              pCsRegStatus->LAI[1],
              pSmInfo->sStatusInfo.nCellId[0], pSmInfo->sStatusInfo.nCellId[1]);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("cs status = 0x%x, cs roaming = 0x%x ,PLMN = %x%x%x%x\n",0x08100958)),
              pCsRegStatus->Status,
              pCsRegStatus->Roaming,
              pSmInfo->nOperatorId[0],
              pSmInfo->nOperatorId[1],
              pSmInfo->nOperatorId[2], pSmInfo->nOperatorId[3]);
#ifdef TEST_SPECIAL_PLMN
    pCsRegStatus->Roaming = TRUE;
    pCsRegStatus->Status = API_NW_FULL_SVCE;
    //pSmInfo->nOperatorId ={0x04,0x00,0x04,0x00,0x03,0x0f};
    pSmInfo->nOperatorId[0] = 0x04;
    pSmInfo->nOperatorId[1] = 0x00;
    pSmInfo->nOperatorId[2] = 0x04;
    pSmInfo->nOperatorId[3] = 0x00;
    pSmInfo->nOperatorId[4] = 0x03;
    pSmInfo->nOperatorId[5] = 0x0f;
    CFW_ChangeRoamByPLMNName(pCsRegStatus, pSmInfo->nOperatorId, nSimID);
#else
    CFW_ChangeRoamByPLMNName(pCsRegStatus, pSmInfo->nOperatorId, nSimID);
#endif
    NetStatusApi2At(pCsRegStatus->Status, pCsRegStatus->Roaming, &pSmInfo->sStatusInfo.nStatus);//frank move to here

    bRoamingEx      = pSmInfo->bRoaming;
    bNetAvailableEx = pSmInfo->bNetAvailable;

    UINT8 nRat = 0x00;
#ifdef __MULTI_RAT__
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwCsStatusConf  CurrRat 0x%x\n",0x08100959)) , pCsRegStatus->CurrRat);
    nRat = pCsRegStatus->CurrRat;
    CFW_NWSetStackRat(nRat, nSimID);
    pSmInfo->sStatusInfo.nCurrRat = pCsRegStatus->CurrRat;
#else
    if(pCsRegStatus->Status == API_NW_FULL_SVCE)
    {
        pSmInfo->sStatusInfo.nCurrRat = 2; // 2 : API_RAT_GSM
    }
    else
    {
        pSmInfo->sStatusInfo.nCurrRat = 0;
    }
#endif
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), TSTXT("conf sStatusInfo.nCurrRat 0x%x, nSimID=%d\n"), pSmInfo->sStatusInfo.nCurrRat, nSimID);
#ifdef CHIP_HAS_AP
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_NwCsStatusConf gSatFreshComm[nSimID] 0x%x, ACLBStep:%d\n",0x0810095a)), gSatFreshComm[nSimID], _GetACLBStep());
    if((2 == _GetACLBStep())&&(0 == nSimID))
    {
        if((pCsRegStatus->Status == API_NW_FULL_SVCE)||(pCsRegStatus->Status == API_NW_LTD_SVCE))
        {
            CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, 0, 0, CFW_NW_IND_UTI | (nSimID << 24),0x00);
        }
        else
        {
            CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, 0, 0, CFW_NW_IND_UTI | (nSimID << 24),0xF0);
        }
    }
#else
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_NwCsStatusConf gSatFreshComm[nSimID] 0x%xd\n",0x0810095b)), gSatFreshComm[nSimID]);
#endif
    if(0x02 == (gSatFreshComm[nSimID] & 0x0F))
    {
        if(0x20 != (gSatFreshComm[nSimID] & 0xF0))
        {
            if(g_cfw_sim_status[nSimID].UsimFlag)
                CFW_SimPatchEX(API_USIM_EF_LOCI, APP_UTI_SHELL, nSimID);
            else
                CFW_SimPatchEX(API_SIM_EF_LOCI, APP_UTI_SHELL, nSimID);

            gSatFreshComm[nSimID] = 3 + (0xF0 & gSatFreshComm[nSimID]);

        }
        else
        {
            gSatFreshComm[nSimID] = ( gSatFreshComm[nSimID] & 0xF0) + 0x03;
            UINT8 nRUti = 0x00;

            CFW_GetFreeUTI(0, &nRUti);
            CFW_SetComm(CFW_ENABLE_COMM, 1, nRUti, nSimID);
        }

    }
    else if(0x03 == (gSatFreshComm[nSimID] & 0x0F))
    {
        gSatFreshComm[nSimID] = 0x00;
        CFW_NW_STATUS_INFO nStatusInfo;
        UINT32 nStatus = 0x00;
        if (ERR_SUCCESS != (nRet = CFW_NwGetStatus(&nStatusInfo, nSimID)))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_NwGetStatus Error\n",0x0810095c));

            return nRet;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTR("NWSATCellInfo nStatusInfo.nStatus  %d\n",0x0810095d), nStatusInfo.nStatus);

        if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING || nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
        {
            nStatus = 0x06;
        }

        if(0x00 == nStatus)
            SendRefreshSclMsg(0, nSimID);
        else
            SendRefreshSclMsg(0/*6*/, nSimID);
    }
    CFW_CfgGetNwStatus(&nStatusURC, nSimID);
    if (pCsRegStatus->Roaming)
    {
        sIndEventInfo.roam = 1;
        pSmInfo->bRoaming  = TRUE;
    }
    else
    {
        sIndEventInfo.roam = 0;
        pSmInfo->bRoaming  = FALSE;
    }

    if (pCsRegStatus->Status == API_NW_FULL_SVCE)
    {
        pSmInfo->bNetAvailable = TRUE;
    }
    else
    {
        pSmInfo->bNetAvailable = FALSE;
    }

    // nAreaCellId = (((pSmInfo->sStatusInfo.nAreaCode[0] << 8) | pSmInfo->sStatusInfo.nAreaCode[1]) << 16) | ((pSmInfo->sStatusInfo.nCellId[0] << 8) | pSmInfo->sStatusInfo.nCellId[1]);
    CFW_CfgSetIndicatorEventInfo(&sIndEventInfo, nSimID);
    // nStatusURC = 1;
    if (nStatusURC == 1 || nStatusURC == 2)
    {
        UINT16 nIndEvent = 0;

        CFW_CfgGetIndicatorEvent(&nIndEvent, nSimID);
        // has a problem : if roam and avail both; after post the ind event will be confused
        if (nIndEvent & CFW_CFG_IND_ROAM_EVENT)
        {
            if (pSmInfo->bRoaming != bRoamingEx)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("roam change 11\n",0x0810095e)));
                CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, pSmInfo->bRoaming, nRat, CFW_NW_IND_UTI | (nSimID << 24), 1);
            }
        }
        if (nIndEvent & CFW_CFG_IND_NW_EVENT)
        {
            if ((pSmInfo->bNetAvailable != bNetAvailableEx) || (pCsRegStatus->Status == API_NW_NO_SVCE))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("net avail change 22\n",0x0810095f)));

                CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, pSmInfo->bNetAvailable, nRat, CFW_NW_IND_UTI | (nSimID << 24), 0);
            }
        }

        // CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID|C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("before send  EV_CFW_NW_REG_STATUS_IND, pSmInfo->nAreaCellId=0x%x, nAreaCellId=0x%x\n",0x08100960)), pSmInfo->nAreaCellId, nAreaCellId);
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2),
                  TSTXT(TSTR("before send  EV_CFW_NW_REG_STATUS_IND, pSmInfo->bNetAvailable=0x%x, bNetAvailableEx=0x%x\n",0x08100961)),
                  pSmInfo->bNetAvailable, bNetAvailableEx);
        if(pSmInfo->bNetAvailable == bNetAvailableEx)
        {
            pSmInfo->sStatusInfo.nCsStatusChange = FALSE;
        }
        else
        {
            pSmInfo->sStatusInfo.nCsStatusChange = TRUE;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2),
                  TSTXT(TSTR("before send  EV_CFW_NW_REG_STATUS_IND, pCsRegStatus->Status=0x%x, API_NW_NO_SVCE=0x%x\n",0x08100962)),
                  pCsRegStatus->Status, API_NW_NO_SVCE);

        // frank modify this line for MMI,because MMI don't process CS status when other card is calling. If our phone is calling,we will lose CS Event.
        CFW_SIM_ID nSimId;
        UINT8 nAOCount = 0;
        INT16 nCmp     = 0;

        for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
        {
            if (nSimId != nSimID)
            {
                nAOCount += CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
            }
        }

        nCmp = SUL_MemCompare(&pCsRegStatus->LAI[0], pSmInfo->sStatusInfo.nAreaCode, 5);

        // if nAOCount > 0 ,have other sim have calling.if nCmp ==0,have sent event to mmi before.
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("before send  EV_CFW_NW_REG_STATUS_IND, nAOCount=0x%x, nCmp=0x%x\n",0x08100963)), nAOCount, nCmp);
        if (((nAOCount == 0) && (nCmp != 0))  // pSmInfo->nAreaCellId != nAreaCellId
                || (pSmInfo->bNetAvailable != bNetAvailableEx)
                || (pCsRegStatus->Status == API_NW_NO_SVCE) || (pCsRegStatus->Status == API_NW_LTD_SVCE))
        {
            SUL_MemCopy8(pSmInfo->sStatusInfo.nAreaCode, &pCsRegStatus->LAI[0], 5);
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send  EV_CFW_NW_REG_STATUS_IND to ATL 33\n",0x08100964)));

            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("send  EV_CFW_NW_REG_STATUS_IND nSimID=%d, status to MMI is: %d\n",0x08100965)), nSimID,
                      pSmInfo->sStatusInfo.nStatus);
            if ((SOS_NO_SEND_SIM_OPEN == (UINT8)CFW_GetSimOpenStatus(nSimID)) && (pCsRegStatus->Status == API_NW_LTD_SVCE))
            {
                CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, (UINT32)CFW_NW_STATUS_NOTREGISTERED_SEARCHING, nRat,
                                    CFW_NW_IND_UTI | (nSimID << 24), 2);
            }
            else
            {
                CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, (UINT32)pSmInfo->sStatusInfo.nStatus, nRat,
                                    CFW_NW_IND_UTI | (nSimID << 24), 2);
            }
            CFW_PostNotifyEvent(EV_CFW_NW_REG_STATUS_IND, (UINT32)pSmInfo->sStatusInfo.nStatus, nRat,
                                GENERATE_SHELL_UTI() | (nSimID << 24), 2);
        }
    }
    if ((pSmInfo->bDetached) && (pSmInfo->nCsStatusIndCount == 0))  // just for omit the first cs status ind message
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("get cs status first \n",0x08100966)));
        pSmInfo->nCsStatusIndCount = 1;
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwCsStatusConf);
        return ERR_SUCCESS;
    }

    // if rel or sel... need do something
    if (pSmInfo->bChangeBand)
    {

        pSmInfo->bChangeBand = FALSE;
        pSmInfo->bDetached   = FALSE;
        pSmInfo->bReselNW    = FALSE;
        if (pCsRegStatus->Status == API_NW_FULL_SVCE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("change the band success\n",0x08100967)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_BAND_RSP, pSmInfo->nBand, 0, pSmInfo->nBandUTI | (nSimID << 24), 0);
        }
        else if (pCsRegStatus->Status == API_NW_LTD_SVCE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("change the band limited service\n",0x08100968)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_BAND_RSP, ERR_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY, 0,
                                pSmInfo->nBandUTI | (nSimID << 24), 0xF0);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("change the band fail\n",0x08100969)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_BAND_RSP, ERR_CME_NO_NETWORK_SERVICE, 0,
                                pSmInfo->nBandUTI | (nSimID << 24), 0xF0);
        }
    }
    else if (pSmInfo->bDetached)
    {
        if (pCsRegStatus->Status == API_NW_FULL_SVCE)
        {
            pSmInfo->bDetached = FALSE;
            pSmInfo->bReselNW  = FALSE;
            if (pSmInfo->nNetMode != CFW_NW_AUTOMATIC_MODE)
            {
                CFW_NwSetRegistration(pSmInfo->nAttOperatorId, pSmInfo->nNetMode, pSmInfo->nReselUTI, nSimID);
            }
            else
            {

                UINT8 *pPLMN = (UINT8 *)CSW_NW_MALLOC(6);

                SUL_MemCopy8(pPLMN, pSmInfo->nOperatorId, 6);
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send  EV_CFW_NW_SET_REGISTRATION_RSP to ATL\n",0x0810096a)));

                CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP,
                                    (UINT32)pPLMN,
                                    (UINT32)(pSmInfo->nNetMode ==
                                             API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE),
                                    pSmInfo->nBandUTI | (nSimID << 24), 0);
            }
        }
        else if (pCsRegStatus->Status == API_NW_NO_SVCE)
        {
#ifdef CSW_USER_DBS
            UINT8 nStrtFlag = 0xff;
#endif
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("No service start stack s_proc->nBand = 0x%x\n",0x0810096b)), pSmInfo->nBand);

#ifdef CSW_USER_DBS
            CFW_NwStartStack(pSmInfo->nBand, nStrtFlag, nSimID);
#else
#ifdef __MULTI_RAT__
            CFW_NwStartStack(pSmInfo->nBand, pSmInfo->nUBand, nSimID);
#else
            CFW_NwStartStack(pSmInfo->nBand, nSimID);
#endif
#endif

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("status is not full service, send API_SIM_OPEN_IND to Stack\n",0x0810096c)));

            // CFW_SendSclMessage(API_SIM_OPEN_IND, 0, nSimID);

        }
    }
    else if (pSmInfo->bReselNW) // attach to another plmn
    {
        pSmInfo->bReselNW = FALSE;

        if(FALSE == CFW_GetNWTimerOutFlag(nSimID))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Maybe high layer has timer out and no long wait the message\n",0x0810096d)));
            return ERR_SUCCESS;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pCsRegStatus->Status %d\n",0x0810096e)), pCsRegStatus->Status);
            CFW_SetNWTimerOutFlag(FALSE, nSimID);
        }

        if (pCsRegStatus->Status == API_NW_FULL_SVCE)
        {
            UINT8 *pPLMN = (UINT8 *)CSW_NW_MALLOC(6);

            SUL_MemCopy8(pPLMN, pSmInfo->nOperatorId, 6);
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("send  EV_CFW_NW_SET_REGISTRATION_RSP to ATL\n",0x0810096f)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP,
                                (UINT32)pPLMN,
                                (UINT32)(pSmInfo->nNetMode ==
                                         API_NW_AUTOMATIC_MODE ? API_NW_AUTOMATIC_MODE : API_NW_MANUAL_MODE),
                                pSmInfo->nCOPSUTI | (nSimID << 24), 0);
        }
        else if (pCsRegStatus->Status == API_NW_LTD_SVCE)
        {

            if (pSmInfo->nNetMode == CFW_NW_MANUAL_AUTOMATIC_MODE)
            {
                BOOL ret;

                UINT32 nTimerId = 0;

                if (CFW_SIM_0 == nSimID)
                    nTimerId = PRV_CFW_NW_SIM0_TIMER_ID;
                else if (CFW_SIM_1 == nSimID)
                    nTimerId = PRV_CFW_NW_SIM1_TIMER_ID;
                else if ((CFW_SIM_1 + 1) == nSimID)
                    nTimerId = PRV_CFW_NW_SIM2_TIMER_ID;
                else if ((CFW_SIM_1 + 2) == nSimID)
                    nTimerId = PRV_CFW_NW_SIM3_TIMER_ID;
                ret = COS_SetTimerEX(0xFFFFFFFF, nTimerId, COS_TIMER_MODE_SINGLE, 5 * 1000 MILLI_SECOND);
                pSmInfo->bTimerneed = TRUE;

                if (ret)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("register fail,  CFW_NW_MANUAL_AUTOMATIC_MODE mode, nw timer start success\n",0x08100970)));
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("nw timer start fail\n",0x08100971)));
                }

            }
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("try to attach to another plmn fail limited service\n",0x08100972)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP, ERR_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY, 0,
                                pSmInfo->nCOPSUTI | (nSimID << 24), 0xF0);

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("try to attach to another plmn fail no service\n",0x08100973)));

            CFW_PostNotifyEvent(EV_CFW_NW_SET_REGISTRATION_RSP, ERR_CME_NO_NETWORK_SERVICE, 0,
                                pSmInfo->nCOPSUTI | (nSimID << 24), 0xF0);
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("warning !CFW_NwCsStatusConf attach to another plmn  \n",0x08100974)));
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwCsStatusConf end \n",0x08100975)));

    // ..........................
    // CFW_PostNotifyEvent
    CSW_PROFILE_FUNCTION_EXIT(CFW_NwCsStatusConf);
    return ERR_SUCCESS;
}

UINT32 CFW_NwSetFrequencyBand(UINT8 nBand, UINT16 nUTI
                              , CFW_SIM_ID nSimID
                             )
{
    NW_SM_INFO *s_proc = NULL;  // static sm info
    HAO s_hAo = 0;  // static handle

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_NwSetFrequencyBand nBand = 0x%x, nUTI = 0x%x\n",0x08100976)), nBand, nUTI);

#if 0 //Remove for Samsung issue by XP 20130925
    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwSetFrequencyBand SIM ERROR!\n",0x08100977)));

        return ret;
    }
#endif

    if ((nBand == 0) || (nBand & API_GSM_450) || (nBand & API_GSM_480) || (nBand & API_GSM_900R))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwSetFrequencyBand input error! \n",0x08100978)));

        return ERR_CFW_INVALID_PARAMETER;
    }
    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_NwSetFrequencyBand UTI_IS_BUSY \n",0x08100979)));
        return ERR_CFW_UTI_IS_BUSY;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !s_hAo == 0 \n",0x0810097a)));
        return - 1; // AOM error
    }

    s_proc                    = CFW_GetAoUserData(s_hAo);
    s_proc->nCsStatusIndCount = 0;

    s_proc->nBand       = nBand;
    s_proc->bChangeBand = TRUE;
    s_proc->nStpCause   = STOP_CAUSE_FB;
    CFW_CfgNwSetFrequencyBand(nBand, nSimID);

    // CFW_NwStopStack();

    CFW_NwStop(FALSE, nUTI, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwSetFrequencyBand end \n",0x0810097b)));

    return ERR_SUCCESS;
}

// AT+CGATT  ps_ind should be move to gprs modle
// ps attach and detach
// nState:
// CFW_GPRS_DETACHED 0 detached
// CFW_GPRS_ATTACHED 1 attached
// event:EV_CFW_GPRS_ATT_NOTIFY
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the attach or detach
#ifdef CFW_GPRS_SUPPORT
UINT32 CFW_GprsAtt(UINT8 nState, UINT16 nUTI
                   , CFW_SIM_ID nSimID)
{
    return CFW_GprsAttEX(nState,nUTI,nSimID,0);
}

UINT32 CFW_GprsAttEX(UINT8 nState, UINT16 nUTI
                   , CFW_SIM_ID nSimID, COS_CALLBACK_FUNC_T func)
{
    NW_SM_INFO *proc = NULL;  // sm info
    HAO hAo         = 0;  // ao
    UINT8 nAttstate = 0xFF;
    HAO s_hAo       = 0;  // staitc ao

    NW_SM_INFO *s_proc = NULL;  // static sm
    CSW_PROFILE_FUNCTION_ENTER(CFW_GprsAtt);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_GprsAtt state = 0x%x, nUTI = 0x%x\n",0x0810097c)), nState, nUTI);

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsAtt SIM ERROR!\n",0x0810097d)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ret;
    }

    if ((nState != CFW_GPRS_ATTACHED) && (nState != CFW_GPRS_DETACHED))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR CFW_GprsAtt input error\n",0x0810097e)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_GprsAtt ERR_CFW_UTI_IS_BUSY \n",0x0810097f)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    CFW_GetGprsAttState(&nAttstate, nSimID);

    if (((nAttstate == CFW_GPRS_ATTACHED) && (nState == CFW_GPRS_ATTACHED))
            || ((nAttstate == CFW_GPRS_DETACHED) && (nState == CFW_GPRS_DETACHED)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_GprsAtt get data error \n",0x08100980)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }

    if(nState == CFW_GPRS_DETACHED)
    {
        CFW_StopDetachTimer(nSimID);
    }
    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL) // malloc error
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("malloc exception\n",0x08100981)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);
    s_proc->func = func;

    /*
         if((CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING  == s_proc->sGprsStatusInfo.nStatus)&&(
            (API_P_ILLEGAL_USR == s_proc->sGprsStatusInfo.nCause)||
            (API_P_SRVC_NOT_ALLOWD == s_proc->sGprsStatusInfo.nCause)))
         {
             CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !API_NW_NO_SVCE  == s_proc->sGprsStatusInfo.nStatus \n",0x08100982)) );
             return ERR_CME_OPERATION_NOT_ALLOWED;
         }
            */
    // Clear the proc structure, ensure that there is not existing un-intialized member.
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));
    if (nState == CFW_GPRS_ATTACHED)  // attach
    {
#ifdef LTE_NBIOT_SUPPORT
        if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSimID)) 
        {
            proc->nMsgId   = API_START_REQ;
	        s_proc->nAttach = 1;
	        proc->bOnlyAtt = 1;
        }
	 else
#endif
       {
            proc->nMsgId   = API_NW_PSATTACH_REQ;
       }
        //add by wuys 2011-11-28,
        proc->AttachType = 1;
        //add end

        proc->pMsgBody = NULL;
    }
    else  // detach
    {
  #ifdef LTE_NBIOT_SUPPORT
        if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSimID)) 
        {
           // proc->nMsgId   = API_STOP_REQ;
	     s_proc->nStpCause = STOP_CAUSE_NOMAL;
	     s_proc->nAttach =  0;
	     CFW_NwStop(0, nUTI, nSimID);
	     return ERR_SUCCESS;
        }
	 else
#endif
	 {
             proc->nMsgId   = API_NW_PSDETACH_REQ;
	 }
        //add by wuys 2011-11-28,
        proc->DetachType = 1;
        //add end
        proc->pMsgBody = NULL;
    }

    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsAtt end \n",0x08100983)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);

    return ERR_SUCCESS;
}


UINT32 CFW_AttDetach (UINT8 nState, UINT16 nUTI, UINT8 AttDetachType
                      , CFW_SIM_ID nSimID
                     , COS_CALLBACK_FUNC_T func)
{
    NW_SM_INFO *proc = NULL;  // sm info
    HAO hAo         = 0;      // ao
    UINT8 nAttstate = 0xFF;
    HAO s_hAo       = 0;      // staitc ao

    NW_SM_INFO *s_proc = NULL;  // static sm
    CSW_PROFILE_FUNCTION_ENTER(CFW_Detach);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_GprsAtt state = 0x%x, nUTI = 0x%x, detachType: 0x%x\n",0x08100984)), nState, nUTI, AttDetachType);

    UINT8 ret;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsAtt SIM ERROR!\n",0x08100985)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_Detach);
        return ret;
    }

    if ((nState != CFW_GPRS_ATTACHED) && (nState != CFW_GPRS_DETACHED))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR CFW_GprsAtt input error\n",0x08100986)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_Detach);
        return ERR_CFW_INVALID_PARAMETER;
    }


    if (((nAttstate == CFW_GPRS_ATTACHED) && (nState == CFW_GPRS_ATTACHED))
            || ((nAttstate == CFW_GPRS_DETACHED) && (nState == CFW_GPRS_DETACHED)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !CFW_GprsAtt get data error \n",0x08100987)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }

    if(nState == CFW_GPRS_DETACHED)
    {
        CFW_StopDetachTimer(nSimID);
    }

    proc = (NW_SM_INFO *)CSW_NW_MALLOC(SIZEOF(NW_SM_INFO));
    if (proc == NULL) // malloc error
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("malloc exception\n",0x08100988)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_Detach);
        return ERR_CME_MEMORY_FULL; // should be updated by macro
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);
    s_proc = CFW_GetAoUserData(s_hAo);
    s_proc->func = func;
    SUL_ZeroMemory32(proc, SIZEOF(NW_SM_INFO));
    if (nState == CFW_GPRS_ATTACHED)  // attach
    {
        proc->nMsgId   = API_NW_PSATTACH_REQ;
        proc->AttachType = AttDetachType;
        proc->pMsgBody = NULL;
    }
    else
    {
        proc->nMsgId   = API_NW_PSDETACH_REQ;
        proc->DetachType = AttDetachType;
        proc->pMsgBody = NULL;
    }

    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, proc, CFW_NwAoProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsAtt end \n",0x08100989)));
    CSW_PROFILE_FUNCTION_EXIT(CFW_GprsAtt);

    return ERR_SUCCESS;
}

// get the att state attached or not?
// [out]pState

UINT32 CFW_GetGprsAttState(UINT8 *pState
                           , CFW_SIM_ID nSimID
                          )
{
    NW_SM_INFO *s_proc = NULL;  // sm info
    HAO s_hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_GetGprsAttState);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GetGprsAttState start \n",0x0810098a)));

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsAtt SIM ERROR!\n",0x0810098b)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GetGprsAttState);
        return ret;
    }

    if (pState == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR CFW_GprsAtt SIM input error!\n",0x0810098c)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GetGprsAttState);
        return ERR_CFW_INVALID_PARAMETER;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !s_hAo == 0 \n",0x0810098d)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_GetGprsAttState);
        return - 1; // AOM error
    }

    s_proc  = CFW_GetAoUserData(s_hAo);
    *pState = s_proc->nGprsAttState;
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GetGprsAttState end state = 0x%x\n",0x0810098e)), *pState);
    CSW_PROFILE_FUNCTION_EXIT(CFW_GetGprsAttState);
    return ERR_SUCCESS;
}

// get the gprs status
// [out]pStatus
UINT32 CFW_GprsGetstatus(CFW_NW_STATUS_INFO *pStatus
                         , CFW_SIM_ID nSimID
                        )
{
    NW_SM_INFO *s_proc = NULL;  // sm info
    HAO s_hAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_GprsGetstatus);

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetstatus start \n",0x0810098f)));

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetstatus SIM ERROR!\n",0x08100990)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsGetstatus);
        return ret;
    }

    if (pStatus == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR CFW_GprsGetstatus SIM input error!\n",0x08100991)));

        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsGetstatus);
        return ERR_CFW_INVALID_PARAMETER;
    }

    s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

    if (s_hAo == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR !s_hAo == 0 \n",0x08100992)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_GprsGetstatus);
        return - 1; // AOM error
    }
    s_proc = CFW_GetAoUserData(s_hAo);
    SUL_MemCopy8(pStatus, &s_proc->sGprsStatusInfo, SIZEOF(CFW_NW_STATUS_INFO));
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTXT(TSTR("CFW_GprsGetstatus status = 0x%x LAC = 0x%x%x, CI = 0x%x%x, Pstype: %d\n",0x08100993)),
              pStatus->nStatus, pStatus->nAreaCode[3], pStatus->nAreaCode[4], pStatus->nCellId[0], pStatus->nCellId[1], pStatus->PsType);

    CSW_PROFILE_FUNCTION_EXIT(CFW_GprsGetstatus);
    return ERR_SUCCESS;
}
#endif

// This message is used by the MMI to abort the construction of the list of surrounding PLMNs.
// If the CFW_NwGetAvailableOperators does not sent before, the CFW_NwListAbort should not be called.
// return

UINT32 CFW_NwAbortListOperators(UINT16 nUTI
                                , CFW_SIM_ID nSimID
                               )
{
    NW_SM_INFO *proc = NULL;
    HAO hAo        = 0;
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_NwAbortListOperators);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAbortListOperators start\n",0x08100994)));

    UINT8 ret;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsGetstatus SIM ERROR!\n",0x08100995)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwAbortListOperators);
        return ret;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_NW_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR! CFW_GprsGetstatus SIM ERROR!UTI_IS_BUSY\n",0x08100996)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_NwAbortListOperators);
        return ERR_CFW_UTI_IS_BUSY;
    }
    CFW_RelaseUsingUTI(0,nUTI);//free this uti,because the AO have a UTI.
    nAoCount = CFW_GetAoCount(CFW_NW_SRV_ID, nSimID);
    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, nSimID);

        proc = CFW_GetAoUserData(hAo);

        if (proc->nMsgId == API_NW_LIST_REQ)
        {
            proc->nMsgId    = API_NW_LISTABORT_REQ;
            proc->pMsgBody  = NULL;
            proc->bStaticAO = FALSE;
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);  // trigger the aom
            CSW_PROFILE_FUNCTION_EXIT(CFW_NwAbortListOperators);
            return ERR_SUCCESS;
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_NwAbortListOperators end\n",0x08100997)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_NwAbortListOperators);

    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
}

#ifdef CFW_GPRS_SUPPORT
extern CFW_GPRS_PDPCONT_INFO *PdpContList[CFW_SIM_COUNT][7];
VOID CFW_GprsDeactiveAll(
    CFW_SIM_ID nSimID
)
{
    HAO hAo        = 0;  // ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    UINT32 nUTI = 0;
    GPRS_SM_INFO* pUserData = NULL;  
    CSW_PROFILE_FUNCTION_ENTER(CFW_GprsDeactiveAll);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsDeactiveAll start\n",0x08100998)));

    nAoCount = CFW_GetAoCount(CFW_GPRS_SRV_ID, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsDeactiveAll nAoCount: %d\n",0x08100999)), nAoCount);

    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, nSimID);

        pUserData = CFW_GetAoUserData(hAo); 
		if(pUserData != NULL)
		{
	        if(PdpContList[nSimID][pUserData->Cid2Index.nCid - 1] != NULL)
            {
                CFW_GprsRemovePdpCxt(pUserData->Cid2Index.nCid, nSimID);
            }
            if(!IsCidSavedInApp(pUserData->Cid2Index.nCid, nSimID))
                CFW_ReleaseCID(pUserData->Cid2Index.nCid, nSimID);
	        CFW_GetUTI(hAo, &nUTI);
            CFW_PostNotifyEvent(EV_CFW_GPRS_CXT_DEACTIVE_IND, pUserData->Cid2Index.nCid, 0, nUTI | (nSimID << 24), 0);
        }
        CFW_UnRegisterAO(CFW_GPRS_SRV_ID, hAo);
    }
#ifdef LTE_NBIOT_SUPPORT
    CFW_StopDetachTimer(nSimID);
#endif
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsDeactiveAll end\n",0x0810099a)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_GprsDeactiveAll);
}
#endif



VOID CFW_IMSItoHomePlmn(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen)
{
    UINT8 i;
    //08 39 43 30 00 29 69 00 41 01 00
    OutPut[0] = ((InPut[1] & 0xF0) >> 4);
    for (i = 2; i < InPut[0] + 1; i++)
    {
        OutPut[2 * (i - 1) - 1] = (InPut[i] & 0x0F) ;
        OutPut[2 * (i - 1)] = ((InPut[i] & 0xF0) >> 4) ;
    }
    OutPut[2 * InPut[0] - 1] = 0x00;
    *OutLen = 2 * InPut[0] - 1;

    return ;
}
#ifdef CHIP_HAS_AP
UINT8 bipOperatorId[6] = {0x06,0x04,0x06,0x00,0x05,0x0f};
UINT8 telmaOperatorId[6] = {0x06,0x04,0x06,0x00,0x04,0x0f};
BOOL WhetherTheSameOperator(UINT8 *nHomeOperatorId,UINT8 *nOperatorId,UINT8 *nHomeListId,UINT8 *nOperatorListId);
#endif
UINT32 CFW_ChangeRoamByPLMNName(api_NwCsRegStatusInd_t *pCsRegStatus, UINT8 *nOperatorId, CFW_SIM_ID nSimID)
{
    UINT8 nPlmn[30] = {0,};
    UINT8 *pPlmnName = nPlmn;
    UINT8 nHomePlmn[30] = {0,};
    UINT8 *pHomePlmnName = nHomePlmn;
    UINT8 nHomeOperatorId[6] = {0x0f,};
    UINT8 nTempOperatorId[16] = {0x0f,};
    UINT8 nIMSI[11] = {0,};
    UINT8 *pHomePlmn = NULL;
    UINT32 nRet = ERR_SUCCESS;
    UINT8  nIMSILen = 0;
    UINT8  nMNCLen = 0;
	#ifdef AT_MODULE_SUPPORT_OTA
    UINT8  nHplmn[6] = {0,};
	#endif

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_ChangeRoamByPLMNName pCsRegStatus->Roaming:%d\n",0x0810099b)), pCsRegStatus->Roaming);
    if(API_NW_FULL_SVCE != pCsRegStatus->Status)
        return ERR_SUCCESS;
    //CSW_TC_MEMBLOCK(CFW_NW_TS_ID, nOperatorId, 6, 16);
    //reg plmn name
    nRet = CFW_CfgNwGetOperatorName(nOperatorId, &pPlmnName);
    if(ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName: ERROR plmn name no find\n",0x0810099c)));
        return ERR_SUCCESS;
    }

    //home plmn name
    nRet = CFW_CfgGetIMSI(nIMSI, nSimID);
    if(ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName: ERROR get issue IMSI\n",0x0810099d)));
        return ERR_SUCCESS;
    }
#ifdef TEST_SPECIAL_PLMN
    //nHomeOperatorId={0x4,0x0,0x4,0x0,0x2,0xf};
    nHomeOperatorId[0] = 0x04;
    nHomeOperatorId[1] = 0x00;
    nHomeOperatorId[2] = 0x04;
    nHomeOperatorId[3] = 0x00;
    nHomeOperatorId[4] = 0x02;
    nHomeOperatorId[5] = 0x0f;
#else
    //08 39 43 30 00 29 69 00 41 01 00
    CSW_TC_MEMBLOCK(CFW_NW_TS_ID, nIMSI, 11, 16);

    pHomePlmn = nIMSI;
    CFW_IMSItoHomePlmn(pHomePlmn, nTempOperatorId, &nIMSILen);

    //CSW_TC_MEMBLOCK(CFW_NW_TS_ID, nTempOperatorId, 11, 16);
    SUL_MemSet8(nHomeOperatorId, 0x0f, 6);

    //0x03,0x03,0x04,0x00,0x03,0x00
    CFW_GetMNCLen(&nMNCLen, nSimID);
    if(nMNCLen == 0xFF)
        nMNCLen = 2;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_ChangeRoamByPLMNName: nMNCLen:%d\n",0x0810099e)), nMNCLen);
    SUL_MemCopy8(nHomeOperatorId, nTempOperatorId, 3 + nMNCLen);
    //CSW_TC_MEMBLOCK(CFW_NW_TS_ID, nHomeOperatorId, 6, 16);
#endif
    #ifdef AT_MODULE_SUPPORT_OTA
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), TSTXT("CFW_ChangeRoamByPLMNName nHomeOperatorId = %d %d %d %d %d %d\n"),nHomeOperatorId[0],nHomeOperatorId[1],nHomeOperatorId[2],nHomeOperatorId[3],nHomeOperatorId[4],nHomeOperatorId[5]);
    cfw_IMSItoASC( pHomePlmn,nHplmn, &nIMSILen);
    SUL_MemCopy8(gHomePLMNNum[nSimID],nHplmn,3 + nMNCLen);
    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), TSTXT("CFW_ChangeRoamByPLMNName nSimID = %d homeplmn = %d %d %d %d %d %d\n"), nSimID,gHomePLMNNum[nSimID][0],gHomePLMNNum[nSimID][1],gHomePLMNNum[nSimID][2],gHomePLMNNum[nSimID][3],gHomePLMNNum[nSimID][4],gHomePLMNNum[nSimID][5]);
    //strcpy(gPLMN[nSimID], nOperatorId);
    //CSW_TRACE(CFW_NW_TS_ID | C_DETAIL, TSTXT("CFW_ChangeRoamByPLMNName gplmn = %d %d %d %d %d %d\n"),gPLMN[nSimID][0],gPLMN[nSimID][1],gPLMN[nSimID][2],gPLMN[nSimID][3],gPLMN[nSimID][4],gPLMN[nSimID][5]);
    if(FALSE != auto_update_gprs_profile(nSimID))
    {
        wap_apn_auto_matching(nSimID);
    }
    #endif

    CSW_TC_MEMBLOCK(CFW_NW_TS_ID, nHomeOperatorId, 6, 16);
#ifdef CHIP_HAS_AP
    if(WhetherTheSameOperator(nHomeOperatorId,nOperatorId,bipOperatorId,telmaOperatorId))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT("special plmn to change Roaming to 0\n"));
        pCsRegStatus->Roaming = FALSE;
    }
#endif
    nRet = CFW_CfgNwGetOperatorName(nHomeOperatorId, &pHomePlmnName);
    if(ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_ChangeRoamByPLMNName: ERROR home plmn name no find:nRet:0x%x\n",0x0810099f)), nRet);
        return ERR_SUCCESS;
    }
    //CSW_TC_MEMBLOCK(CFW_NW_TS_ID, pHomePlmnName, 15, 16);
    //CSW_TC_MEMBLOCK(CFW_NW_TS_ID, pPlmnName, 15, 16);
    if(0 == SUL_StrCompare(pHomePlmnName, pPlmnName))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName Change Roaming is 0\n",0x081009a0)));
        pCsRegStatus->Roaming = FALSE;
    }

    UINT8 nHomeCompany[30] = {0,};
    UINT8 *pHomeCompanyName = nHomeCompany;

    UINT8 nCompany[30] = {0,};
    UINT8 *pCompanyName = nCompany;
    //The diffrent plmn and the same operator,in india is roam.
    nRet = CFW_CfgNwGetCompanyName(nHomeOperatorId, &pHomeCompanyName);
    if( nRet != ERR_SUCCESS )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName home Plmn isn't in special table\n",0x081009a1)));
        return ERR_SUCCESS;
    }

    nRet = CFW_CfgNwGetCompanyName(nOperatorId, &pCompanyName);
    if( nRet != ERR_SUCCESS )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName Plmn isn't in special table\n",0x081009a2)));
        return ERR_SUCCESS;
    }

    if(SUL_StrCompare(pHomeCompanyName, pCompanyName) != 0 )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName Change Roaming is TRUE\n",0x081009a3)));
        pCsRegStatus->Roaming = TRUE;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_ChangeRoamByPLMNName end\n",0x081009a4)));
    return ERR_SUCCESS;
}
#ifdef CHIP_HAS_AP
BOOL WhetherTheSameOperator(UINT8 *nHomeOperatorId,UINT8 *nOperatorId,UINT8 *nHomeListId,UINT8 *nOperatorListId)
{
    if((0==SUL_MemCompare(nHomeOperatorId, bipOperatorId, 6))
            &&(0==SUL_MemCompare(nOperatorId, telmaOperatorId, 6)))
    {
        return TRUE;
    }
    return FALSE;
}
#endif
#ifdef __MULTI_RAT__
UINT32 CFW_XcpuTestProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;
    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    CFW_XCPUTEST *pXcpuTest = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStageBootProc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pXcpuTest = CFW_GetAoUserData(hSimInitializeAo);

    switch (pXcpuTest->nXcpuTest_currStatus)
    {
        case CFW_XCPU_TEST_STATE_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                api_XcpuTestReq_t *pXcpuTestReq = NULL;
                pXcpuTestReq = (api_XcpuTestReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_XcpuTestReq_t));
                if (pXcpuTestReq == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x081009a5));
                    CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
                    return ERR_NO_MORE_MEMORY;
                }
                //SUL_MemCopy8(pXcpuTestReq->para, pXcpuTest->para, 6);
                for(UINT8 i=0; i<6; i++)
                    pXcpuTestReq->para[i] = pXcpuTest->para[i];
                pXcpuTestReq->type = pXcpuTest->type;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SendXcpuTestReq nSimID 0x%x \n",0x081009a6), nSimID);
                CFW_SendSclMessage(API_XCPUTEST_REQ, pXcpuTestReq, nSimID);
                pXcpuTest->nXcpuTest_currStatus = CFW_XCPU_TEST_STATE_READY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
        }
        break;
        case CFW_XCPU_TEST_STATE_READY:
        {
            if (nEvtId == API_XCPUTEST_RSP)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Recv API_XCPUTEST_RSP\n",0x081009a7));
                api_XcpuTestRsp_t *pXcpuTestCnf = (api_XcpuTestRsp_t *)nEvParam;
                CFW_PostNotifyEvent(EV_CFW_XCPU_TEST_RSP, pXcpuTestCnf, 0, 0,0);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
        }
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
    return ERR_SUCCESS;
}
UINT32 CFW_XcpuTest(UINT32 para[6],UINT32 type,CFW_SIM_ID nSimID,UINT8 UTI)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Enter CFW_XcpuTest\n",0x081009a8));
    HAO hAo = 0x00;
    CFW_XCPUTEST *pXcpuTest = NULL;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x081009a9), ret);
        return ret;
    }
    pXcpuTest = (CFW_XCPUTEST *)CSW_SIM_MALLOC(SIZEOF(CFW_XCPUTEST));
    if (pXcpuTest == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x081009aa));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pXcpuTest, SIZEOF(CFW_XCPUTEST));
    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, pXcpuTest, CFW_XcpuTestProc,nSimID);
    //SUL_MemCopy8(pXcpuTest->para, para, 6);
    for(UINT8 i=0; i<6; i++)
        pXcpuTest->para[i] = para[i];
    pXcpuTest->type = type;
    pXcpuTest->nXcpuTest_currStatus = CFW_XCPU_TEST_STATE_IDLE;
    CFW_SetUTI(hAo, UTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    return ERR_SUCCESS;
}
UINT32 CFW_WcpuTestProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;
    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    CFW_WCPUTEST *pWcpuTest = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStageBootProc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pWcpuTest = CFW_GetAoUserData(hSimInitializeAo);

    switch (pWcpuTest->nWcpuTest_currStatus)
    {
        case CFW_WCPU_TEST_STATE_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                api_WcpuTestReq_t *pWcpuTestReq = NULL;
                pWcpuTestReq = (api_WcpuTestReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_WcpuTestReq_t));
                if (pWcpuTestReq == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x081009ab));
                    CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
                    return ERR_NO_MORE_MEMORY;
                }
                //SUL_MemCopy8(pWcpuTestReq->para, pWcpuTest->para, 6);
                for(UINT8 i=0; i<6; i++)
                    pWcpuTestReq->para[i] = pWcpuTest->para[i];
                pWcpuTestReq->type = pWcpuTest->type;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SendXcpuTestReq nSimID 0x%x \n",0x081009ac), nSimID);
                CFW_SendSclMessage(API_WCPUTEST_REQ, pWcpuTestReq, nSimID);
                pWcpuTest->nWcpuTest_currStatus = CFW_WCPU_TEST_STATE_READY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
        }
        break;
        case CFW_WCPU_TEST_STATE_READY:
        {
            if (nEvtId == API_WCPUTEST_RSP)
            {
                api_WcpuTestRsp_t *pWcpuTestCnf = (api_WcpuTestRsp_t *)nEvParam;
                CFW_PostNotifyEvent(EV_CFW_WCPU_TEST_RSP, pWcpuTestCnf, 0, 0,0);
                CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
            }
        }
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
    return ERR_SUCCESS;
}
UINT32 CFW_WcpuTest(UINT32 para[6],UINT32 type,CFW_SIM_ID nSimID,UINT8 UTI)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Enter CFW_WcpuTest\n",0x081009ad));
    HAO hAo = 0x00;
    CFW_WCPUTEST *pWcpuTest = NULL;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x081009ae), ret);
        return ret;
    }
    pWcpuTest = (CFW_WCPUTEST *)CSW_SIM_MALLOC(SIZEOF(CFW_WCPUTEST));
    if (pWcpuTest == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x081009af));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pWcpuTest, SIZEOF(CFW_WCPUTEST));
    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, pWcpuTest, CFW_WcpuTestProc,nSimID);
    //SUL_MemCopy8(pWcpuTest->para, para, 6);
    for(UINT8 i=0; i<6; i++)
        pWcpuTest->para[i] = para[i];
    pWcpuTest->type = type;
    pWcpuTest->nWcpuTest_currStatus = CFW_WCPU_TEST_STATE_IDLE;
    CFW_SetUTI(hAo, 0, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    return ERR_SUCCESS;
}
#endif
#endif // ENALB

#define API_GSM_450  ( 1 << 0 )
#define API_GSM_480  ( 1 << 1 )
#define API_GSM_900P ( 1 << 2 )
#define API_GSM_900E ( 1 << 3 )
#define API_GSM_900R ( 1 << 4 )
#define API_GSM_850  ( 1 << 5 )
#define API_DCS_1800 ( 1 << 6 )
#define API_PCS_1900 ( 1 << 7 )

UINT32 CFW_GetFreqInfoProc(HAO hAo, CFW_EV *pEvent)
{
	NW_QSCANF_INFO *pQscanfInfo=NULL;
	NW_FREQ_INFO *pFreqs=NULL;
    UINT32 nRet = 0;
    CFW_EV ev;
    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    UINT32 nUTI = 0x00;
    CFW_SIM_ID nSimID;

    CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc()\r\n"));
    if ((hAo == 0) || (pEvent == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc  ao or pEvent ERROR!!!\r\n"));
        CSW_PROFILE_FUNCTION_EXIT(CFW_GetFreqInfoProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    nSimID = CFW_GetSimCardID(hAo);
    CFW_GetUTI(hAo, &nUTI);
    pQscanfInfo = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    switch(pQscanfInfo->n_CurrStatus)
    {
        case CFW_SM_NW_STATE_IDLE:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc CFW_SM_NW_STATE_IDLE\r\n"));
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
                ev.nParam1 = 0xff;
                pEvent     = &ev;
                
                api_PowerScanReq_t *pPowerScanReq= (api_PowerScanReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PowerScanReq_t));
				SUL_ZeroMemory8(pPowerScanReq, SIZEOF(api_PowerScanReq_t));
                if(1 == pQscanfInfo->nAllBand )
				{
					pPowerScanReq->Band= API_PCS_1900|API_GSM_850;
					pPowerScanReq->Nb = 0;
				}
				else
				{
					pPowerScanReq->Band= pQscanfInfo->nBand;
					pPowerScanReq->Nb = pQscanfInfo->nFreqNb;
					SUL_MemCopy8(pPowerScanReq->List, pQscanfInfo->nFreqInfo, SIZEOF(NW_FREQ_INFO)*pQscanfInfo->nFreqNb );
				}
				CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc pPowerScanReq->Nb:%d,pPowerScanReq->Band:%d\r\n"),pPowerScanReq->Nb,pPowerScanReq->Band);

                CFW_SendSclMessage(API_POWER_SCAN_REQ, (api_PowerScanReq_t *)pPowerScanReq, nSimID);
                pQscanfInfo->n_PrevStatus = pQscanfInfo->n_CurrStatus;
                pQscanfInfo->n_CurrStatus = CFW_SM_NW_STATE_SEARCH;
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc Send API_POWER_SCAN_REQ to stack\r\n"));
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc !!! if ((UINT32)pEvent == 0xFFFFFFFF\r\n"));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        break;

        case CFW_SM_NW_STATE_SEARCH:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc CFW_SM_NW_STATE_SEARCH\r\n"));
            if (nEvtId == API_POWER_SCAN_RSP)
            {
                api_PowerScanRsp_t *pQscanfRsp =(api_PowerScanRsp_t *)nEvParam;
                if(0 == pQscanfInfo->nAllBand )
				{
					pQscanfInfo->nBand=0xff;//mean we have receive stack rsq
					pQscanfInfo->nFreqNb=pQscanfRsp->Nb;
					pFreqs=CSW_NW_MALLOC(pQscanfInfo->nFreqNb*SIZEOF(NW_FREQ_INFO));
					SUL_ZeroMemory8(pFreqs, pQscanfInfo->nFreqNb*SIZEOF(NW_FREQ_INFO));
					SUL_MemCopy8(pFreqs,pQscanfRsp->List,  SIZEOF(NW_FREQ_INFO)*pQscanfRsp->Nb );
					CFW_PostNotifyEvent(EV_CFW_NW_GET_QSCANF_RSP, pFreqs, pQscanfInfo->nFreqNb, nUTI | (nSimID << 24), 0x00);
					CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc Send EV_CFW_NW_GET_QSCANF_RSP to MMI\r\n"));

					CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
					hAo = HNULL;
				}
				else if(1 == pQscanfInfo->nAllBand )
				{
					pQscanfInfo->nBand=0xfe;//mean we had received band1900 and band8500
					pQscanfInfo->nFreqNb=pQscanfRsp->Nb;
					if(pQscanfInfo->nFreqNb >= 600)
					{
						CFW_PostNotifyEvent(EV_CFW_NW_GET_QSCANF_RSP,NULL ,0, nUTI | (nSimID << 24), 0xfe);
						CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc Send EV_CFW_NW_GET_QSCANF_RSP to MMI\r\n"));
						CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
						hAo = HNULL;
						return ERROR_MORE_LARGE_IMAGE;
					}
					SUL_MemCopy8(pQscanfInfo->nFreqInfo,pQscanfRsp->List,  SIZEOF(NW_FREQ_INFO)*pQscanfRsp->Nb );
					pQscanfInfo->nAllBand = 2;

					api_PowerScanReq_t *pPowerScanReq= (api_PowerScanReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PowerScanReq_t));
					SUL_ZeroMemory8(pPowerScanReq, SIZEOF(api_PowerScanReq_t));
					pPowerScanReq->Band= API_GSM_900P|API_DCS_1800;
					pPowerScanReq->Nb = 0;
					CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc STATE_SEARCH pPowerScanReq->Nb:%d,pPowerScanReq->Band:%d\r\n"),pPowerScanReq->Nb,pPowerScanReq->Band);

					CFW_SendSclMessage(API_POWER_SCAN_REQ, (api_PowerScanReq_t *)pPowerScanReq, nSimID);
					CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc STATE_SEARCH Send API_POWER_SCAN_REQ to stack\r\n"));
				}
				else if(2 == pQscanfInfo->nAllBand )
				{
					UINT16 nTmpFreqNb = pQscanfInfo->nFreqNb;
					pQscanfInfo->nBand=0xff;//mean we have receive stack rsq
					pQscanfInfo->nFreqNb += pQscanfRsp->Nb;
					pFreqs=CSW_NW_MALLOC(pQscanfInfo->nFreqNb*SIZEOF(NW_FREQ_INFO));
					SUL_ZeroMemory8(pFreqs, pQscanfInfo->nFreqNb*SIZEOF(NW_FREQ_INFO));

					SUL_MemCopy8(pFreqs,pQscanfInfo->nFreqInfo,  SIZEOF(NW_FREQ_INFO)* nTmpFreqNb);
					SUL_MemCopy8(pFreqs + nTmpFreqNb,pQscanfRsp->List,  SIZEOF(NW_FREQ_INFO)*pQscanfRsp->Nb );
					
					CFW_PostNotifyEvent(EV_CFW_NW_GET_QSCANF_RSP, pFreqs, pQscanfInfo->nFreqNb, nUTI | (nSimID << 24), 0x00);
					CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc AllBand is 2 Send EV_CFW_NW_GET_QSCANF_RSP to MMI\r\n"));

					CFW_UnRegisterAO(CFW_NW_SRV_ID, hAo);
					hAo = HNULL;
				}
				else
				{
					CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc STATE_SEARCH pQscanfInfo->nAllBand ERROR!\r\n"));
				}
			}
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoProc isn't API_POWER_SCAN_RSP\r\n"));
            }
        }
        break;

        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GetFreqInfoProc error! \n",0x08100875)));
            break;

    }
    return ERR_SUCCESS;
}



UINT32 CFW_GetFreqInfo(UINT8 nBand, UINT16 nFreqNb,UINT16 *pFreqs, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    NW_QSCANF_INFO *pQcan=NULL;
    UINT32 ret = ERR_SUCCESS;
    UINT16 i = 0;

    if(nFreqNb > 600)
      return ERR_MORE_DATA;

    pQcan =( NW_QSCANF_INFO *)CSW_NW_MALLOC(SIZEOF(NW_QSCANF_INFO));
    SUL_ZeroMemory8(pQcan, SIZEOF(NW_QSCANF_INFO));
    pQcan->n_CurrStatus = CFW_SM_NW_STATE_IDLE;
    pQcan->n_PrevStatus = CFW_SM_NW_STATE_IDLE;
    if(0 == nBand)
    {
    	pQcan->nBand=API_GSM_900P;
    }else if(1 == nBand)
    {
    	pQcan->nBand=API_DCS_1800;
    }
    else if(2 == nBand)
    {
    	pQcan->nBand=API_PCS_1900;
    }
    else if(3 == nBand)
    {
    	pQcan->nBand=API_GSM_850;
    }
    else
    {
    	return ERR_INVALID_PARAMETER;
    }
    pQcan->nFreqNb=nFreqNb;
    for(i = 0;i < nFreqNb; i++) 
    {
		if((0==i)&&(9999 == pFreqs[0]))
		{
            pQcan->nAllBand = 1;
			CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("CFW_GetFreqInfoc Scan all freqs!!!\r\n"));
			break;
		}
        pQcan->nFreqInfo[i].Arfcn=pFreqs[i];    
    }
    
    hAo = CFW_RegisterAo(CFW_NW_SRV_ID, pQcan, CFW_GetFreqInfoProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ret;
}

#ifdef LTE_NBIOT_SUPPORT
UINT8 CFW_GprsGetPsType(UINT8 nSimID)
{
    CFW_NW_STATUS_INFO sStatus;

    if (CFW_GprsGetstatus(&sStatus, nSimID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "ERROR! CFW_GprsGetPsType CFW_GprsGetstatus");
        return 0; 
    }

    //CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID), "CFW_GprsGetPsType PS type = %d", sStatus.PsType);
    return sStatus.PsType;
}

#endif //LTE_NBIOT_SUPPORT
UINT32 CFW_NW_SendDetectMBSMessage(UINT8 nMode, CFW_SIM_ID nSimID)
{
	api_DetectMBSReq_t *pDetectMBS = CFW_MemAllocMessageStructure(sizeof(api_DetectMBSReq_t));

	pDetectMBS->bDetectMBS = nMode;   

	if (ERR_SUCCESS != CFW_SendSclMessage(API_DETECT_MBS_REQ , pDetectMBS, nSimID))
	{
		CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("SendDetectMBSMessage Send Message ERROR!!\r\n"));
		return ERR_BUSY_DRIVE;
	}
	CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID | C_DETAIL), ("SendDetectMBSMessage Send API_DETECT_MBS_REQ Sucess! bDetectMBS=%d \r\n"),nMode);
	return ERR_SUCCESS;
}
