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
#include <base.h>
#include <cfw_prv.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_sat.h"

#define CardStatus    0x02

void CFW_ValidatePDU(UINT8 *pTPDU)
{
    UINT8 offSet = 0;
    offSet = (pTPDU[2] % 2 == 0) ? (pTPDU[2] / 2) : (pTPDU[2] / 2 + 1);
    if (pTPDU[5 + offSet] == 0x00)
    {
        pTPDU[5 + offSet] = 0x04;
    }
}

UINT8 USSDDCS2SMSDCS(UINT8 nInputData)
{
    //Parse TP-DCS , more in "3G TS 23.038 V3.3.0"
    /*
        00000000 7bit 0
        00000100 8bit 4
        00001000 UCS2 8
    */
    if((nInputData & 0xf0) == 0x00)// bit(7-0): 0000 xxxx
    {
        return 0;
    }
    else if((nInputData & 0xf0) == 0x10)// bit(7-0): 0001 xxxx
    {
        if((nInputData & 0x0f) == 0x00) // bit(7-0): 0001 0000
            return 0;
        else if ((nInputData & 0x0f) == 0x01)// bit(7-0): 0001 0001
            return 8;
        else
            return 0;
    }
    else if((nInputData & 0xc0) == 0x40)  //bit(7-0): 01xx xxxx
    {
        //Set pAlphabet
        if((nInputData & 0x0c) == 0x00)// bit(7-0): 01xx 00xx
        {
            return 0;
        }
        else if((nInputData & 0x0c) == 0x04)// bit(7-0): 01xx 01xx
        {
            return 4;
        }
        else if((nInputData & 0x0c) == 0x08)// bit(7-0): 01xx 10xx
        {
            return 8;
        }
        else if((nInputData & 0x0c) == 0x0c)// bit(7-0): 01xx 11xx
        {
            return 0;
        }
    }
    else if((nInputData & 0xf0) == 0x90)  //bit(7-0): 1001 xxxx
    {
        //Set pAlphabet
        if((nInputData & 0x0c) == 0x00)// bit(7-0): 1001 00xx
        {
            return 0;
        }
        else if((nInputData & 0x0c) == 0x04)// bit(7-0): 1001 01xx
        {
            return 4;
        }
        else if((nInputData & 0x0c) == 0x08)// bit(7-0): 1001 10xx
        {
            return 8;
        }
        else if((nInputData & 0x0c) == 0x0c)// bit(7-0): 1001 11xx
        {
            return 0;
        }
    }
    else if((nInputData & 0xf0) == 0xf0)  //bit(7-0): 1111 0xxx
    {
        if((nInputData & 0x04) == 0x04) // bit(7-0): 1111 01xx
            return 4;
        else// bit(7-0): 1111 00xx
            return 0;
    }
    return 0;

}
static BOOL gCfwSatIndFormate = CFW_SAT_IND_TEXT;
BOOL CFW_GetSATIndFormate(VOID)
{
    return gCfwSatIndFormate;
}
VOID CFW_SetSATIndFormate(BOOL bMode)
{
    gCfwSatIndFormate = bMode;
}

typedef struct _NW_STORELIST_INFO
{
    CFW_StoredPlmnList p_StoreList[CFW_SIM_COUNT];
} CFW_STORELIST_INFO;

VOID _SAT_SendSimOpen(CFW_SIM_ID nSimID)
{
    CFW_SIM_STATUS sSimStatus = CFW_SIM_STATUS_END;
    sSimStatus = CFW_GetSimStatus(nSimID);
    if (CFW_SIM_ABSENT == sSimStatus)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("_SAT_SendSimOpen NO SIM  simid %d\n",0x08100e7e)), nSimID);

        // CFW_PostNotifyEvent(EV_CFW_SET_COMM_RSP, ERR_CME_SIM_NOT_INSERTED, 0, nUTI|(nSimID<<24), 0xF0);
        // CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        // hAo = HNULL;
        // CSW_PROFILE_FUNCTION_EXIT(CFW_SetFlightModeProc);
        // return ERR_SUCCESS;
    }
    else
    {
        CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
        CFW_SIM_ID s = CFW_SIM_0;
        for (; s < CFW_SIM_COUNT; s++)
        {
            CFW_CfgGetStoredPlmnList(&pStoreList->p_StoreList[s], s);
        }
        if (ERR_SUCCESS != CFW_SendSclMessage( API_SIM_OPEN_IND, pStoreList, nSimID ))
        {
            return;
        }
    }
}

