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
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_chv.h"
#include "cfw_usim.h"
#include "hal_host.h"

extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;

UINT32 CFW_SimGetAuthenStatusProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimGetAuthPwAo = hAo;

    CFW_GETAUTH *pSimGetAuthData = NULL;
    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    UINT32 nUTI = 0;
    UINT32 Result = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetAuthenStatusProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimGetAuthPwAo);
    pSimGetAuthData = CFW_GetAoUserData(hSimGetAuthPwAo);
    CFW_GetUTI(hSimGetAuthPwAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("(CFW_SimGetAuthenStatusProc) currStatus = 0x%x",0x08100bb6), pSimGetAuthData->nSm_GetAuth_currStatus);

    switch (pSimGetAuthData->nSm_GetAuth_currStatus)
    {
        case SM_GETAUTH_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    UINT8 SimAuthPIN[8] = {0};
                    Result  = SimVerifyCHVReq(SimAuthPIN, CHV10, nSimID);
                }
                else
                    Result = SimDedicFileStatusReq(API_DF_TELECOM, nSimID);

                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100bb7), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetAuthPwAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenStatusProc);
                    return Result;
                }
                pSimGetAuthData->nSm_GetAuth_prevStatus = pSimGetAuthData->nSm_GetAuth_currStatus;

                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    pSimGetAuthData->nSm_GetAuth_currStatus = SM_GETAUTH_USIM;
                else
                    pSimGetAuthData->nSm_GetAuth_currStatus = SM_GETAUTH_DEDICATED;
            }
            else
                CFW_SetAoProcCode(hSimGetAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_GETAUTH_DEDICATED:
        {
            if (nEvtId == API_SIM_DEDICFILESTATUS_CNF)
            {
                api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("API_SIM_DEDICFILESTATUS_CNF SW1 0x%x SW2 0x%x \n",0x08100bb8), pSimDedicFileStatusCnf->Sw1,
                          pSimDedicFileStatusCnf->Sw2);

                if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (00 == pSimDedicFileStatusCnf->Sw2))
                        || (0x91 == pSimDedicFileStatusCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param = NULL;
                    Remains = SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)pG_Chv_Param->nCHVAuthen,
                                        Remains, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    Result = Sim_ParseSW1SW2(pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetAuthPwAo);
            }
            else
                CFW_SetAoProcCode(hSimGetAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_GETAUTH_USIM:
        {
            if (nEvtId == API_SIM_VERIFYCHV_CNF)
            {
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100bb9), pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);

                if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;
                    SIM_CHV_PARAM *pG_Chv_Param;

                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    Remains = CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, CHV1, nSimID);
                    if(pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PUK1)
                    {
                        UINT8  SimAuthPuk[8] = {0};
                        SimUnblockCHVReq(SimAuthPuk, SimAuthPuk, CHV10, nSimID);
                        break;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)pG_Chv_Param->nCHVAuthen, Remains, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    Result = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetAuthPwAo);
            }
            else  if (nEvtId == API_SIM_UNBLOCKCHV_CNF)
            {
                api_SimUnblockCHVCnf_t *pSimUnblockCHVCnf = (api_SimUnblockCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_UNBLOCKCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100bba), pSimUnblockCHVCnf->Sw1, pSimUnblockCHVCnf->Sw2);

                if ((0x63 == pSimUnblockCHVCnf->Sw1) || (0x69 == pSimUnblockCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;
                    SIM_CHV_PARAM *pG_Chv_Param;
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    Remains = CFW_USimParsePukStatus((api_SimVerifyCHVCnf_t *)pSimUnblockCHVCnf, pG_Chv_Param, 1, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)pG_Chv_Param->nCHVAuthen, Remains, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    Result = Sim_ParseSW1SW2(pSimUnblockCHVCnf->Sw1, pSimUnblockCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_AUTH_STATUS_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetAuthPwAo);
            }
            else
                CFW_SetAoProcCode(hSimGetAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        case SM_GETAUTH_MMI:
            break;
        case SM_GETAUTH_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenStatusProc);
    return Result;
}

