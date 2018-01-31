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
#include "at_lp_ctrl.h"
#include "at_device.h"
#include "at_dispatch.h"
#include "at_module.h"
#include "at_cmd_gc.h"
#include "at_cfg.h"
#include "at_utils.h"
#include "csw.h"
#include "dm_audio.h"
#include "pal_gsm.h"
#include "l1s.h"

#define ERR_CFW_REPEAT_TRANSFER 0x10000011

// #define CFG_FILE_NAME "at_user_cfg.bin"

UINT8 g_pCeer[64] = {0};

UINT8 g_gc_pwroff = 0;        // hameina[+]for smso&cfun, moved to top of the file
UINT8 g_cfg_cfun[2] = {0, 0}; // switch on/off; 5:current status is on

#define BIT_SET(uint8_num, bit_seq) (((uint8_num) & (1 << (bit_seq))) ? (bit_seq) : 0)
#define TIM_ALARM_INDEX_MAX 15

extern UINT32 CFW_CfgNumRangeCheck(UINT32 nInput, UINT8 *pString);
extern BOOL AT_CIND_GetBattery(UINT8 *pBattchg);
extern BOOL AT_CIND_GetCC(BOOL *pCall, BOOL *pSounder);
extern BOOL AT_CIND_SMS(UINT8 *pNewMsg, UINT8 *pMemFull, UINT8 nSim);
extern BOOL AT_CIND_NetWork(UINT8 *pSignalQ, UINT8 *pRegistS, UINT8 *pRoamS, UINT8 nDLCI, UINT8 nSim);
extern BOOL CSW_ATCcIsDTMF(INT8 iTone);
extern BOOL CFW_EmodSaveIMEI(UINT8 *pImei, UINT8 imeiLen, UINT8 nSimID);
extern UINT32 _SetACLBStatus(UINT8 nACLBNWStatus);
extern BOOL hal_HstSendEvent(UINT32 ch);
extern UINT32 CFW_SimInit(BOOL bRstSim, CFW_SIM_ID nSimID);
extern VOID AT_Audio_Init(VOID);

#ifdef __IOT_SUPPORT__
//add for test IOT 2015-07-13
extern char *dev_test;
extern char *dev1_id;
extern char *dev2_id;
extern SOCKET edp_socketId;
extern VOID edp_GPIO_set(UINT8 nGpioID, UINT32 dur);
void set_pushData(UINT8 *pDtata, UINT16 len);
extern BOOL AT_KillEdpTestTimer(VOID);

//add end
#endif

VOID AT_GC_CPOF_CFUN_IND(CFW_EVENT *cfwEvent);

// extern INT32 fat_OEM2Unicode(UINT8* pszOEM, UINT16 iOLen, UINT8* pszUnicode, UINT16* piULen);

BOOL AT_GC_Init(VOID)
{
// DM may be busy, and AT_Audio_Init will delay stack init.
// Using event, most likely EV_DM_POWER_ON_IND will be handle before AT_Audio_Init.
#ifdef AT_SOUND_RECORDER_SUPPORT
    COS_TaskCallback(g_hAtTask, (COS_CALLBACK_FUNC_T)AT_Audio_Init, 0);
#endif
    return TRUE;
}

UINT32 AT_GC_CfgGetResultCodePresentMode(UINT8 *pMode)
{
    if (pMode == NULL)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }

    *pMode = gATCurrenteResultcodePresentation;

    return ERR_SUCCESS;
}

UINT32 AT_GC_CfgGetResultCodeFormatMode(UINT8 *pMode)
{
    if (pMode == NULL)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }

    // ZXB: [+]We have a bug on init CFG, here it's just a patch for a correct command V setting.
    if (gATCurrenteResultcodeFormat != 0 && gATCurrenteResultcodeFormat != 1)
        gATCurrenteResultcodeFormat = 1;

    // <<

    *pMode = gATCurrenteResultcodeFormat;

    return ERR_SUCCESS;
}

UINT32 AT_GC_CfgGetEchoMode(UINT8 *pMode)
{
    if (pMode == NULL)
    {
        return ERR_AT_CME_PARAM_INVALID;
    }

    *pMode = gATCurrenteCommandEchoMode;

    AT_TC(g_sw_UART, "AT_GC_CfgGetEchoMode pMode=%d", *pMode);
    return ERR_SUCCESS;
}

UINT8 AT_GC_CfgGetS0Value(VOID)
{
    return gATCurrentcc_s0;
}

UINT8 AT_GC_CfgGetS3Value(VOID)
{

    // *pValue = gATCurrentnS3;
    return gATCurrentnS3;
}

UINT8 AT_GC_CfgGetS4Value(VOID)
{
    return gATCurrentnS4;
}

UINT8 AT_GC_CfgGetS5Value(VOID)
{
    return gATCurrentnS5;
}

/*
   UINT32  AT_GC_CfgSetResultCodePresentMode(UINT8 nMode)
   {
   if ((nMode != GC_RESULTCODE_PRESENTATION_ENABLE)
   && (nMode != GC_RESULTCODE_PRESENTATION_DISABLE))
   {
   return ERR_AT_CME_PARAM_INVALID;
   }

   gATCurrenteResultcodePresentation = nMode;
   return ERR_SUCCESS;
   }

   UINT32  AT_GC_CfgSetResultCodeFormatMode( UINT8 nMode )
   {
   if ((nMode != GC_RESULTCODE_FORMAT_VERBOSE)
   && (nMode != GC_RESULTCODE_FORMAT_LIMITED))
   {
   return ERR_AT_CME_PARAM_INVALID;
   }

   gATCurrenteResultcodeFormat = nMode;
   return ERR_SUCCESS;
   }

   UINT32  AT_GC_CfgSetEchoMode( UINT8 nMode )
   {
   if ((nMode != GC_COMMANDECHO_MODE_ENABLE)
   && (nMode != GC_COMMANDECHO_MODE_DISABLE))
   {
   return ERR_AT_CME_PARAM_INVALID;
   }

   gATCurrenteCommandEchoMode = nMode;
   return ERR_SUCCESS;
   }
   */
#ifdef LTE_NBIOT_SUPPORT
UINT8 ready_for_cfun0 = 0;
#endif

VOID AT_GC_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT cfwEvent;
    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &cfwEvent);
    AT_TC(g_sw_GC, "AT_GC_AsyncEventProcess, EV_ID=0X%x, EV_TIM_ALRAM_IND=0x%x\n", cfwEvent.nEventId, EV_TIM_ALRAM_IND);
    nSim = cfwEvent.nFlag;
    switch (cfwEvent.nEventId)
    {
    case EV_CFW_SIM_CLOSE_RSP:
    {
        AT_TC(g_sw_GC, "=======EV_CFW_SIM_CLOSE_RSP=======");
        CFW_SimInit(TRUE, nSim);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfwEvent.nUTI, nSim);
        AT_TC(g_sw_GC, "=======SIM %d has been reseted!=======", nSim);
    }
    case EV_CFW_EXIT_IND:
    {
        // AT_GC_CPOF_CFUN_IND(&cfwEvent);
        AT_ZERO_PARAM1(pEvent);
#ifdef LTE_NBIOT_SUPPORT
        ready_for_cfun0 = 1;
        AT_TC(g_sw_GC, "ready_for_cfun0 : %d", ready_for_cfun0);
#endif
        break;
    }

    case EV_TIM_ALRAM_IND: // alarm IND
    {
        UINT8 OutStr[20];

        memset(OutStr, 0, 20);
        AT_TC(g_sw_GC, "AT_GC_AsyncEventProcess, 2, param1=0X%x, param2=0x%x, param3=0x%x\n",
              pEvent->nParam1, pEvent->nParam2, pEvent->nParam3);
        AT_Sprintf(OutStr, "+CALV: %u", pEvent->nParam3 >> 16);
        AT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), cfwEvent.nUTI, nSim);
    }

    default:
        break;
    }

    return;
}

UINT32 AT_CfgGetErrorReport(UINT8 *pLevel)
{
    *pLevel = gATCurrentuCmee;
    return (ERR_SUCCESS);
}

VOID AT_GC_CmdFunc_CMEE(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount > 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t cmee = at_ParamDefUintInRange(pParam->params[0], 0, 0, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        gATCurrentuCmee = cmee;
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+CMEE: (0-2)");
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        AT_Sprintf(rsp, "+CMEE: %u", gATCurrentuCmee);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

VOID AT_Cond_ResumeIrqCmd()
{

    pal_SetUsrVector(PAL_FINT_IRQ, (void (*)(u32))l1s_FrameInterrupt);
    pal_SetUsrVector(PAL_DSP_IRQ, (void (*)(u32))l1s_DspIrq);
    pal_SetUsrVectorMsk(PAL_DSP_IRQ, PAL_PCH_READY | PAL_EQUALIZATION_COMPLETE | PAL_USF_READY);
}
extern UINT32 CFW_NwStopStack(
    CFW_SIM_ID nSimID);

/*************************************************************
//AT+SETOOKMODE=1 : param1 OOKmode.
//AT+SETOOKMODE=0 : close OOKmode.
 *************************************************************/
VOID AT_GC_SETOOKMODE(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize = 0;
    UINT8 param1 = 0;
    extern VOID rfd_XcvSetXcvMode(bool chorfremod);  // 0 channel mode , 1 frequency mode
    extern VOID rfd_XcvSetOokPower(char power);      // 4 power level
    extern VOID rfd_XcvSetOokChannel(INT16 channel); // [0-163] max channel is 163
    extern VOID rfd_XcvSetOokData(bool Ookdata);     //Ookdata
    extern VOID rfd_XcvSetXcvTestMode(bool testmod); // 1 testmode, 0 normal mode
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount != 1))
    {
        AT_TC(g_sw_DFMS, "exe       in AT_DFMS_SETOOKMODE, parameter error\n");
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    uIndex = 0;
    uSize = SIZEOF(param1);
    iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &param1, &uSize);
    if (iResult != ERR_SUCCESS)
    {
        AT_TC(g_sw_DFMS, "exe       in AT_DFMS_SETOOKMODE, get parameter error1\n");
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    uIndex++;

    CFW_NwStopStack(0);
    CFW_NwStopStack(1);
    COS_Sleep(3000);
    // ×?3éatμ?μ÷ó?・?ê?
    if (param1)
    {

        {
            rfd_XcvSetXcvTestMode(1);
            rfd_XcvSetXcvMode(1);
            rfd_XcvSetOokPower(0);
            rfd_XcvSetOokChannel(1);
        }
        {
            for (int numook = 0; numook < 100; numook++)
            {
                rfd_XcvSetOokData(numook % 2);
                //sleep 125 us
            }
        }
    }
    else
    {

        AT_Cond_ResumeIrqCmd();

        extern PUBLIC UINT32 CFW_StackInit(
            CFW_SIM_ID nSimID);
        CFW_StackInit(0);
        CFW_StackInit(1);
        rfd_XcvSetXcvTestMode(0);
        rfd_XcvSetXcvMode(0);
        rfd_XcvSetOokPower(0);
    }
}
VOID AT_GC_CmdFunc_CEER(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_EXE:

            if (AT_StrLen(g_pCeer))
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, g_pCeer, AT_StrLen(g_pCeer), pParam->nDLCI, nSim);
            }
            else
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+CEER: 255", AT_StrLen("+CEER: 255"), pParam->nDLCI, nSim);
            }

            break;

        case AT_CMD_TEST:
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}