BOOL gSatFreshComm[CFW_SIM_COUNT] = {0,};
extern UINT32 gSatPollTimer;

UINT32 _ResetSimTimer(UINT8 nTimeInterval, UINT8 nTimeUnit, CFW_SIM_ID nSimID)
{
    //    UINT32 nPollTimer = 0x00;

    if(nTimeUnit == 0x00)               gSatPollTimer = 600;
    else if(nTimeUnit == 0x01)          gSatPollTimer = 10;
    else if(nTimeUnit == 0x02)          gSatPollTimer = 1;

    gSatPollTimer *= nTimeInterval;
    //    gSatPollTimer  = gSatPollTimer * 10;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("we reset timer %d\n",0x08100e7f), gSatPollTimer);

    api_SimPollReq_t *p = NULL;

    p = (api_SimPollReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimPollReq_t));
    if (p == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100e80));
        CSW_PROFILE_FUNCTION_EXIT(SimReadBinaryReq);
        return ERR_NO_MORE_MEMORY;
    }
    p->Duration = gSatPollTimer;

    return CFW_SendSclMessage(API_SIM_POLL_REQ, p, nSimID);
}

UINT8 CFW_GetCurrentCmdNum(CFW_SIM_ID nSimID)
{
    SIM_SAT_PARAM *pG_SimSat = NULL;
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_GetCurrentCmdNum  Cmd 0x%x CmdNum %d,nSimID %d\n",0x08100e81),
              pG_SimSat->nCurCmd, nCommondNum[pG_SimSat->nCurCmd - 1][nSimID], nSimID);

    return nCommondNum[pG_SimSat->nCurCmd - 1][nSimID];
}

// Add by XP at 20130121 End;
UINT8 _GetTimerIdentifier(UINT8 *p)
{
    /*
    Byte(s) Description Length
    1   Timer identifier tag    1
    2   Length='01' 1
    3   Timer identifier    1
    */
    return *(p + 2);
}
UINT32 _GetTimerValue(UINT8 *p)
{
    /*
    Timer value

    Byte(s) Description Length
    1   Timer value tag 1
    2   Length='03' 1
    3 - 5   Timer value 3
    -   byte 3: hour; this byte is coded exactly in the same way as the hour field of the TP-Service-Centre-Time-Stamp in TS 23.040 [6].
    -   byte 4: minute; this byte is coded exactly in the same way as the minute field of the TP-Service-Centre-Time-Stamp in TS 23.040 [6].
    -   byte 5: second; this byte is coded exactly in the same way as the second field of the TP-Service-Centre-Time-Stamp in TS 23.040 [6].

    */
    UINT8 nHour =      *(p + 2);
    UINT8 nMinute =      *(p + 3);
    UINT8 nSecond =      *(p + 4);

    UINT32 nRet = ((nHour << 16 ) + (nMinute << 8) + nSecond);
    //    hal_HstSendEvent(nHour);
    //    hal_HstSendEvent(nMinute);
    //    hal_HstSendEvent(nSecond);
    //    hal_HstSendEvent(nRet);
    return nRet;
}