UINT32 CFW_SimEnterAuthenticationProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimEnterAuthPwAo = hAo;
    CFW_ENTERAUTH *pSimEnterAuthData = NULL;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    UINT32 nUTI     = 0;
    UINT32 Result = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimEnterAuthenticationProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimEnterAuthPwAo);
    pSimEnterAuthData = CFW_GetAoUserData(hSimEnterAuthPwAo);
    CFW_GetUTI(hSimEnterAuthPwAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("(CFW_SimEnterAuthenticationProc) currStatus = 0x%x ",0x08100bbb), pSimEnterAuthData->nSm_EntAuth_currStatus);
    hal_HstSendEvent(0xfa061402);
    hal_HstSendEvent(csw_TMGetTick());
    hal_HstSendEvent(pSimEnterAuthData->nSm_EntAuth_currStatus);
    hal_HstSendEvent(nEvtId);
    switch (pSimEnterAuthData->nSm_EntAuth_currStatus)
    {
        case SM_ENTAUTH_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    UINT8 SimAuthPIN[8] = {0};
                    if(pSimEnterAuthData->nOption == 2)
                        Result  = SimVerifyCHVReq(SimAuthPIN, CHV20, nSimID);
                    else // pSimEnterAuthData->nOption: 0 or 1
                        Result  = SimVerifyCHVReq(SimAuthPIN, CHV10, nSimID);
                }
                else
                    Result = SimDedicFileStatusReq(API_DF_TELECOM, nSimID);

                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100bbc), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                    return Result;
                }

                pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_USIM;
                else
                    pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_DEDICATED;
            }
            else
                CFW_SetAoProcCode(hSimEnterAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_ENTAUTH_DEDICATED:
        {
            api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimDedicFileStatusCnf->Sw1 in SM_ENTAUTH_DEDICATED %d\n",0x08100bbd), pSimDedicFileStatusCnf->Sw1);

            if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (00 == pSimDedicFileStatusCnf->Sw2))
                    || (0x91 == pSimDedicFileStatusCnf->Sw1))
            {
                UINT8 CHV = 0;
                SIM_CHV_PARAM *pG_Chv_Param;

                SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimEnterAuthData->nOption %d\n",0x08100bbe), pSimEnterAuthData->nOption);

                if (pSimEnterAuthData->nOption == 0)
                {

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pG_Chv_Param->nCHVAuthen %d\n",0x08100bbf), pG_Chv_Param->nCHVAuthen);
                    if ((pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN1BLOCK) ||
                            (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN2BLOCK))
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, pG_Chv_Param->nCHVAuthen, 0, nUTI | (nSimID << 24), 0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                        return ERR_SUCCESS;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PUK1)
                    {
                        pSimEnterAuthData->nOption = 1;
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bc0), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PUK2)
                    {
                        pSimEnterAuthData->nOption = 2;
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV2, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bc1), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN1)
                    {
                        pSimEnterAuthData->nOption = 1;
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bc2), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN2)
                    {
                        pSimEnterAuthData->nOption = 2;
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV2, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bc3), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error pG_Chv_Param->nCHVAuthen %d\n",0x08100bc4), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, pG_Chv_Param->nCHVAuthen, 0, nUTI | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                        return Result;

                    }
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimEnterAuthData->nOption %d\n",0x08100bc5), pSimEnterAuthData->nOption);
                    if (pSimEnterAuthData->nOption == 1)
                        CHV = CHV1;
                    else
                        CHV = CHV2;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimEnterAuthData->pNewPwd[0] 0x%x\n",0x08100bc6), pSimEnterAuthData->pNewPwd[0]);
                    if (pSimEnterAuthData->pNewPwd[0] != 0xFF)
                    {
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bc7), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else
                    {
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bc8), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }
                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                }
                CFW_SetAoProcCode(hSimEnterAuthPwAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                SIM_MISC_PARAM *pG_Mic_aram = NULL;
                CFW_CfgSimGetMicParam(&pG_Mic_aram, nSimID);
                Result = Sim_ParseSW1SW2(pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2, nSimID);

                CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                return Result;
            }
        }
        break;

        case SM_ENTAUTH_USIM:
        {
            api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
            if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
            {
                UINT8 CHV = 0;
                SIM_CHV_PARAM *pG_Chv_Param;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                if(nEvtId == API_SIM_VERIFYCHV_CNF)
                    CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, pSimEnterAuthData->nOption, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimEnterAuthData->nOption %d\n",0x08100bc9), pSimEnterAuthData->nOption);
                if (pSimEnterAuthData->nOption == 0)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pG_Chv_Param->nCHVAuthen %d\n",0x08100bca), pG_Chv_Param->nCHVAuthen);
                    if ((pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN1BLOCK) ||
                            (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN2BLOCK))
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, pG_Chv_Param->nCHVAuthen, 0, nUTI | (nSimID << 24), 0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                        return ERR_SUCCESS;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PUK1)
                    {
                        pSimEnterAuthData->nOption = 1;
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bcb), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PUK2)
                    {
                        pSimEnterAuthData->nOption = 2;
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV2, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bcc), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN1)
                    {
                        pSimEnterAuthData->nOption = 1;
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bcd), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_AUTH_PIN2)
                    {
                        pSimEnterAuthData->nOption = 2;
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV2, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bce), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);

                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error pG_Chv_Param->nCHVAuthen %d\n",0x08100bcf), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, pG_Chv_Param->nCHVAuthen, 0, nUTI | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                        return Result;
                    }
                }
                else
                {
                    if (pSimEnterAuthData->nOption == 1)
                        CHV = CHV1;
                    else
                        CHV = CHV2;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimEnterAuthData->pNewPwd[0] 0x%x\n",0x08100bd0), pSimEnterAuthData->pNewPwd[0]);
                    if (pSimEnterAuthData->pNewPwd[0] != 0xFF)
                    {
                        Result = SimUnblockCHVReq(pSimEnterAuthData->pPinPwd, pSimEnterAuthData->pNewPwd, CHV, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimUnblockCHVReq return 0x%x \n",0x08100bd1), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }

                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_UNBLOCKCHV;
                    }
                    else
                    {
                        Result = SimVerifyCHVReq(pSimEnterAuthData->pPinPwd, CHV, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bd2), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                            return Result;
                        }
                        pSimEnterAuthData->nSm_EntAuth_prevStatus = pSimEnterAuthData->nSm_EntAuth_currStatus;
                        pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_VERIFYCHV;
                    }
                }
                CFW_SetAoProcCode(hSimEnterAuthPwAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                SIM_MISC_PARAM *pG_Mic_aram = NULL;
                CFW_CfgSimGetMicParam(&pG_Mic_aram, nSimID);
                Result = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                return Result;
            }
        }
        break;

        case SM_ENTAUTH_UNBLOCKCHV:
        {
            if (nEvtId == API_SIM_UNBLOCKCHV_CNF)
            {
                SIM_MISC_PARAM *pG_Mic_Param;
                SIM_CHV_PARAM *pG_Chv_Param;

                api_SimUnblockCHVCnf_t *pSimUnblockCHVCnf = (api_SimUnblockCHVCnf_t *)nEvParam;
                CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (((0x90 == pSimUnblockCHVCnf->Sw1) && (0x00 == pSimUnblockCHVCnf->Sw2))
                        || (0x91 == pSimUnblockCHVCnf->Sw1))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_UNBLOCKCHV_CNF OK\n",0x08100bd3));

                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        if (pSimEnterAuthData->nOption == 1)
                        {
                            pG_Chv_Param->nPin1Remain = pG_Chv_Param->nPin1Max;
                            pG_Chv_Param->nPuk1Remain = pG_Chv_Param->nPuk1Max;
                        }
                        else  if (pSimEnterAuthData->nOption == 2)
                        {
                            pG_Chv_Param->nPin2Remain = pG_Chv_Param->nPin2Max;
                            pG_Chv_Param->nPuk2Remain = pG_Chv_Param->nPuk2Max;
                        }
                    }

                    if(pSimEnterAuthData->nOption == 1)
                    {
                        pG_Chv_Param->nCHVAuthen  = CFW_SIM_AUTH_PIN1READY;
                        pG_Chv_Param->nPin1Status = CHV_VERIFY;
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                        //Here should send a message to SHELL and active stack.
                        //if(pG_Mic_Param->bStartPeriod == TRUE)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SM_ENTAUTH_UNBLOCKCHV1 OK\n",0x08100bd4));
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_STAGE2);
                            pG_Mic_Param->bStartPeriod = FALSE;
                        }
                    }
                    else  // pSimEnterAuthData->nOption == 2
                    {
                        pG_Chv_Param->bCHV2VerifyReq = FALSE; // NEED VERIFY????
                        pG_Chv_Param->nPin2Status    = CHV_VERIFY;
                        pG_Chv_Param->nPin2Status    = CHV_VERIFY;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SM_ENTAUTH_UNBLOCKCHV2 OK\n",0x08100bd5));

                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                    }
                }
                else
                {
                    if ((0x63 == pSimUnblockCHVCnf->Sw1) || (0x69 == pSimUnblockCHVCnf->Sw1))  // USIM chv ERROR
                        CFW_USimParsePukStatus((api_SimVerifyCHVCnf_t *)pSimUnblockCHVCnf, pG_Chv_Param, pSimEnterAuthData->nOption, nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_UNBLOCKCHV_CNF  ERROR\n",0x08100bd6));
                    Result = Sim_ParseSW1SW2(pSimUnblockCHVCnf->Sw1, pSimUnblockCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                CSW_PROFILE_FUNCTION_EXIT(hSimEnterAuthPwAo);
                return Result;

            }
            else
                CFW_SetAoProcCode(hSimEnterAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_ENTAUTH_VERIFYCHV:
        {
            if (nEvtId == API_SIM_VERIFYCHV_CNF)
            {
                SIM_MISC_PARAM *pG_Mic_Param;
                SIM_CHV_PARAM *pG_Chv_Param;
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;

                CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (((0x90 == pSimVerifyCHVCnf->Sw1) && (00 == pSimVerifyCHVCnf->Sw2))
                        || (0X91 == pSimVerifyCHVCnf->Sw1))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_VERIFYCHV_CNF  OK\n",0x08100bd7));

                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        if (pSimEnterAuthData->nOption == 1)
                            pG_Chv_Param->nPin1Remain = pG_Chv_Param->nPin1Max;
                        else  if (pSimEnterAuthData->nOption == 2)
                            pG_Chv_Param->nPin2Remain = pG_Chv_Param->nPin2Max;
                    }

                    if (pSimEnterAuthData->nOption == 1)
                    {
                        pG_Chv_Param->nCHVAuthen  = CFW_SIM_AUTH_PIN1READY;
                        pG_Chv_Param->nPin1Status = CHV_VERIFY;
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                        if (pG_Mic_Param->bStartPeriod == TRUE)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SM_ENTAUTH_VERIFYCHV1 OK\n",0x08100bd8));
                            CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_STAGE2);
                            pG_Mic_Param->bStartPeriod = FALSE;
                        }
                    }
                    else  // pSimEnterAuthData->nOption == 2
                    {
                        pG_Chv_Param->bCHV2VerifyReq = FALSE; // NEED VERIFY????
                        pG_Chv_Param->nPin2Status    = CHV_VERIFY;
                        pG_Chv_Param->nPin2Status    = CHV_VERIFY;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SM_ENTAUTH_VERIFYCHV2 OK\n",0x08100bd9));
                        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                    }
                }
                else if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))  // USIM chv ERROR
                {
                    CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, pSimEnterAuthData->nOption, nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM ERROR API_SIM_VERIFYCHV_CNF \n",0x08100bda));
                    Result = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR API_SIM_VERIFYCHV_CNF \n",0x08100bdb));
                    Result = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimEnterAuthPwAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
                return Result;
            }
            else
                CFW_SetAoProcCode(hSimEnterAuthPwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_ENTAUTH_MMI:
            break;
        case SM_ENTAUTH_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthenticationProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimChangePasswordProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimChangePwAo = hAo;

    CFW_CHANGEPW *pSimChangePwData = NULL;
    SIM_CHV_PARAM *pG_Chv_Param;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    UINT32 nUTI     = 0;
    UINT32 Result = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimChangePasswordProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimChangePwAo);
    pSimChangePwData = CFW_GetAoUserData(hSimChangePwAo);
    CFW_GetUTI(hSimChangePwAo, &nUTI);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

    switch (pSimChangePwData->nSm_ChgPw_currStatus)
    {
        case SM_CHGPW_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                Result = SimChangeCHVReq(pSimChangePwData->pOldPwd, pSimChangePwData->pNewPwd,
                                         pSimChangePwData->nCHVNumber, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimChangeCHVReq return 0x%x\n",0x08100bdc), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_CHANGE_PWD_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimChangePwAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePasswordProc);
                    return Result;
                }

                pSimChangePwData->nSm_ChgPw_prevStatus = pSimChangePwData->nSm_ChgPw_currStatus;
                pSimChangePwData->nSm_ChgPw_currStatus = SM_CHGPW_CHANGECHV;
                CFW_SetAoProcCode(hSimChangePwAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimChangePwAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_CHGPW_CHANGECHV:
        {
            if (nEvtId == API_SIM_CHANGECHV_CNF)
            {
                api_SimChangeCHVCnf_t *pSimChangeCHVCnf = (api_SimChangeCHVCnf_t *)nEvParam;
                if ((0x90 == pSimChangeCHVCnf->Sw1) && (00 == pSimChangeCHVCnf->Sw2))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Change CHV SUCCESS\n",0x08100bdd));

                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        if (pSimChangePwData->nCHVNumber == 1)
                            pG_Chv_Param->nPin1Remain = pG_Chv_Param->nPin1Max;
                        else  if (pSimChangePwData->nCHVNumber == 2)
                            pG_Chv_Param->nPin2Remain = pG_Chv_Param->nPin2Max;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SIM_CHANGE_PWD_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Change USIM CHV1 Failed:pSimChangePwData->nCHVNumber = %d",0x08100bde), pSimChangePwData->nCHVNumber);
                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        if ((0x63 == pSimChangeCHVCnf->Sw1) || (0x69 == pSimChangeCHVCnf->Sw1))  // USIM chv ERROR
                            CFW_USimParsePinStatus((api_SimVerifyCHVCnf_t *)pSimChangeCHVCnf, pG_Chv_Param, pSimChangePwData->nCHVNumber, nSimID);

                        if((0x63 == pSimChangeCHVCnf->Sw1)&&(0xC0 == pSimChangeCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Change USIM CHV1 Failed and no attempt left!",0x08100bdf));
                            CFW_PostNotifyEvent(EV_CFW_SIM_CHANGE_PWD_RSP, (UINT32)(0x63C0),  pSimChangePwData->nCHVNumber, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                    }
                    else
                    {
                        if((0x98 == pSimChangeCHVCnf->Sw1)&&(0x40 == pSimChangeCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Change SIM CHV1 Failed and no attempt left!",0x08100be0));
                            CFW_PostNotifyEvent(EV_CFW_SIM_CHANGE_PWD_RSP, (UINT32)(0x9840),  pSimChangePwData->nCHVNumber, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error ChangeCHV\n",0x08100be1));

                    Result = Sim_ParseSW1SW2(pSimChangeCHVCnf->Sw1, pSimChangeCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_CHANGE_PWD_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimChangePwAo);
            }
        }
        break;
        case SM_CHGPW_MMI:
            break;
        case SM_CHGPW_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePasswordProc);
    return Result;
}

UINT32 CFW_SimGetFacilityProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimGetFacilityAo = hAo;
    CFW_GETFACILITY *pSimGetFacilityData = NULL;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    UINT32 nUTI     = 0;
    UINT32 nErrCode = 0;

    SIM_PBK_PARAM *pG_Pbk_Param;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetFacilityProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimGetFacilityAo);
    pSimGetFacilityData = CFW_GetAoUserData(hSimGetFacilityAo);
    CFW_GetUTI(hSimGetFacilityAo, &nUTI);

    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("(CFW_SimGetFacilityProc) currStatus = 0x%x",0x08100be2), pSimGetFacilityData->nSm_GetFac_currStatus);

    switch (pSimGetFacilityData->nSm_GetFac_currStatus)
    {

        case SM_GETFAC_IDLE:
        {
            UINT32 Result = 0;

            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if (pSimGetFacilityData->nFac == CFW_STY_FAC_TYPE_SC)
                {
                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        UINT8 SimAuthPIN[8] = {0};
                        Result  = SimVerifyCHVReq(SimAuthPIN, CHV10, nSimID);
                    }
                    else
                        Result = SimDedicFileStatusReq(API_DF_TELECOM, nSimID);

                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100be3), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                        return Result;
                    }

                    pSimGetFacilityData->nSm_GetFac_prevStatus = pSimGetFacilityData->nSm_GetFac_currStatus;
                    if(g_cfw_sim_status[nSimID].UsimFlag)
                        pSimGetFacilityData->nSm_GetFac_currStatus = SM_GETFAC_USIM;
                    else
                        pSimGetFacilityData->nSm_GetFac_currStatus = SM_GETFAC_DEDICATED;
                }
                else if (pSimGetFacilityData->nFac == CFW_STY_FAC_TYPE_FD)
                {
                    if(g_cfw_sim_status[nSimID].UsimFlag)
                        Result = SimReadBinaryReq(API_USIM_EF_EST, 0, 1, nSimID);
                    else
                        Result = SimElemFileStatusReq(API_SIM_EF_ADN, nSimID);

                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100be4), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                        return Result;
                    }

                    pSimGetFacilityData->nSm_GetFac_prevStatus = pSimGetFacilityData->nSm_GetFac_currStatus;

                    if(g_cfw_sim_status[nSimID].UsimFlag)
                        pSimGetFacilityData->nSm_GetFac_currStatus = SM_GETFAC_READBINARY;
                    else
                        pSimGetFacilityData->nSm_GetFac_currStatus = SM_GETFAC_ELEMENTARY;
                }
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_GETFAC_DEDICATED:
        {
            if (nEvtId == API_SIM_DEDICFILESTATUS_CNF)
            {
                UINT8 nStatus = 0;
                api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
                if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (00 == pSimDedicFileStatusCnf->Sw2))
                        || (0x91 == pSimDedicFileStatusCnf->Sw1))
                {
                    UINT32 Remains = 0;
                    SIM_CHV_PARAM *pG_Chv_Param;
                    Remains = SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                    if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
                        nStatus = 0;
                    else
                        nStatus = 1;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("DedicatedStatus %x\n",0x08100be5), nStatus);

                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nStatus, Remains, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error DedicatedStatus\n",0x08100be6));

                    nErrCode = Sim_ParseSW1SW2(pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                return ERR_SUCCESS;
            }
            else
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_GETFAC_USIM:
        {
            if ((nEvtId == API_SIM_VERIFYCHV_CNF) || (nEvtId == API_SIM_UNBLOCKCHV_CNF))
            {
                UINT8 nStatus = 0;
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100be7), pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);

                if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;
                    SIM_CHV_PARAM *pG_Chv_Param;
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    if(nEvtId == API_SIM_VERIFYCHV_CNF)
                        Remains = CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, CHV1, nSimID);

                    if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
                        nStatus = 0;
                    else
                        nStatus = 1;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("DedicatedStatus %x\n",0x08100be8), nStatus);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nStatus, Remains, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Error DedicatedStatus\n",0x08100be9));

                    nErrCode = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                return ERR_SUCCESS;
            }
            else
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_GETFAC_ELEMENTARY:
        {
            UINT8 nStatus = 0;

            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1) )
                {
                    if ((pSimElemFileStatusCnf->Data[11] & 0x01) == 0)
                        nStatus = 1;
                    else
                        nStatus = 0;

                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nStatus, 0, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Elementary\n",0x08100bea));
                    nErrCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                }

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                return ERR_SUCCESS;
            }
            else
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_GETFAC_READBINARY:
        {
            UINT8 nStatus = 0;

            if (nEvtId == API_SIM_READBINARY_CNF)
            {
                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
                if (((0x90 == pSimReadBinaryCnf->Sw1) && (00 == pSimReadBinaryCnf->Sw2))
                        || (0x91 == pSimReadBinaryCnf->Sw1) )
                {
                    if ((pSimReadBinaryCnf->Data[0] & 0x01) == 0x01)
                        nStatus = 1;
                    else
                        nStatus = 0;

                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nStatus, 0, nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Error Elementary\n",0x08100beb));
                    nErrCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FACILITY_LOCK_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                }

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetFacilityAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                return ERR_SUCCESS;
            }
            else
                CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_GETFAC_MMI:
            break;
        case SM_GETFAC_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimSetFacilityProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI     = 0;
    UINT32 Result = 0;
    HAO hSimSetFacilityAo = hAo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetFacilityProc);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimSetFacilityAo);
    CFW_SETFACILITY *pSimSetFacilityData = CFW_GetAoUserData(hSimSetFacilityAo);
    CFW_GetUTI(hSimSetFacilityAo, &nUTI);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("(CFW_SimSetFacilityProc) currStatus = 0x%x",0x08100bec), pSimSetFacilityData->nSm_SetFac_currStatus);
    switch (pSimSetFacilityData->nSm_SetFac_currStatus)
    {
        case SM_SETFAC_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if (pSimSetFacilityData->nFac == CFW_STY_FAC_TYPE_SC)
                {
                    if (pSimSetFacilityData->nMode == 1)
                    {
                        Result = SimEnableCHVReq(pSimSetFacilityData->pBufPwd, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimEnableCHVReq return 0x%x \n",0x08100bed), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                            return Result;
                        }
                        pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                        pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_ENABLECHV;
                    }
                    else  // if(pSimSetFacilityData.nMode==0)
                    {
                        Result = SimDisableCHVReq(pSimSetFacilityData->pBufPwd, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDisableCHVReq return 0x%x \n",0x08100bee), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                            return Result;
                        }
                        pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                        pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_DISABLECHV;
                    }
                }
                else if (pSimSetFacilityData->nFac == CFW_STY_FAC_TYPE_FD)
                {
                    SIM_CHV_PARAM *pG_Chv_Param = NULL;
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                    if (pG_Chv_Param->nPin2Status == CHV_NOVERIFY)
                        pG_Chv_Param->bCHV2VerifyReq = TRUE;
#if 0
                    if (pSimSetFacilityData->nPwdSize == 0)
                    {
                        if (pSimSetFacilityData->nMode == 0)
                        {
                            UINT32 Result;

                            if(g_cfw_sim_status[nSimID].UsimFlag)
                                Result = SimReadBinaryReq(API_USIM_EF_EST, 0, 1, nSimID);
                            else
                                Result = SimRehabilitateReq(API_SIM_EF_ADN, nSimID);

                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimRehabilitateReq return 0x%x \n",0x08100bef), Result);

                                CFW_GetUTI(hSimSetFacilityAo, &nUTI);
                                CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                                return ERR_SUCCESS;
                            }

                            pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;

                            if(g_cfw_sim_status[nSimID].UsimFlag)
                                pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_READBINARY;
                            else
                                pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_REHABILITATE;
                        }
                        else  // if(pSimSetFacilityData->nMode==1)
                        {
                            UINT32 Result;

                            if(g_cfw_sim_status[nSimID].UsimFlag)
                                Result = SimReadBinaryReq(API_USIM_EF_EST, 0, 1, nSimID);
                            else
                                Result = SimInvalidateReq(API_SIM_EF_ADN, nSimID);

                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimInvalidateReq return 0x%x \n",0x08100bf0), Result);

                                CFW_GetUTI(hSimSetFacilityAo, &nUTI);
                                CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                                return ERR_SUCCESS;
                            }

                            pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                            if(g_cfw_sim_status[nSimID].UsimFlag)
                                pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_READBINARY;
                            else
                                pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_INVALIDATE;
                        }
                    }
                    else
