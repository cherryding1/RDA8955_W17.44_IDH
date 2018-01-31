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


#include "at.h"
#include "at_sa.h"
#include "at_module.h"
#include "at_cmd_cc.h"
#include "at_cmd_ss.h"
#include "at_cmd_gc.h"
#include "at_cfg.h"
#include "at_utils.h"
#include "csw.h"
#include "cfw_prv.h"
#include "dm_audio.h"
#include "at_cmd_pbk.h"

extern UINT8 g_Pin1PukStauts[];
extern BOOL Play_music; // added by yangtt at 5-30-2008 for bug 8608
UINT8 g_cc_CHLD = 0;  // this is used to record whether the csw sent one or two rsp to AT.

// UINT8 g_cc_VTD = 1;
// extern UINT8 g_ucSsCallWaitingPresentResultCode;
// extern UINT8 g_uCmer_ind;

CFW_DIALNUMBER g_DailNumber_DLST; // [+]2007.11.09 for AT^DLST command
UINT8 g_DialNum_DLST[22] = { 0 };
UINT8 g_Dial_Presence    = 0;
extern UINT32 AT_SetCmeErrorCode(UINT32 nCfwErrorCode, BOOL bSetParamValid);  // hameina[+]2007012.3
extern VOID AT_AUDIO_DTMF_DetachStop();
extern BOOL Speeching;

UINT8 uExtensionNum[22] = {0,};
UINT8 uExtensionFlag = 0xff;
//
// 该函数会去掉最后的F，同时将A转化为‘*’将B转化为‘#’将C转化为‘p’
// --> 13811189836
//
UINT16 CSW_ATCcGsmBcdToAscii(UINT8 *pBcd, // input
                             UINT8 nBcdLen, UINT8 *pNumber  // output
                            )
{
    UINT8 i;
    UINT8 *pTmp  = pNumber;
    UINT16 nSize = 0; // nBcdLen*2;

    AT_TC(g_sw_AT_CC, "AT+CSW_ATCcGsmBcdToAscii:   nBcdLen = 0x%x, \n", nBcdLen);

    for (i = 0; i < nBcdLen; i++)
    {
        AT_TC(g_sw_AT_CC, "AT+CSW_ATCcGsmBcdToAscii:   pBcd[%d] = 0x%x, \n", i, pBcd[i]);

        if (pBcd[i] == 0xff)
            break;

        if ((pBcd[i] & 0x0F) == 0x0A)
        {
            *pTmp = '*';
        }
        else if ((pBcd[i] & 0x0F) == 0x0B)
        {
            *pTmp = '#';
        }
        else if ((pBcd[i] & 0x0F) == 0x0C)
        {
            *pTmp = 'p';
        }
        else if ((pBcd[i] & 0x0f) <= 9)
        {
            *pTmp = (pBcd[i] & 0x0F) + 0x30;
        }
        else
        {
            nSize = 1;
            *pTmp = 0;
            break;
        }

        pTmp++;

        if ((pBcd[i]) >> 4 == 0x0A)
        {
            *pTmp = '*';
        }
        else if ((pBcd[i]) >> 4 == 0x0B)
        {
            *pTmp = '#';
        }
        else if ((pBcd[i]) >> 4 == 0x0C)
        {
            *pTmp = 'p';
        }
        else if ((pBcd[i] >> 4) <= 9)
        {
            *pTmp = (pBcd[i] >> 4) + 0x30;
        }
        else
        {
            nSize = 1;
            *pTmp = 0;
            break;
        }

        pTmp++;

    }

    AT_TC(g_sw_AT_CC, "AT+CSW_ATCcGsmBcdToAscii: i=%d  nSize = 0x%x, \n", i, nSize);

    // [hameina[mod]2007.12.6 for bug 7166
    // nSize=(1+i)*2 -nSize;
    nSize = i * 2 + nSize;

    // ]]hameina[mod]2007.12.6 for bug 7166
    AT_TC(g_sw_AT_CC, "AT+CSW_ATCcGsmBcdToAscii: i=%d  nSize = 0x%x, \n", i, nSize);
    return nSize;
}

//
// GSMBCD码,如果转化奇数个数字的话，将会在对应高位补F
// 支持'*''#''p'
// 13811189836 --> 0x31 0x18 0x11 0x98 0x38 0xF6
//
UINT16 CSW_ATCcAsciiToGsmBcd(INT8 *pNumber, // input
                             UINT8 nNumberLen, UINT8 *pBCD  // output should >= nNumberLen/2+1
                            )
{
    UINT8 Tmp;
    UINT32 i;
    UINT32 nBcdSize = 0;
    UINT8 *pTmp     = pBCD;

    if (pNumber == (CONST INT8 *)NULL || pBCD == (UINT8 *)NULL)
        return FALSE;

    SUL_ZeroMemory8(pBCD, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        switch (*pNumber)
        {

        case '*':
            Tmp = (INT8)0x0A;
            break;

        case '#':
            Tmp = (INT8)0x0B;
            break;

        case 'p':
            Tmp = (INT8)0x0C;
            break;

        default:
            Tmp = (INT8)(*pNumber - '0');
            break;
        }

        if (i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (INT8)(Tmp & 0x0F);
        }

        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if (i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}

BOOL CSW_ATCcIsDTMF(INT8 iTone)
{
    if (((iTone >= '0') && (iTone <= '9')) || ((iTone >= 'A') && (iTone <= 'D')) || (iTone == '#') || (iTone == '*'))
        return TRUE;

    return FALSE;
}

VOID AT_CC_Result_OK(UINT32 uReturnValue,
                     UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    AT_BACKSIMID(nSim);
    // 填充结果码
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

VOID AT_CC_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    AT_BACKSIMID(nSim);
    // 填充结果码
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

UINT8 g_cc_ciev = 0;  // bit0: sounder, bit1:; call in progress
BOOL AT_CIND_GetCC(BOOL *pCall, BOOL *pSounder)
{
    *pSounder = g_cc_ciev & 1;
    *pCall    = (g_cc_ciev >> 1) & 1;
    return 1;
}

//#if defined(AT_USER_DBS) || defined(__MODEM_LP_MODE_ENABLE__) // ZSC
#define CC_STATE_NULL       0x00
#define CC_STATE_ACTIVE     0x01
#define CC_STATE_HOLD       0x02
#define CC_STATE_WAITING    0x04
#define CC_STATE_INCOMING   0x08
#define CC_STATE_DIALING    0x10
#define CC_STATE_ALERTLING  0x20
#define CC_STATE_RELEASE    0x40

UINT8 g_RingTimes = 0;

static void AT_RingTimerTimeout(void *param);
static void AT_SetRingTimer(UINT32 nElapse, UINT8 nSim)
{
    at_StartCallbackTimer(nElapse * 1000, AT_RingTimerTimeout, (void *)(long)nSim);
}

static void AT_KillRingTimer(UINT8 nSim)
{
    at_StopCallbackTimer(AT_RingTimerTimeout, (void *)(long)nSim);
}

static void AT_RingTimerTimeout(void *param)
{
    UINT8 nSim = (long)param;
    UINT32 uCState = CFW_CcGetCallStatus(nSim);
    UINT8 nS0Status = AT_GC_CfgGetS0Value();

    if ((CC_STATE_INCOMING == uCState) || (CC_STATE_WAITING == uCState))
    {
        if ((nS0Status == g_RingTimes) && (nS0Status != 0))
        {
            CFW_CcAcceptSpeechCall(nSim);
            return;
        }
        g_RingTimes++;
        at_CmdRingInd(nSim);
        AT_SetRingTimer(ATE_RING_TIME_ELAPSE, nSim);
    }
    return;
}

extern BOOL gATSATSetupCallProcessingFlag[NUMBER_OF_SIM];

UINT8 AT_CC_GetCCCount(UINT8 nSim)
{
    UINT8 nCnt = 0;
    CFW_CC_CURRENT_CALL_INFO CallInfo[AT_CC_MAX_NUM];

    CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
    return nCnt;
}

VOID AT_CC_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEv;
    CFW_EVENT *pEv = &CfwEv;
    UINT8 nClip = 0, nCssi = 0, nCssu = 0, nCcwa = 0;
    // UINT16 nMer = 0;
    UINT8 ind = 0;
    // UINT32 nRet = 0;
    UINT8 nSim = 0;
    AT_CosEvent2CfwEvent(pEvent, pEv);

    nSim = pEv->nFlag;

    // [[hameina[mod] 2008-04-14 :CSSI, CSSU的设置不起作用
    nClip = gATCurrentucClip;
    nCssi = gATCurrentucCSSI;
    nCssu = gATCurrentucCSSU;
    nCcwa = gATCurrentucSsCallWaitingPresentResultCode;
    ind   = gATCurrentuCmer_ind[nSim];

    // ]]hameina[mod] 2008-04-14 :CSSI, CSSU的设置不起作用
    AT_TC(g_sw_AT_CC, "AT_CC  nEventId = 0x%x, nType = 0x%x, nParam1 = 0x%x, nParam2 = 0x%x\n\r", pEvent->nEventId,
          pEv->nType, pEv->nParam1, pEv->nParam2);


    switch (pEvent->nEventId)
    {

    case EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP:
        // ---------------------------------------------------------
        // A Event
        // ---------------------------------------------------------
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP=0x%x\n", EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP);

        /* ATA may not from default AT channel,
         * so we shuld get command channel.
         */
        at_ModuleSetRI(TRUE);
        if (AT_isWaitingEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, nSim, pEv->nUTI))
        {
            pEv->nUTI = AT_GetDLCIFromWaitingEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, nSim);
            AT_DelWaitingEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP,nSim, pEv->nUTI);
        }
        pEvent->nParam1 = 0;

        AT_KillRingTimer(nSim);
        if (0x0f == pEv->nType) // success, and the call was connected
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_CONNECT, 0, 0, 0, pEv->nUTI, nSim);
            return;
        }
        else
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_ERROR, 0, 0, 0, pEv->nUTI, nSim);
            return;
        }

        break;

    case EV_CFW_CC_SPEECH_CALL_IND:
        // ---------------------------------------------------------
        // RING Event
        // ---------------------------------------------------------