UINT32 _SATTimerProc(UINT8 TimerID)
{
    UINT8 *pTimerID = (UINT8 *)CSW_SIM_MALLOC(1);
    UINT8 nUTI = 0x00;
    CFW_SIM_ID nSimID ;
    if(PRV_CFW_STK_TM_ID1_TIMER_SIM1_ID > TimerID)
    {
        nSimID = CFW_SIM_0;
    }
    else
    {
        nSimID = CFW_SIM_1;
        TimerID -= 8;
    }
    TimerID -= PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID;
    TimerID++;
    *pTimerID = TimerID;

    CFW_GetFreeUTI(0, &nUTI);
    UINT32 nRet = CFW_SatResponse(0xD7, 0x00, 0x00, pTimerID, 1, nUTI, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("_SATTimerProc CFW_SatResponse [0x%x] \n",0x08100e82), nRet);
    return nRet;
}

UINT32 gSATTimerValue[8][CFW_SIM_COUNT] = {{0x00,},};
UINT32 _SetTimerExpirationDownloadData(UINT8 *pData, UINT32 nID, CFW_SIM_ID nSimID)
{
    *(pData + 8) = nID;
    *(pData + 11) = gSATTimerValue[nID - 1][nSimID] >> 16;
    *(pData + 12) = gSATTimerValue[nID - 1][nSimID] >> 8;
    *(pData + 13) = gSATTimerValue[nID - 1][nSimID] & 0xFF;
    return ERR_SUCCESS;
}
UINT32 CFW_SatResponseProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nReturnValue = 0;
    SIM_SAT_PARAM *pG_SimSat = NULL;
    CFW_SAT_RESPONSE *pSimSatResponseData = NULL;
    HAO hSimSatResponseAo = hAo;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SatResponseProc);
    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hSimSatResponseAo);
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);
    pSimSatResponseData = CFW_GetAoUserData(hSimSatResponseAo);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SatResponseProc nSimID[%d] CmdType[0x%x]\n",0x08100e83), nSimID,
              pSimSatResponseData->nSAT_CmdType);
    UINT32 nUTI    = 0;
    UINT32  nTransID = 0;
    CFW_GetUTI(hSimSatResponseAo, &nUTI);

    nTransID = nUTI | (nSimID << 24);
    switch (pSimSatResponseData->nSm_Resp_currStatus)
    {
        case SM_RESP_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                if ( SIM_SAT_SETUP_MENU_COM == pSimSatResponseData->nSAT_CmdType )  // response the envolope
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SatResponseProc:  sendSetupMenuRsp\n",0x08100e84));
                    //nReturnValue = sendSetupMenuRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    nReturnValue = sendCommonRsp(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if ( SIM_SAT_SELECT_ITEM_COM == pSimSatResponseData->nSAT_CmdType ) // Item selection
                {
                    nReturnValue = sendSelectItemRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SatResponseProc:  ItemID = %d, nReturnValue = %d\n",0x08100e85), \
                              pSimSatResponseData->nSAT_ItemID, nReturnValue);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if ( SIM_SAT_GET_INKEY_COM == pSimSatResponseData->nSAT_CmdType ) // get inkey
                {
                    nReturnValue = sendGetInKeyRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if ( SIM_SAT_GET_INPUT_COM == pSimSatResponseData->nSAT_CmdType ) // get input
                {
                    nReturnValue = sendGetInputRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if (SIM_SAT_POLL_INTERVAL_COM == pSimSatResponseData->nSAT_CmdType)
                {
                    nReturnValue = sendPollIntervalRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if( SIM_SAT_SEND_USSD_COM == pSimSatResponseData->nSAT_CmdType )
                {
                    nReturnValue = sendUSSDRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if( (SIM_SAT_DISPLAY_TEXT_COM == pSimSatResponseData->nSAT_CmdType)
                         || (SIM_SAT_CALL_SETUP_COM == pSimSatResponseData->nSAT_CmdType)
                         || (SIM_SAT_SEND_SMS_COM == pSimSatResponseData->nSAT_CmdType)
                         || (SIM_SAT_SEND_DTMF_COM == pSimSatResponseData->nSAT_CmdType))
                {
                    //For call setup and send DTMF, the processing is the same as DisplayText
                    //nReturnValue = sendDisplayTextRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    nReturnValue = sendCommonRsp(pSimSatResponseData, nSimID);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSimSatResponseData->nSAT_CmdType 0x%x:nReturnValue: 0x%x\n",0x08100e86), pSimSatResponseData->nSAT_CmdType, nReturnValue);

                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if ( SIM_SAT_PROVIDE_LOCAL_INFO_COM == pSimSatResponseData->nSAT_CmdType ) // location infomation
                {
                    nReturnValue = sendProvideLocalInfoRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                // else if(pSimSatResponseData->nSAT_CmdType == SIM_SAT_POWER_ON_CARD_COM)    // power on card
                // {
                // StackCom does not support!
                // }
                else if( SIM_SAT_GET_READER_STATUS_COM == pSimSatResponseData->nSAT_CmdType ) // Get Reader status
                {
                    nReturnValue = sendGetReadStatusRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if ( SIM_SAT_GET_CHANNEL_STATUS_COM == pSimSatResponseData->nSAT_CmdType ) // Get channel status
                {
                    nReturnValue = sendGetChannelStatusRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if(SIM_SAT_PLAY_TONE_COM == pSimSatResponseData->nSAT_CmdType)
                {
//                nReturnValue = sendPlayToneRsp(pSimSatResponseData, pG_SimSat, nSimID);
                    nReturnValue = sendCommonRsp(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if (0xFE == pSimSatResponseData->nSAT_CmdType) // menu selection PDU mode
                {
                    nReturnValue = sendEnvelopePDU(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else if (0xFF == pSimSatResponseData->nSAT_CmdType)        //Setup menu PDU mode
                {
                    nReturnValue = sendResponsePDU(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sendResponsePDU: nReturnValue = %d",0x08100e87), nReturnValue);
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                else if (SIM_SAT_SMSPP_DOWNLOAD == pSimSatResponseData->nSAT_CmdType ) // SMS Profile download data
                {
                    nReturnValue = sendSMSPPDownload(pSimSatResponseData, pG_SimSat, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else if(SIM_SAT_MENU_SELECTION == pSimSatResponseData->nSAT_CmdType ) // menu selection
                {
                    nReturnValue = sendMenuSelection(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else if ( SIM_SAT_CALL_CONTROL == pSimSatResponseData->nSAT_CmdType ) // menu selection
                {
                    nReturnValue = sendCallControl(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else if ( SIM_SAT_EVENT_DOWNLOAD == pSimSatResponseData->nSAT_CmdType ) // menu selection
                {
                    nReturnValue = sendEventDownload(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else if (SIM_SAT_TIMER_EXPIRATION == pSimSatResponseData->nSAT_CmdType ) // menu selection
                {
                    nReturnValue = sendTimerExpiration(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_EVENT_DOWNLOAD[0xD6] command OK!!! \n",0x08100e88));
                        pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                        pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALENVELOPE;
                        CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return ERR_SUCCESS;
                    }
                }
                else
                {
                    nReturnValue = sendCommonRsp(pSimSatResponseData, nSimID);
                    if( nReturnValue != ERR_SUCCESS)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                        return nReturnValue;
                    }
                }
                CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_TERMINALRESPONSE;
            }
            else
                CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_RESP_TERMINALRESPONSE:
        {
            if (pEvent->nEventId == API_SIM_TERMINALRESPONSE_CNF)
            {
                api_SimTerminalResponseCnf_t *pSimTerminalResponeCnf = (api_SimTerminalResponseCnf_t *)pEvent->nParam1;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("API_SIM_TERMINALRESPONSE_CNF Sw1[0x%x]Sw2[0x%x]\n",0x08100e89), pSimTerminalResponeCnf->Sw1,
                          pSimTerminalResponeCnf->Sw2);
                if (pSimTerminalResponeCnf->Sw1 == 0x91)
                {
                    if(TRUE != pSimSatResponseData->nSAT_InnerProcess)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, pSimTerminalResponeCnf->Sw1, nUTI | (nSimID << 24), 0);
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, pSimTerminalResponeCnf->Sw1,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), 0);
                    }
                    pSimSatResponseData->nSAT_InnerProcess = FALSE;
                    pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                    CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                    pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_FETCH;
                    return ERR_SUCCESS;
                }
                else if (pSimTerminalResponeCnf->Sw1 == 0x90)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("nSAT_CmdType: pSimTerminalResponeCnf->Sw1 == 0x90!\n",0x08100e8a));
                    CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, pSimTerminalResponeCnf->Sw1, nTransID, 0);
                    CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    UINT32 nErrorCode = 0x00;
                    nErrorCode = Sim_ParseSW1SW2(pSimTerminalResponeCnf->Sw1, pSimTerminalResponeCnf->Sw2, nSimID);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_TERMINALRESPONSE_CNF: Sw1 != 0x90\n",0x08100e8b));
                    CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nErrorCode, pSimTerminalResponeCnf->Sw1, nTransID, 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                    return nErrorCode;
                }
            }
            else
                CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_RESP_TERMINALENVELOPE:
        {
            if (pEvent->nEventId == API_SIM_TERMINALENVELOPE_CNF)
            {
                api_SimTerminalEnvelopeCnf_t *pSimTerminalEnvelopeCnf = (api_SimTerminalEnvelopeCnf_t *)pEvent->nParam1;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("TERMINALENVELOPE CNF: SW1 = 0x%02X, SW2=0x%02X",0x08100e8c)), pSimTerminalEnvelopeCnf->Sw1, pSimTerminalEnvelopeCnf->Sw2);
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("TERMINALENVELOPE CNF: nSAT_CmdType = %d pSimTerminalEnvelopeCnf->NbByte = %d",0x08100e8d)), pSimSatResponseData->nSAT_CmdType, pSimTerminalEnvelopeCnf->NbByte);
                if (pSimTerminalEnvelopeCnf->Sw1 == 0x91)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_SIM_TERMINALENVELOPE_CNF 0x91()\r\n",0x08100e8e)));
                    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimTerminalEnvelopeCnf->ResponseData, pSimTerminalEnvelopeCnf->NbByte, 16);
                    if((0xD4 == pSimSatResponseData->nSAT_CmdType) || (0xFE == pSimSatResponseData->nSAT_CmdType))
                    {
                        CFW_SAT_TERMINAL_ENVELOPE_CNF *pEnvelope = (CFW_SAT_TERMINAL_ENVELOPE_CNF *)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_TERMINAL_ENVELOPE_CNF));
                        if(pEnvelope == NULL)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Malloc failed\n",0x08100e8f));
                            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, ERR_NO_MORE_MEMORY, 0, nTransID, 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                            return ERR_NO_MORE_MEMORY;
                        }

                        pEnvelope->Sw1 = pSimTerminalEnvelopeCnf->Sw1;
                        pEnvelope->NbByte = pSimTerminalEnvelopeCnf->NbByte;
                        SUL_MemCopy8(pEnvelope->ResponseData, pSimTerminalEnvelopeCnf->ResponseData, pSimTerminalEnvelopeCnf->NbByte);
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, (UINT32)pEnvelope, nTransID, 0);
                    }
                    else
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, pSimTerminalEnvelopeCnf->Sw1, nTransID, 0);
                    pSimSatResponseData->nSm_Resp_prevStatus = pSimSatResponseData->nSm_Resp_currStatus;
                    pSimSatResponseData->nSm_Resp_currStatus = SM_RESP_FETCH;
                    CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                }
                else if(pSimTerminalEnvelopeCnf->Sw1 == 0x90)
                {
                    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimTerminalEnvelopeCnf->ResponseData, pSimTerminalEnvelopeCnf->NbByte, 16);
                    if((0xD4 == pSimSatResponseData->nSAT_CmdType) || (0xFE == pSimSatResponseData->nSAT_CmdType))
                    {
                        CFW_SAT_TERMINAL_ENVELOPE_CNF *p = (CFW_SAT_TERMINAL_ENVELOPE_CNF *)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_TERMINAL_ENVELOPE_CNF));
                        p->Sw1 = pSimTerminalEnvelopeCnf->Sw1;
                        p->NbByte = pSimTerminalEnvelopeCnf->NbByte;
                        SUL_MemCopy8(p->ResponseData, pSimTerminalEnvelopeCnf->ResponseData, pSimTerminalEnvelopeCnf->NbByte);
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, (UINT32)p, nTransID, 0);
                    }
                    else
                        CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, pSimSatResponseData->nSAT_CmdType, pSimTerminalEnvelopeCnf->Sw1, nTransID, 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                    return ERR_SUCCESS;
                }
                else
                {
                    UINT32 nErrorCode = 0x00;
                    nErrorCode = Sim_ParseSW1SW2(pSimTerminalEnvelopeCnf->Sw1, pSimTerminalEnvelopeCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nErrorCode, pSimTerminalEnvelopeCnf->Sw1,
                                        nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                    return nErrorCode;

                }
            }
            else
                CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_RESP_FETCH:
        {
            if (pEvent->nEventId == API_SIM_FETCH_IND)
            {
                nReturnValue = processFetchInd(pEvent, nUTI, nSimID);
                CFW_SetAoProcCode(hSimSatResponseAo, CFW_AO_PROC_CODE_CONSUMED);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatResponseAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                return nReturnValue;
            }
        }
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
    return ERR_SUCCESS;
}

UINT32 SendRefreshSclMsg(UINT8 nSAT_Status, CFW_SIM_ID nSimID)
{

    UINT8 nRUti = 0x00;
    SIM_SAT_PARAM *pG_SimSat = NULL;
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);

    CFW_GetFreeUTI(0, &nRUti);

    // #ifdef CFW_DUAL_SIM
    // CFW_SatResponse(pSimSatResponseData->nSAT_CmdType, 0x00, 0x00, NULL, 0x00, nRUti,nSimID);
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    SAT_BASE_RSP *pBase = (SAT_BASE_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pBase == NULL))
    {
        return ERR_SUCCESS;
    }

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;

    // SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, pSimTerminalResponesReq->NbByte);
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[SIM_SAT_FRESH_IND - 1][nSimID];


    // pSimTerminalResponesReq->TerminalRespData[ComType -1]   = 0x02;

    pSimTerminalResponesReq->TerminalRespData[ComType]   = 0x01;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pBase->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_CmdType    %x\n",0x08100e90), SIM_SAT_FRESH_IND);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier   %x\n",0x08100e91), pBase->nComQualifier);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status     %x\n",0x08100e92), nSAT_Status);

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID | C_DETAIL, (UINT8 *) pSimTerminalResponesReq, sizeof(api_SimTerminalResponseReq_t), 16);
    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID))
    {
        return ERR_SUCCESS;
    }

    return ERR_SUCCESS;
}