#endif
                    {

                        Result = SimVerifyCHVReq(pSimSetFacilityData->pBufPwd, CHV2, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimVerifyCHVReq return 0x%x \n",0x08100bf1), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                            return Result;
                        }
                        pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                        pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_VERIFYCHV;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("OK:--SM_SETFAC_VERIFYCHV--nSimID = %d\n",0x08100bf2), nSimID);
                    }
                }
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SETFAC_ENABLECHV:
        {
            if (pEvent->nEventId == API_SIM_ENABLECHV_CNF)
            {
                api_SimEnableCHVCnf_t *pSimEnableCHVCnf = (api_SimEnableCHVCnf_t *)pEvent->nParam1;
                SIM_CHV_PARAM *pG_Chv_Param;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (((0x90 == pSimEnableCHVCnf->Sw1) && (00 == pSimEnableCHVCnf->Sw2)) || (0x91 == pSimEnableCHVCnf->Sw1))
                {
                    pG_Chv_Param->nPin1Status = CHV_VERIFY;

                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        CFW_UsimPin* UsimPin;
                        CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                        UsimPin->usimSpecificData.pinStatus.enabled = TRUE;
                        pG_Chv_Param->nPin1Remain =  pG_Chv_Param->nPin1Max;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)0, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        if((0x63 == pSimEnableCHVCnf->Sw1)&&(0xC0 == pSimEnableCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("ENABLE USIM CHV1 Failed and no attempt left!",0x08100bf3));
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)(0x63C0), pSimSetFacilityData->nFac, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                        CFW_USimParsePinStatus((api_SimVerifyCHVCnf_t *)pSimEnableCHVCnf, pG_Chv_Param, CHV1, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM SM_SETFAC_ENABLECHV ERROR ",0x08100bf4));
                    }
                    else
                    {
                        if((0x98 == pSimEnableCHVCnf->Sw1)&&(0x40 == pSimEnableCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("ENABLE SIM CHV1 Failed and no attempt left!",0x08100bf5));
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)(0x9840), pSimSetFacilityData->nFac, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                    }
                    Result = Sim_ParseSW1SW2(pSimEnableCHVCnf->Sw1, pSimEnableCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SETFAC_DISABLECHV:
        {
            if (pEvent->nEventId == API_SIM_DISABLECHV_CNF)
            {
                api_SimDisableCHVCnf_t *pSimDisableCHVCnf = (api_SimDisableCHVCnf_t *)pEvent->nParam1;
                SIM_CHV_PARAM *pG_Chv_Param;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (((0x90 == pSimDisableCHVCnf->Sw1) && (00 == pSimDisableCHVCnf->Sw2)) || (0x91 == pSimDisableCHVCnf->Sw1))
                {
                    pG_Chv_Param->nPin1Status = CHV_DISABLE;

                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        CFW_UsimPin* UsimPin;
                        CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                        UsimPin->usimSpecificData.pinStatus.enabled = FALSE;
                        pG_Chv_Param->nPin1Remain = pG_Chv_Param->nPin1Max;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)0, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        if((0x63 == pSimDisableCHVCnf->Sw1)&&(0xC0 == pSimDisableCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("DISABLE USIM CHV1 Failed and no attempt left!",0x08100bf6));
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)(0x63C0), pSimSetFacilityData->nFac, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                        CFW_USimParsePinStatus((api_SimVerifyCHVCnf_t *)pSimDisableCHVCnf, pG_Chv_Param, CHV1, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM SM_SETFAC_DISABLECHV' ERROR ",0x08100bf7));
                    }
                    else
                    {
                        if((0x98 == pSimDisableCHVCnf->Sw1)&&(0x40 == pSimDisableCHVCnf->Sw2))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("DISABLE SIM CHV1 Failed and no attempt left!",0x08100bf8));
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)(0x9840), pSimSetFacilityData->nFac, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        }
                    }

                    Result = Sim_ParseSW1SW2(pSimDisableCHVCnf->Sw1, pSimDisableCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SETFAC_REHABILITATE:
        {
            if ((pEvent->nEventId == API_SIM_REHABILITATE_CNF) || (pEvent->nEventId == API_SIM_UPDATEBINARY_CNF))
            {
                api_SimRehabilitateCnf_t *SimRehabilitateCnf = (api_SimRehabilitateCnf_t *)pEvent->nParam1;
                SIM_PBK_PARAM *pG_Pbk_Param = NULL;
                if (((0x90 == SimRehabilitateCnf->Sw1) && (00 == SimRehabilitateCnf->Sw2)) || (0x91 == SimRehabilitateCnf->Sw1))
                {
                    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
                    pG_Pbk_Param->bADNStatus = EF_ENABLE;

                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        pG_Pbk_Param->bFDNEnable = EF_DISABLE;
                    }
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)0, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("OK: --API_SIM_REHABILITATE_CNF--",0x08100bf9));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    Result = Sim_ParseSW1SW2(SimRehabilitateCnf->Sw1, SimRehabilitateCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("ERROR: --API_SIM_REHABILITATE_CNF--",0x08100bfa));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SETFAC_INVALIDATE:
        {
            if ((pEvent->nEventId == API_SIM_INVALIDATE_CNF) || (pEvent->nEventId == API_SIM_UPDATEBINARY_CNF))
            {
                api_SimInvalidateCnf_t *pSimInvalidateCnf = (api_SimInvalidateCnf_t *)pEvent->nParam1;
                SIM_PBK_PARAM *pG_Pbk_Param = NULL;
                if (((0x90 == pSimInvalidateCnf->Sw1) && (00 == pSimInvalidateCnf->Sw2)) || (0x91 == pSimInvalidateCnf->Sw1))
                {
                    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        pG_Pbk_Param->bFDNEnable = EF_ENABLE;
                    }
                    pG_Pbk_Param->bADNStatus = EF_DISABLE;
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)0, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("OK: --API_SIM_INVALIDATE_CNF--",0x08100bfb));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    Result = Sim_ParseSW1SW2(pSimInvalidateCnf->Sw1, pSimInvalidateCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("ERROR:-- API_SIM_INVALIDATE_CNF--",0x08100bfc));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SETFAC_VERIFYCHV:
        {
            if (pEvent->nEventId == API_SIM_VERIFYCHV_CNF)
            {
                SIM_CHV_PARAM *pG_Chv_Param;
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)pEvent->nParam1;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                pG_Chv_Param->bCHV2VerifyReq = FALSE;
                if (((0x90 == pSimVerifyCHVCnf->Sw1) && (0x00 == pSimVerifyCHVCnf->Sw2)) || (0x91 == pSimVerifyCHVCnf->Sw1))
                {
                    pG_Chv_Param->nPin2Status    = CHV_VERIFY;
                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        pG_Chv_Param->nPin2Remain = pG_Chv_Param->nPin2Max;
                        pG_Chv_Param->nPuk2Remain = pG_Chv_Param->nPuk2Max;
                    }
                    if (pSimSetFacilityData->nMode == 0)
                    {
                        if(g_cfw_sim_status[nSimID].UsimFlag)
                            Result = SimReadBinaryReq(API_USIM_EF_EST, 0, 1, nSimID);
                        else
                            Result = SimRehabilitateReq(API_SIM_EF_ADN, nSimID);

                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimRehabilitateReq return 0x%x \n",0x08100bfd), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                            return Result;
                        }
                        pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                        if(g_cfw_sim_status[nSimID].UsimFlag)
                            pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_READBINARY;
                        else
                            pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_REHABILITATE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("OK:--API_SIM_VERIFYCHV_CNF,pSimSetFacilityData->nMode = 0\n",0x08100bfe));
                    }
                    else  // if(pSimSetFacilityData->nMode==1)
                    {
                        if(g_cfw_sim_status[nSimID].UsimFlag)
                            Result = SimReadBinaryReq(API_USIM_EF_EST, 0, 1, nSimID);
                        else
                            Result = SimInvalidateReq(API_SIM_EF_ADN, nSimID);

                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimInvalidateReq return 0x%x \n",0x08100bff), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityProc);
                            return Result;
                        }

                        pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                        if(g_cfw_sim_status[nSimID].UsimFlag)
                            pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_READBINARY;
                        else
                            pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_INVALIDATE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("OK:--API_SIM_VERIFYCHV_CNF,pSimSetFacilityData->nMode = 1\n",0x08100c00));
                    }
                    CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
                }
                else
                {
                    if(g_cfw_sim_status[nSimID].UsimFlag)
                    {
                        CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, CHV2, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM SM_SETFAC_VERIFYCHV ERROR ",0x08100c01));
                    }
                    Result = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, pSimSetFacilityData->nFac, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR: --API_SIM_VERIFYCHV_CNF--\n",0x08100c02));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        break;

        case SM_SETFAC_READBINARY:
        {
            if (pEvent->nEventId == API_SIM_READBINARY_CNF)
            {
                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)pEvent->nParam1;
                if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
                {
                    UINT8 Data = pSimReadBinaryCnf->Data[0];

                    if (pSimSetFacilityData->nMode == 0)
                        Data &= 0xfe;
                    else
                        Data |= 0x01;

                    Result = SimUpdateBinaryReq(API_USIM_EF_EST, 0, 1, &Data, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimUpdatebinaryReq return 0x%x \n",0x08100c03), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, Result, 0, nUTI, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                        return Result;
                    }

                    pSimSetFacilityData->nSm_SetFac_prevStatus = pSimSetFacilityData->nSm_SetFac_currStatus;
                    if(pSimSetFacilityData->nMode == 0)
                        pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_REHABILITATE;
                    else
                        pSimSetFacilityData->nSm_SetFac_currStatus = SM_SETFAC_INVALIDATE;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("OK:--API_SIM_READBINARY_CNF--\n",0x08100c04));
                    CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
                }
                else
                {
                    Result = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_FACILITY_LOCK_RSP, (UINT32)Result, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetFacilityAo);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR:--API_SIM_READBINARY_CNF--\n",0x08100c05));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityProc);
                    return Result;
                }
            }
            else
                CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        break;
        default:
            break;
    }
    return ERR_SUCCESS;
}