#ifdef AT_SOUND_RECORDER_SUPPORT
        AT_AUDIO_StopAll();
#endif
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_SPEECH_CALL_IND=0x%x\n", EV_CFW_CC_SPEECH_CALL_IND);
        at_ModuleSetRI(FALSE);
#ifdef __MODEM_LP_MODE_ENABLE__
        sync_BpWakeUpAP();
#endif
        if (0 == pEv->nType)
        {
#ifdef AUTO_ANSWER
	     CFW_CcAcceptSpeechCall(nSim);
#endif
            CFW_SPEECH_CALL_IND *pSpeechCallInfo;
            UINT8 TelNum[42] = { 0 };
            UINT16 nNumLen   = 0;
            UINT8 OutStr[64] = { 0 };
            g_RingTimes = 1;
            SUL_ZeroMemory8(OutStr, 64);

            pSpeechCallInfo = (CFW_SPEECH_CALL_IND *)pEv->nParam1;
            nNumLen         = CSW_ATCcGsmBcdToAscii(pSpeechCallInfo->TelNumber.nTelNumber, pSpeechCallInfo->TelNumber.nSize, TelNum);

            // [[Changyg[mod] 2008-04-17 :the CFW_CfgGetClip will change the value of nClip
            // if(ERR_SUCCESS != CFW_CfgGetClip(&nClip))
            // return;
            // [[Changyg[mod] 2008-04-17 :delete it
            // [[ yy [mod] 2008-5-20 for bug ID:8473

            at_CmdRingInd(nSim);
            AT_SetRingTimer(ATE_RING_TIME_ELAPSE, nSim);

            // ]] yy [mod] 2008-5-20 for bug ID:8473
            AT_TC(g_sw_AT_CC, "pSpeechCallInfo->nDisplayType=%d\n", pSpeechCallInfo->nDisplayType);

            switch (pSpeechCallInfo->nDisplayType)
            {

            case 1:  // clip urc

                if (1 == nClip)
                {
                    SUL_StrPrint(OutStr, "+CLIP: \"%s\",%d,,,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            case 2:  // CSSI

                if ((1 == nCssi) && (pSpeechCallInfo->nCode != 0xff))
                {
                    SUL_StrPrint(OutStr, "+CSSI: %d", pSpeechCallInfo->nCode);
                }

                break;

            case 3:  // CSSU

                if ((1 == nCssu) && (pSpeechCallInfo->nCode != 0xff))
                {
                    AT_TC(g_sw_AT_CC, "+CSSU:^^^pSpeechCallInfo->nCode=%d\n", pSpeechCallInfo->nCode);
                    SUL_StrPrint(OutStr, "+CSSU: %d", pSpeechCallInfo->nCode);
                }

                break;

            case 4:  // CCWA

                if (1 == nCcwa)
                {
                    SUL_StrPrint(OutStr, "+CCWA: \"%s\",%d,1,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            case 5:  // CSSI+CCWA

                if ((1 == nCssi) && (pSpeechCallInfo->nCode != 0xff))
                {
                    SUL_StrPrint(OutStr, "+CSSI: %d", pSpeechCallInfo->nCode);
                    AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
                }

                if (1 == nCcwa)
                {
                    SUL_StrPrint(OutStr, "+CCWA: \"%s\",%d,1,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            case 6:  // CSSU+CCWA

                if ((1 == nCssu) && (pSpeechCallInfo->nCode != 0xff))
                {
                    SUL_StrPrint(OutStr, "+CSSU: %d", pSpeechCallInfo->nCode);
                    AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
                }

                if (1 == nCcwa)
                {
                    SUL_StrPrint(OutStr, "+CCWA: \"%s\",%d,1,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            case 7:  // CSSi+CLIP

                if ((1 == nCssi) && (pSpeechCallInfo->nCode != 0xff))
                {
                    SUL_StrPrint(OutStr, "+CSSI: %d", pSpeechCallInfo->nCode);
                    AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
                }

                if (1 == nClip)
                {
                    SUL_StrPrint(OutStr, "+CLIP: \"%s\",%d,,,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            case 8:  // CSSU+CLIP

                if ((1 == nCssu) && (pSpeechCallInfo->nCode != 0xff))
                {
                    SUL_StrPrint(OutStr, "+CSSU: %d", pSpeechCallInfo->nCode);
                    AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
                }

                if (1 == nClip)
                {
                    SUL_StrPrint(OutStr, "+CLIP: \"%s\",%d,,,,%d", TelNum, pSpeechCallInfo->TelNumber.nType,
                                 pSpeechCallInfo->nCLIValidity);
                }

                break;

            default:
                return;
            }
            AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
        }

        return;

    case EV_CFW_CC_INITIATE_SPEECH_CALL_RSP:
        // ---------------------------------------------------------
        // D Event
        // ---------------------------------------------------------
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_INITIATE_SPEECH_CALL_RSP=0x%x\n", EV_CFW_CC_INITIATE_SPEECH_CALL_RSP);

        if (0 == pEv->nType)
        {
            AT_TC(g_sw_AT_CC, "EV_CFW_CC_INITIATE_SPEECH_CALL_RSP uExtensionFlag =0x%x\n", uExtensionFlag);

            if( uExtensionFlag < 2)
            {
                AT_SetExtensionTimerMux( ATE_EXTENSION_TIME_ELAPSE, pEv->nUTI);
                uExtensionFlag = 0xff;
            }
            //AT_TC(g_sw_AT_CC, "EV_CFW_CC_INITIATE_SPEECH_CALL_RSP g_AtModuleMux[nDLCI].bExtensionTimer =0x%x\n", g_AtModuleMux[pEv->nUTI].bExtensionTimer);
            UINT8 uOutStr[22];

            SUL_ZeroMemory8(uOutStr, 22);
            SUL_StrPrint(uOutStr, "+CIEV: \"SOUNDER\",1");
            AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutStr, AT_StrLen(uOutStr), pEv->nUTI, nSim);

            if(gATCurrentucColp)
            {
                UINT8 OutStr[42];
                UINT8 uParaDialNum[22];
                SUL_ZeroMemory8(OutStr, 42);
                CSW_ATCcGsmBcdToAscii(g_DialNum_DLST,g_DailNumber_DLST.nDialNumberSize,(UINT8 *)uParaDialNum);
                SUL_StrPrint(OutStr, "+COLP: \"%s\",%d",uParaDialNum,g_DailNumber_DLST.nType);
                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
            }

            pEvent->nParam1 = 0;
            return;
        }
        else if (1 == pEv->nType)
        {
            CFW_TELNUMBER *uTelnum = (CFW_TELNUMBER *)pEv->nParam1;

            UINT8 nTelNum[TEL_NUMBER_MAX_LEN + 1];
            AT_TC(g_sw_AT_CC, "EV_CFW_CC_INITIATE_SPEECH_CALL_RSP 11 uExtensionFlag =0x%x\n", uExtensionFlag);
            if( uExtensionFlag < 2)
            {
                AT_SetExtensionTimerMux( ATE_EXTENSION_TIME_ELAPSE, pEv->nUTI);
                uExtensionFlag = 0xff;
            }
            //AT_TC(g_sw_AT_CC, "EV_CFW_CC_INITIATE_SPEECH_CALL_RSP 11 g_AtModuleMux[nDLCI].bExtensionTimer =0x%x\n", g_AtModuleMux[pEv->nUTI].bExtensionTimer);
            UINT16 nTellen = SUL_GsmBcdToAscii(uTelnum->nTelNumber, uTelnum->nSize, nTelNum);
            nTelNum[nTellen + 1] = 0;
            UINT8 nOutstr[60] = { 0, };
            SUL_ZeroMemory8(nOutstr, 60);
            SUL_StrPrint(nOutstr, "+COLP: \"%s\",%d", nTelNum, uTelnum->nType);
            AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nOutstr, AT_StrLen(nOutstr), pEv->nUTI, nSim);
            return;
        }
        else if (0x10 == pEv->nType)  // hameina[+] time up
        {
            AT_CC_Result_Err(ERR_AT_CME_NETWORK_TIMOUT, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
            //AT_StrCpy(g_pCeer, "+CEER: SET UP CALL FAIL, NETWORK TIME OUT");
            pEvent->nParam1 = 0;
            SUL_StrPrint(g_pCeer, "+CEER: %d", pEv->nParam1); //adapt to android
            return;
        }
        else if (0xF0 == pEv->nType)
        {
            // [[Hameina[+]2007.9.5: here we should check the value of ATX, then decide how to output the result code
            UINT32 RetTmp = 0;


            if (CFW_CC_CAUSE_USER_BUSY == pEv->nParam1)
            {
                RetTmp = CMD_RC_BUSY;
                //AT_StrCpy(g_pCeer, "+CEER: SET UP CALL FAIL, TERMINAL USER RETURN UDUB");
            }
            else if (CFW_CC_CAUSE_NO_USER_RESPONDING == pEv->nParam1)
            {
                RetTmp = CMD_RC_NODIALTONE;
                //AT_StrCpy(g_pCeer, "+CEER: SET UP CALL FAIL, TERMINAL USER HAS POWERED OFF");
            }
            else if (CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER == pEv->nParam1)
            {
                RetTmp = CMD_RC_NOANSWER;
                //AT_StrCpy(g_pCeer, "+CEER: SET UP CALL FAIL, TERMINAL USER DOESN'T ANSWER THE CALL");
            }
            else if (CFW_CC_CAUSE_NOUSED_CAUSE == pEv->nParam1)
            {
                RetTmp = CMD_RC_NOANSWER;
                //AT_StrCpy(g_pCeer, "+CEER: SET UP CALL FAIL, TERMINAL USER DOESN'T ANSWER THE CALL");

            }
            else
            {
#ifndef CHIP_HAS_AP
                RetTmp = CMD_RC_ERROR;
#else
                RetTmp = CMD_RC_NOCARRIER;
#endif

                //SUL_StrPrint(g_pCeer, "+CEER: SET UP CALL FAIL, CSW ERR CODE:0X%x", pEv->nParam1);
            }
            SUL_StrPrint(g_pCeer, "+CEER: %d", pEv->nParam1); //adapt to android by wulc
            pEvent->nParam1 = 0;
            AT_CC_Result_OK(CSW_IND_NOTIFICATION, RetTmp, 0, 0, 0, pEv->nUTI, nSim);

            // ]]hameina[+]
            return;
        }
        else
        {
            AT_Result_Err(pEv->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
            pEvent->nParam1 = 0;
            return;
        }

        break;

    case EV_CFW_CC_RELEASE_CALL_RSP:
    {
        // ---------------------------------------------------------
        // H Event, CHUP event
        // ---------------------------------------------------------
        UINT32 nRet = 0;
        CFW_CC_CURRENT_CALL_INFO CallInfo[7];
        UINT8 nCnt = 0;
        nRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_RELEASE_CALL_RSP=0x%x\n", EV_CFW_CC_RELEASE_CALL_RSP);
        AT_TC(g_sw_AT_CC, "nCnt=%d\n", nCnt);

        AT_ZERO_PARAM1(pEvent);
        at_ModuleSetRI(TRUE);
        if (0 == nCnt)
        {
            if ((0x10 == pEv->nType) || (0x20 == pEv->nType))
            {
                // [[hameina[+]2007.12.06 bug 7150
                if (ind)
                {
                    if (g_cc_ciev & 1)  // sounder is on
                    {
                        UINT8 uOutStr[22];

                        g_cc_ciev = g_cc_ciev & 0xfe;
                        SUL_ZeroMemory8(uOutStr, 22);
                        SUL_StrPrint(uOutStr, "+CIEV: \"SOUNDER\",0");
                        AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutStr, AT_StrLen(uOutStr), pEv->nUTI, nSim);
                    }
                }

                // ]]hameina[+]2007.12.06 bug 7150
                //AT_StrCpy(g_pCeer, "+CEER: CALL RELEASE, USER RELEASED NORMALLY");
                SUL_StrPrint(g_pCeer, "+CEER: %d", pEv->nParam2); //adapt to android

                if (AT_isWaitingEvent(EV_CFW_CC_RELEASE_CALL_RSP, nSim, pEv->nUTI))
                {
#ifdef CHIP_HAS_AP
                    UINT8 nDLCI = AT_GetDLCIFromWaitingEvent(EV_CFW_CC_RELEASE_CALL_RSP, nSim);
                    if (nDLCI > 0)
                        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDLCI, nSim);
#else
                    UINT8 nDLCI = 0;
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDLCI, nSim);
#endif

                    AT_DelWaitingEvent(EV_CFW_CC_RELEASE_CALL_RSP, nSim, nDLCI);
                    return;
                }
            }
            else if (0xF0 == pEv->nType)
            {
                SUL_StrPrint(g_pCeer, "+CEER: %d", pEv->nParam2);
                if (pEv->nParam2 == 0x10008)  // 建立链路过程中，想挂断电话会返回err
                {
                    pEv->nParam1 = ERR_AT_CME_OPERATION_NOT_ALLOWED;
                }
                AT_Result_Err(pEv->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
                return;
            }
            else
            {
                AT_Result_Err(pEv->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
                return;
            }
        }

        // [[hameina[+] 2007.12.06 for bug 7147
        else
        {
            UINT8 i = 0;

            for (i = 0; i < nCnt; i++)
            {
                AT_TC(g_sw_AT_CC, "release call rsp, status[%d]=%d, number[0]=0x%x, number[1]=0x%x\n", i, CallInfo[i].status,
                      CallInfo[i].dialNumber.nTelNumber[0], CallInfo[i].dialNumber.nTelNumber[1]);
                // AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI);
            }

        }

        // ]]hameina[+] 2007.12.06 for bug 7147
        break;
    }
    case EV_CFW_CC_RELEASE_CALL_IND:
        // ---------------------------------------------------------
        // remote call release Event
        // ---------------------------------------------------------
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_RELEASE_CALL_IND=0x%x, pEv->nParam2=0x%x\n", EV_CFW_CC_RELEASE_CALL_IND, pEv->nParam2);

        at_ModuleSetRI(TRUE);
        if (0 == pEv->nType)
        {
            UINT32 RetTmp = 0;

            if (pEv->nParam2 == CFW_CC_CAUSE_USER_BUSY)
            {
                RetTmp = CMD_RC_BUSY;
                //AT_StrCpy(g_pCeer, "+CEER: CALL RELEASED, NETWORK SENT UDUB TO ME");
            }
            else if (CFW_CC_CAUSE_NO_USER_RESPONDING == pEv->nParam2)
            {
                //AT_StrCpy(g_pCeer, "+CEER: CALL RELEASED, TERMINAL USER POWERED OFF");
                RetTmp = CMD_RC_NODIALTONE;
            }
            else if (CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER == pEv->nParam2)
            {
                //AT_StrCpy(g_pCeer, "+CEER: CALL RELEASED, TERMINAL USER DOESN'T ANSWER THE CALL");
                RetTmp = CMD_RC_NOANSWER;
            }
            else
            {
                //SUL_StrPrint(g_pCeer, "+CEER: CALL RELEASED, NETWORK RETURN CAUSE:0X%X", pEv->nParam2);
                RetTmp = CMD_RC_NOCARRIER;
            }

            SUL_StrPrint(g_pCeer, "+CEER: %d", pEv->nParam2); //adapt to android by wulc
            at_CmdReportUrcCode(nSim, RetTmp);
        }
        pEvent->nParam1 = 0;

        return;

    case EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP:
    {
        // ---------------------------------------------------------
        // CHLD Event
        // ---------------------------------------------------------
        if (g_cc_CHLD)
            g_cc_CHLD--;
        pEvent->nParam1 = 0;

        AT_TC(g_sw_AT_CC, "EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, g_cc_CHLD =%d, pEv->nType =0x%x\n", g_cc_CHLD, pEv->nType);

        if (AT_isWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, nSim, pEv->nUTI))
        {
            pEv->nUTI = AT_GetDLCIFromWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, nSim);
        }

        if ((0x00 == pEv->nType) || (0x01 == pEv->nType) || (0x0F == pEv->nType))
        {
            if (!g_cc_CHLD)
            {
                AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP,nSim, pEv->nUTI);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pEv->nUTI, nSim);
            }
            return;
        }
        else if (0x10 == pEv->nType)
        {
            AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP,nSim, pEv->nUTI);
            AT_CC_Result_Err(CMD_FUNC_SUCC, CMD_RC_ERROR, pEv->nUTI, nSim);
            return;
        }
        else
        {
            //AT_CC_Result_Err(pEv->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
            AT_DelWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP,nSim, pEv->nUTI);
            AT_CC_Result_Err(0, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
            return;
        }

        break;
    }
    case EV_CFW_CC_PROGRESS_IND:
    case EV_CFW_CC_CALL_INFO_IND:
        // ---------------------------------------------------------
        // CIEV(CIND) Events
        // ---------------------------------------------------------
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_CALL_INFO_IND=0x%x\n", EV_CFW_CC_CALL_INFO_IND);

        // --------------------------------------------------
        // CFW_CFG_CMER_IND(cmer)//This can get cind from cmer,
        // but how can i get the value of cmer?--
        // ------------------------------------------------------------

        //if (0 != ind) // +CIEV should be set to UART
        {
            UINT8 uOutStr[22];

            if (0 == pEv->nType)  // set sounder
            {
                SUL_ZeroMemory8(uOutStr, 22);
                SUL_StrPrint(uOutStr, "+CIEV: \"SOUNDER\",%d", pEv->nParam1);
                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutStr, AT_StrLen(uOutStr), pEv->nUTI, nSim);
            }
            if (1 == pEv->nType)  // call in progress
            {
                SUL_ZeroMemory8(uOutStr, 22);
                SUL_StrPrint(uOutStr, "+CIEV: \"CALL\",%d", pEv->nParam1);
                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutStr, AT_StrLen(uOutStr), pEv->nUTI, nSim);
            }
        }

        extern BOOL gATSATSetupCallFlag[];
        // [[hameina[+]2007-10-30 for CIND
        if (0 == pEv->nType)  // set sounder
        {
            g_cc_ciev = (g_cc_ciev & 0xfe) | pEv->nParam1;
            if(gATSATSetupCallFlag[pEv->nFlag])
            {
                if(FALSE == gATSATSetupCallProcessingFlag[pEv->nFlag])
                {
                    gATSATSetupCallProcessingFlag[pEv->nFlag] = TRUE;
                    CFW_SatResponse(0x10, 0x00, 0x00, NULL, 0x00, 12, pEv->nFlag);
                }
                //gATSATSetupCallFlag[pEv->nFlag] = FALSE;
            }
        }

        else if (1 == pEv->nType)  // call in progress
        {
            g_cc_ciev = (g_cc_ciev & 0xfd) | (pEv->nParam1 << 1);
            if((gATSATSetupCallFlag[pEv->nFlag]) && (pEv->nParam1 == 0))
            {
                if(FALSE == gATSATSetupCallProcessingFlag[pEv->nFlag])
                {
                    gATSATSetupCallProcessingFlag[pEv->nFlag] = TRUE;
                    CFW_SatResponse(0x10, 0x35, 0x00, NULL, 0x00, 12, pEv->nFlag);
                }

                // gATSATSetupCallFlag[pEv->nFlag] = FALSE;
            }
        }
        else
        {
            if(gATSATSetupCallFlag[pEv->nFlag])
            {
                CFW_SatResponse(0x10, 0x35, 0x00, NULL, 0x00, 12, pEv->nFlag);
                // gATSATSetupCallFlag[pEv->nFlag] = FALSE;
            }
        }
        
        pEvent->nParam1 = 0;
        // ]]hameina[+]2007-10-30 for CIND
        return;

    case EV_CFW_CC_CRSSINFO_IND:
        // ---------------------------------------------------------
        // CSSI & CSSU Events (CSSN)
        // ---------------------------------------------------------
        AT_TC(g_sw_AT_CC, "EV_CFW_CC_CRSSINFO_IND=0x%x\n", EV_CFW_CC_CRSSINFO_IND);

        if (0x11 == pEv->nType)
        {
            UINT8 OutStr[12] = { 0, };

            if ((1 == nCssi) && (pEv->nParam1 != 0xff))
            {
                SUL_StrPrint(OutStr, (CONST TCHAR *)"+CSSI: %d", pEv->nParam1);
                AT_TC(g_sw_AT_CC, "EV_CFW_CC_CRSSINFO_IND,  CSSI=%d\n", pEv->nParam1);
                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, (UINT16)AT_StrLen(OutStr), pEv->nUTI, nSim);
            }

            if ((1 == nCssu) && (0xff != pEv->nParam2))
            {
                SUL_StrPrint((TCHAR *)OutStr, (CONST TCHAR *)"+CSSU: %d", pEv->nParam2);
                AT_TC(g_sw_AT_CC, "EV_CFW_CC_CRSSINFO_IND,  CSSU=%d\n", pEv->nParam2);
                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
            }
        }
        pEvent->nParam1 = 0;

        return;

    case EV_DM_SPEECH_IND:
        AT_ZERO_PARAM1(pEvent);
        break;

    default:
        //AT_CC_Result_Err(pEv->nParam1, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
        AT_CC_Result_Err(0, CMD_ERROR_CODE_TYPE_CME, pEv->nUTI, nSim);
        break;
    }

    return;
}