// static UINT32 nACTNUM = 0x00;
UINT32 gSatPollTimer = 30 * 16384;

//PCSTR TS_GetSATEventName(UINT32 nEvId);



UINT32 CFW_SatActivationProc(HAO hAo, CFW_EV *pEvent)
{

    CSW_PROFILE_FUNCTION_ENTER(CFW_SatActivationProc);

    HAO hSimSatActivateAo = hAo;
    CFW_SAT_RESPONSE *pSimSatActivateData = NULL;
    pSimSatActivateData = CFW_GetAoUserData(hSimSatActivateAo);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimSatActivateAo);

    SIM_SAT_PARAM *pG_SimSat = NULL;
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);
    UINT32 nUTI   = 0;
    CFW_GetUTI(hSimSatActivateAo, &nUTI);

    UINT32 nTransID = 0;
    nTransID = nUTI | (nSimID << 24);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimSatActivateData->nSm_Act_currStatus 0x%x\n",0x08100e93), pSimSatActivateData->nSm_Act_currStatus);
//    hal_HstSendEvent(0xAAAAAAB1);
//    hal_HstSendEvent(pSimSatActivateData->nSm_Act_currStatus);
    switch (pSimSatActivateData->nSm_Act_currStatus)
    {
        case SM_ACT_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                api_SimToolkitActivateReq_t *pSimToolkitActivate = NULL;
                pSimToolkitActivate = (api_SimToolkitActivateReq_t *)CFW_MemAllocMessageStructure
                                      (sizeof(api_SimToolkitActivateReq_t));
                if (pSimToolkitActivate == NULL)
                {
                    CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_NO_MORE_MEMORY, 0, nTransID, 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                    return ERR_NO_MORE_MEMORY;
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimSatActivateData->nSAT_Mode= 0x%x\n",0x08100e94), pSimSatActivateData->nSAT_Mode);

                if (pSimSatActivateData->nSAT_Mode == 0x00)
                {
                    CFW_PROFILE *pMeProfile;

                    // activate the fetch function
                    api_SimTerminalProfileReq_t *pSimTerminalProfile = NULL;

                    pSimToolkitActivate->Activate = TRUE;
                    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_STKACTIVATE_REQ, pSimToolkitActivate, nSimID))
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_SCL_QUEUE_FULL, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                        return ERR_SCL_QUEUE_FULL;
                    }

                    // ProfileDownload function
                    pSimTerminalProfile = (api_SimTerminalProfileReq_t *)CFW_MemAllocMessageStructure
                                          (sizeof(api_SimTerminalProfileReq_t));
                    if (pSimTerminalProfile == NULL)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_NO_MORE_MEMORY, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                        return ERR_NO_MORE_MEMORY;
                    }
                    CFW_SimGetMeProfile(&pMeProfile, nSimID);

                    pSimTerminalProfile->NbByte = pMeProfile->nSat_ProfileLen;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pMeProfile->nSat_ProfileLen = 0x%x\n",0x08100e95), pMeProfile->nSat_ProfileLen);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("<<<<API_SIM_TERMINALPROFILE_REQ>>>>\n",0x08100e96));

                    SUL_MemCopy8(pSimTerminalProfile->ProfileData, pMeProfile->pSat_ProfileData, pMeProfile->nSat_ProfileLen);
                    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_TERMINALPROFILE_REQ, pSimTerminalProfile, nSimID))
                    {
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_NO_MORE_MEMORY, 0, nTransID, 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                        return ERR_SCL_QUEUE_FULL;
                    }

                    CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_CONSUMED);
                    pSimSatActivateData->nSm_Act_prevStatus = pSimSatActivateData->nSm_Act_currStatus;
                    pSimSatActivateData->nSm_Act_currStatus = SM_ACT_TERMINALPROFILE;
                }
                else if (pSimSatActivateData->nSAT_Mode == 0x01)
                {
                    pSimToolkitActivate->Activate = TRUE;
                    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_STKACTIVATE_REQ, pSimToolkitActivate, nSimID))
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_SCL_QUEUE_FULL, 0, nTransID, 0xF0);
                    else
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, 0, 0, nTransID, 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                }
                else if (pSimSatActivateData->nSAT_Mode == 0x02)
                {
                    pSimToolkitActivate->Activate = FALSE;
                    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_STKACTIVATE_REQ, pSimToolkitActivate, nSimID))
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_SCL_QUEUE_FULL, 0, nTransID, 0xF0);
                    else
                        CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, 0, 0, nTransID, 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                }
            }
            else
                CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;
        case SM_ACT_TERMINALPROFILE:
        {
            if (pEvent->nEventId == API_SIM_TERMINALPROFILE_CNF)
            {
                api_SimTerminalProfileCnf_t *pSimTerminalProfileCnf = (api_SimTerminalProfileCnf_t *)pEvent->nParam1;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("STK:API_SIM_TERMINALPROFILE_CNF sw1=%x,sw2=%x\n",0x08100e97), pSimTerminalProfileCnf->Sw1, pSimTerminalProfileCnf->Sw2);

                if (pSimTerminalProfileCnf->Sw1 == 0x91)
                {
                    CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, 0, pSimTerminalProfileCnf->Sw1, nTransID, 0);
                    CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_CONSUMED);
                    pSimSatActivateData->nSm_Act_prevStatus = pSimSatActivateData->nSm_Act_currStatus;
                    pSimSatActivateData->nSm_Act_currStatus = SM_ACT_FETCH;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("STK:CFW_PostNotifyEvent: EV_CFW_SAT_ACTIVATION_RSP\n",0x08100e98));
                }
                else
                {
                    UINT32 nErrorCode = 0;

                    if (pSimTerminalProfileCnf->Sw1 != 0x90)
                    {
                        nErrorCode = Sim_ParseSW1SW2(pSimTerminalProfileCnf->Sw1, pSimTerminalProfileCnf->Sw2, nSimID);
                    }
                    else
                        nErrorCode = ERR_CME_SIM_PROFILE_ERROR;
                    CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, nErrorCode, pSimTerminalProfileCnf->Sw1, nTransID, 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                    return nErrorCode;
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("API_SIM_TERMINALPROFILE_CNF pSimSatActivateData->nSm_Act_currStatus = %d\n",0x08100e99), pSimSatActivateData->nSm_Act_currStatus);
            }
            else
                CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_ACT_FETCH:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("<<<SM_ACT_FETCH>>>",0x08100e9a));
            if (pEvent->nEventId == API_SIM_FETCH_IND)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("<<<API_SIM_FETCH_IND>>>",0x08100e9b));
                UINT32 nReturnValue = processFetchInd(pEvent, nUTI, nSimID);
                CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_CONSUMED);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SatResponseProc);
                return nReturnValue;
            }
        }
        break;
        case SM_ACT_TERMINALRESPONSE:
        {
            if (pEvent->nEventId == API_SIM_TERMINALRESPONSE_CNF)
            {
                api_SimTerminalResponseCnf_t *pSimTerminalResponseCnf = (api_SimTerminalResponseCnf_t *)pEvent->nParam1;

                if ((pSimTerminalResponseCnf->Sw1 == 0x90) && (pSimTerminalResponseCnf->Sw2 == 0x00))
                {
                    CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, ERR_CME_SIM_PROFILE_ERROR, pSimTerminalResponseCnf->Sw1, nTransID, 0x00);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                    return ERR_CME_SIM_PROFILE_ERROR;
                }
                else if (pSimTerminalResponseCnf->Sw1 == 0x91)
                {

                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ERMINALPROFILE_CNF sw1 0x%x\n",0x08100e9c), pSimTerminalResponseCnf->Sw1);
                    pSimSatActivateData->nSm_Act_prevStatus = pSimSatActivateData->nSm_Act_currStatus;
                    pSimSatActivateData->nSm_Act_currStatus = SM_ACT_FETCH;
                }
                else
                {
                    UINT32 nErrorCode = 0x00;

                    nErrorCode = Sim_ParseSW1SW2(pSimTerminalResponseCnf->Sw1, pSimTerminalResponseCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SAT_ACTIVATION_RSP, nErrorCode, 0, nTransID, 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSatActivateAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
                    return nErrorCode;
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("API_SIM_TERMINALPROFILE_CNF status sw1 0x%x\n",0x08100e9d), pSimTerminalResponseCnf->Sw1);
            }
            else
                CFW_SetAoProcCode(hSimSatActivateAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SatActivationProc);
    return ERR_SUCCESS;
}

