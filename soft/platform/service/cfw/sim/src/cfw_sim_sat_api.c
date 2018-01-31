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
//#include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_sat.h"

UINT32 CFW_SatResponseProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SatActivationProc (HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SatActivation(UINT8 nMode, UINT16 nUTI, CFW_SIM_ID nSimID)
{

    HAO hSimSatActivationAo;
    CFW_SAT_RESPONSE *pSatActivationData;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SatActivation);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMode = 0x%x\n",0x08100e79), nMode);

    if(nMode > 2)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivation);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivation);
        return ERR_CFW_UTI_EXIST;
    }

    pSatActivationData = (CFW_SAT_RESPONSE *)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_RESPONSE));
    if(pSatActivationData == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivation);
        return ERR_NO_MORE_MEMORY;
    }

    pSatActivationData->nSAT_Mode = nMode;
    hSimSatActivationAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSatActivationData, CFW_SatActivationProc, nSimID);
    CFW_SetUTI(hSimSatActivationAo, nUTI, 0);
    pSatActivationData->nSm_Act_prevStatus = SM_ACT_INIT;
    pSatActivationData->nSm_Act_currStatus = SM_ACT_IDLE;

    CFW_SetAoProcCode(hSimSatActivationAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivation);
    return(ERR_SUCCESS);
}


UINT32 CFW_SatResponse(UINT8 nCmdType, UINT8 nStatus, UINT8 nItemId,
                       VOID *pInputStr, UINT8 InputStrLen, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hSimSatResponseAo;
    CFW_SAT_RESPONSE *pSimSatResponseData;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("CFW_SatResponse nCmdType %x nStatus %x nItemId %x InputStrLen %x nSimID[%d]\n",0x08100e7a), nCmdType, nStatus, nItemId, InputStrLen, nSimID);
    CSW_PROFILE_FUNCTION_ENTER(CFW_SatResponse);
    if ((nCmdType == 0x00) || ((nCmdType > 0x05) && (nCmdType < 0x10)))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((nCmdType > 0x15) && (nCmdType < 0x20))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if((nCmdType == 0x29) ||((nCmdType > 0x35) && (nCmdType < 0x40)))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((nCmdType > 0x44) && ((nCmdType != 0xD1) && (nCmdType > 0xD7) && ((nCmdType & 0xFE) != 0xFE)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "nCmdType = %x", nCmdType);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if(InputStrLen)
    {
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pInputStr, InputStrLen, 16);
    }

    pSimSatResponseData = (CFW_SAT_RESPONSE *)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_RESPONSE) + InputStrLen);
    if(pSimSatResponseData == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_MemSet8(pSimSatResponseData, 0, (SIZEOF(CFW_SAT_RESPONSE) + InputStrLen));

    pSimSatResponseData->nSAT_CmdType = nCmdType;
    pSimSatResponseData->nSAT_Status    = nStatus;
    pSimSatResponseData->nSAT_ItemID    = nItemId;
    pSimSatResponseData->nSAT_InputLen = InputStrLen;
    if((SIM_SAT_GET_INPUT_COM == nCmdType) || (nCmdType == SIM_SAT_GET_INKEY_COM) )
    {
        if( NULL != pInputStr )
        {
            pSimSatResponseData->nSAT_InputSch = *(UINT8 *)pInputStr;
        }
        else
        {
            pSimSatResponseData->nSAT_InputSch = 0;
            pSimSatResponseData->nSAT_InputLen = 0;
        }
        if(InputStrLen != 0)
        {
            SUL_MemCopy8(pSimSatResponseData->nSAT_InputStr, (UINT8 *)pInputStr + 1, InputStrLen - 1);
        }
    }
    else if ((nCmdType == SIM_SAT_PROVIDE_LOCAL_INFO_COM) || (nCmdType == 0xFE) || (nCmdType == 0xFF))
    {
        SUL_MemCopy8(pSimSatResponseData->nSAT_InputStr, (UINT8 *)pInputStr, InputStrLen);
    }
    else if((nCmdType == SIM_SAT_SMSPP_DOWNLOAD) || (nCmdType == SIM_SAT_CALL_CONTROL) ||
            (nCmdType == SIM_SAT_TIMER_EXPIRATION))
    {
        pSimSatResponseData->nSAT_InputLen = InputStrLen;
        SUL_MemCopy8(pSimSatResponseData->nSAT_InputStr, (UINT8 *)pInputStr, InputStrLen);
    }
    else if( (nCmdType == SIM_SAT_CALL_SETUP_COM) || (nCmdType == SIM_SAT_SEND_USSD_COM) ||
             (nCmdType == SIM_SAT_DISPLAY_TEXT_COM) || (nCmdType == SIM_SAT_MENU_SELECTION) ||
             (nCmdType == SIM_SAT_SEND_DTMF_COM))
    {
        if( (pInputStr != NULL) && (InputStrLen > 0 ))
        {
            pSimSatResponseData->nSAT_InputLen = InputStrLen;
            SUL_MemCopy8(pSimSatResponseData->nSAT_InputStr, (UINT8 *)pInputStr, InputStrLen);
        }
    }

    pSimSatResponseData->nSm_Resp_prevStatus = SM_RESP_INIT;
    pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_IDLE;
    hSimSatResponseAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimSatResponseData, CFW_SatResponseProc, nSimID);

    CFW_SetUTI(hSimSatResponseAo, nUTI, 0);
    CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SatResponse OK!\n",0x08100e7b));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponse);
    return ERR_SUCCESS ;
}