VOID AT_CC_CmdFunc_A(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    // Check [in] Param
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        if (Play_music == TRUE) // added by yangtt at 5-30-2008 for bug 8608
            DM_Audio_StopStream();  // added by yangtt at 5-30-2008 for bug 8608
#ifdef AT_SOUND_RECORDER_SUPPORT
            AT_AUDIO_StopAll();
#endif
        if ((AT_CMD_EXE == pParam->iType) || (AT_CMD_SET == pParam->iType))
        {
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (0 != uParaCount)
                {
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                UINT32 nRet = CFW_CcGetCallStatus(nSim);
                AT_TC(g_sw_AT_CC, "CFW_CcGetCallStatus   nRet = 0x%x\n\r", nRet);
                // changyg[mod]2008-5-28, discussing with wein,modify the following code to fix 8072.
                // if it cause other bug, we will look for CSW to resolve this problem.
                // all we don't know why the code judge the nRet to only support the incoming call.
                if ((0x08 != nRet) && (0x04 != nRet))
                {
#ifdef CHIP_HAS_AP
                    AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
#else
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_ERROR, 0, 0, 0, pParam->nDLCI, nSim);
#endif

                    return;
                }
                nRet = CFW_CcAcceptSpeechCall(nSim);
                if (ERR_SUCCESS != nRet)
                {
                    AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    AT_AddWaitingEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, nSim, pParam->nDLCI);
                    AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    return;
}

VOID AT_CC_CmdFunc_CLCC(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 i = 0;
    UINT8 pTxtBuf[68];
    UINT8 dialnum[44];
    UINT16 numTxtlen = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_EXE:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (0 != uParaCount)
                {
                    AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                CFW_CC_CURRENT_CALL_INFO CallInfo[AT_CC_MAX_NUM];
                AT_MemSet(CallInfo, 0, AT_CC_MAX_NUM * sizeof(CFW_CC_CURRENT_CALL_INFO));
                UINT8 nCnt = 0;
                SUL_ZeroMemory8(CallInfo, AT_CC_MAX_NUM * sizeof(CFW_CC_CURRENT_CALL_INFO));
                UINT32 nRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
                if ((ERR_SUCCESS != nRet) && (ERR_CFW_NO_CALL_INPROGRESS != nRet))
                {
                    AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else if (ERR_CFW_NO_CALL_INPROGRESS == nRet)
                {
                    if (0 == nCnt)
                    {
                        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                        return;
                    }
                }
                else
                {
                    AT_TC(g_sw_AT_CC, "AT+CLCC:   nCnt = %d\n\r", nCnt);

                    if (0 == nCnt)
                    {
                        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {
                        for (i = 0; i < nCnt; i++)
                        {
                            SUL_ZeroMemory8(dialnum, 44);
                            SUL_ZeroMemory8(pTxtBuf, 68);

                            numTxtlen =
                                CSW_ATCcGsmBcdToAscii(CallInfo[i].dialNumber.nTelNumber, CallInfo[i].dialNumber.nSize, &dialnum[1]);
                            dialnum[0] = 34;
                            AT_TC(g_sw_AT_CC, "AT+CLCC:   dialnum[numTxtlen] = 0x%x, dialnum[numTxtlen-1] =0x%x\n", dialnum[numTxtlen],
                                  dialnum[numTxtlen - 1]);
                            dialnum[numTxtlen + 1] = 34;
                            dialnum[numTxtlen + 2] = '\0';
                            AT_TC(g_sw_AT_CC, "AT+CLCC:   pTxtBuf = 0x%x, numTxtlen =%d\n", pTxtBuf, numTxtlen);

                            SUL_StrPrint((pTxtBuf), "+CLCC: %d,%d,%d,%d,%d,%s,%d",
                                         (UINT8)(CallInfo[i].idx),
                                         (UINT8)(CallInfo[i].direction),
                                         (UINT8)(CallInfo[i].status),
                                         (UINT8)(CallInfo[i].mode),
                                         (UINT8)(CallInfo[i].multiparty), dialnum, (UINT8)(CallInfo[i].dialNumber.nType));

                            AT_TC(g_sw_AT_CC, "AT+CLCC:   AT_StrLen(pTxtBuf)= %d\n", AT_StrLen(pTxtBuf));
                            AT_CC_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pTxtBuf, AT_StrLen(pTxtBuf), pParam->nDLCI, nSim);
                            // return;
                        }
                    }
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
            }

            break;

        case AT_CMD_TEST:
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        default:
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    return;
}

VOID AT_CC_CmdFunc_O(AT_CMD_PARA *pParam)
{
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_RESUME, pParam->nDLCI);
}

UINT8 gEmergencyNum[][2] = {{0x11, 0xF2}, {0x19, 0xF1}};
UINT8 gEmergencyNumNoSim[][2] = {{0x11, 0xF2}, {0x19, 0xF1},{0x11, 0xF0},{0x11, 0xF9},{0x99, 0xF9}};

VOID AT_CC_CmdFunc_D(AT_CMD_PARA *pParam)
{

    INT32 iRetValue;
    UINT8 uParaCount;
    CFW_DIALNUMBER sDailNumber;
    UINT8 iBcdLen;
    UINT8 nClir = 0;
    UINT32 iRet;
    UINT8 uDialNum[22] = { 0, };
    UINT8 uParaDialNum[22] = { 0, };

    UINT8 uIndex = 0;
    UINT16 uLen = 22;
    UINT8 uOpType;
    UINT8 iLen;
    UINT32 uSimErr; // [changyg,new, for LD operation
    UINT16 uBCDlen; // LD OP
    CFW_SIM_PBK_ENTRY_INFO sEntryToBeAdded = { 0 }; // [changyg,new, for LD operation

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_AT_CC, "ATD");

    if (AT_GC_GetAudioTest() == FALSE)
    {
        AT_TC(g_sw_AT_CC, "ATD Line %d",__LINE__);
        AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_CC, "ATD Line %d",__LINE__);
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check parameter validity
    if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
    {
        AT_TC(g_sw_AT_CC, "ATD Line %d",__LINE__);
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    // check parameter count
    if (1 != uParaCount)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    // check command mode
    if ((pParam->iType != AT_CMD_EXE) && (pParam->iType != AT_CMD_SET))
    {
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    iRet = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, uParaDialNum, &uLen);
    AT_TC(g_sw_AT_CC, "ATD,uParaDialNum=%s\n\r", uParaDialNum);
    if (iRet != ERR_SUCCESS)
    {
        AT_TC(g_sw_AT_CC, "ATD Line %d iRet:0x%x",__LINE__,iRet);
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    // parse parameter

    uOpType = AT_CC_CmdFunc_D_ParaParse(pParam->pPara, uParaDialNum);

    AT_TC(g_sw_AT_CC, "ATD,uOpType=%d", uOpType);
#ifdef AT_SOUND_RECORDER_SUPPORT
        AT_AUDIO_StopAll();
#endif
    // execute
    switch (uOpType)
    {

    case AT_CMD_D_OPTYPE_VOICECALL:

        iLen = (UINT8)SUL_Strlen(uParaDialNum);//pParam->pPara-->uParaDialNum

        // changyg end

        if ('+' == pParam->pPara[0])
        {
            if (pParam->pPara[1] < '0' || pParam->pPara[1] > '9')
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        AT_TC(g_sw_AT_CC, "phone num = %s\n\r", uParaDialNum);//pParam->pPara-->uParaDialNum

        if (iLen > AT_CMD_D_ParaMaxLen)
            iLen = AT_CMD_D_ParaMaxLen;
        AT_TC(g_sw_AT_CC, "atd:pPara[iLen - 1] = 0x%x\n\r", pParam->pPara[iLen - 1]);
        if((pParam->pPara[iLen - 1] == 'I')||(pParam->pPara[iLen - 1] == 'i'))
        {
            if(pParam->pPara[iLen - 1] == 'I')
            {
                nClir =1;
            }
            else
            {
                nClir =2;
            }
            iLen--;
        }
        if ('+' == pParam->pPara[0])
        {
            iLen--;
            uBCDlen = CSW_ATCcAsciiToGsmBcd((INT8 *)(uParaDialNum + 1), iLen, uDialNum); // SUL_AsciiToGsmBcd  //pParam->pPara-->uParaDialNum
        }
        else if (('0' == pParam->pPara[0]) && ('0' == pParam->pPara[1]))
        {
            iLen -= 2;
            uBCDlen = CSW_ATCcAsciiToGsmBcd((INT8 *)(uParaDialNum + 2), iLen, uDialNum); // SUL_AsciiToGsmBcd  //pParam->pPara-->uParaDialNum
        }
        else
        {
            uBCDlen = CSW_ATCcAsciiToGsmBcd((INT8 *)(uParaDialNum), iLen, uDialNum); // SUL_AsciiToGsmBcd  //pParam->pPara-->uParaDialNum

        }
        // changyg[+]2008-6-4 limit the max length of dial number 20.
        // and SIM cann't save more than 20 numbers of the phone
        if (SIM_PBK_NUMBER_SIZE * 2 < iLen)
        {
            AT_CC_Result_Err(ERR_AT_CME_DAIL_STR_LONG, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (FALSE != uBCDlen)
        {
            sEntryToBeAdded.pNumber       = uDialNum;
            sEntryToBeAdded.nNumberSize   = uBCDlen;
            sEntryToBeAdded.iFullNameSize = 0;

            if (('+' == pParam->pPara[0]) || (('0' == pParam->pPara[0]) && ('0' == pParam->pPara[1])))
            {
                sEntryToBeAdded.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
            }
            else
            {
                sEntryToBeAdded.nType = CFW_TELNUMBER_TYPE_NATIONAL;
            }

            // gATCurrentnTempStorage = CFW_PBK_SIM_LAST_DIALLING;//Changyg [mod]2008.5.20 PBK requirement
            AT_TC_MEMBLOCK(g_sw_AT_CC, uDialNum, uBCDlen, 16);
            uSimErr = CFW_SimAddPbkEntry(CFW_PBK_SIM_LAST_DIALLING, &sEntryToBeAdded, pParam->nDLCI, nSim);

            AT_TC(g_sw_AT_CC, "LD operation = %x\n\r", uSimErr);
        }

        // ]]changyg[+] 2008.4.30 end

        sDailNumber.pDialNumber = uDialNum;

        iBcdLen = (iLen & 0x01) ? (iLen / 2 + 1) : (iLen / 2);

        iLen = iBcdLen;

        if(CFW_SIM_NORMAL != CFW_GetSimStatus( nSim))
        {
            for( UINT8 i = 0x00; i <  SIZEOF( gEmergencyNumNoSim ) / SIZEOF( gEmergencyNumNoSim[i]); i++ )
            {
                //if (iLen == 2 && sDailNumber.pDialNumber[0] == gEmergencyNum[i][0] && sDailNumber.pDialNumber[1] == gEmergencyNum[i][1])
                if(2 == iLen && (!SUL_MemCompare(sDailNumber.pDialNumber, gEmergencyNumNoSim[i], iLen)))
                {
                    // [[hameina[+] 2007-10-30:bug 6929
                    CFW_CC_CURRENT_CALL_INFO CallInfo[7];
                    UINT8 nCnt = 0;
                    iRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
                    while (nCnt)
                    {
                        if (!CallInfo[nCnt - 1].status) // status==0,active
                        {
                            CFW_CcCallHoldMultiparty(1, CallInfo[nCnt - 1].idx, nSim);
                        }

                        nCnt--;
                    }
                    COS_Sleep(1000);
                    iRet = CFW_CcEmcDial(sDailNumber.pDialNumber, iLen, nSim);
                    if (iRet != ERR_SUCCESS)
                    {
                        AT_TC(g_sw_AT_CC, "CFW_CcEmcDial     iRet = 0x%x\n\r", iRet);
                        AT_CC_Result_Err(iRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    g_DailNumber_DLST.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL; // unknown
                    g_DailNumber_DLST.nDialNumberSize = iLen;
                    g_DailNumber_DLST.nClir = gATCurrentucClir;
                    g_Dial_Presence = 1;
                    memset(g_DialNum_DLST, 0, 22);
                    memcpy(g_DialNum_DLST, sDailNumber.pDialNumber, iLen);
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
            }
        }
        else
        {
            for( UINT8 i = 0x00; i <  SIZEOF( gEmergencyNum ) / SIZEOF( gEmergencyNum[i]); i++ )
            {
                //if (iLen == 2 && sDailNumber.pDialNumber[0] == gEmergencyNum[i][0] && sDailNumber.pDialNumber[1] == gEmergencyNum[i][1])
                if(2 == iLen && (!SUL_MemCompare(sDailNumber.pDialNumber, gEmergencyNum[i], iLen)))
                {
                    // [[hameina[+] 2007-10-30:bug 6929
                    CFW_CC_CURRENT_CALL_INFO CallInfo[7];
                    UINT8 nCnt = 0;
                    iRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
                    while (nCnt)
                    {
                        if (!CallInfo[nCnt - 1].status) // status==0,active
                        {
                            CFW_CcCallHoldMultiparty(1, CallInfo[nCnt - 1].idx, nSim);
                        }

                        nCnt--;
                    }
                    COS_Sleep(1000);
                    iRet = CFW_CcEmcDial(sDailNumber.pDialNumber, iLen, nSim);
                    if (iRet != ERR_SUCCESS)
                    {
                        AT_TC(g_sw_AT_CC, "CFW_CcEmcDial     iRet = 0x%x\n\r", iRet);
                        AT_CC_Result_Err(iRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                    g_DailNumber_DLST.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL; // unknown
                    g_DailNumber_DLST.nDialNumberSize = iLen;
                    g_DailNumber_DLST.nClir = gATCurrentucClir;
                    g_Dial_Presence = 1;
                    memset(g_DialNum_DLST, 0, 22);
                    memcpy(g_DialNum_DLST, sDailNumber.pDialNumber, iLen);
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
            }
        }


        sDailNumber.nDialNumberSize = iLen;
        sDailNumber.nClir = gATCurrentucClir;
        if(nClir > 0)
        {
            sDailNumber.nClir = nClir;
        }
        AT_TC(g_sw_AT_CC, "atd:nClir=0x%x\n\r", sDailNumber.nClir);
        // [[Changyg[+]2007.11.26 for bug 8174
        if (1 == g_Pin1PukStauts[nSim])
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (('+' == pParam->pPara[0]) || (('0' == pParam->pPara[0]) && ('0' == pParam->pPara[1])))
        {
            sDailNumber.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            sDailNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }

        g_DailNumber_DLST.nType = sDailNumber.nType;
        g_DailNumber_DLST.nDialNumberSize = sDailNumber.nDialNumberSize;
        g_DailNumber_DLST.nClir = sDailNumber.nClir;
        g_Dial_Presence = 1;
        memset(g_DialNum_DLST, 0, 22);
        memcpy(g_DialNum_DLST, sDailNumber.pDialNumber, sDailNumber.nDialNumberSize);
        AT_TC(g_sw_AT_CC, "Number hex\n\r");
        AT_TC_MEMBLOCK(g_sw_AT_CC, sDailNumber.pDialNumber, sDailNumber.nDialNumberSize, 16);

        if(AT_IsSSString(uParaDialNum, nSim))
        {
            at_ss_parsing_string_req_struct sStringSTR;
            AT_TC(g_sw_AT_CC | g_sw_AT_SS, "CmdFunc_D This is ss operation!!\n\r");
            AT_MemZero(&sStringSTR, SIZEOF(at_ss_parsing_string_req_struct));
            AT_MemCpy(sStringSTR.input, uParaDialNum, uLen);
#ifndef  AT_WITHOUT_SS
            if(AT_SS_Adaption_ParseStringREQ(&sStringSTR, pParam->nDLCI, nSim))
            {
                AT_TC(g_sw_AT_CC | g_sw_AT_SS, "CmdFunc_D The operation is OK!!\n\r");
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
#endif
            {
                AT_TC(g_sw_AT_CC | g_sw_AT_SS, "CmdFunc_D The operation is fail!!\n\r");
                AT_CC_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            return ;
        }
        if(pgATFNDlist[nSim])
        {
            if( 1 == pgATFNDlist[nSim]->nFDNSatus )
            {
                if( FALSE == AT_FDN_CompareNumber(sDailNumber.pDialNumber, sDailNumber.nDialNumberSize, sDailNumber.nType,nSim) )
                {
                    AT_TC(g_sw_AT_CC, "CmdFunc_D Warning don't FDN record number!\n\r");
                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED); //adapt to android by wulc
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
        }
        iRet = CFW_CcInitiateSpeechCall(&sDailNumber, pParam->nDLCI, nSim);
        AT_TC(g_sw_AT_CC, "CFW_CcInitiateSpeechCall     iRet = 0x%x\n\r", iRet);
        if (iRet != ERR_SUCCESS)
        {
            if (ERR_CFW_INVALID_PARAMETER == iRet)
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else if (ERR_CFW_NOT_EXIST_FREE_UTI == iRet)
            {
                AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else if (ERR_CME_MEMORY_FULL == iRet)
            {
                AT_CC_Result_Err(ERR_AT_CMS_MEM_FULL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;

            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    case AT_CMD_D_OPTYPE_GPRSDIAL:
#if defined(CFW_PPP_SUPPORT)
        AT_PppProcess(pParam->nDLCI, nSim, uParaDialNum[0]);
#endif
        return;

    default:
        AT_CC_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

INT8 AT_CC_CmdFunc_D_ParaParse(UINT8 *pPara, UINT8 *pDialNum)
{
    UINT8 pParaStr[AT_CMD_D_ParaMaxLen] = { 0 ,};
    UINT8 pPwVar[5] = {'P', 'p', 'W', 'w', 0};
    UINT8 PWIndex = 0;
    UINT8 uDialNumLen = 0;
    UINT8 *pStr = NULL;

    AT_TC(g_sw_AT_CC, "D VOICECALL: pPara:%s\n\r", pPara);

    if(AT_StrLen(pPara) > AT_CMD_D_ParaMaxLen)
        return AT_CMD_D_OPTYPE_NOTSUPPORT;

    AT_MemCpy(pParaStr, pPara, AT_StrLen(pPara));
    pStr = pParaStr;
    // consider the priority here. voice should be the last one.

    if (!AT_MemCmp(pParaStr, AT_GPRS_DIALSTR1, AT_StrLen(AT_GPRS_DIALSTR1))
            || !AT_MemCmp(pParaStr, AT_GPRS_DIALSTR2, AT_StrLen(AT_GPRS_DIALSTR2))
            || !AT_MemCmp(pParaStr, AT_GPRS_DIALSTR3, AT_StrLen(AT_GPRS_DIALSTR3))
            || !AT_MemCmp(pParaStr, AT_GPRS_DIALSTR4, AT_StrLen(AT_GPRS_DIALSTR4))
            || !AT_MemCmp(pParaStr, AT_GPRS_DIALSTR5, AT_StrLen(AT_GPRS_DIALSTR5))
            || !AT_MemCmp(pParaStr, AT_GPRS_DIALSTR6, AT_StrLen(AT_GPRS_DIALSTR6)))
    {
        if (pParaStr[AT_StrLen(pPara)-1] == '#' && pParaStr[AT_StrLen(pPara)-3] == '*')
            pDialNum[0] = pParaStr[AT_StrLen(pPara)-2]-'0';
        else
            pDialNum[0] = 1;
        return AT_CMD_D_OPTYPE_GPRSDIAL;
    }
    else if ((pParaStr[0] >= '0' && pParaStr[0] <= '9')
             || ('+' == pParaStr[0] && pParaStr[1] >= '0' && pParaStr[2] <= '9') || (pParaStr[0] == '*'))
    {
        //AT_TC(g_sw_AT_CC, "D VOICECALL: 1111:pStr:%s,pPwVal:%s\n\r",pStr,pPwVar);
        AT_MemZero(uExtensionNum, 22);

        uExtensionFlag = 0xff;
        do
        {
            //AT_TC(g_sw_AT_CC, "D VOICECALL: DO:uDialNumLen:%d\n\r",uDialNumLen);
            //      uDialNumLen++;
            for(PWIndex = 0; PWIndex < 4; PWIndex++)
            {
                //AT_TC(g_sw_AT_CC, "D VOICECALL: DO:*pStr:%c,pPwVar[PWIndex]:%c\n\r",*pStr,pPwVar[PWIndex]);
                if(*pStr == pPwVar[PWIndex])
                {
                    uExtensionFlag = PWIndex;
                    strcpy(uExtensionNum, pStr);
                    AT_MemZero(pDialNum, 22);
                    strncpy(pDialNum, pParaStr, uDialNumLen);
                    AT_TC(g_sw_AT_CC, "D VOICECALL: uExtensionFlag:%d,uExtensionNum:%s,pDialNum:%s\n\r", uExtensionFlag, uExtensionNum, pDialNum);
                    return AT_CMD_D_OPTYPE_VOICECALL;
                }
            }
            uDialNumLen++;
        }
        while(*pStr++ != 0);
        return AT_CMD_D_OPTYPE_VOICECALL;
    }
    else
    {
        // ss etc. ref 3GPP 22.030
        return AT_CMD_D_OPTYPE_NOTSUPPORT;
    }

}

// ]] yy [mod] 2008-04-23 for command implementation reorgnization

// [+]for AT^DLST redial last call 2007.11.09
VOID AT_CC_CmdFunc_DLST(AT_CMD_PARA *pParam)
{
    UINT32 nRet = 0;
    // Check [in] Param
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_CC, "0");
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_EXE == pParam->iType)  // [mod]2007.11.20 because only exe command
    {
        // [+] 2008.6.26 fix bug 8882
        if (0 == g_Dial_Presence)
        {
            AT_TC(g_sw_AT_CC, "no dialnumber presence");
            AT_CC_Result_Err(ERR_AT_CMS_INVALID_TXT_CHAR, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // end

        g_DailNumber_DLST.pDialNumber = g_DialNum_DLST;

        if (g_DailNumber_DLST.pDialNumber[0] == 0x11 && g_DailNumber_DLST.pDialNumber[1] == 0xf2)
        {
            CFW_CC_CURRENT_CALL_INFO CallInfo[7];
            UINT8 nCnt = 0;
            nRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);
            while (nCnt)
            {
                if (!CallInfo[nCnt - 1].status) // status==0,active
                {
                    CFW_CcCallHoldMultiparty(1, CallInfo[nCnt - 1].idx, nSim);
                }

                nCnt--;
            }

            COS_Sleep(1000);
            nRet = CFW_CcEmcDial(g_DailNumber_DLST.pDialNumber, g_DailNumber_DLST.nDialNumberSize, nSim);
            if (nRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_CC, "CFW_CcEmcDial     nRet = 0x%x\n\r", nRet);
                AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        nRet = CFW_CcInitiateSpeechCall(&g_DailNumber_DLST, pParam->nDLCI, nSim);
        AT_TC(g_sw_AT_CC, "CFW_CcGetCallStatus   nRet = 0x%x\n\r", nRet);
        if (0x00 == nRet)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        if (nRet != ERR_SUCCESS)
        {
            AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

    }
    else
    {
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_CC_CmdFunc_H(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT32 nRet = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_CC, "0");
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        if (AT_CMD_EXE == pParam->iType)
        {
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_TC(g_sw_AT_CC, "1");
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (0 != uParaCount)
                {
                    AT_TC(g_sw_AT_CC, "2");
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                UINT32 uStat = CFW_CcGetCallStatus(nSim);
                AT_TC(g_sw_AT_CC, "CFW_CcGetCallStatus   uStat = 0x%x\n\r", uStat);
                if (0x00 == uStat)
                {
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    // [[zhangxb [+] temp, to be conformed

                       // SA_SetSysStatus(SA_ATCMD_ATH_RESULT, SA_ATCMD_ATH_OK);
                    return;
                }
                nRet = CFW_CcReleaseCall(nSim);
                AT_TC(g_sw_AT_CC, "CFW_CcReleaseCall() nRet = %d\n\r", nRet);
                if (ERR_SUCCESS != nRet)
                {
                    AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    AT_TC(g_sw_AT_CC, "3");

                    // [[hameina[Mod]2007.12.21 for bug 7261

                    if (uStat != 0x40)
                    {
                        g_cc_ciev = 0;  // changyg[+]2008.5.20 for bug 8497
                        AT_AddWaitingEvent(EV_CFW_CC_RELEASE_CALL_RSP, nSim, pParam->nDLCI);
                        AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        g_cc_ciev = 0;  // changyg[+]2008.5.20 for bug 8497
                        // [[zhangxb [+] temp, to be conformed
                        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);

                            //SA_SetSysStatus(SA_ATCMD_ATH_RESULT, SA_ATCMD_ATH_OK);

                        // [[zhangxb [+] temp, to be conformed
                    }

                    // ]]hameina[Mod]2007.12.21 for bug 7261
                }

                return;
            }
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    return;
}

VOID AT_CC_CmdFunc_VTD(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT16 uStrLen;
    UINT8 nOutputStr[16] = { 0, };
    UINT8 nToneDuration;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    SUL_ZeroMemory8(nOutputStr, 16);

    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            else
            {
                if (1 != uParaCount)
                {
                    AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nToneDuration, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if (!nToneDuration || nToneDuration > 10)
                {
                    AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                gATCurrentcc_VTD = nToneDuration;
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }

            break;
        case AT_CMD_TEST:
            // =?
            SUL_StrCat(nOutputStr, "+VTD: (1-10)");
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
            return;
        case AT_CMD_READ:
            // iRetValue = CFW_CfgGetToneDuration(&nToneDuration);
            nToneDuration = gATCurrentcc_VTD;
            SUL_StrPrint(nOutputStr, "+VTD: %d", nToneDuration);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
            return;
        default:
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    return;
}

// add by wulc 20121114 begin
VOID StopTone(UINT8 nDLCI)
{
    UINT8 nSim = AT_SIM_ID(nDLCI);
    AT_KillDtmfTimerMux(nDLCI);
    CFW_CcStopTone(nSim);
    DM_StopTone();
}

struct AT_PLAY_DTMF_CONTEXT
{
    AT_CMD_ENGINE_T *engine;
    uint8_t dtmf[AT_DTMF_STRING_LEN];
    uint8_t count;
    uint8_t duration;
    uint8_t pos;
    uint8_t sim;
};

static void AT_CC_PlayDTMF(void *param)
{
    struct AT_PLAY_DTMF_CONTEXT *ctx = (struct AT_PLAY_DTMF_CONTEXT *)param;

    if (ctx->pos >= ctx->count)
    {
        at_CmdRespOK(ctx->engine);
        at_free(ctx);
        return;
    }

    uint8_t tone = ctx->dtmf[ctx->pos++];
    uint32_t ret = CFW_CcPlayTone(at_DtmfTone2Char(tone), ctx->sim);
    if (ret != ERR_SUCCESS)
    {
        at_CmdRespCmeError(ctx->engine, AT_SetCmeErrorCode(ret, 1));
        at_free(ctx);
    }
    else if (!DM_PlayTone(tone, DM_TONE_m3dB, ctx->duration * 100, 0))
    {
        at_CmdRespCmeError(ctx->engine, ERR_AT_CME_EXE_FAIL);
        at_free(ctx);
    }
    else
    {
        at_StartCallbackTimer(ctx->duration * 100, AT_CC_PlayDTMF, ctx);
    }
}

// add by wulc 20121114 end
VOID AT_CC_CmdFunc_VTS(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1 && pParam->paramCount != 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT));

        bool paramok = true;
        const uint8_t *dtmf = at_ParamDtmf(pParam->params[0], &paramok);
        uint16_t duration = at_ParamDefUintInRange(pParam->params[1], gATCurrentcc_VTD, 1, 10, &paramok);
        if (!paramok || dtmf[0] == 0)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT));

        unsigned len = strlen(dtmf);
        if (len > AT_DTMF_STRING_LEN)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT));

        struct AT_PLAY_DTMF_CONTEXT *ctx = (struct AT_PLAY_DTMF_CONTEXT *)at_malloc(sizeof(*ctx));
        if (ctx == NULL)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));

        ctx->engine = pParam->engine;
        ctx->duration = duration;
        ctx->count = len;
        ctx->pos = 0;
        ctx->sim = AT_SIM_ID(pParam->nDLCI);
        for (int n = 0; n < len; n++)
        {
            int tone = at_DtmfChar2Tone(dtmf[n]);
            if (tone < 0)
            {
                at_free(ctx);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT));
            }
            ctx->dtmf[n] = tone;
        }

        at_TaskCallback(AT_CC_PlayDTMF, ctx);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        // =?
        static const uint8_t rsp[] = "+VTS: (0-9,*,#,A,B,C,D),(1-10)";
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

VOID AT_CC_CmdFunc_CHLD(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    UINT8 nCmd;
    UINT8 nIndex;
    UINT8 nOutputStr[28] = { 0, };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {
        // case AT_CMD_EXE:

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                UINT32 nRet = 0;
                UINT8 nCnt  = 0;
                UINT8 wait  = 0, active = 0, hold = 0;
                UINT8 loop  = 0;
                UINT8 sign  = 0;
                CFW_CC_CURRENT_CALL_INFO CallInfo[7];

                if (1 < uParaCount)
                {
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else if (!uParaCount) // just return ok
                {
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                AT_TC(g_sw_AT_CC, "AT+CHLD:   uData = %d", uData);

                if ((0 != uData) && (1 != uData) && (2 != uData) && (3 != uData) && ((11 > uData) || (17 < uData))
                        && ((21 > uData) || (27 < uData)))
                {
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if (10 < uData)
                {
                    nCmd   = uData / 10;
                    nIndex = uData % 10;
                }
                else
                {
                    nCmd   = uData;
                    nIndex = 0xFF;
                }
                nRet = CFW_CcGetCurrentCall(CallInfo, &nCnt, nSim);

                if (!nCnt)
                {
                    AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }

                for (loop = 0; loop < nCnt; loop++)
                {
                    if (CallInfo[loop].status == 0) // active
                        active++;
                    else if (CallInfo[loop].status == 5)  // waiting,incoming call  hung up by +CHLD=0
                        wait++;
                    else if (CallInfo[loop].status == 1)  // hold
                        hold++;

                    if (nIndex == CallInfo[loop].idx)
                    {
                        sign = 1;
                    }
                }

                AT_TC(g_sw_AT_CC, "AT+CHLD:   active = %d, wait=%d, hold=%d", active, wait, hold);

                g_cc_CHLD = 0;

                if (0xff == nIndex)
                {
                    if (0 == nCmd)
                        g_cc_CHLD = wait ? 1 : 0 + ((!wait) ? hold : 0);
                    else if (1 == nCmd)
                        g_cc_CHLD = active + ((hold + wait) ? 1 : 0);
                    else if (2 == nCmd)
                        g_cc_CHLD = (active ? 1 : 0) + ((hold + wait) ? 1 : 0);
                    else if (3 == nCmd)
                        g_cc_CHLD = (active && hold) ? 1 : 0;
                }
                else
                {
                    if (1 == nCmd)
                        g_cc_CHLD = sign ? 1 : 0;
                    else if (2 == nCmd)
                    {
                        AT_TC(g_sw_AT_CC, "AT+CHLD:   CallInfo[0].multiparty = %d", CallInfo[0].multiparty);
                        active    = (active && CallInfo[0].multiparty) ? 1 : active;
                        hold      = (hold && CallInfo[0].multiparty) ? 1 : hold;
                        g_cc_CHLD = active + hold + wait;
                        g_cc_CHLD = (g_cc_CHLD > 2) ? 1 : g_cc_CHLD;

                        // changyg[+]2008.6.20 due to network reason, we limit the split operation. reference bug8594
                        if ((1 == nCnt) || (!sign))
                        {
                            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }
                }

                if (0 == g_cc_CHLD)
                {
                    AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                /*
                                if (nCmd == 0) //release all hold call or set UDUB to a waiting call
                                {
                                    if (wait)
                                        g_cc_CHLD = 1;

                                    //[[hameina[mod] for bug 7008
                                    else if (hold)
                                        g_cc_CHLD = hold;
                                    else
                                    {
                                        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                        return;
                                    }

                                    //]]hameina[mod] for bug7008
                                }
                                else if (nCmd == 1)
                                {
                                    if (nIndex == 0xff)//release all active calls and accept an other call(hold or waiting, waiting first
                                    {
                                        if (wait || hold)
                                            g_cc_CHLD = 1 + active;
                                        else
                                        {
                                            //[[hameina[+]for bug 6947
                                            if (active)
                                                g_cc_CHLD = active;
                                            else
                                            {
                                                AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                                return;
                                            }

                                            //]]hameina[+]for bug 6947
                                        }
                                    }

                                    //[[hameina[+]2007_11_12 for bug7008
                                    else
                                    {
                                        UINT8 sign = 0;

                                        for (loop = 0; loop < nCnt; loop++)
                                        {
                                            if (nIndex == CallInfo[loop].idx)
                                            {
                                                sign = 1;
                                                break;
                                            }
                                        }

                                        if (!sign)
                                        {
                                            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                            return;
                                        }
                                        else
                                            g_cc_CHLD = 1;
                                    }

                                    //]]hameina[+]2007_11_12
                                }
                                else if (nCmd == 2)
                                {
                                    if (nIndex == 0xff)//place all active call on hold and accept the other call.
                                    {
                                        //[[hameina[mod] 2007.12.6 fix bug 7143
                                        UINT8 uCnt = 0;

                                        if (active)
                                            active = 1;

                                        if (hold)
                                            hold = 1;

                                        uCnt = hold + wait + active;

                                        if (uCnt == 2)
                                        {
                                            if (active)
                                            {
                                                g_cc_CHLD = 2;
                                            }
                                            else
                                            {
                                                g_cc_CHLD = 1;
                                            }
                                        }
                                        else if (uCnt == 1)
                                            g_cc_CHLD = 1;
                                        else
                                        {
                                            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                            return;
                                        }

                                        //]]hameina[mod] 2007.12.6 fix bug 7143
                                    }

                                    //[[hameina[+]2007_11_12 for bug7008
                                    else
                                    {
                                        UINT8 sign = 0;

                                        for (loop = 0; loop < nCnt; loop++)
                                        {
                                            if (nIndex == CallInfo[loop].idx)
                                            {
                                                sign = 1;
                                                break;
                                            }
                                        }

                                        if (!sign)
                                        {
                                            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                            return; //changyg[+] 2008.5.20 fix bug 8454
                                        }
                                        else
                                        {
                                            //[[hameina[mod] 2008.1.4 for bug 7295
                                            //g_cc_CHLD = 1;

                                            //[[Changyg[mod] 2008-04-28 :init the parameter
                                            active = wait = hold = 0;
                                            //[[Changyg[mod] 2008-04-28 :end

                                            //[[Hameina[+] 2008,4,14 for bug 8075;if multi-call, just count once.

                                            for (loop = 0; loop < nCnt; loop++)
                                            {
                                                if (CallInfo[loop].status == 0)//active
                                                {
                                                    if (CallInfo[loop].multiparty)
                                                        active = 1;
                                                    else
                                                        active ++;
                                                }
                                                else if (CallInfo[loop].status == 5)//waiting
                                                    wait++;
                                                else if (CallInfo[loop].status == 1) //hold
                                                {
                                                    if (CallInfo[loop].multiparty)
                                                        hold = 1;
                                                    else
                                                        hold ++;
                                                }
                                            }

                                            //]]Hameina[+] 2008,4,14 for bug 8075

                                            g_cc_CHLD = active + hold+wait;

                                            //changyg[+]2008.6.20 due to network reason, we limit the split operation. reference bug8594
                                            if (1==nCnt)
                                            {
                                                AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                                return;
                                            }

                                            if (g_cc_CHLD > 2)
                                                g_cc_CHLD = 1;

                                            //]]hameina[mod] 2008.1.4
                                        }
                                    }

                                    //]]hameina[+]2007_11_12
                                }
                                else if (nCmd == 3)//build multiparty
                                {
                                    if ((0 == hold) || (0 == active))//changyg[+] to fix bug 8091 and 8094
                                    {
                                        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                        return;
                                    }
                                    else if ((nCnt > 1) && hold)//hameina[+] the first part to fix bug 7287
                                        g_cc_CHLD = 1;
                                    else
                                    {
                                        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                                        return;
                                    }
                                }
                */
                nRet = CFW_CcCallHoldMultiparty(nCmd, nIndex, nSim);
                AT_TC(g_sw_AT_CC, "AT+CHLD:   nCmd = %d, nIndex=%x\n", nCmd, nIndex);
                AT_TC(g_sw_AT_CC, "AT+CHLD:   nRet = %d, g_cc_CHLD=%d\n", nRet, g_cc_CHLD);
                if (ERR_SUCCESS != nRet)
                {
                    if (ERR_CFW_INVALID_PARAMETER == nRet)
                    {
                        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                }
                else
                {
                    AT_AddWaitingEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, nSim, pParam->nDLCI);
                    AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }

            break;

        case AT_CMD_TEST:
            // =?
            SUL_ZeroMemory8(nOutputStr, 28);
            SUL_StrCopy(nOutputStr, "+CHLD: (0,1,1X,2,2X,3)");
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
            return;

        default:
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

    return;
}

// AT+CHUP挂断的是其他设备发起的呼叫???
VOID AT_CC_CmdFunc_CHUP(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_AT_CC, "AT_CC_CmdFunc_CHUP, pParam->iType=0x%x\n\r", pParam->iType);

    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        if (AT_CMD_EXE == pParam->iType)
        {
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (0 != uParaCount)
                {
                    AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                UINT32 nRet = CFW_CcReleaseCall(nSim);  // CFW_CfgGprsSetPdpCxt
                if (ERR_SUCCESS != nRet)
                {
                    AT_CC_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    AT_AddWaitingEvent(EV_CFW_CC_RELEASE_CALL_RSP, nSim, pParam->nDLCI);
                    AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }
        }
        else if (AT_CMD_TEST == pParam->iType)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }

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
VOID AT_CC_CmdFunc_CRC(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 uParamCnt;

    UINT16 uSize;
    UINT8 uMode;

#define STR_LEN 20

    UINT8 pOutbuff[STR_LEN] = { 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    switch (pParam->iType)
    {

    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCnt);

        if (uParamCnt > 1)
        {
            AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (FALSE == AT_Util_CheckValidateU8(pParam->pPara))
        {
            AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uMode, &uSize);
        if (ERR_SUCCESS != iResult)
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (uMode > 1)
        {
            AT_TC(g_sw_GPRS, "KEN :: +CRC set Mode invalide!!!");
            AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        gATCurrentCrc = uMode;
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        SUL_ZeroMemory8(pOutbuff, STR_LEN);
        SUL_StrPrint(pOutbuff, "+CRC: (0,1)");  // yy [mod] for Bug ID: 8193
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutbuff, AT_StrLen(pOutbuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        SUL_ZeroMemory8(pOutbuff, STR_LEN);
        SUL_StrPrint(pOutbuff, "+CRC: %d", gATCurrentCrc);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pOutbuff, AT_StrLen(pOutbuff), pParam->nDLCI, nSim);
        break;

    default:
        AT_CC_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

#ifdef AT_USER_DBS
VOID AT_CC_CmdFunc_SENDSOUND(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    BOOL nRet      = FALSE;
    INT32 iRet     = 0;
    UINT8 nType    = 0;
    UINT16 nTypSize = 0;
    UINT8 nIndex   = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_AT_CC, "AT+SENDSOUND:Start!\n");
    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:pParam NULL!\n");
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:GetParaCount!\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (1 != uParaCount)
            {
                AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:1 != uParaCount!\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            nIndex   = 0;
            nTypSize = 1;
            iRet     = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nType, &nTypSize);

            AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:nType:%d\n", nType);
#  if 0
            if (ERR_SUCCESS != iRet || (nType > 2))
            {
                AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:1GET TYPE FALSE!\n");
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
#endif
            switch (nType)
            {
            case DBS_DIAL_TONE:
                nRet = DM_PlayTone(19, 1, 60 * 1000, 9);

                // nRet = DM_PlayTone(24, 1, 3000,4);

                break;

            case DBS_DIAL_CONGESTION:
                // nRet = DM_PlayTone(23,DM_TONE_m15dB,60*1000,4);
                nRet = DM_StopTone();
                break;

            case DBS_DIAL_BUSY:
                // nRet = DM_PlayTone(17,DM_TONE_m15dB,60*1000,4);
                nRet = DM_PlayTone(17, 1, 60 * 1000, 9);
                break;

            case 3:
                // nRet = DM_PlayTone(17,DM_TONE_m15dB,60*1000,4);
                nRet = DM_StopTone();
                break;
            case 4:
                nRet = DM_PlayTone(22, 1, 60 * 1000, 9);

                // nRet = DM_PlayTone(24, 1, 3000,4);

                break;

            default:
                break;
            }

            if (TRUE == nRet)
            {
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }

            break;

        case AT_CMD_TEST:
            AT_CC_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        default:
            AT_CC_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;

        }
    }

    return;
}
#endif



VOID AT_CC_CmdFunc_VTSEX(AT_CMD_PARA *pParam)
{
    INT32 iRetValue     = 0x00;
    UINT8 uParaCount    = 0x00;
    //UINT8 nOutputStr[36]  = { 0x00, };
    //UINT16 nDuration      = 0x00;
    INT32 nRet          = 0x00;


    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_AT_CC, "AT_CC_CmdFunc_VTSEX\n");
    if (pParam == NULL)
    {
        AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
        {
            AT_TC(g_sw_AT_CC, "AT+SENDSOUND:ERROR:GetParaCount!\n");
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if(0x01 != uParaCount)
        {
            AT_TC(g_sw_AT_CC, "uParaCount %d\n", uParaCount);
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nTypSize = 1;
        UINT8 nType = 0x00;

        //INT32 iRet
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nType, &nTypSize);

        if(1 == nType)
        {
#ifdef CHIP_HAS_AP
            if(TRUE == Speeching)
#endif
            {
                iRetValue = DM_PlayTone(22, 1, 1 * 100, 6);
                COS_Sleep(300);
                DM_StopTone();
                COS_Sleep(700);
                iRetValue = DM_PlayTone(22, 1, 1 * 100, 6);
                COS_Sleep(300);
                DM_StopTone();
                AT_TC(g_sw_AT_CC, "AT_CC_CmdFunc_VTSEX iRetValue = %d\n", iRetValue);
                if (iRetValue)
                    nRet = 0;
                if (ERR_SUCCESS != nRet )
                {
                    AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    //StopTone(pParam->nDLCI);
                    AT_SetDtmfTimerMuxEX(5 * 1000, pParam->nDLCI);
                }
            }
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else if(2 == nType)
        {
#ifdef CHIP_HAS_AP
            if(TRUE == Speeching)
#endif
            {
                iRetValue = DM_PlayTone(22, 1, 1* 100, 6);
                COS_Sleep(300);
                DM_StopTone();
                COS_Sleep(700);
                iRetValue = DM_PlayTone(22, 1, 1* 100, 6);
                COS_Sleep(300);
                DM_StopTone();

                AT_TC(g_sw_AT_CC, "AT_CC_CmdFunc_VTSEX iRetValue = %d\n", iRetValue);
                if (iRetValue)
                    nRet = 0;
                if (ERR_SUCCESS != nRet )
                {
                    AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return ;
                }

            }
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_KillDtmfTimerMuxEX( pParam->nDLCI);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
    }
    return;
}