VOID AT_GC_CmdFunc_CMER(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 uMode = 0xff, uKey = 0xff, uDisp = 0xff, uInd = 0xff; // , ubf=0xff
    UINT8 iCount = 0;
    UINT16 uLen = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

            if (eResult != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (!iCount) // no parameter, just return ok
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }

            // get first param
            uLen = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uMode, &uLen);

            if (eResult != ERR_SUCCESS && eResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (eResult == ERR_SUCCESS && uMode != 3)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // second param
            uLen = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &uKey, &uLen);

            if (eResult != ERR_SUCCESS && eResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (eResult == ERR_SUCCESS && uKey != 0)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // third param
            uLen = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &uDisp, &uLen);

            if (eResult != ERR_SUCCESS && eResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (eResult == ERR_SUCCESS && uDisp != 0)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // fourth param
            uLen = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &uInd, &uLen);

            if (eResult != ERR_SUCCESS) // && eResult != ERR_AT_UTIL_CMD_PARA_NULL
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (uInd != 2 && uInd != 0)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                AT_TC(g_sw_SA, TSTXT("CMER We set gATCurrentuCmer_ind nSim[%d] to %x\n"), nSim, uInd);
                gATCurrentuCmer_ind[nSim] = uInd;
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }
        }

        case AT_CMD_TEST:
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+CMER: (3),(0),(0),(0,2)", AT_StrLen("+CMER: (3),(0),(0),(0,2)"),
                         pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
        {
            UINT8 uOutStr[24] = {
                0,
            };

            // +CMER: <mode>,<keyp>,<disp>,<ind>,<bfr>
            AT_Sprintf(uOutStr, "+CMER: 3,0,0,%u", gATCurrentuCmer_ind[nSim]); // added by yangtt at 2008-5-12
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
            break;
        }

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}

BOOL CFW_CheckPowerCommStatus(CFW_SIM_ID nSimID);

VOID AT_GC_CmdFunc_CFUN(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iCount = 0;
    UINT8 fun = 0, rst = 0;
    UINT16 len = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT32 nFM = 0;

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        if (TRUE == CFW_CheckPowerCommStatus(nSim))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
        if (eResult == ERR_SUCCESS)
        {
            if (iCount > 2)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else if (!iCount)
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }

            // get first param <fun>
            len = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &fun, &len);
#ifdef __IOT_SUPPORT__
            //add for test IOT,2015-07-13
            if (eResult == ERR_SUCCESS)

            {
                if (1 == fun) //set dev1
                {
                    dev_test = dev1_id;
                }
                else if (2 == fun) //set dev2
                {
                    dev_test = dev2_id;
                }
                else if (3 == fun) //go forward
                {
                    set_pushData("11111", 6);

                    test_main(4);
                }
                else if (4 == fun) //back off
                {

                    set_pushData("22222", 6);

                    test_main(4);
                }
                else if (0 == fun) // reset edp test
                {
                    AT_TC(g_sw_SA, TSTXT("####reset edp test\n"));

                    AT_KillEdpTestTimer();
                    CFW_TcpipSocketClose(edp_socketId);

                    //test_main(4);
                }
            }
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
//add end
#endif
            if (eResult == ERR_SUCCESS)
            {
#ifdef CHIP_HAS_AP
                if ((fun == 2) || (fun == 3) || (fun > 8)) // 8 for SIM reset
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (fun == 5 || fun == 6 || fun == 7) //to adapt android ,WCDMA- PREFER(7),GSM(5),WCDMA-ONLY(6)
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
#else
                if (fun != 0 && fun != 1)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

#endif
            }
            else if (eResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // get second param <rst>
            if (iCount > 1)
            {
                len = 1;
                eResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &rst, &len);

                if (eResult != ERR_SUCCESS || rst)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            switch (fun)
            {
            case 1:
                if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                g_cfg_cfun[nSim] = fun;
#ifdef LTE_NBIOT_SUPPORT
                ready_for_cfun0 = 0;
#endif
                if (CFW_ENABLE_COMM == nFM)
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                else
                {
                    CFW_SetComm(CFW_ENABLE_COMM, 0, pParam->nDLCI, nSim);
                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                break;

            case 0:
            case 4:
//if cfun=0, MM willl do detach by poweroff
#if 0
                // Modify for case 34.123-9.1
#ifndef __MULTI_RAT__
#ifdef CFW_GPRS_SUPPORT
                CFW_GprsAtt(0, pParam->nDLCI, nSim);
#endif
#endif
#endif
                if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                g_cfg_cfun[nSim] = fun;

                if (CFW_DISABLE_COMM == nFM)

                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                else
                {
                    CFW_SetComm(CFW_DISABLE_COMM, 0, (UINT16)pParam->nDLCI, nSim);
#ifdef CHIP_HAS_AP

                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 90, 0, 0, pParam->nDLCI, nSim);
#else
                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
#endif
                }
                break;

#ifdef CHIP_HAS_AP
            case 8:
            {
                UINT8 i = 0;
                for (i = 1; i < 8; i++) //MAX_CID_NUM
                {
                    UINT8 nGprsStatus = 0;
                    if (ERR_SUCCESS != CFW_GetGprsActState(i, &nGprsStatus, nSim))
                    {
                        AT_TC(g_sw_GC, "----------AT_GC_CmdFunc_CFUN-----3-----");
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                    if (nGprsStatus == CFW_GPRS_ACTIVED)
                    {
#if 0 //stack will deactive gprs,
                        CFW_GprsAct(CFW_GPRS_DEACTIVED, i, pParam->nDLCI, nSim);
#endif
                        CFW_ReleaseCID(i, nSim);
                    }
                }
                CFW_SimClose(pParam->nDLCI, nSim);
            }
            break;
#endif
            default:
                break;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    case AT_CMD_TEST:
    {
#ifdef CHIP_HAS_AP
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+CFUN: (0,1,4,5,6,7)", AT_StrLen("+CFUN: (0,1,4,5,6,7)"), pParam->nDLCI, nSim);
#else
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+CFUN: (0,1)", AT_StrLen("+CFUN: (0,1)"), pParam->nDLCI, nSim);
#endif
        return;
    }

    case AT_CMD_READ:
    {
        UINT8 OutStr[12];
        if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // err_code = CFW_CfgGetPhoneFunctionality(&cfw_para[0]);
        AT_Sprintf(OutStr, "+CFUN: %u", nFM);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, OutStr, AT_StrLen(OutStr), pParam->nDLCI, nSim);
        return;
    }

    default:
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    }
}

VOID AT_GC_CmdFunc_SFUN(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iCount = 0;
    UINT8 fun = 0, rst = 0;
    UINT16 len = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT32 nFM = 0;

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

        if (TRUE == CFW_CheckPowerCommStatus(nSim))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (eResult == ERR_SUCCESS)
        {
            if (iCount > 2)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else if (!iCount)
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }

            // get first param <fun>
            len = 1;

            eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &fun, &len);
            if (eResult == ERR_SUCCESS)
            {
#ifdef CHIP_HAS_AP
                if (fun != 0 && fun != 1 && fun != 4 && fun != 5 && fun != 6 && fun != 7)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (fun == 5 || fun == 6 || fun == 7) //to adapt android ,WCDMA- PREFER(7),GSM(5),WCDMA-ONLY(6)
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
#else
                if (fun != 0 && fun != 1)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

#endif
            }
            else if (eResult != ERR_AT_UTIL_CMD_PARA_NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // get second param <rst>
            if (iCount > 1)
            {
                len = 1;
                eResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &rst, &len);

                if (eResult != ERR_SUCCESS || rst)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            switch (fun)
            {
            case 1:
                if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                g_cfg_cfun[nSim] = fun;

                if (CFW_ENABLE_COMM == nFM)
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                else
                {
                    CFW_SetComm(CFW_ENABLE_COMM, 0, pParam->nDLCI, nSim);
                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                break;

            case 0:
            case 4:
// Modify for case 34.123-9.1
#ifndef __MULTI_RAT__
#ifdef CFW_GPRS_SUPPORT
#ifndef AT_WITHOUT_GPRS
                CFW_GprsAtt(0, pParam->nDLCI, nSim);
#endif
#endif
#endif
                if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                g_cfg_cfun[nSim] = fun;

                if (CFW_DISABLE_COMM == nFM)

                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
                else
                {
                    CFW_SetComm(CFW_DISABLE_COMM, 0, (UINT16)pParam->nDLCI, nSim);
#ifdef CHIP_HAS_AP

                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 90, 0, 0, pParam->nDLCI, nSim);
#else
                    AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
#endif
                }
                break;

            default:
                break;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    case AT_CMD_TEST:
    {
#ifdef CHIP_HAS_AP
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+SFUN: (0,1,4,5,6,7)", AT_StrLen("+SFUN: (0,1,4,5,6,7)"), pParam->nDLCI, nSim);
#else
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+SFUN: (0,1)", AT_StrLen("+SFUN: (0,1)"), pParam->nDLCI, nSim);
#endif
        return;
    }

    case AT_CMD_READ:
    {
        UINT8 OutStr[12];
        if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // err_code = CFW_CfgGetPhoneFunctionality(&cfw_para[0]);
        AT_Sprintf(OutStr, "+SFUN: %u", nFM);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, OutStr, AT_StrLen(OutStr), pParam->nDLCI, nSim);
        return;
    }

    default:
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    }
}