UINT32 CFW_SatGetInformation( UINT8 nCmdType, VOID **CmdData , CFW_SIM_ID nSimID)
{
    SIM_SAT_PARAM *pSatGetInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SatGetInformation);

    CFW_CfgSimGetSatParam(&pSatGetInfo, nSimID);
    switch(nCmdType)
    {
        case SIM_SAT_SETUP_MENU_COM:
        {
            if(pSatGetInfo->pSetupMenu == NULL)
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
                return ERR_CFW_INVALID_COMMAND;
            }
            *CmdData = pSatGetInfo->pSetupMenu;
        }
        break;
        case SIM_SAT_REFRESH_COM:
        case SIM_SAT_MORE_TIME_COM:
        case SIM_SAT_POLL_INTERVAL_COM:
        case SIM_SAT_POLLING_OFF_COM:
        case SIM_SAT_SETUP_EVENT_LIST_COM:
        case SIM_SAT_CALL_SETUP_COM:
        case SIM_SAT_SEND_SS_COM:
        case SIM_SAT_SEND_USSD_COM:
        case SIM_SAT_SEND_SMS_COM:
        case SIM_SAT_SEND_DTMF_COM:
        case SIM_SAT_LAUNCH_BROWSER_COM:
        case SIM_SAT_DISPLAY_TEXT_COM:
        case SIM_SAT_GET_INKEY_COM:
        case SIM_SAT_GET_INPUT_COM:
        case SIM_SAT_SELECT_ITEM_COM:
        case SIM_SAT_PROVIDE_LOCAL_INFO_COM:
        case SIM_SAT_POWER_OFF_CARD_COM:
        case SIM_SAT_GET_READER_STATUS_COM:
        case SIM_SAT_GET_CHANNEL_STATUS_COM:
        {
            if(pSatGetInfo->pSatComData == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SatGetInformation: pSatGetInfo->pSatComData == NULL",0x08100e7c));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
                return ERR_CFW_INVALID_COMMAND;
            }
            if(nCmdType == pSatGetInfo->nCurCmd)
                *CmdData = pSatGetInfo->pSatComData;
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SatGetInformation: nCmdType(0x%x) != pSatGetInfo->nCurCmd(0x%x)",0x08100e7d), \
                          nCmdType,  pSatGetInfo->nCurCmd);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
                return ERR_CFW_INVALID_PARAMETER;
            }
        }
        break;
        case SIM_SAT_PLAY_TONE_COM:
        case SIM_SAT_TIMER_MANAGEMENT_COM:
        case SIM_SAT_SETUP_IDLE_DISPLAY_COM:
        case SIM_SAT_CARD_APDU_COM:
        case SIM_SAT_POWER_ON_CARD_COM:
        case SIM_SAT_RUN_AT_COMMAND_COM:
        case SIM_SAT_LANG_NOTIFY_COM:
        case SIM_SAT_OPEN_CHANNEL_COM:
        case SIM_SAT_CLOSE_CHANNEL_COM:
        case SIM_SAT_RECEIVE_DATA_COM:
        case SIM_SAT_SEND_DATA_COM:
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
            return ERR_CFW_NOT_SUPPORT;
        }
        default:
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SatGetInformation);
    return ERR_SUCCESS;
}

UINT8 CFW_SatGetCurCMD(CFW_SIM_ID nSimID)
{
    SIM_SAT_PARAM *pSatGetInfo = NULL;

    UINT32 ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        return ret;
    }
    CFW_CfgSimGetSatParam(&pSatGetInfo, nSimID);
    return pSatGetInfo->nCurCmd;
}

//b0  == FDN
//b1  == BDN
//b2  == CC
//0: Not Support,1: Support

UINT32 CFW_CfgSimGetMeProfile(CFW_PROFILE *pMeProfile)
{
    SUL_MemCopy8(pMeProfile, &nMEProfile, SIZEOF(CFW_PROFILE));
    return ERR_SUCCESS;
}

UINT32 CFW_CfgSimSetMeProfile(CFW_PROFILE *pMeProfile)
{
    SUL_MemCopy8(&nMEProfile, pMeProfile, SIZEOF(CFW_PROFILE));
    return ERR_SUCCESS;
}

extern UINT32 g_SimInit1Status[CFW_SIM_COUNT];
extern UINT8 bSatSimInit;
void CFW_SatSimInit(CFW_SIM_ID nSimID)
{
    bSatSimInit = 1;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "===CFW_SatSimInit===");
    UINT32 status = g_SimInit1Status[nSimID];
    if((status & CFW_INIT_SIM_PIN1_READY) == CFW_INIT_SIM_PIN1_READY)
        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, 0,GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);
    else if(((status & CFW_INIT_SIM_WAIT_PUK1) == CFW_INIT_SIM_WAIT_PUK1) || ((status & CFW_INIT_SIM_WAIT_PIN1) == CFW_INIT_SIM_WAIT_PIN1))
        CFW_PostNotifyEvent(EV_CFW_SIM_ENTER_AUTH_RSP, 0, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0);
}