VOID AT_GC_CmdFunc_CSCS(AT_CMD_PARA *pParam)
{
    UINT8 arrRes[40];
    INT32 iRet = ERR_SUCCESS;
    UINT16 paraCount = 0;
    at_chset_t chset;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+CSCS: (\"GSM\",\"HEX\",\"PCCP936\",\"UCS2\")");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, (UINT8 *)&paraCount);

        if (iRet != ERR_SUCCESS || paraCount > 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (paraCount == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        UINT8 arrCharacterSet[15] = {0};
        UINT16 nParamLen = sizeof(arrCharacterSet);

        AT_MemZero(arrCharacterSet, sizeof(arrCharacterSet));
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, arrCharacterSet, &nParamLen);

        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        chset = cfg_GetTeChset();
        AT_StrUpr(arrCharacterSet);
        if (AT_StrCmp(arrCharacterSet, "GSM") == 0)
        {
            if (chset != cs_gsm)
                cfg_SetTeChset(cs_gsm);
        }
        else if (AT_StrCmp(arrCharacterSet, "HEX") == 0)
        {
            if (chset != cs_hex)
                cfg_SetTeChset(cs_hex);
        }
        else if (AT_StrCmp(arrCharacterSet, "PCCP936") == 0)
        {
            if (chset != cs_gbk)
                cfg_SetTeChset(cs_gbk);
        }
        else if (AT_StrCmp(arrCharacterSet, "UCS2") == 0)
        {
            if (chset != cs_ucs2)
                cfg_SetTeChset(cs_ucs2);
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_MemZero(arrRes, sizeof(arrRes));

        chset = cfg_GetTeChset();
        if (chset == cs_ucs2)
            AT_StrCpy(arrRes, "+CSCS: \"UCS2\"");
        else if (chset == cs_hex)
            AT_StrCpy(arrRes, "+CSCS: \"HEX\"");
        else if (chset == cs_gbk)
            AT_StrCpy(arrRes, "+CSCS: \"PCCP936\"");
        else if (chset == cs_gsm)
            AT_StrCpy(arrRes, "+CSCS: \"GSM\"");
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

// to check the time format yangtt at 08-4-22 begin
#define _U 0x01  // upper
#define _L 0x02  // lower
#define _D 0x04  // digit
#define _C 0x08  // cntrl
#define _P 0x10  // punct
#define _S 0x20  // white space (space/lf/tab)
#define _X 0x40  // hex digit
#define _SP 0x80 // hard space (0x20)

const unsigned char at_ctype[] = {
    _C, _C, _C, _C, _C, _C, _C, _C,                                       /* 0-7 */
    _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,              /* 8-15 */
    _C, _C, _C, _C, _C, _C, _C, _C,                                       /* 16-23 */
    _C, _C, _C, _C, _C, _C, _C, _C,                                       /* 24-31 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P,                                 /* 32-39 */
    _P, _P, _P, _P, _P, _P, _P, _P,                                       /* 40-47 */
    _D, _D, _D, _D, _D, _D, _D, _D,                                       /* 48-55 */
    _D, _D, _P, _P, _P, _P, _P, _P,                                       /* 56-63 */
    _P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,         /* 64-71 */
    _U, _U, _U, _U, _U, _U, _U, _U,                                       /* 72-79 */
    _U, _U, _U, _U, _U, _U, _U, _U,                                       /* 80-87 */
    _U, _U, _U, _P, _P, _P, _P, _P,                                       /* 88-95 */
    _P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,         /* 96-103 */
    _L, _L, _L, _L, _L, _L, _L, _L,                                       /* 104-111 */
    _L, _L, _L, _L, _L, _L, _L, _L,                                       /* 112-119 */
    _L, _L, _L, _P, _P, _P, _P, _C,                                       /* 120-127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       /* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       /* 144-159 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 160-175 */
    _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P,       /* 176-191 */
    _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U,       /* 192-207 */
    _U, _U, _U, _U, _U, _U, _U, _P, _U, _U, _U, _U, _U, _U, _U, _L,       /* 208-223 */
    _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L,       /* 224-239 */
    _L, _L, _L, _L, _L, _L, _L, _P, _L, _L, _L, _L, _L, _L, _L, _L};      /* 240-255 */

#define AT_isdigit(c) ((_AT_ismask(c) & (_D)) != 0)
#define _AT_ismask(x) (at_ctype[(int)(unsigned char)(x)])

// to check the time format  end  yangtt at 08-4-22
BOOL AT_TimeFormat_isAvailable(UINT8 *pString)
{
    UINT8 *p;
    UINT8 i, iLen;

    if (NULL == pString)
    {
        return FALSE;
    }

    p = pString;

    iLen = (UINT8)AT_StrLen(p);

    // "yy/mm/dd,hh:mm:ss"
    // 01234567890123456

    if (iLen > 17)
    {
        if ((p[17] != '+'))
        {
            if ((p[17] != '-'))
                return FALSE;
        }

        if ((p[2] != '/') || (p[5] != '/') || (p[8] != ',') || (p[11] != ':') || (p[14] != ':'))
            return FALSE;

        for (i = 0; i < iLen; i++)
        {
            if ((i == 2) || (i == 5) || (i == 8) || (i == 11) || (i == 14) || (i == 17))
            {
            }
            else
            {

                if (!(AT_isdigit(p[i])))
                    return FALSE;
            }
        }
    }
    else // // yangtt at 08-4-22 for bug  8222
    {
        if ((p[2] != '/') || (p[5] != '/') || (p[8] != ',') || (p[11] != ':') || (p[14] != ':'))
            return FALSE;

        for (i = 0; i < iLen; i++)
        {
            if ((i == 2) || (i == 5) || (i == 8) || (i == 11) || (i == 14))
            {
            }
            else
            {

                if (!(AT_isdigit(p[i])))
                    return FALSE;
            }
        }
    }

    return TRUE;
}

/****************************************************
 *******Set command sets the real time clock of the MT
 ****************************************************/
VOID AT_GC_CmdFunc_CCLK(AT_CMD_PARA *pParam)
{
    UINT8 iCount = 0;
    UINT8 cfw_para[24];
    UINT8 respond[80];
    UINT8 String[18];
    UINT16 len = 24;
    INT8 iTimeZone = 0;
    BOOL tmRet = 0;
    INT32 iResult;
    TM_FILETIME ft;
    TM_SYSTEMTIME st;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_MemSet(cfw_para, 0x00, 20);
    AT_MemSet(respond, 0x00, 80);

    AT_TC(g_sw_GC, "=======CCLK 111======,pParam->iType=%d\n", pParam->iType);

    if (pParam->pPara == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
        AT_TC(g_sw_GC, "=======CCLK,33 iResult is 0x%x, iCount=%d=====\n", iResult, iCount);

        if (iResult != ERR_SUCCESS || (iCount != 1))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (PVOID)cfw_para, &len);

        AT_TC(g_sw_GC, "=======CCLK,44 iResult is 0x%x, len=%d=====\n", iResult, len);

        if (ERR_SUCCESS != iResult || len == 0)
        {
            AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_Util_TrimAll(cfw_para);

        if (!AT_StrLen(cfw_para) || (AT_StrLen(cfw_para) > 20)) // amend yangtt 04-16 for bug 7974 //HAMEINA [+] add the last part
        {
            AT_TC(g_sw_GC, "=======CCLK,55 =====\n");
            AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // added by yangtt at 08-04-22 for 8074
        if (FALSE == AT_TimeFormat_isAvailable(cfw_para))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        memset(String, 0, 17);

        AT_StrNCpy(String, cfw_para, 17); // yangtt at 08-4-22 for bug 8074 8222
        tmRet = TM_FormatDateTimeEx(&ft, String);

        if (tmRet)
        {
            AT_TC(g_sw_GC, "=======CCLK 66!====ft=%ld==\n", ft);
            tmRet = TM_FileTimeToSystemTime(ft, &st);
            AT_TC(g_sw_GC, "=======CCLK 67!====st.uYear=%ld==\n", st.uYear);

            if (!tmRet)
            {
                AT_TC(g_sw_GC, "=======CCLK 77!======\n");
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

            tmRet = TM_SetSystemTime(&st);

            AT_TC(g_sw_GC, "=======CCLK, 88 execution is %d!======\n", tmRet);

            if (!tmRet)
            {
                AT_TC(g_sw_GC, "=======CCLK, 10 ======\n");
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            else
            {
                if (AT_StrLen(cfw_para) > 18)
                {
                    len = 1;

                    // [[hameina [mod]2007.12.07 verify timezone, for bug 7195

                    if (cfw_para[18] >= '0' && cfw_para[18] <= '9') // hameina[+] 2007.12.06:bug 7195
                    {
                        if (AT_StrLen(cfw_para) > 19)
                        {
                            if (cfw_para[19] < '0' || cfw_para[19] > '9') // hameina[mod]2007.12.11 for bug 7217 <= to <, >= to >
                            {
                                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                                return;
                            }
                        }

                        iTimeZone = atoi(&cfw_para[18]);

                        if (cfw_para[17] == '-')
                            iTimeZone = -iTimeZone;

                        AT_TC(g_sw_GC, "=======CCLK 99, get timezone=%d! zone str=%s\n", iTimeZone, &cfw_para[17]);

                        // [[hameina [+] 07-11-27 :bug 7124
                        if (iTimeZone > 13 || iTimeZone < -12) // yangtt at 2008-05-04 for bug 8222
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }

                        // ]]hameina [+] 07-11-27 :bug 7124
                        tmRet = TM_SetTimeZone(iTimeZone);

                        if (tmRet)
                            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                        else
                        {
                            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        }
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // [[hameina [mod]2007.12.07 verify timezone, for bug 7195
                }
                else
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }
            }
        }
        else
        {
            AT_TC(g_sw_GC, "=======CCLK, 100 ======\n");
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    case AT_CMD_TEST:
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:
        tmRet = TM_GetSystemTime(&st);

        if (!tmRet)
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        tmRet = TM_SystemTimeToFileTime(&st, &ft);

        if (!tmRet)
        {
            AT_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        tmRet = TM_FormatFileTimeToStringEx(ft, String);

        if (!tmRet)
        {
            AT_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iTimeZone = TM_GetTimeZone();

        if (iTimeZone >= 0)
        {
            AT_Sprintf(respond, "+CCLK: \"%s+%d\"", String, iTimeZone);
        }
        else
        {
            AT_Sprintf(respond, "+CCLK: \"%s%d\"", String, iTimeZone);
        }
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, respond, AT_StrLen(respond), pParam->nDLCI, nSim);
        break;

    default:
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    }

    return;
}

VOID AT_GC_CmdFunc_E(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iCount = 0;
    UINT8 atl_para;
    UINT8 cfw_para;
    UINT16 len = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_EXE)
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

        if (eResult == ERR_SUCCESS)
        {
            if ((iCount != 1) && (iCount != 0))
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else if (iCount == 0)
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                len = 1;

                if (ERR_SUCCESS == AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atl_para, &len))
                {
                    cfw_para = (UINT8)atl_para;

                    if (cfw_para > 1)
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    gATCurrenteCommandEchoMode = atl_para;

                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

/******************************************************************************************
Function    :   AT_GC_CmdFunc_V
Description     :   AT General Command
The setting of this parameter determines the contents of the header
and trailer transmitted with result codes and information responses.
It also determines whether result codes are transmitted in a numeric
form or an alphabetic (or "verbose") form.
The text portion of information responses is not affected by this setting.

Command format  :   ATV[<value>]
Called By       :   AT_RunCommand()
Data Accessed   :
Data Updated    :
Input       :   pParam
Output      :
Return      :   void
Others      :   modify by wangqunyang 20/03/2008
 *******************************************************************************************/
VOID AT_GC_CmdFunc_V(AT_CMD_PARA *pParam)
{

    INT32 eResult = 0;                             // Return result code
    UINT8 iCount = 0;                              // Parameters count
    UINT8 atl_para = GC_RESULTCODE_FORMAT_VERBOSE; /* according to itu-v.25ter:
                               The default value is: 1 */
    UINT16 len = 0;                                // parameters length
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        // /////////////////////////////////////////////////
        /* Judge the command type, it is execute command */
        // /////////////////////////////////////////////////
        if (pParam->iType == AT_CMD_EXE)
        {

            // Fill value in the "iCount" and return result
            eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

            if (eResult == ERR_SUCCESS)
            {

                if (iCount > 1) // iCount bigger than 1
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else if (iCount == 0) // iCount equal to 0
                {
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
                else // iCount equal to 1
                {

                    len = 1;

                    // Get parameters and fill with "atl_para" and "len"

                    if (ERR_SUCCESS == AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atl_para, &len))
                    {

                        // if (atl_para < 0 || atl_para > 1)
                        if (atl_para > 1)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }

                        gATCurrenteResultcodeFormat = atl_para;

                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }

            // /////////////////////////////////////////////////
            /* Judge the command type, it is non execute command */
            // /////////////////////////////////////////////////
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
}

/******************************************************************************************
Function    :   AT_GC_CmdFunc_Q
Description     :   AT General Command
The setting of this parameter determines whether or not the DCE
transmits result codes to the DTE

Command format  :   ATV[<value>]
Called By       :   AT_RunCommand()
Data Accessed   :
Data Updated    :
Input       :   pParam
Output      :
Return      :   void
Others      :   modify by wangqunyang 20/03/2008
 *******************************************************************************************/
VOID AT_GC_CmdFunc_Q(AT_CMD_PARA *pParam)
{

    INT32 eResult = 0;                                  // Return result code
    UINT8 iCount = 0;                                   // Parameters count
    UINT8 atl_para = GC_RESULTCODE_PRESENTATION_ENABLE; /* according to itu-v.25ter:
                               The default value is: 0 */
    UINT16 len = 0;                                     // parameters length
    /* The parameter is NULL, return */
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        // /////////////////////////////////////////////////
        /* Judge the command type, it is execute command */
        // /////////////////////////////////////////////////
        if (pParam->iType == AT_CMD_EXE)
        {

            // Fill value in the "iCount" and return result
            eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
            AT_TC(g_sw_GC, "=======iCount=======%d\n", iCount);

            if (eResult == ERR_SUCCESS)
            {

                if ((iCount != 1) && (iCount != 0))
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else if (iCount == 0)
                {

                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    len = 1;

                    if (ERR_SUCCESS == AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atl_para, &len))
                    {
                        if (atl_para > 1)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }

                        gATCurrenteResultcodePresentation = atl_para;
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }

            // /////////////////////////////////////////////////
            /* Judge the command type, it is non execute command */
            // /////////////////////////////////////////////////
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
}

VOID AT_GC_CmdFunc_CALA(AT_CMD_PARA *pParam)
{

    UINT8 alarm_index = 0;

    INT32 eResult;
    UINT8 iCount = 0;
    UINT8 i;
    UINT16 str_len;
    UINT8 str_temp = 0;
    UINT8 alarm_num = 0;
    TM_ALARM *alarm_list = NULL;
    TM_ALARM cfw_alarm;
    TM_FILETIME ft;
    CALA cala;
    UINT16 len = 0;
    UINT8 msg[36] = {0}; // hameina[mod] 2007.12.10 bug 7201: max text len is 32,30->36
    UINT8 String[24];
    UINT8 respond[80];
    UINT8 cfw_para[20];

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_MemSet(&cala, 0, sizeof(CALA));

    // AT_MemSet(&week, 0, sizeof(RECURR));

    AT_TC(g_sw_GC, "=======CALA 1======,pParam->iType=%d\n", pParam->iType);
    AT_MemSet(cfw_para, 0x00, 20);
    AT_MemSet(&cfw_alarm, 0x00, sizeof(TM_ALARM));
    AT_MemSet(respond, 0x00, 80);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
        AT_TC(g_sw_GC, "=======CALA 2======,iCount=%d, eResult=0x%x\n", iCount, eResult);

        if (eResult != ERR_SUCCESS || iCount < 1 || iCount > 5) // hameina[mod]2007.12.10 bug 7203 6->5
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        for (i = 0; i < 5; i++)
        {
            switch (i)
            {

            case 0: // time
                str_len = 20;
                eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (PVOID)(cala.time), &str_len);
                AT_TC(g_sw_GC, "=======CALA 3======,cala.time=%s, eResult=0x%x\n", cala.time, eResult);

                if (ERR_SUCCESS != eResult || !str_len)
                {
                    AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                str_temp = str_len;

                AT_Util_TrimAll(cala.time);
                break;

            case 1: // index

                if (iCount)
                {
                    len = 1;
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &(cala.index), &len);
                    AT_TC(g_sw_GC, "=======CALA 4======,cala.index=%d, eResult=0x%x\n", cala.index, eResult);

                    if (eResult == ERR_SUCCESS)
                    {
                        // cala.index = uIndex;
                        if ((cala.index > TIM_ALARM_INDEX_MAX) || (cala.index < 1))
                        {
                            AT_Result_Err(ERR_AT_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }

                    // [[hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult == ERR_AT_UTIL_CMD_PARA_NULL && iCount == 2)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // ]]hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult == ERR_AT_UTIL_CMD_PARA_NULL)
                    {
                        cala.index = 1;
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
                else
                    cala.index = 1;

                break;

            case 2: // type
                if (iCount > 1)
                {
                    len = 1;
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &(cala.type), &len);
                    AT_TC(g_sw_GC, "=======CALA 5======,cala.type=%d, eResult=0x%x\n", cala.type, eResult);

                    // added by yangtt at 2008-03-22 for bug 8074

                    if (cala.type != 0)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // [[hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    if (eResult == ERR_AT_UTIL_CMD_PARA_NULL && iCount == 3)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // ]]hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult == ERR_AT_UTIL_CMD_PARA_NULL)
                    {
                        cala.type = 0;
                    }
                    else if (eResult != ERR_SUCCESS)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
                else
                    cala.type = 0;

                break;

            case 3: // text to be displayed when the alarm ring
                if (iCount > 2)
                {
                    str_len = 36; // hameina 07-11-27 :bug 7133 32->36
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_STRING, (PVOID)(cala.text), &str_len);
                    AT_TC(g_sw_GC, "=======CALA 6======,cala.text=%s, eResult=0x%x\n", cala.text, eResult);

                    // [[hameina[+]2007.12.11 bug 7203:the last param can't be ignored.

                    if (eResult == ERR_AT_UTIL_CMD_PARA_NULL && iCount == 4)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // ]]hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult != ERR_SUCCESS && eResult != ERR_AT_UTIL_CMD_PARA_NULL)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // [[hameina [+]07.11.27 :bug 7133
                    if (str_len > 32)
                    {
                        AT_Result_Err(ERR_AT_CME_TEXT_LONG, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // ]]hameina [+]07.11.27 :bug 7133
                }

                break;

            case 4: // recurr,

                if (iCount > 3)
                {
                    len = 16;
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_STRING, (PVOID)cala.reccurr, &len);
                    AT_TC(g_sw_GC, "=======CALA 7======,cala.reccurr=%s, eResult=0x%x\n", cala.reccurr, eResult);

                    if (eResult == ERR_SUCCESS)
                    {
                        AT_Util_TrimAll(cala.reccurr);
                    }

                    // [[hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult == ERR_AT_UTIL_CMD_PARA_NULL && iCount == 5)
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    // ]]hameina[+]2007.12.11 bug 7203:the last param can't be ignored.
                    else if (eResult == ERR_AT_UTIL_CMD_PARA_NULL)
                    {
                        cala.reccurr[0] = 0;
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
                else
                    cala.reccurr[0] = 0;

                break;

            default:
                break;
            }
        }

        cfw_alarm.nIndex = cala.index;

        cfw_alarm.nType = cala.type;
        cfw_alarm.pText = cala.text;
        cfw_alarm.nTextLength = (UINT8)AT_StrLen(cala.text);
        AT_TC(g_sw_GC, "=======CALA 8======,cala.reccurr[0]=%d\n", cala.reccurr[0]);

        if (!cala.reccurr[0]) // --->yy/mm/dd,hh:mm:ss
        {
            if (!TM_FormatDateTimeEx(&ft, cala.time))
            {
                AT_TC(g_sw_GC, "=======CALA 9======,cala.time=%s, e\n", cala.time);
                AT_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            cfw_alarm.nFileTime = ft.DateTime;
            AT_TC(g_sw_GC, "=======CALA 10======,ft.DateTime=%d\n", ft.DateTime);
        }
        else // --->hh:mm:ss
        {
            UINT8 tmp_time = 0;
            UINT16 tme_len = 0;

            if (cala.time[2] != ':' || cala.time[5] != ':')
            {
                AT_TC(g_sw_GC, "=======CALA 11======,ft.cala.time[2]=%c, cala.time[5]=%c\n", cala.time[2], cala.time[2]);
                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                cala.time[2] = ',';
                cala.time[5] = ',';
            }

            tme_len = 1;
            eResult = AT_Util_GetParaWithRule(cala.time, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&tmp_time, &tme_len);
            if (eResult != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            cfw_alarm.nFileTime = tmp_time * 60 * 60;
            eResult = AT_Util_GetParaWithRule(cala.time, 1, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&tmp_time, &tme_len);
            if (eResult != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                AT_TC(g_sw_GC, "=======CALA 13======\n");
                return;
            }

            cfw_alarm.nFileTime += tmp_time * 60;

            eResult = AT_Util_GetParaWithRule(cala.time, 2, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&tmp_time, &tme_len);

            if (eResult != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                AT_TC(g_sw_GC, "=======CALA 14======\n");
                return;
            }

            cfw_alarm.nFileTime += tmp_time;

            AT_TC(g_sw_GC, "=======CALA 15======cfw_alarm.nFileTime=%d\n", cfw_alarm.nFileTime);
        }

        if (!cala.reccurr[0])
        {
            cfw_alarm.nRecurrent = 1;
        }
        else if (cala.reccurr[0] == 0x30)
        {
            cfw_alarm.nRecurrent = 0;
        }
        else if (len) // string recurr not null
        {
            eResult = AT_Util_GetParaCount(cala.reccurr, &iCount);

            if (eResult != ERR_SUCCESS || iCount > 7)
            {
                AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                AT_TC(g_sw_GC, "=======CALA 16======\n");
                return;
            }
            else
            {
                UINT8 uRecurr_tmp = 0;

                for (i = 0; i < iCount; i++)
                {
                    len = 1;
                    eResult = AT_Util_GetParaWithRule(cala.reccurr, i, AT_UTIL_PARA_TYPE_UINT8, &uRecurr_tmp, &len);

                    if (eResult == ERR_SUCCESS)
                    {
                        if (!uRecurr_tmp || uRecurr_tmp > 7)
                        {
                            AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            AT_TC(g_sw_GC, "=======CALA 17,i=%d=====\n", i);
                            return;
                        }
                        else
                            cfw_alarm.nRecurrent |= 1 << uRecurr_tmp;

                        AT_TC(g_sw_GC, "=======CALA 18, i=%d,cfw_alarm.nRecurrent=0x%x\n", i, cfw_alarm.nRecurrent);
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        AT_TC(g_sw_GC, "=======CALA 19,i=%d=====\n", i);
                        return;
                    }
                }

                cfw_alarm.nRecurrent &= 0xfe; // set bit 0 to zero
            }
        }

        if (!TM_SetAlarm(&cfw_alarm, &alarm_num))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            AT_TC(g_sw_GC, "=======CALA 20===sul_err=0x%x===\n", SUL_GetLastError());
            return;
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;
    }

    case AT_CMD_TEST:

        AT_Sprintf(msg, "+CALA: (1-%d),(0),(32),(15)", TIM_ALARM_INDEX_MAX);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, msg, AT_StrLen(msg), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        if (TM_ListAlarm(&alarm_num, &alarm_list))
        {
            // UINT8 time_str[20];
            // UINT8 send_msg[80];

            for (alarm_index = 0; alarm_index < alarm_num; alarm_index++)
            {
                AT_MemSet(respond, 0, 80);
                AT_MemSet(String, 0x00, 24);
                ft.DateTime = alarm_list[alarm_index].nFileTime;

                if (!TM_FormatFileTimeToStringEx(ft, String))
                {
                    if (alarm_list != NULL)
                    {
                        AT_FREE(alarm_list);
                        alarm_list = NULL;
                    }

                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                AT_TC(g_sw_GC, "=======CALA 21===String=%s, alarm_list[%d].nTextLength =%d\n", String, alarm_index,
                      alarm_list[alarm_index].nTextLength);

                AT_StrNCpy(msg, alarm_list[alarm_index].pText, alarm_list[alarm_index].nTextLength);

                // msg[alarm_list[alarm_index].nTextLength]=0;

                if (alarm_list[alarm_index].nRecurrent == 1) // just alerm once
                {
                    AT_TC(g_sw_GC, "=======CALA 21===String=%s\n", String);
                    AT_Sprintf(respond, "+CALA: \"%s\",%u,%u,\"%s\"",
                               String, alarm_list[alarm_index].nIndex, alarm_list[alarm_index].nType, msg);
                }
                else if (alarm_list[alarm_index].nRecurrent == 0) // from monday to sunday
                {
                    AT_Sprintf(respond, "+CALA: \"%s\",%u,%u,\"%s\",\"1,2,3,4,5,6,7\"",
                               String, alarm_list[alarm_index].nIndex, alarm_list[alarm_index].nType, msg);
                }
                else
                {
                    UINT8 recurr = 0;

                    AT_Sprintf(respond, "+CALA: \"%s\",%u,%u,\"%s\",\"",
                               String, alarm_list[alarm_index].nIndex, alarm_list[alarm_index].nType, msg);

                    for (i = 0; i < 7; i++)
                    {
                        recurr = BIT_SET(alarm_list[alarm_index].nRecurrent, (i + 1));

                        if (recurr)
                        {
                            AT_Sprintf(cfw_para, "%u,", recurr);
                            AT_StrCat(respond, cfw_para);
                        }
                    }

                    // AT_StrCat(respond, "\"");
                    respond[AT_StrLen(respond) - 1] = '"';
                }
                AT_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, respond, AT_StrLen(respond), pParam->nDLCI, nSim);
            }

            alarm_index = 0;

            if (alarm_list != NULL)
            {
                AT_FREE(alarm_list);
                alarm_list = NULL;
            }

            AT_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, 0, 0, pParam->nDLCI, nSim);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            if (alarm_list != NULL)
            {
                AT_FREE(alarm_list);
                alarm_list = NULL;
            }

            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;

    default:
        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_GC_CmdFunc_CALD(AT_CMD_PARA *pParam)
{

    UINT8 alarm_index = 0;

    INT32 eResult;
    UINT8 iCount = 0;

    // UINT8 i;
    UINT16 len = 0;
    UINT8 msg[6] = {0, 0, 0, 0, 0, 0};
    UINT8 String[50];

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    AT_TC(g_sw_GC, "=======CALD 1======,pParam->iType=%d\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
        AT_TC(g_sw_GC, "=======CALD 2======,iCount=%d, eResult=0x%x\n", iCount, eResult);

        if (eResult != ERR_SUCCESS || iCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        len = 1;

        eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &alarm_index, &len);
        AT_TC(g_sw_GC, "=======CALD 4======,alarm_index=%d, eResult=0x%x\n", alarm_index, eResult);

        if (eResult == ERR_SUCCESS)
        {
            // cala.index = uIndex;
            if ((alarm_index > TIM_ALARM_INDEX_MAX) || (alarm_index < 1))
            {
                AT_Result_Err(ERR_AT_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (!TM_KillAlarm(alarm_index))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            AT_TC(g_sw_GC, "=======CALD 20===sul_err=0x%x===\n", SUL_GetLastError());
            return;
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;
    }

    case AT_CMD_TEST:
    {
        TM_ALARM *sAlarmList = NULL;

        if (TM_ListAlarm(&alarm_index, &sAlarmList))
        {

            AT_MemSet(String, 0x00, 50);

            if (!alarm_index)
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            else
            {
                AT_StrCpy(String, "+CALD: ");

                for (iCount = 0; iCount < alarm_index; iCount++)
                {
                    AT_Sprintf(msg, "%u,", sAlarmList[iCount].nIndex);
                    AT_StrCat(String, msg);
                }

                String[AT_StrLen(String) - 1] = 0;
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, String, AT_StrLen(String), pParam->nDLCI, nSim);
            }

            alarm_index = 0;

            if (sAlarmList != NULL)
            {
                AT_FREE(sAlarmList);
                sAlarmList = NULL;
            }

            break;
        }
        else
        {
            if (sAlarmList != NULL)
            {
                AT_FREE(sAlarmList);
                sAlarmList = NULL;
            }

            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    default:
        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_GC_CmdFunc_X(AT_CMD_PARA *pParam)
{
#define AT_X_RANGE "0-4"
    INT32 eResult;
    PAT_CMD_RESULT presult = NULL;
    UINT8 iCount = 0;
    UINT8 atl_para;
    UINT8 nValue;

    if (pParam == NULL)
    {
        presult =
            AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0, pParam->nDLCI);

        AT_Notify2ATM(presult, pParam->nDLCI);

        if (presult != NULL)
        {
            AT_FREE(presult);
            presult = NULL;
        }

        return;
    }
    else
    {
        if (pParam->iType == AT_CMD_EXE)
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

            if (eResult == ERR_SUCCESS)
            {
                if (iCount == 1)
                {
                    UINT16 len = 1;

                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atl_para, &len);

                    if (ERR_SUCCESS == eResult)
                    {
                        UINT32 ret = ERR_SUCCESS;

                        ret = CFW_CfgNumRangeCheck((UINT32)atl_para, (UINT8 *)AT_X_RANGE);

                        if (ret != ERR_SUCCESS)
                        {
                            presult = AT_CreateRC(CMD_FUNC_FAIL,
                                                  CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                                  pParam->nDLCI);
                            AT_Notify2ATM(presult, pParam->nDLCI);

                            if (presult != NULL)
                            {
                                AT_FREE(presult);
                                presult = NULL;
                            }

                            return;
                        }

                        ret = CFW_CfgSetConnectResultFormatCode(atl_para);

                        if (ret != ERR_SUCCESS)
                        {
                            presult = AT_CreateRC(CMD_FUNC_FAIL,
                                                  CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                                  pParam->nDLCI);
                            AT_Notify2ATM(presult, pParam->nDLCI);

                            if (presult != NULL)
                            {
                                AT_FREE(presult);
                                presult = NULL;
                            }

                            return;
                        }

                        ret = CFW_CfgGetConnectResultFormatCode(&nValue);

                        if (ret != ERR_SUCCESS)
                        {
                            presult = AT_CreateRC(CMD_FUNC_FAIL,
                                                  CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                                  pParam->nDLCI);
                            AT_Notify2ATM(presult, pParam->nDLCI);

                            if (presult != NULL)
                            {
                                AT_FREE(presult);
                                presult = NULL;
                            }

                            return;
                        }
                        else if (nValue != atl_para)
                        {
                            presult = AT_CreateRC(CMD_FUNC_FAIL,
                                                  CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                                  pParam->nDLCI);
                            AT_Notify2ATM(presult, pParam->nDLCI);

                            if (presult != NULL)
                            {
                                AT_FREE(presult);
                                presult = NULL;
                            }

                            return;
                        }

                        presult =
                            AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0, pParam->nDLCI);

                        AT_Notify2ATM(presult, pParam->nDLCI);

                        if (presult != NULL)
                        {
                            AT_FREE(presult);
                            presult = NULL;
                        }

                        return;
                    }
                }
                else
                {
                    presult =
                        AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                    pParam->nDLCI);

                    AT_Notify2ATM(presult, pParam->nDLCI);

                    if (presult != NULL)
                    {
                        AT_FREE(presult);
                        presult = NULL;
                    }

                    return;
                }
            }
            else
            {
                presult =
                    AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                                pParam->nDLCI);

                AT_Notify2ATM(presult, pParam->nDLCI);

                if (presult != NULL)
                {
                    AT_FREE(presult);
                    presult = NULL;
                }

                return;
            }
        }

        else
        {
            presult =
                AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, ERR_INVALID_PARAMETER, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0,
                            pParam->nDLCI);

            AT_Notify2ATM(presult, pParam->nDLCI);

            if (presult != NULL)
            {
                AT_FREE(presult);
                presult = NULL;
            }

            return;
        }
    }
}

VOID AT_GC_CmdFunc_CPOF(AT_CMD_PARA *pParam)
{
    UINT32 err_code = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {

        if (pParam->iType == AT_CMD_EXE)
        {
            g_gc_pwroff = 1;
            err_code = CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF);

            if (err_code != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                // AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI);
                // just for debugging,need more work  ,by wulc
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);

                // Stop Audio for FS flush
                DM_StopAudio();
                DM_DeviceSwithOff(FALSE);
                return;
            }
        }
        else if (AT_CMD_TEST == pParam->iType)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
}

VOID AT_GC_CPOF_CFUN_IND(CFW_EVENT *cfwEvent)
{
    UINT8 nType = 0x0;
    UINT32 nParam1;
    UINT32 nParam2;
    UINT8 nSim;
    CFW_EVENT *pCfwEvent = cfwEvent;
    nType = pCfwEvent->nType;
    nParam1 = pCfwEvent->nParam1;
    nParam2 = pCfwEvent->nParam2;
    nSim = pCfwEvent->nFlag;

    if (nType == 0x00)
    {
        if ((nParam1 != 0x01) || (nParam2 != 0))
        {
            g_gc_pwroff = 0;
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pCfwEvent->nUTI, nSim);
            return;
        }
        else
        {
            if (g_gc_pwroff == 1) // smso
            {
                g_gc_pwroff = 0;
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "+CPOF: MS OFF OK", AT_StrLen("+CPOF: MS OFF OK"), pCfwEvent->nUTI,
                             nSim);
                DM_DeviceSwithOff(FALSE);
            }
            else if (g_gc_pwroff == 2) // CFUN, just switch of cfw
            {
                g_gc_pwroff = 0;
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pCfwEvent->nUTI, nSim);
            }
            return;
        }
    }
    else // if (nType == 0xF0)
    {
        g_gc_pwroff = 0;
        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pCfwEvent->nUTI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_S3(AT_CMD_PARA *pParam)
{

    UINT8 arrRes[4];
    INT32 iRet = 0;
    UINT8 nParaCount = 0;
    UINT8 nS3 = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    AT_TC(g_sw_GC, "=======AT_GC_CmdFunc_S3======,pParam->iType=%d\n", pParam->iType);

    if (pParam->iType == AT_CMD_READ)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_Sprintf(arrRes, "%3u", gATCurrentnS3);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &nParaCount);

        if (iRet != ERR_SUCCESS || nParaCount > 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nParaCount == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nS3, &nParamLen);

        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // added by yangtt at 2008-06-13 for bug 8733 begin
        if (nS3 > 31) // added by yangtt at 2008-06-13 for bug 8776
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        gATCurrentnS3 = nS3;

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_S4(AT_CMD_PARA *pParam)
{

    UINT8 arrRes[4];
    INT32 iRet = 0;
    UINT8 nParaCount = 0;
    UINT8 nS4 = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_READ)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_Sprintf(arrRes, "%3u", gATCurrentnS4);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &nParaCount);

        if (iRet != ERR_SUCCESS || nParaCount > 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nParaCount == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nS4, &nParamLen);

        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nS4 > 31) // added by yangtt at 2008-06-13 for bug 8776
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        gATCurrentnS4 = nS4;

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_S5(AT_CMD_PARA *pParam)
{

    UINT8 arrRes[4];
    INT32 iRet = 0;
    UINT8 nParaCount = 0;
    UINT8 nS5 = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_READ)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_Sprintf(arrRes, "%3u", gATCurrentnS5);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &nParaCount);

        if (iRet != ERR_SUCCESS || nParaCount > 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nParaCount == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nS5, &nParamLen);

        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // added by yangtt at 2008-06-13 for bug 8733 begin
        if (nS5 > 31) // added by yangtt at 2008-06-13 for bug 8776
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        gATCurrentnS5 = nS5;
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

// +ICF: DTE-DCE character framing
VOID AT_GC_CmdFunc_ICF(AT_CMD_PARA *pParam)
{
    AT_DEVICE_T *device = at_GetDeviceByChannel(pParam->nDLCI);

    if (at_DeviceGetNumber(device) != 0) // not default device
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount > 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t format = at_ParamDefUintInRange(pParam->params[0], AT_DEVICE_FORMAT_8N1, 2, 6, &paramok);
        uint8_t parity = at_ParamDefUintInRange(pParam->params[1], AT_DEVICE_PARITY_SPACE, 0, 3, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        gATCurrentIcfFormat = format;
        gATCurrentIcfParity = parity;
        at_CmdDeviceSetFormatNeeded(pParam->engine);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+ICF:(2-6),(0-3)");
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        AT_Sprintf(rsp, "+ICF: %u, %u", gATCurrentIcfFormat, gATCurrentIcfParity);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// +IPR: Fixed DTE rate
VOID AT_GC_CmdFunc_IPR(AT_CMD_PARA *pParam)
{
    AT_DEVICE_T *device = at_GetDeviceByChannel(pParam->nDLCI);

    if (at_DeviceGetNumber(device) != 0) // not default device
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint32_t baud = at_ParamUint(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_DeviceIsBaudSupported(device, baud))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        gATCurrentu32nBaudRate = baud;
        at_CmdDeviceSetFormatNeeded(pParam->engine);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+IPR: (0,2400,4800,9600,14400,19200,28800,33600,38400,57600,115200),(230400,460800,921600)");
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        AT_Sprintf(rsp, "+IPR: %u", gATCurrentu32nBaudRate);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// +IFC: DTE-DCE local flow control
VOID AT_GC_CmdFunc_IFC(AT_CMD_PARA *pParam)
{
    AT_DEVICE_T *device = at_GetDeviceByChannel(pParam->nDLCI);

    if (at_DeviceGetNumber(device) != 0) // not default device
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount > 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        static const uint32_t list[] = {0, 2};
        bool paramok = true;
        uint8_t rxfc = at_ParamDefUintInList(pParam->params[0], 0, list, 2, &paramok);
        uint8_t txfc = at_ParamDefUintInList(pParam->params[1], 0, list, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        at_DeviceSetFlowCtrl(device, rxfc, txfc);
        at_CmdRespOK(pParam->engine);
        gATCurrentRxfc = rxfc;
        gATCurrentTxfc = txfc;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        AT_Sprintf(rsp, "+IFC: %d,%d", gATCurrentRxfc, gATCurrentTxfc);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+IFC: (0,2),(0,2)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// &D: Circuit 108 (Data terminal ready) behaviour
void AT_GC_CmdFunc_AndD(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        if (pParam->paramCount > 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t dtrData = at_ParamDefUintInRange(pParam->params[0], 1, 0, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        gATCurrentDtrData = dtrData;
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

// AT_20071123_CAOW_B,reopen by wulc 2012.02.14
VOID AT_GC_CmdFunc_CPAS(AT_CMD_PARA *pParam)
{
    UINT8 res[10] = {0};
    UINT16 PhoneState = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_GC, "AABB----------SPAS CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        PhoneState = AT_GetPhoneActiveStatus(nSim);
        if (PhoneState == AT_ACTIVE_STATUS_READY ||
            PhoneState == AT_ACTIVE_STATUS_UNAVAILABLE ||
            PhoneState == AT_ACTIVE_STATUS_UNKNOWN ||
            PhoneState == AT_ACTIVE_STATUS_RINGING ||
            PhoneState == AT_ACTIVE_STATUS_CALL_IN_PROGRESS)
        {
            AT_MemZero(res, 10);
            AT_Sprintf(res, "+CPAS:%u", PhoneState);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, res, AT_StrLen(res), pParam->nDLCI, nSim);
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
        break;

    case AT_CMD_TEST: // AT+CPAS=?
        AT_MemZero(res, 10);
        AT_Sprintf(res, "+CPAS:0,1,3,4");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, res, AT_StrLen(res), pParam->nDLCI, nSim);
        break;

    default:
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    AT_TC(g_sw_GC, "AABB----------CPAS Cmd END------------\n");
}

VOID AT_GC_CmdFunc_SUPS(AT_CMD_PARA *pParam)
{
    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, 0);
}

// AT_20071123_CAOW_E

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_GC_CmdFunc_CIND(AT_CMD_PARA *pParam)
{
    // CFW_IND_EVENT_INFO                 IndInfo;
    UINT8 sz_result[200] = {0};

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_GC, "----------CIND CmdType : %d----------\n", pParam->iType);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET: // +CIND=[<ind>[,<ind>[,...]]]
    {
        INT32 eResult;
        UINT8 iParaCount = 0;
        UINT8 TEMP[10];
        UINT8 i = 0;
        UINT16 nParaLen[10];
        UINT16 nIndId = 0;

        eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);
        if ((eResult != ERR_SUCCESS) || 10 < iParaCount) // || 0 > iParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_MemZero(TEMP, sizeof(TEMP));

        for (i = 0; i < 10; i++)
        {
            nParaLen[i] = 1;
        }

        for (i = 0; i < iParaCount; i++)
        {
            eResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT8, &TEMP[i], &nParaLen[i]);

            // if para is null , set nParaLen[i] = 0;
            if (eResult == ERR_AT_UTIL_CMD_PARA_NULL)
            {
                nParaLen[i] = 0;
            }

            if (((eResult != ERR_SUCCESS) && (eResult != ERR_AT_UTIL_CMD_PARA_NULL)) || (TEMP[i] > 1)) // || TEMP[i]<0))
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        eResult = CFW_CfgGetIndicatorEvent(&nIndId, nSim);
        if (eResult == ERR_SUCCESS)
        {
            for (i = 0; i < 10; i++)
            {
                if (1 != nParaLen[i]) // if para is null ,get csw value
                    TEMP[i] = (nIndId & ((UINT16)0x01 << i)) || 0;
            }

            nIndId = 0;
            for (i = 0; i < 10; i++)
            {
                nIndId |= (UINT16)TEMP[i] << i;
            }

            eResult = CFW_CfgSetIndicatorEvent(nIndId, nSim);

            if (eResult == ERR_SUCCESS)
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        break;
    }

    case AT_CMD_READ: // AT+CIND?
    {

        UINT8 nBatteryCharge = 0;
        UINT8 nSignal = 0;
        UINT8 nService = 0;
        BOOL bCall = FALSE;
        BOOL bSounder = FALSE;
        UINT8 nMessage = 0;
        UINT8 nRoam = 0;
        UINT8 nSmsfull = 0;

        AT_TC(g_sw_GC, "----------CIND CmdType : %d----------\n", pParam->iType);

        // get battery chareg level

        if (!AT_CIND_GetBattery(&nBatteryCharge))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get sounder & call ind
        if (!AT_CIND_GetCC(&bCall, &bSounder))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get signal & service & roam  ind
        if (!AT_CIND_NetWork(&nSignal, &nService, &nRoam, pParam->nDLCI, nSim))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get sms-receive & sms-full ind
        if (!AT_CIND_SMS(&nMessage, &nSmsfull, nSim))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_MemZero(sz_result, sizeof(sz_result));

        AT_Sprintf(sz_result, "+CIND: %u,%u,%u,%d,%u,%d,%u,%u", nBatteryCharge, nSignal, nService, bSounder, nMessage,
                   bCall, nRoam, nSmsfull);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, sz_result, AT_StrLen(sz_result), pParam->nDLCI, nSim);
    }

    break;

    case AT_CMD_TEST:
        AT_MemZero(sz_result, sizeof(sz_result));
        AT_Sprintf(sz_result,
                   "+CIND: (\"battchg\",(0-5)),(\"signal\",(0-5)),(\"service\",(0-1)),(\"sounder\",\",(0-1)),(\"message\",(0-1)),(\"call\",(0-1)),(\"roam\",(0-1)),(\"smsfull\",(0-1))");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, sz_result, AT_StrLen(sz_result), pParam->nDLCI, nSim);
        break;

    default: // error type
        // CMD type error, Create error result
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
}

extern BOOL g_bTesting; // added by yangtt at 2008-05-05 for bug 8274

struct AT_PLAY_DTMF_CONTEXT
{
    AT_CMD_ENGINE_T *engine;
    uint8_t dtmf[AT_DTMF_STRING_LEN];
    uint8_t count;
    uint8_t duration;
    uint8_t pos;
};

static void AT_GC_PlayDTMF(void *param)
{
    struct AT_PLAY_DTMF_CONTEXT *ctx = (struct AT_PLAY_DTMF_CONTEXT *)param;

    if (ctx->pos >= ctx->count)
    {
        at_CmdRespOK(ctx->engine);
        at_free(ctx);
        return;
    }

    uint8_t tone = ctx->dtmf[ctx->pos++];
    if (!DM_PlayTone(tone, DM_TONE_m3dB, ctx->duration * 100, 0))
    {
        at_CmdRespCmeError(ctx->engine, ERR_AT_CME_EXE_FAIL);
        at_free(ctx);
    }
    else
    {
        at_StartCallbackTimer(ctx->duration * 100, AT_GC_PlayDTMF, ctx);
    }
}

// [+]for AT^CDTMF 2007.11.12
VOID AT_GC_CmdFunc_CDTMF(AT_CMD_PARA *pParam)
{
    if (g_bTesting == TRUE) // added by yangtt at 2008-05-05 for bug 8274
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

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

        at_TaskCallback(AT_GC_PlayDTMF, ctx);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        // =?
        static const uint8_t rsp[] = "+CDTMF: (0-9,*,#,A,B,C,D),(1-10)";
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

// atw == atw0

VOID AT_GC_CmdFunc_AndW(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 iCount = 0;
    BOOL err_code;
    UINT8 atw_para;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_GC, "AT_GC_CmdFunc_AndW");

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }
    else
    {
        if (pParam->iType == AT_CMD_EXE)
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, (UINT8 *)&iCount);

            if (eResult == ERR_SUCCESS)
            {
                switch (iCount)
                {

                case 0:
                    err_code = at_CfgSetAtSettings(iCount);

                    if (TRUE == err_code)
                    {
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }

                    AT_TC(g_sw_GC, "at_CfgSetAtSettings=0x%x, ", err_code);

                    break;

                case 1:
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atw_para, &iCount);

                    if (ERR_SUCCESS == eResult)
                    {
                        if (0 != atw_para)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        }
                        else
                        {
                            err_code = at_CfgSetAtSettings(atw_para);

                            if (TRUE == err_code)
                            {
                                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                            }
                            else
                            {
                                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            }
                        }
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    break;

                default:
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    break;
                }
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    return;
}

VOID AT_GC_CmdFunc_AndF(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 iCount = 0;
    BOOL err_code;
    UINT8 atf_para;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }
    else
    {
        if (pParam->iType == AT_CMD_EXE)
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, (UINT8 *)&iCount);

            if (eResult == ERR_SUCCESS)
            {
                switch (iCount)
                {

                case 0:
                    err_code = at_CfgGetAtSettings(MANUFACTURER_DEFALUT_SETING, NULL);

                    if (TRUE != err_code)
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }
                    break;

                case 1:
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atf_para, &iCount);

                    if (ERR_SUCCESS == eResult)
                    {
                        if (0 != atf_para)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return; // added by yangtt at 2008-5-30 for bug 8603
                        }
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }

                    err_code = at_CfgGetAtSettings(MANUFACTURER_DEFALUT_SETING, NULL);

                    if (TRUE != err_code)
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }

                    break;

                default:
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    break;
                }
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    return;
}

// ATZ == ATZ0
//#include "dbg.h"
VOID AT_GC_CmdFunc_Z(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 iCount = 0;
    UINT8 atz_para = 0;
    BOOL err_code;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }
    else
    {
        if (pParam->iType == AT_CMD_EXE)
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, (UINT8 *)&iCount);

            if (eResult == ERR_SUCCESS)
            {
                switch (iCount)
                {

                case 0:
#ifdef CHIP_HAS_AP
                    err_code = TRUE;
#else
                    err_code = at_CfgGetAtSettings(USER_SETTING_1, atz_para);
#endif
                    AT_TC(g_sw_GC, "=======ERRCODE =%d\n", err_code);
                    //err_code = TRUE;
                    if (TRUE == err_code)
                    {
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }

                    break;

                case 1:
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atz_para, &iCount);

                    if (ERR_SUCCESS == eResult)
                    {
                        if (0 != atz_para)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        }
                        else
                        {
#ifdef CHIP_HAS_AP
                            err_code = TRUE;
#else
                            err_code = at_CfgGetAtSettings(USER_SETTING_1, atz_para);
#endif

                            if (TRUE == err_code)
                            {
                                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                            }
                            else
                            {
                                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            }
                        }
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }

                    break;

                default:
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    break;
                }
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    return;
}

//
// FUNCTION: AT_CC_CmdFunc_S0
//
// DESCRIPTION:
// The Implementation of AT Command REQ,
// config the count of which the call rings, MT should auto answer the call.
// config function should record the value
// This Function will process all related asynchronous operations to other
// software layers(DMS,DSS,TAPI,APS,...).
// CoolStudio REQ Event: <NO REQ EVENT>
// AT Command      : S0
// AT Command Type : 2. Configuration Commands(CFG)
// At Command Mask : 0
// /////////////////////////////////////////////
// the definition of S0 is gATCurrentcc_s0
// //////////////////////////////////////////////
// ARGUMENTS:
//
// RETURN:
//
// COMMENTS:
//
VOID AT_GC_CmdFunc_S0(AT_CMD_PARA *pParam)
{
    UINT8 strS0[12];
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT32 uRetVal = ERR_SUCCESS;
    UINT8 uParam = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    AT_TC(g_sw_GC, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

    // init param
    memset(strS0, 0, 24);

    // at command type judge

    switch (pParam->iType)
    {
    // extend command (set)

    case AT_CMD_SET:
    {
        // get param count, check the input string
        uRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        // result check

        if (ERR_SUCCESS != uRetVal || ucParaCount > 1)
        {
            // return parameter error
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // none param input, just return OK
        if (!ucParaCount)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        ucLen = SIZEOF(UINT8);

        uRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&uParam, &ucLen);

        if (ERR_SUCCESS != uRetVal)
        {
            // return parameter error
            AT_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        AT_TC(g_sw_GC, "Para count:%d Parameter 1: %d\n", ucParaCount, uParam);

        gATCurrentcc_s0 = uParam;

        if (ERR_SUCCESS != uRetVal)
        {
            AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
        else
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "0-255"
        SUL_StrCopy(strS0, "0-255");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, strS0, AT_StrLen(strS0), pParam->nDLCI, nSim);
        break;

    // extend command (read)

    case AT_CMD_READ:
        // get stored value from config function
        uParam = gATCurrentcc_s0;

        // output debug information
        AT_TC(g_sw_GC, "ATS0 read ret: %x\n", uRetVal);

        // execute result return

        if (ERR_SUCCESS == uRetVal)
        {
            SUL_StrPrint(strS0, "%d", uParam);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, strS0, AT_StrLen(strS0), pParam->nDLCI, nSim);
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    // error command type

    default:
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_GC_CmdFunc_EGMR(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT32 eParamOk = 0;
    UINT8 uCommand = 0;
    UINT8 imeisize = 0;
    UINT8 imei[40] = {0};
    UINT16 uStrLen = 0;
    UINT8 arrRes[50];
    UINT8 Revtype = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_MemZero(arrRes, sizeof(arrRes));

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(arrRes, "+EGMR: (1,2),(7)");
        at_CmdRespInfoText(pParam->engine, arrRes);
        at_CmdRespOK(pParam->engine);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {

            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if ((2 > uParaCount) || (3 < uParaCount))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (uParaCount == 2)
        {
            /* get all param */
            uStrLen = 1;

            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uCommand, &uStrLen);

            if (eParamOk != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if (uCommand != 2)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            uStrLen = 1;
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &Revtype, &uStrLen);

            if (eParamOk != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if (Revtype == 7)
            {
                CFW_EmodGetIMEI(imei, &imeisize, nSim);
                AT_Sprintf(arrRes, "+EGMR:%s", imei);
                at_CmdRespInfoText(pParam->engine, arrRes);
                at_CmdRespOK(pParam->engine);
                return;
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
        }
        else if (uParaCount == 3)
        {
            /* get all param */
            uStrLen = 1;

            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uCommand, &uStrLen);

            if (eParamOk != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if (uCommand != 1)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            uStrLen = 1;
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &Revtype, &uStrLen);

            if (eParamOk != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if (Revtype == 7)
            {
                uStrLen = 40;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, imei, &uStrLen);

                if (eParamOk != ERR_SUCCESS)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if(CFW_EmodSaveIMEI(imei, AT_StrLen(imei), nSim) != TRUE)
		  {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
		  }
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
        }
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

UINT16 gc_GetCfgInfo_IndCtrlMask(VOID)
{
    return gATCurrentCfgInfoInd_ctrl_mask;
}

BOOL gc_SetCfgInfo_IndCtrlMask(UINT16 mask)
{
    gATCurrentCfgInfoInd_ctrl_mask = mask;
    if (at_CfgSetAtSettings(1) == TRUE)
    {
        AT_TC(g_sw_GC, "Succeeded to save configuration information into flash.");
        return TRUE;
    }
    else
    {
        AT_TC(g_sw_GC, "Failed to save configuration information into flash!");
        return FALSE;
    }
}

at_chset_t cfg_GetMtChset(void)
{
    return gATCurrentCfgInfoMt_chset;
}

at_chset_t cfg_GetTeChset(void)
{
    return gATCurrentCfgInfoTe_chset;
}

void cfg_SetTeChset(at_chset_t chset)
{
    AT_ASSERT(chset >= cs_gsm && chset < cs_COUNT_);
    gATCurrentCfgInfoTe_chset = chset;

    if (at_CfgSetAtSettings(0) == TRUE)
        AT_TC(g_sw_GC, "Save configuration informaton okay.");
    else
        AT_TC(g_sw_GC, "Save configuration informaton failed.");
}
//UINT8 g_set_ACLB = 0;
UINT8 g_ACLB_FM = CFW_CHECK_COMM;
UINT8 g_ACLB_SIM1_FM = CFW_CHECK_COMM;
extern BOOL g_auto_calib;

UINT8 gACLBNWStatus[CFW_SIM_COUNT] = {CFW_CHECK_COMM};
#ifdef MMI_ONLY_AT
UINT8 gACLBStatus = FALSE;
#endif
UINT8 gACLBStep = 0x00;

BOOL _GetACLBNWStatus(CFW_SIM_ID nSimID)
{
    return gACLBNWStatus[nSimID];
}

UINT32 _SetACLBNWStatus(UINT8 nACLBNWStatus, CFW_SIM_ID nSimID)
{
    gACLBNWStatus[nSimID] = nACLBNWStatus;
    return ERR_SUCCESS;
}
#ifdef MMI_ONLY_AT
BOOL _GetACLBStatus()
{
    return gACLBStatus;
}

UINT32 _SetACLBStatus(UINT8 nACLBNWStatus)
{
    gACLBStatus = nACLBNWStatus;
    return ERR_SUCCESS;
}
#endif
UINT8 _GetACLBStep()
{
    return gACLBStep;
}

UINT32 _SetACLBStep(UINT8 nACLBNWStep)
{
    hal_HstSendEvent(0xfa120810);
    hal_HstSendEvent(nACLBNWStep);
    gACLBStep = nACLBNWStep;
    return ERR_SUCCESS;
}

VOID AT_GC_CmdFunc_ACLB(AT_CMD_PARA *pParam)
{
    CFW_COMM_MODE nMode;
    UINT32 uRetVal = ERR_SUCCESS;
    AT_TC(g_sw_GC, "ACLB: start!\n");
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (1 == nSim)
    {
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
    {
        CFW_GetComm(&nMode, nSim);
        _SetACLBNWStatus(nMode, nSim);
        if (CFW_DISABLE_COMM == nMode)
        {
            _SetACLBStatus(TRUE);
            _SetACLBStep(2);
            uRetVal = CFW_SetComm(CFW_ENABLE_COMM, 0, pParam->nDLCI, nSim);
        }
        else
        {
            _SetACLBStatus(TRUE);
            _SetACLBStep(1);
            uRetVal = CFW_SetComm(CFW_DISABLE_COMM, 0, pParam->nDLCI, nSim);
        }
        ///close anther sim
        CFW_GetComm(&nMode, 1);
        g_ACLB_SIM1_FM = nMode;
        if (CFW_ENABLE_COMM == nMode)
        {
            uRetVal = CFW_SetComm(CFW_DISABLE_COMM, 0, pParam->nDLCI, 1);
        }
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 255, 0, 0, pParam->nDLCI, nSim); //10s
    }
    break;
    case AT_CMD_TEST:
    //break;
    case AT_CMD_READ:
    //break;
    default:
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    return;
}

//this at command for get csw and cos heap space of remain.
VOID AT_GC_CmdFunc_GCHS(AT_CMD_PARA *pParam)
{
    UINT32 uRetVal = ERR_SUCCESS;
    COS_HEAP_STATUS pHeapStatus;
    UINT32 nCSWHeapRemain = 0;
    UINT32 nCOSHeapRemain = 0;
    UINT8 nRetString[40] = {0x00};
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_GC, "GCHS: start!\n");
    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
    {
        uRetVal = COS_GetHeapUsageStatus(&pHeapStatus, 2); //#define     CSW_HEAP_INDEX         2
        if (uRetVal != ERR_SUCCESS)
        {
            AT_TC(g_sw_GC, "GCHS: get CSW heap failure!\n");
            AT_Result_Err(ERR_AT_CME_NOT_FOUND, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        nCSWHeapRemain = pHeapStatus.nTotalSize - pHeapStatus.nUsedSize;

        uRetVal = COS_GetHeapUsageStatus(&pHeapStatus, 3); //#define     COS_HEAP_INDEX         3
        if (uRetVal != ERR_SUCCESS)
        {
            AT_TC(g_sw_GC, "GCHS: get COS heap failure!\n");
            AT_Result_Err(ERR_AT_CME_NOT_FOUND, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        nCOSHeapRemain = pHeapStatus.nTotalSize - pHeapStatus.nUsedSize;
        AT_Sprintf(nRetString, "+GCHS:CSW:%d,COS:%d", nCSWHeapRemain, nCOSHeapRemain);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nRetString, sizeof(nRetString), pParam->nDLCI, nSim);
    }
    break;
    case AT_CMD_TEST:
    //break;
    case AT_CMD_READ:
    //break;
    default:
        AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    return;
}

VOID AT_GC_CmdFunc_CSCLK(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        UINT8 sclk = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_LpCtrlSetMode((AT_DEVICE_LP_MODE_T)sclk))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        static const UINT8 OutStr[] = "+CSCLK: (0,1,2)";
        at_CmdRespInfoText(pParam->engine, OutStr);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        UINT8 OutStr[12];
        AT_DEVICE_LP_MODE_T ctrl = at_LpCtrlGetMode();
        AT_Sprintf(OutStr, "+CSCLK: %u", ctrl);
        at_CmdRespInfoText(pParam->engine, OutStr);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
    }
}

void AT_GC_CmdFunc_CEAUART(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        // +CEAUART: <0|1>[,<baud>[,<format>[,<parity>]]]
        if (pParam->paramCount < 1 && pParam->paramCount > 4)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        uint32_t baud = at_ParamDefUintInRange(pParam->params[1], 115200, 0, -1U, &paramok);
        uint8_t format = at_ParamDefUintInRange(pParam->params[2], AT_DEVICE_FORMAT_8N1, 1, 6, &paramok);
        uint8_t parity = at_ParamDefUintInRange(pParam->params[3], AT_DEVICE_PARITY_SPACE, 0, 3, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_ModuleEnableSecUart(enable, baud, format, parity))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[160];
        if (gATCurrentSecUartEnable)
        {
            AT_Sprintf(rsp, "+CEAUART: 1,%d,%d,%d",
                       gAtCurrentSecUartBaud, gAtCurrentSecUartIcfFormat, gAtCurrentSecUartIcfParity);
            at_CmdRespInfoText(pParam->engine, rsp);
        }
        else
        {
            at_CmdRespInfoText(pParam->engine, "+CEAUART: 0");
        }
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+CEAUART: (0,1),(0,2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,460800,921600),(1-6),(0-3)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void AT_GC_CmdFunc_CURCCH(AT_CMD_PARA *pParam)
{
#ifdef AT_SECOND_UART_SUPPORT
    int channel = at_GetUrcChannel();

    if (pParam->iType == AT_CMD_SET)
    {
        // +CURCCH: <port> (for non-cmux uart port)
        // +CURCCH: <port>,<dlc> (for mux uart port)
        if (pParam->paramCount != 1 && pParam->paramCount != 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t port = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        uint8_t dlci = at_ParamDefUintInRange(pParam->params[1], 0, 1, 10, &paramok);
        int chset = at_ChannelFromDeviceDlci(port, dlci);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_SetUrcChannel(chset))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        uint8_t port = at_Channel2DeviceNum(channel);
        uint8_t dlci = at_Channel2Dlci(channel);
        if (dlci == 0)
            AT_Sprintf(rsp, "+CURCCH: %d", port);
        else
            AT_Sprintf(rsp, "+CURCCH: %d,%d", port, dlci);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+CURCCH: (0,1),(1-10)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
#else
    AT_DISPATCH_T *ch = at_DispatchGetByChannel(at_ChannelFromDevice(0));
    if (!at_DispatchIsCmuxMode(ch))
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

    if (pParam->iType == AT_CMD_SET)
    {
        // +CURCCH: <dlci>
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint8_t dlci = at_ParamUintInRange(pParam->params[0], 1, 10, &paramok);
        int chset = at_ChannelFromDeviceDlci(0, dlci);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_SetUrcChannel(chset))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t rsp[32];
        uint8_t dlci = at_Channel2Dlci(at_GetUrcChannel());
        AT_Sprintf(rsp, "+CURCCH: %d", dlci);
        at_CmdRespInfoText(pParam->engine, rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, "+CURCCH: (1-10)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
#endif
}

#ifdef LTE_NBIOT_SUPPORT
extern int nvmWriteStatic(void);
extern u8 nbiot_nvGetScrambleMode();
extern void nbiot_nvSetScrambleMode(u8 mode);
extern s8 nbiot_nvGetRsrpOffset();
extern void nbiot_nvSetRsrpOffset(s8 offset);
extern u8 nbiot_nvGetRrcRlsTimer();
extern void nbiot_nvSetRrcRlsTimer(u8 rrcRlsTimer10);
extern void nbiot_nvGetFreqList(u8 *p_freq_num, u32 a_earfcn[], s8 a_offset[]);
extern void nbiot_nvSetFreqList(u8 freq_num, u32 a_earfcn[], s8 a_offset[]);
extern void nbiot_nvSetLockFreqList(u8 freq_num, u32 a_earfcn[], s8 a_offset[]);
extern void nbiot_nvSetLockCell(u16 pci, u32 earfcn, s8 offset);
extern void nbiot_nvSetNoLock();
extern void nbiot_nvGetLockFreqList(u8 *pfreq_num, u32 a_earfcn[], s8 a_offset[], u16 *ppci);
extern void nbiot_nvSetSupportBand(u8 bandNum, u8 band[]);
extern void nbiot_nvGetSupportBand(u8 *pBandNum, u8 band[]);
extern void nbiot_nvSetSwitchBs(u8 switchBs);
extern u8 nbiot_nvGetSwitchBs();

VOID AT_GC_CmdFunc_NVCFGARFCN(AT_CMD_PARA *pParam)
{

    u8 freq_num = 0;
    u32 a_earfcn[10]; /*MAX_POWER_ON_FREQ_LEN*/
    s8 a_offset[10];  /*MAX_POWER_ON_FREQ_LEN*/
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 uIndex = 0;
    UINT8 uNvIndex = 0;
    UINT32 iResult;
    UINT32 arfcn = 0;
    UINT8 arfcnCount = 0;
    INT8 offset = 0;
    UINT8 str[256];
    UINT8 uPrintLen = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 > uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        uIndex = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &arfcnCount, &uSize);
        if ((arfcnCount * 2 + 1) != uParaCount)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVCFGARFCN: param error,uParaCount = %d, arfcnCount = %d.\n", uParaCount, arfcnCount);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN, uParaCount = %d, arfcnCount = %d.\n", uParaCount, arfcnCount);

        for (uNvIndex = 0; uNvIndex < arfcnCount; uNvIndex++)
        {
            uSize = 4;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &arfcn, &uSize);

            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVCFGARFCN:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &offset, &uSize);
            /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/
            if ((ERR_SUCCESS != iResult) || (offset < 0) || (offset > 38))
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVCFGARFCN:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN, uParaCount = %d, arfcn = %d, offset = %d.\n",
                  uParaCount, arfcn, offset);
            a_earfcn[uNvIndex] = arfcn;
            a_offset[uNvIndex] = offset - 20;
        }

        freq_num = arfcnCount;

        nbiot_nvSetFreqList(freq_num, a_earfcn, a_offset);

        nvmWriteStatic();
        //VDS_CacheFlush();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN, set nvmWriteStatic ok.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVCFGARFCN read.\n");

        nbiot_nvGetFreqList(&freq_num, a_earfcn, a_offset);
        uPrintLen = SUL_StrPrint(str, "%d in total:", freq_num);
        for (uNvIndex = 0; uNvIndex < freq_num; uNvIndex++)
        {
            uPrintLen += SUL_StrPrint(str + uPrintLen, "(%u %d) ",
                                      a_earfcn[uNvIndex],
                                      a_offset[uNvIndex]);
        }
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVCFGARFCN, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_NVSETSCMODE(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 scMode = 0;
    UINT8 str[256];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETSCMODE!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 != uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &scMode, &uSize);
        if (1 < scMode)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETSCMODE: param error,uParaCount = %d, scMode = %d.\n", uParaCount, scMode);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETSCMODE, uParaCount = %d, scMode = %d.\n", uParaCount, scMode);

        nbiot_nvSetScrambleMode(scMode);

        nvmWriteStatic();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETSCMODE, set nvmWriteStatic ok.\n");

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETSCMODE, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVSETSCMODE read.\n");

        SUL_StrPrint(str, "scrambleMode is %d.", nbiot_nvGetScrambleMode());

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETSCMODE, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_LOCKFREQ(AT_CMD_PARA *pParam)
{
    u8 freq_num = 0;
    u32 a_earfcn[10]; /*MAX_POWER_ON_FREQ_LEN*/
    s8 a_offset[10];  /*MAX_POWER_ON_FREQ_LEN*/
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 uIndex = 0;
    UINT8 uNvIndex = 0;
    UINT32 iResult;
    UINT32 arfcn = 0;
    UINT8 arfcnCount = 0;
    UINT8 mode = 0;
    INT8 offset = 0;
    const u16 invalidU16 = 0xffff;
    u16 cellId = invalidU16;
    UINT8 str[256];
    UINT8 uPrintLen = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 > uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        uIndex = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);
        if (2 < mode)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: param error,uParaCount = %d, mode = %d.\n", uParaCount, mode);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /*1 �رչ���*/
        if (0 == mode)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: close.\n", uParaCount, mode);
            nbiot_nvSetNoLock();
            nvmWriteStatic();
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        /*2 ��С��*/
        if (1 == mode)
        {
            if (4 != uParaCount)
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: lock cell param error,uParaCount = %d, mode = %d.\n", uParaCount, mode);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 4;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &arfcn, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &offset, &uSize);
            /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/
            if ((ERR_SUCCESS != iResult) || (offset < 0) || (offset > 38))
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 2;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &cellId, &uSize);
            if ((ERR_SUCCESS != iResult) || (503 < cellId))
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: lock cell.\n");
            nbiot_nvSetLockCell(cellId, arfcn, offset - 20);

            nvmWriteStatic();
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        /*3 ��Ƶ��*/
        if (0 != (uParaCount % 2))
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: lock freq param error,uParaCount = %d, mode = %d.\n", uParaCount, mode);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        uIndex++;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &arfcnCount, &uSize);
        if ((arfcnCount * 2 + 2) != uParaCount)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ: lock freq param error,uParaCount = %d, arfcnCount = %d.\n", uParaCount, arfcnCount);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ, lock freq uParaCount = %d, arfcnCount = %d.\n", uParaCount, arfcnCount);

        for (uNvIndex = 0; uNvIndex < arfcnCount; uNvIndex++)
        {
            uSize = 4;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &arfcn, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &offset, &uSize);
            /*NB-IoT channel number offset(-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,2,4,6,8,10,12,14,16,18),  offset value is 20*/
            if ((ERR_SUCCESS != iResult) || (offset < 0) || (offset > 38))
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_LOCKFREQ:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ, lock freq uParaCount = %d, arfcn = %d, offset = %d.\n",
                  uParaCount, arfcn, offset);
            a_earfcn[uNvIndex] = arfcn;
            a_offset[uNvIndex] = offset - 20;
        }

        nbiot_nvSetLockFreqList(arfcnCount, a_earfcn, a_offset);

        nvmWriteStatic();
        //VDS_CacheFlush();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ, set nvmWriteStatic ok.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_LOCKFREQ read.\n");

        nbiot_nvGetLockFreqList(&freq_num, a_earfcn, a_offset, &cellId);

        if (0 == freq_num)
        {
            AT_MemZero(str, sizeof(str));
            AT_StrCpy(str, "LOCKFREQ FUNCTION CLOSED!");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
            return;
        }

        if (invalidU16 != cellId)
        {
            AT_MemZero(str, sizeof(str));
            uPrintLen = SUL_StrPrint(str, "lock cell: freq(%u %d), pci(%d)",
                                     a_earfcn[0],
                                     a_offset[0],
                                     cellId);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
            return;
        }

        AT_MemZero(str, sizeof(str));
        uPrintLen = SUL_StrPrint(str, "lock freq %d in total:", freq_num);
        for (uNvIndex = 0; uNvIndex < freq_num; uNvIndex++)
        {
            uPrintLen += SUL_StrPrint(str + uPrintLen, "(%u %d) ",
                                      a_earfcn[uNvIndex],
                                      a_offset[uNvIndex]);
        }
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_LOCKFREQ, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_NVSETBAND(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 uIndex = 0;
    UINT8 uNvIndex = 0;
    UINT32 iResult;
    UINT8 band[6] = {0};
    UINT8 bandNum = 0;
    UINT8 str[256];
    UINT8 uPrintLen = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETBAND!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (2 > uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        uIndex = 0;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &bandNum, &uSize);
        if ((0 == bandNum) || (6 < bandNum) || (uParaCount != (bandNum + 1)))
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETBAND: param error,uParaCount = %d, bandNum = %d.\n", uParaCount, bandNum);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        for (uNvIndex = 0; uNvIndex < bandNum; uNvIndex++)
        {
            uSize = 1;
            uIndex++;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &band[uNvIndex], &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETBAND:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex, iResult);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if ((1 != band[uNvIndex]) && (2 != band[uNvIndex]) && (3 != band[uNvIndex]) && (5 != band[uNvIndex]) && (8 != band[uNvIndex]) && (20 != band[uNvIndex]))
            {
                AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETBAND:band[%d] = %d.", uNvIndex, band[uNvIndex]);
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        nbiot_nvSetSupportBand(bandNum, band);

        nvmWriteStatic();
        //VDS_CacheFlush();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETBAND, set nvmWriteStatic ok.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETBAND, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVSETBAND read.\n");

        nbiot_nvGetSupportBand(&bandNum, band);

        AT_MemZero(str, sizeof(str));
        uPrintLen = SUL_StrPrint(str, "%d band in total:", bandNum);
        for (uNvIndex = 0; uNvIndex < bandNum; uNvIndex++)
        {
            uPrintLen += SUL_StrPrint(str + uPrintLen, "%d ",
                                      band[uNvIndex]);
        }
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETBAND, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_NVSWITCHBS(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 switchBs = 0;
    UINT8 str[256];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSWITCHBS!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 != uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &switchBs, &uSize);
        if (1 < switchBs)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSWITCHBS: param error,uParaCount = %d, switchBs = %d.\n", uParaCount, switchBs);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSWITCHBS, uParaCount = %d, switchBs = %d.\n", uParaCount, switchBs);

        nbiot_nvSetSwitchBs(switchBs);

        nvmWriteStatic();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSWITCHBS, set nvmWriteStatic ok.\n");

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSWITCHBS, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVSWITCHBS read.\n");

        if (1 == nbiot_nvGetSwitchBs())
        {
            SUL_StrPrint(str, "Band search is switch on.");
        }
        else
        {
            SUL_StrPrint(str, "Band search is switch off.");
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSWITCHBS, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_NVSETRSRPOFFSET(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 offset = 0;
    UINT8 str[256];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRSRPOFFSET!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 != uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &offset, &uSize);
        if (100 < offset)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETRSRPOFFSET: param error,uParaCount = %d, offset = %d.\n", uParaCount, offset);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRSRPOFFSET, uParaCount = %d, offset = %d.\n", uParaCount, offset);

        nbiot_nvSetRsrpOffset((s8)(offset - 50));

        nvmWriteStatic();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRSRPOFFSET, set nvmWriteStatic ok.\n");

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRSRPOFFSET, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVSETRSRPOFFSET read.\n");

        SUL_StrPrint(str, "rsrpOffset is %d.", nbiot_nvGetRsrpOffset());

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRSRPOFFSET, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

VOID AT_GC_CmdFunc_NVSETRRCRLSTIMER10(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount = 0;
    UINT16 uSize = 0;
    UINT8 value = 0;
    UINT8 str[256];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10!!!\n");

    if (NULL == pParam)
    {
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if ((NULL == pParam->pPara) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if (1 != uParaCount)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &value, &uSize);
        if (1 < value)
        {
            AT_TC(g_sw_GPRS, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10: param error,uParaCount = %d, value = %d.\n", uParaCount, value);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10, uParaCount = %d, value = %d.\n", uParaCount, value);

        nbiot_nvSetRrcRlsTimer(value);

        nvmWriteStatic();
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10, set nvmWriteStatic ok.\n");

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10, iType = AT_CMD_TEST.\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        // output debug information
        AT_TC(g_sw_GC, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10 read.\n");

        SUL_StrPrint(str, "rrcRlsTimer10 is %d.", nbiot_nvGetRrcRlsTimer());

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, str, AT_StrLen(str), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_ATE, "AT_GC_CmdFunc_NVSETRRCRLSTIMER10, error.\n");
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

#endif //LTE_NBIOT_SUPPORT
// ///////////////////////////////////////////////////////////////////////////////
// The end of the file
// ///////////////////////////////////////////////////////////////////////////////
