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
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special_emod.h"
#include "at_cfg.h"
#include "dm.h"

#include "calibaud_m.h"
#include "vpp_speech.h"
#include "vois_m.h"
#include "aud_codec_common.h"
#include "sxr_mem.h"
#include "calib_m.h"

#define EMOD_ASER_START 1
#define EMOD_ASER_STOP  0

#define CFG_SENDDATALEN     512

#define  ERROR_RETURN(pResult,reason,nDLCI) do{ pResult = AT_CreateRC(CMD_FUNC_FAIL,\
        CMD_RC_ERROR, \
        reason,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        NULL,\
        0,\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
    }\
    return;\
}while(0)
#define  OK_RETURN(pResult,pString,nDLCI) do{   pResult = AT_CreateRC(  CMD_FUNC_SUCC,\
        CMD_RC_OK, \
        CMD_ERROR_CODE_OK,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        pString,\
        strlen(pString),\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
    }\
    return;\
}while(0)

#define  ERROR_CALIB_RETURN(pResult,reason,nDLCI) do{ pResult = AT_CreateRC(CMD_FUNC_FAIL,\
        CMD_RC_ERROR, \
        reason,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        NULL,\
        0,\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
        }\
        goto _exit; \
}while(0)
#define  OK_CALIB_RETURN(pResult,pString,nDLCI) do{ pResult = AT_CreateRC(  CMD_FUNC_SUCC,\
        CMD_RC_OK, \
        CMD_ERROR_CODE_OK,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        pString,\
        strlen(pString),\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
        }\
        goto _exit;  \
}while(0)

#define  OK_CALIB_RESET_RETURN(pResult,pString,nDLCI) do{   pResult = AT_CreateRC(  CMD_FUNC_SUCC,\
        CMD_RC_OK, \
        CMD_ERROR_CODE_OK,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        pString,\
        strlen(pString),\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
        }\
}while(0)

#define  ERROR_CALIB_RESET_RETURN(pResult,reason,nDLCI) do{ pResult = AT_CreateRC(CMD_FUNC_FAIL,\
        CMD_RC_ERROR, \
        reason,\
        CMD_ERROR_CODE_TYPE_CME,\
        0,\
        NULL,\
        0,\
        nDLCI);\
    AT_Notify2ATM(pResult,nDLCI);\
    if(pResult != NULL){ \
        AT_FREE(pResult);\
        pResult = NULL; \
        }\
}while(0)

INT32 EX_EmodeClearSMSInfo()
{
    return 1;
}

INT32 EX_EmodeClearREGInfo()
{
    return 1;
}

INT32 EX_EmodeClearPBKInfo()
{
    return 1;
}

INT32 EX_EmodeClearFFSInfo()
{
    return 1;
}

CFW_TSM_FUNCTION_SELECT *g_pSelecFUN_t = NULL;

PRIVATE BOOL EX_EmodeOFTStartSerCell(UINT8 nFreq);
PRIVATE BOOL EX_EmodeOFTStopSerCell(VOID);
PRIVATE BOOL EX_EmodeOFTStartNerCell(UINT8 nFreq);
PRIVATE BOOL EX_EmodeOFTStopNeiCell(VOID);

extern UINT8 SUL_hex2ascii(UINT8* pInput, UINT16 nInputLen, UINT8* pOutput);
extern UINT16 CSW_ATCcGsmBcdToAscii(UINT8 *pBcd, UINT8 nBcdLen, UINT8 *pNumber);
extern UINT16 SMS_PDUAsciiToBCD(UINT8 *pNumber, UINT16 nNumberLen, UINT8 *pBCD);
extern VOID CSW_TC_MEMBLOCK(UINT16 uMask, UINT8 *buffer, UINT16 len, UINT8 radix);

extern AUD_ITF_T        audioItf;
extern SND_ITF_T        musicItf;
extern VOID vpp_AudioMP3DecSetReloadFlag(void);

#ifdef VOIS_DUMP_PCM
#ifndef CHIP_HAS_AP
#ifndef ONLY_AT_SUPPORT
extern BOOL checkTCardExist(void);
#endif
#endif
#endif

#ifdef __VDS_QUICK_FLUSH__
extern VOID DRV_SuspendEnable(VOID);
#endif

// extern UINT32 CFW_SimGetICCID(UINT16 nUTI);

VOID AT_Emod_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CFWev;
    CFW_EVENT *pcfwEv = &CFWev;
    PAT_CMD_RESULT pResult = NULL;
    AT_CosEvent2CfwEvent(pEvent, pcfwEv);
    switch (pcfwEv->nEventId)
    {
    case EV_CFW_SIM_GET_ICCID_RSP:
        if (pcfwEv->nType == 0)
        {
            UINT8 nICCID[21]= {0};
            UINT8 nResponse[30] = {0};
            SUL_gsmbcd2asciiEx((UINT8 *)(pcfwEv->nParam1), 10, nICCID);

            AT_Sprintf(nResponse, "+CCID: %s", nICCID);
            pResult = AT_CreateRC(CMD_FUNC_SUCC,CMD_RC_OK,CMD_ERROR_CODE_OK,
                                  CMD_ERROR_CODE_TYPE_CME,0,nResponse,AT_StrLen(nResponse),pcfwEv->nUTI);

        }
        else
        {
            /*
               TODO: 该处改动是为了解决CFW的一个bug，即它不能识别SIM Not Inserted这种情况。等CFW解决了这个问题后
               要去掉下面的补丁代码。
               * */
#  if 1
            // PATCH CODE
            // AT_20071123_CAOW_B   for bug#7088, but whether it's reasonable? wt's situation about the rsp "ERR_CME_SIM_UNKNOW"?
            if (pcfwEv->nParam1 == ERR_CME_SIM_UNKNOW)
                pcfwEv->nParam1 = ERR_CME_SIM_NOT_INSERTED;

            // AT_20071123_CAOW_E
#endif
            AT_EX_FAIL_CreateRC(pResult,
                                pcfwEv->nParam1 - ERR_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pcfwEv->nUTI);
        }
        AT_Notify2ATM(pResult, pcfwEv->nUTI);
        AT_FREE(pResult);
        break;
    default:
        break;
    }
}

VOID AT_EMOD_CmdFunc_TRACE(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount = 0;
    INT32 iResult = 0;
    UINT16 uSize = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 trace = 0;
    UINT32 sys_freq = 0;
    UINT8 outstr[20] = {0,};

    switch (pParam->iType)
    {
        case AT_CMD_SET:
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
            {
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &trace, &uSize);
            if (iResult != ERR_SUCCESS || (trace != 0 && trace != 1))
            {
                AT_TC(g_sw_GPRS, "AT TRACE param error");
                AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if (trace == 1)
                csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, CSW_SYS_FREQ_104M);
            else
                csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, CSW_SYS_FREQ_52M);

            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            break;
        case AT_CMD_READ:
            sys_freq = csw_GetResourceActivity(CSW_LP_RESOURCE_UNUSED_1);
            if (sys_freq == CSW_SYS_FREQ_52M)
                AT_Sprintf(outstr, "+TRACE: 0,%d", sys_freq);
            else
                AT_Sprintf(outstr, "+TRACE: 1,%d", sys_freq);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, outstr, strlen(outstr), pParam->nDLCI, nSim);
            break;
        default:
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
    }
}

// AT_20071107_CAOW_E

/*********************************************************************
 * @fn
 * @brief
 * @param
 * @return
 */
PRIVATE BOOL EX_EmodeOFTStartSerCell(UINT8 nFreq)
{
    BOOL result;

    result = COS_SetTimer(CSW_AT_TASK_HANDLE, AT_EX_MONI_TIM_ID, COS_TIMER_MODE_PERIODIC, (UINT32)(nFreq * 16384));
    return result;
}

/*********************************************************************
 * @fn
 * @brief
 * @param
 * @return
 */
PRIVATE BOOL EX_EmodeOFTStopSerCell(VOID)
{
    BOOL result;

    result = COS_KillTimer(CSW_AT_TASK_HANDLE, AT_EX_MONI_TIM_ID);
    return result;
}

/*********************************************************************
 * @fn
 * @brief
 * @param
 * @return
 */
PRIVATE BOOL EX_EmodeOFTStartNerCell(UINT8 nFreq)
{
    BOOL result;

    result = COS_SetTimer(CSW_AT_TASK_HANDLE, AT_EX_MONP_TIM_ID, COS_TIMER_MODE_PERIODIC, (UINT32)(nFreq * 16384));
    return result;
}

/*********************************************************************
 * @fn
 * @brief
 * @param
 * @return
 */
PRIVATE BOOL EX_EmodeOFTStopNeiCell(VOID)
{
    BOOL result;

    result = COS_KillTimer(CSW_AT_TASK_HANDLE, AT_EX_MONP_TIM_ID);
    return result;
}

/**************************************************************************************************
**************************************************************************************************/

/*----------------------------------------PRIVATE FUNCTION----------------------------------------*/

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_MONI(AT_CMD_PARA *pParam)
{
    INT32 eResult = 0;
    UINT8 iParaCount;
    UINT16 nParaLen         = 0;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 tnMONIPara;
    UINT8 res[100];

    AT_TC(g_sw_SPEC, "MONI CmdType : %d\n", pParam->iType);
    switch (pParam->iType)
    {

    case AT_CMD_SET: // AT^MONI=<period>  1-127
        // check parameter count
        eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

        if ((eResult == ERR_SUCCESS) && (1 == iParaCount))
        {
            // Get parameter
            nParaLen = 1;
            eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &tnMONIPara, &nParaLen);

            // Get para success

            if (eResult == ERR_SUCCESS)
            {
                // check para valid or invalid
                if ((127 < tnMONIPara) || (1 > tnMONIPara))
                {
                    // para invalid,Create error result
                    eResult = ERR_INVALID_PARAMETER;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    // para valid,Create success result
                    gATCurrentu8nMONIPara = tnMONIPara;
                    AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);
                }
            }
            else
            {
                // Get parameter error,Create error result
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }

        }
        else
        {
            // Get parameter count error,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    case AT_CMD_READ:  // AT+MONI?
        // copy result info & Create success result
        AT_MemZero(res, 100);
        AT_Sprintf(res, "^MONI:%u", gATCurrentu8nMONIPara);
        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        break;

    case AT_CMD_EXE: // AT+MONI

        if (TRUE == EX_EmodeOFTStartSerCell(gATCurrentu8nMONIPara))
        {
            AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        }
        else
        {
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);  // Return fall
        }

        break;

    case AT_CMD_TEST:  // AT+MONI=?
        // copy result info & Create success result
        AT_MemZero(res, 100);
        AT_Sprintf(res, "^MONI:1-127");
        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        break;

    default: // error Type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "----------MONI Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/

VOID AT_EX_TimerFunc_MONI(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 eUTI            = pParam->nDLCI;
    PAT_CMD_RESULT pResult = NULL;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_SPEC, "AABB------------------------- MONI TIMER FUNCTION -------------------");

    // Get free UTI
    eResult = AT_GetFreeUTI(CFW_EMOD_SRV_ID, (UINT8 *)&eUTI);

    if (eResult == ERR_SUCCESS)
    {
        g_pSelecFUN_t->nServingCell  = 1;
        g_pSelecFUN_t->nNeighborCell = 0;

        // call csw function to execute this cmd

        if (ERR_SUCCESS == CFW_EmodOutfieldTestStart(g_pSelecFUN_t, eUTI, nSim))
        {
            // Create success result nead wait asynchronism
            AT_EX_SUCC_ASYN_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }
    }
    else
    {
        // Get TTI fall,Create error result
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_MONIEND(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;

    switch (pParam->iType)
    {
    case AT_CMD_EXE: // AT^MONIEND
        eResult = (UINT32)EX_EmodeOFTStopSerCell();

        if (eResult)
        {
            AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        }
        else
        {
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_TimerFunc_MONIEND(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 eUTI            = pParam->nDLCI;
    PAT_CMD_RESULT pResult = NULL;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Get free UTI
    eResult = AT_GetFreeUTI(CFW_EMOD_SRV_ID, (UINT8 *)&eUTI);

    if (eResult == ERR_SUCCESS)
    {
        // call csw function to execute this cmd
        if (ERR_SUCCESS == CFW_EmodOutfieldTestEnd(eUTI, nSim))
        {
            // Create success result nead wait asynchronism
            AT_EX_SUCC_ASYN_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }
    }
    else
    {
        // Get TTI fall,Create error result
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_MONP(AT_CMD_PARA *pParam)
{
    INT32 eResult = 0;
    UINT8 iParaCount;
    UINT16 nParaLen         = 0;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 tnMONPPara;
    UINT8 res[100];

    switch (pParam->iType)
    {
    case AT_CMD_SET: // AT^MONP=<period>  1-127
        // check parameter count
        eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

        if ((eResult == ERR_SUCCESS) && (1 == iParaCount))
        {
            // Get parameter
            nParaLen = 1;
            eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &tnMONPPara, &nParaLen);

            // Get para success

            if (eResult == ERR_SUCCESS)
            {
                // check para valid or invalid
                if ((127 < tnMONPPara) || (1 > tnMONPPara))
                {
                    // para invalid,Create error result
                    eResult = ERR_INVALID_PARAMETER;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    // para valid,Create success result
                    gATCurrentu8nMONPPara = tnMONPPara;
                    AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);
                }
            }
            else
            {
                // Get parameter error,Create error result
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }

        }
        else
        {
            // Get parameter count error,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    case AT_CMD_EXE: // AT^MONP

        if (TRUE == EX_EmodeOFTStartNerCell(gATCurrentu8nMONPPara))
        {
            AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        }
        else
        {
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);  // Return fall
        }

        break;

    case AT_CMD_READ:  // AT^MONP?
        // copy result info & Create success result
        AT_MemZero(res, 100);
        AT_Sprintf(res, "^MONP: %u", gATCurrentu8nMONPPara);
        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        break;

    case AT_CMD_TEST:  // AT^MONP=?
        // copy result info & Create success result
        AT_MemZero(res, 100);
        AT_Sprintf(res, "^MONP: 1-127");
        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        break;

    default: // error Type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_TimerFunc_MONP(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 eUTI            = pParam->nDLCI;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Get free UTI
    eResult = AT_GetFreeUTI(CFW_EMOD_SRV_ID, (UINT8 *)&eUTI);

    if (eResult == ERR_SUCCESS)
    {
        g_pSelecFUN_t->nServingCell  = 0;
        g_pSelecFUN_t->nNeighborCell = 1;

        // call csw function to execute this cmd

        if (ERR_SUCCESS == CFW_EmodOutfieldTestStart(g_pSelecFUN_t, eUTI, nSim))
        {
            // Create success result nead wait asynchronism
            AT_EX_SUCC_ASYN_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }
    }
    else
    {
        // Get TTI fall,Create error result
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_MONPEND(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;

    switch (pParam->iType)
    {
    case AT_CMD_EXE: // AT^MONPEND
        eResult = (UINT32)EX_EmodeOFTStopNeiCell();

        if (eResult)
        {
            AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        }
        else
        {
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);  // Return fall
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_TimerFunc_MONPEND(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 eUTI            = pParam->nDLCI;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Get free UTI
    eResult = AT_GetFreeUTI(CFW_EMOD_SRV_ID, (UINT8 *)&eUTI);

    if (eResult == ERR_SUCCESS)
    {
        // call csw function to execute this cmd
        if (ERR_SUCCESS == CFW_EmodOutfieldTestEnd(eUTI, nSim))
        {
            // Create success result nead wait asynchronism
            AT_EX_SUCC_ASYN_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }
    }
    else
    {
        // Get TTI fall,Create error result
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
}

/**************************************************************************************************
* @fn
*
* @brief
*
* @param
*
* @return
**************************************************************************************************/
// this command it the same as CIMI

VOID AT_EMOD_CmdFunc_SCID(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    if(pParam == NULL)
    {
        goto FAILE_EXIT;
    }
    AT_TC(g_sw_SPEC, "AABB----------SCID CmdType : %d----------\n", pParam->iType);
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    switch (pParam->iType)
    {
    case AT_CMD_EXE: // AT^SCID?
    {
        // call csw function to execute this cmd
        UINT8* pICCID = CFW_GetICCID(nSim);
        AT_TC(1, "pICCID =%x\n", pICCID);
        if(pICCID != NULL)
        {
            UINT8 ICCID[23] = {0};
            SUL_gsmbcd2asciiEx(pICCID,10, ICCID);
            AT_TC(1, "pICCID =%s\n", ICCID);
            UINT8 nResponse[30]={0};
            AT_Sprintf(nResponse, "+CCID: %s", ICCID);
            pResult = AT_CreateRC(CMD_FUNC_SUCC,CMD_RC_OK,CMD_ERROR_CODE_OK,CMD_ERROR_CODE_TYPE_CME,0,nResponse,strlen(nResponse),pParam->nDLCI);
        }
        else
        {
            eResult = CFW_SimGetICCID(pParam->nDLCI, nSim);
            AT_TC(g_sw_SPEC, "AABB----------SCID eResult: %d----------\n", eResult);
            if (eResult == ERR_SUCCESS)
            {
                AT_EX_SUCC_ASYN_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);
            }
            else
            {
                AT_TC(g_sw_SPEC, "AABB----55555------SCID CmdType : %d----------\n", pParam->iType);  // good trace  for bug 8527
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }
        }
    }
    break;
    default: // error Type
        // CMD type error, Create error result
FAILE_EXIT:
        eResult = ERR_AT_CME_PARAM_INVALID;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    AT_TC(g_sw_SPEC, "AABB----------SCID Cmd END------------\n");
    return;

    // AT_20071107_CAOW_E
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_DPBK(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    AT_TC(g_sw_SPEC, "AABB----------DPBK CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_EXE: // AT^MONIEND
        eResult = (UINT32)EX_EmodeClearPBKInfo();

        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "+DPBK:Flash PBK was cleared");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------DPBK Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_DREG(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    AT_TC(g_sw_SPEC, "AABB----------DREG CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_EXE: // AT^DREG
        eResult = EX_EmodeClearREGInfo();

        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "+DREG:REG was cleared");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------DREG Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_DFFS(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    switch (pParam->iType)
    {
    case AT_CMD_EXE: // AT^DFFS
        eResult = EX_EmodeClearFFSInfo();
        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "+DFFS:FS was cleared");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }
        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    AT_TC(g_sw_SPEC, "AABB----------DFFS Cmd END-----pParam->iType = %d--nDLCI =%d-----\n", pParam->iType, pParam->nDLCI);
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_DSMS(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    AT_TC(g_sw_SPEC, "----------DSMS CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_EXE: // AT^DSMS
        eResult = EX_EmodeClearSMSInfo();

        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "^SMS:Flash SMS was cleared");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------DSMS Cmd END------------\n");
}

// Add by wulc

VOID AT_EMOD_CmdFunc_SLCD(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    AT_TC(g_sw_SPEC, "----------DSMS CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_EXE: // AT^DSMS
        eResult = EX_EmodeClearSMSInfo();

        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "+SLCD");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------DSMS Cmd END------------\n");

}
VOID AT_EMOD_CmdFunc_STCD(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[50];

    AT_TC(g_sw_SPEC, "----------DSMS CmdType : %d----------\n", pParam->iType);

    switch (pParam->iType)
    {

    case AT_CMD_EXE: // AT^DSMS
        eResult = EX_EmodeClearSMSInfo();

        if (eResult)
        {
            AT_MemZero(res, 50);
            AT_Sprintf(res, "+STCD");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
        }
        else
        {
            // Csw Function execute fall,Create error result
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        }

        break;

    default: // error type
        // CMD type error, Create error result
        eResult = ERR_INVALID_TYPE;
        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        break;
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------DSMS Cmd END------------\n");

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_SBBI(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iParaCount;
    UINT16 nParaLen         = 0;
    PAT_CMD_RESULT pResult = NULL;
    BOOL nSIMMark;
    BOOL nNWMark;
    UINT8 tLen;
    UINT8 res[100];

    if (pParam == NULL)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET: // AT^SBBI=<bsimbindmark>,<bnwbindmark>
            // check parameter count
            eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

            if ((eResult == ERR_SUCCESS) && (2 == iParaCount))
            {
                // Get parameter
                nParaLen = 1;

                if ((ERR_SUCCESS == AT_Util_GetParaWithRule(pParam->pPara,
                        0,
                        AT_UTIL_PARA_TYPE_UINT8,
                        &nSIMMark,
                        &nParaLen))
                        && (ERR_SUCCESS == AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nNWMark, &nParaLen)))
                {
                    // Get all para success
                    // check para valid or invalid
                    if ((1 < nNWMark) || (0 > nNWMark) || (1 < nSIMMark) || (0 > nSIMMark))
                    {
                        // para invalid,Create error result
                        eResult = ERR_AT_CME_PARAM_INVALID;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }
                    else
                    {
                        // para valid, call csw function to process,
                        // Create success result
                        if (ERR_SUCCESS == CFW_EmodSetBindInfo(nSIMMark, nNWMark))
                            AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);
                        else
                        {
                            // Csw Function execute fall,Create error result
                            eResult = ERR_AT_CME_EXE_FAIL;
                            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        }
                    }
                }
                else
                {
                    // Get parameter error,Create error result
                    eResult = ERR_AT_CME_PARAM_INVALID;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
            }
            else
            {
                // Get parameter count error,Create error result
                eResult = ERR_AT_CME_PARAM_INVALID;
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }

            break;

        case AT_CMD_READ:  // AT^SBBI?
            // call csw function to execute this cmd
            CFW_EmodGetBindInfo(&nSIMMark, &nNWMark);

            // Create success result
            AT_MemZero(res, 100);

            if (nSIMMark == TRUE)
                AT_StrCpy(res, "^SBBI: SIM Bind:Enable,");
            else
                AT_StrCpy(res, "^SBBI: SIM Bind:Disable,");

            tLen = AT_StrLen(res);

            if (nNWMark == TRUE)
                AT_StrCpy(res + tLen, "NetWork Bind:Enable");
            else
                AT_StrCpy(res + tLen, "NetWork Bind:Disable");

            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

            break;

        case AT_CMD_TEST:  // AT^SBBI=?
            // copy result info & Create success result
            AT_MemZero(res, 100);

            AT_Sprintf(res, "^SBBI: SIM Bind:(0,1); NW Bind:(0,1)");

            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

            break;

        default: // error Type
            // CMD type error, Create error result
            eResult = ERR_AT_CME_EXE_NOT_SURPORT;

            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

            break;
        }
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_SBBR(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iParaCount;
    UINT16 nParaLen         = 0;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[400];
    UINT8 rTemp[50];
    CFW_EMOD_READ_BIND_SIM bindInfo;

    CFW_EMOD_BIND_SIM *pSim = NULL;
    void *phandle     = NULL;
    UINT8 nStartIndex = 0;
    UINT8 nEndIndex   = 0;
    UINT8 i;
    UINT8 j;
    UINT8 tflag = 0;
    UINT16 len  = 0;

    AT_TC(g_sw_SPEC, "AABB----------SBBR CmdType : %d----------\n", pParam->iType);

    if (pParam == NULL)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET: // READ
            eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

            if ((eResult != ERR_SUCCESS) || (iParaCount > 2) || (iParaCount < 1))
            {
                eResult = ERR_AT_CME_PARAM_INVALID;
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                if (iParaCount == 1)
                {
                    nParaLen = 1;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nStartIndex, &nParaLen);
                }
                else if (iParaCount == 2)
                {
                    nParaLen = 1;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nStartIndex, &nParaLen);

                    if (eResult == ERR_SUCCESS)
                    {
                        nParaLen = 1;
                        eResult  = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nEndIndex, &nParaLen);
                    }
                }

                if (ERR_SUCCESS != eResult)
                {
                    eResult = ERR_AT_CME_PARAM_INVALID;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    if (1 == iParaCount)
                    {
                        CFW_EmodReadBindSIM(&bindInfo);
                        pSim    = AT_MALLOC(sizeof(CFW_EMOD_BIND_SIM) * bindInfo.nNumber);
                        phandle = pSim;

                        if ((pSim != NULL) && (0 < nStartIndex) && (10 >= nStartIndex))
                        {
                            pSim = (CFW_EMOD_BIND_SIM *)bindInfo.nPointer;

                            for (j = 0; j < bindInfo.nNumber; j++)
                            {
                                if (nStartIndex == pSim->nIndex)
                                {
                                    AT_MemZero(res, 400);
                                    AT_MemZero(rTemp, 50);
                                    AT_Sprintf(rTemp, "+SBBR:  ");  // yangtt 2008-05-07 for bug 8301
                                    AT_StrCat(res, rTemp);
                                    AT_Sprintf(rTemp, "%u,  \"", pSim->nIndex);
                                    AT_StrCat(res, rTemp);

                                    for (i = 0; i < pSim->nImsiLen; i++)
                                    {
                                        AT_Sprintf(rTemp, "%u", pSim->pImsiNum[i] - 0x30);
                                        AT_StrCat(res, rTemp);
                                    }

                                    AT_Sprintf(rTemp, "\"");

                                    AT_StrCat(res, rTemp);
                                    tflag = 1;
                                    break;
                                }

                                pSim++;

                            }

                            // if no  pSim->nIndex = nStartIndex
                            if (tflag == 0)
                            {
                                AT_MemZero(res, 400);

                                // AT_Sprintf(  res,  "^SBBR: NULL"  );
                            }

                            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

                            AT_FREE(phandle);
                            phandle = NULL;

                        }
                        else
                        {

                            if (pSim != NULL)
                            {
                                AT_FREE(phandle);
                                phandle = NULL;
                            }

                            eResult = ERR_AT_CME_PARAM_INVALID;

                            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        }
                    }
                    else if (2 == iParaCount)
                    {
                        CFW_EmodReadBindSIM(&bindInfo);
                        pSim = AT_MALLOC(sizeof(CFW_EMOD_BIND_SIM) * bindInfo.nNumber);
                        AT_TC(g_sw_SPEC, "AABB----------bindInfo.nNumber =  %d----------\n", bindInfo.nNumber);
                        phandle = pSim;

                        if ((pSim != NULL)
                                && (0 < nStartIndex)
                                && (11 > nStartIndex) && (0 < nEndIndex) && (11 > nEndIndex) && (nEndIndex > nStartIndex))
                        {
                            pSim = (CFW_EMOD_BIND_SIM *)bindInfo.nPointer;
                            AT_MemZero(res, 400);
                            AT_MemZero(rTemp, 50);

                            // AT_Sprintf(  rTemp,  "^SBBR:  "  );
                            // AT_StrCat(  res,  rTemp  );

                            for (j = 0; j < bindInfo.nNumber; j++)
                            {
                                if ((pSim->nIndex >= nStartIndex) && (pSim->nIndex <= nEndIndex))
                                {
                                    AT_Sprintf(rTemp, "^SBBR:   %u,  \"", pSim->nIndex);
                                    AT_StrCat(res, rTemp);

                                    for (i = 0; i < pSim->nImsiLen; i++)
                                    {
                                        AT_Sprintf(rTemp, "%u", pSim->pImsiNum[i] - 0x30);
                                        AT_StrCat(res, rTemp);
                                    }

                                    // AT_20071207_CAOW_B
                                    // AT_20071224_CAOW_B for bug#7272
                                    if (j + 1 != nEndIndex)
                                        // AT_20071224_CAOW_E
                                    {
                                        AT_Sprintf(rTemp, "\"\r\n");
                                        AT_StrCat(res, rTemp);
                                    }
                                    else
                                    {
                                        AT_Sprintf(rTemp, "\"\r");
                                        AT_StrCat(res, rTemp);
                                    }

                                    // AT_20071207_CAOW_E
                                    tflag = 1;
                                }

                                pSim++;
                            }

                            // if no  pSim->nIndex  in[ nStartIndex,nEndIndex]
                            if (tflag == 0)
                            {
                                AT_MemZero(res, 400);
                                AT_Sprintf(res, "\n");  // add "\n"
                                // AT_Sprintf(  res,  "^SBBR:\r\n  NULL\n"  ); //add "\n"
                            }

                            // delect the laster "\n"
                            AT_MemZero(res + AT_StrLen(res) - 1, 1);

                            len = strlen(res);

                            AT_Util_TrimRspStringSuffixCrLf(res, &len);

                            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

                            AT_FREE(phandle);

                            phandle = NULL;
                        }
                        else
                        {
                            if (pSim != NULL)
                            {
                                AT_FREE(phandle);
                                phandle = NULL;
                            }

                            eResult = ERR_AT_CME_PARAM_INVALID;

                            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        }

                    }
                    else
                    {
                        eResult = ERR_AT_CME_PARAM_INVALID;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }

                }

            }

            break;

        case AT_CMD_TEST:  // AT^SBBR=?
            // copy result info & Create success result
            AT_MemZero(res, 400);
            AT_Sprintf(res, "+SBBR: 1-10,15");  // yangtt 2008-05-07 for bug 8301
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
            break;

        default: // error Type
            // CMD type error, Create error result
            eResult = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            break;
        }
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------SBBR Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_NBBR(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iParaCount;
    UINT16 nParaLen = 0;
    UINT8 res[400];
    UINT8 rTemp[50];
    UINT8 nStartIndex = 0;
    UINT8 nEndIndex   = 0;
    UINT8 i;
    UINT8 j;
    UINT8 tflag   = 0;
    void *phandle = NULL;
    CFW_EMOD_READ_BIND_NW bindInfo;

    CFW_EMOD_BIND_NW *pNw = NULL;
    PAT_CMD_RESULT pResult = NULL;
    UINT16 len             = 0;

    AT_TC(g_sw_SPEC, "AABB----------NBBR CmdType : %d----------\n", pParam->iType);

    if (pParam == NULL)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET: // READ

            eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

            if ((eResult != ERR_SUCCESS) || (iParaCount > 2) || (iParaCount < 1))
            {
                eResult = ERR_AT_CME_PARAM_INVALID;
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                if (iParaCount == 1)
                {
                    nParaLen = 1;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nStartIndex, &nParaLen);
                }
                else if (iParaCount == 2)
                {
                    nParaLen = 1;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nStartIndex, &nParaLen);

                    if (eResult == ERR_SUCCESS)
                    {
                        nParaLen = 1;
                        eResult  = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nEndIndex, &nParaLen);
                    }
                }

                if (ERR_SUCCESS != eResult)
                {
                    eResult = ERR_AT_CME_PARAM_INVALID;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    if (1 == iParaCount)
                    {
                        CFW_EmodReadBindNW(&bindInfo);
                        pNw     = AT_MALLOC(sizeof(CFW_EMOD_BIND_NW) * bindInfo.nNumber);
                        phandle = pNw;

                        if ((pNw != NULL) && (0 < nStartIndex) && (5 >= nStartIndex))
                        {
                            pNw = (CFW_EMOD_BIND_NW *)bindInfo.nPointer;

                            for (j = 0; j < bindInfo.nNumber; j++)
                            {
                                if (nStartIndex == pNw->nIndex)
                                {
                                    AT_MemZero(res, 400);
                                    AT_MemZero(rTemp, 50);
                                    AT_Sprintf(rTemp, "^NBBR:  ");
                                    AT_StrCat(res, rTemp);
                                    AT_Sprintf(rTemp, "%u,  \"", pNw->nIndex);
                                    AT_StrCat(res, rTemp);

                                    for (i = 0; i < 3; i++) // 3//3 adapt to the array length :pNw->pNccNum[3]
                                    {
                                        AT_Sprintf(rTemp, "%u", pNw->pNccNum[i] - 0x30);
                                        AT_StrCat(res, rTemp);
                                    }

                                    AT_Sprintf(rTemp, "\"");

                                    AT_StrCat(res, rTemp);
                                    tflag = 1;
                                    break;
                                }

                                pNw++;

                            }

                            // if no  pNw->nIndex = nStartIndex
                            if (tflag == 0)
                            {
                                AT_MemZero(res, 400);

                                // AT_Sprintf(  res,  "^SBBR:\r\n  NULL\n"  );
                            }

                            len = strlen(res);

                            AT_Util_TrimRspStringSuffixCrLf(res, &len);
                            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
                            AT_FREE(phandle);
                            phandle = NULL;

                        }
                        else
                        {
                            if (pNw != NULL)
                            {
                                AT_FREE(phandle);
                                phandle = NULL;
                            }

                            eResult = ERR_AT_CME_PARAM_INVALID;

                            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        }
                    }
                    else if (2 == iParaCount)
                    {
                        CFW_EmodReadBindNW(&bindInfo);
                        pNw     = AT_MALLOC(sizeof(CFW_EMOD_BIND_NW) * bindInfo.nNumber);
                        phandle = pNw;

                        if ((pNw != NULL)
                                && (0 < nStartIndex)
                                && (6 > nStartIndex) && (0 < nEndIndex) && (6 > nEndIndex) && (nEndIndex > nStartIndex))
                        {
                            pNw = (CFW_EMOD_BIND_NW *)bindInfo.nPointer;
                            AT_MemZero(res, 400);
                            AT_MemZero(rTemp, 50);

                            // AT_Sprintf(  rTemp,  "^NBBR:  "  );
                            // AT_StrCat(  res,  rTemp  );

                            for (j = 0; j < bindInfo.nNumber; j++)
                            {
                                if ((pNw->nIndex >= nStartIndex) && (pNw->nIndex <= nEndIndex))
                                {
                                    AT_Sprintf(rTemp, "^NBBR:   %u,  \"", pNw->nIndex);
                                    AT_StrCat(res, rTemp);

                                    for (i = 0; i < 3; i++) // 3//3 adapt to the array length :pNw->pNccNum[3]
                                    {
                                        AT_Sprintf(rTemp, "%u", pNw->pNccNum[i] - 0x30);
                                        AT_StrCat(res, rTemp);
                                    }

                                    // AT_20071207_CAOW_B
                                    // AT_20071224_CAOW_B for bug#7272
                                    if (j + 1 != nEndIndex)
                                        // AT_20071224_CAOW_E
                                    {
                                        AT_Sprintf(rTemp, "\"\r\n");
                                        AT_StrCat(res, rTemp);
                                    }
                                    else
                                    {
                                        AT_Sprintf(rTemp, "\"\r");
                                        AT_StrCat(res, rTemp);
                                    }

                                    // AT_20071207_CAOW_E
                                    tflag = 1;
                                }

                                pNw++;
                            }

                            // if no  pNw->nIndex  in[ nStartIndex,nEndIndex]
                            if (tflag == 0)
                            {
                                AT_MemZero(res, 400);
                                AT_Sprintf(res, "\n");  // add "\n"
                                // AT_Sprintf(  res,  "^SBBR:\r\n  NULL\n"  ); //add "\n"
                            }

                            // delect the laster "\n"
                            AT_MemZero(res + AT_StrLen(res) - 1, 1);

                            len = strlen(res);

                            AT_Util_TrimRspStringSuffixCrLf(res, &len);

                            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

                            AT_FREE(phandle);

                            phandle = NULL;
                        }
                        else
                        {
                            if (pNw != NULL)
                            {
                                AT_FREE(phandle);
                                phandle = NULL;
                            }

                            eResult = ERR_AT_CME_PARAM_INVALID;

                            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        }

                    }
                    else
                    {
                        eResult = ERR_AT_CME_PARAM_INVALID;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }
                }
            }

            break;

        case AT_CMD_TEST:  // AT^NBBR=?
            // copy result info & Create success result
            AT_MemZero(res, 400);
            AT_Sprintf(res, "^NBBR: 1-5,3");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
            break;

        default: // error Type
            // CMD type error, Create error result
            eResult = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            break;
        }
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------NBBR Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_SBBW(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iParaCount;
    UINT16 nParaLen = 0;
    UINT8 i;
    UINT8 nLen;
    UINT8 *pBindSIM        = NULL;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[100];
    UINT8 nIndex;
    UINT8 nBSimNum[30];
    BOOL tFlag = 1;
    CFW_EMOD_UPDATE_RETURN ret;

    AT_TC(g_sw_SPEC, "AABB----------SBBW CmdType : %d----------\n", pParam->iType);
    if (pParam == NULL)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET: // AT^SBBW=<nIndex>, <numbers>
            // get para num.
            eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

            if ((eResult != ERR_SUCCESS) || (iParaCount != 2))
            {
                eResult = ERR_AT_CME_PARAM_INVALID;
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                // get para value.
                nParaLen = 1;
                eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nIndex, &nParaLen);

                if (eResult == ERR_SUCCESS)
                {
                    nParaLen = 30;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, nBSimNum, &nParaLen);

                    // check para num

                    if (nParaLen != 15)
                        tFlag = 0;

                    // check para valid or invalid.
                    for (i = 0; i < nParaLen; i++)
                    {
                        if (0 > (nBSimNum[i] - 0x30) || 9 < (nBSimNum[i] - 0x30))
                            tFlag = 0;
                    }

                    if (10 < nIndex)
                        tFlag = 0;
                }

                // process
                if ((ERR_SUCCESS != eResult) || (tFlag == 0))
                {
                    eResult = ERR_AT_CME_PARAM_INVALID;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    // AT_Util_Trim(nBSimNum);
                    nLen     = AT_StrLen(nBSimNum);
                    pBindSIM = (UINT8 *)AT_MALLOC(nLen);

                    if (pBindSIM == NULL)
                    {

                        eResult = ERR_AT_CME_MEMORY_FAILURE;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                        break;
                    }

                    AT_MemZero(pBindSIM, nLen);

                    for (i = 0; i < nLen; i++)
                    {
                        *((UINT8 *)pBindSIM + i) = *((UINT8 *)nBSimNum + i) - 0x30;
                    }

                    AT_TC(g_sw_SPEC, "AABB----------nIndex = %d----------\n", nIndex);

                    // call csw function to write the information.
                    ret = CFW_EmodUpdateSimBind(nIndex, nBSimNum, AT_StrLen(nBSimNum));

                    AT_FREE(pBindSIM);
                    pBindSIM = NULL;

                    if (ERR_SUCCESS == ret.nErrCode)
                    {
                        AT_MemZero(res, 100);
                        AT_Sprintf(res, "^SBBW: nIndex=%u", ret.nIndex);
                        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);

                        AT_TC(g_sw_SPEC, "AABB----------ret.nIndex = %d----------\n", ret.nIndex);
                    }
                    else
                    {
                        eResult = ERR_AT_CME_EXE_FAIL;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }
                }
            }

            break;

        case AT_CMD_TEST:  // AT^SBBW=?
            // copy result info & Create success result
            AT_MemZero(res, 100);
            AT_Sprintf(res, "^SBBW: 1-10,15");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
            break;

        default: // error Type
            // CMD type error, Create error result
            eResult = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            break;
        }
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------SBBW Cmd END------------\n");
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EMOD_CmdFunc_NBBW(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iParaCount;
    UINT16 nParaLen = 0;
    UINT8 i;
    UINT8 nLen;
    UINT8 *pBindNW         = NULL;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 res[100];
    UINT8 nIndex;
    UINT8 nBNwNum[10];
    BOOL tFlag = 1;
    CFW_EMOD_UPDATE_RETURN ret;

    if (pParam == NULL)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET: // AT^NBBW=<nIndex>, <numbers>
            // get para num.
            eResult = AT_Util_GetParaCount(pParam->pPara, &iParaCount);

            if ((eResult != ERR_SUCCESS) || (iParaCount != 2))
            {
                eResult = ERR_AT_CME_PARAM_INVALID;
                AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                // get para value.
                nParaLen = 1;
                eResult  = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nIndex, &nParaLen);

                if (eResult == ERR_SUCCESS)
                {
                    nParaLen = 10;
                    eResult  = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, nBNwNum, &nParaLen);

                    // check para num

                    if (nParaLen != 3)
                        tFlag = 0;

                    // check para valid or invalid.
                    for (i = 0; i < nParaLen; i++)
                    {
                        if (0 > (nBNwNum[i] - 0x30) || 9 < (nBNwNum[i] - 0x30))
                            tFlag = 0;
                    }

                    if (5 < nIndex)
                        tFlag = 0;
                }

                // process
                if ((ERR_SUCCESS != eResult) || (tFlag == 0))
                {
                    eResult = ERR_AT_CME_PARAM_INVALID;
                    AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    // AT_Util_Trim(nBNwNum);
                    nLen    = AT_StrLen(nBNwNum);
                    pBindNW = (UINT8 *)AT_MALLOC(nLen);

                    if (pBindNW == NULL)
                    {
                        eResult = ERR_AT_CME_MEMORY_FAILURE;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }

                    AT_MemZero(pBindNW, nLen);

                    for (i = 0; i < nLen; i++)
                    {
                        *((UINT8 *)pBindNW + i) = *((UINT8 *)pBindNW + i) - 0x30;
                    }

                    AT_TC(g_sw_SPEC, "AABB----------nIndex = %d----------\n", nIndex);

                    // call csw function to write the information.
                    ret = CFW_EmodUpdateNwBind(nIndex, nBNwNum, AT_StrLen(nBNwNum));

                    AT_FREE(pBindNW);
                    pBindNW = NULL;

                    if (ERR_SUCCESS == ret.nErrCode)
                    {
                        AT_MemZero(res, 100);
                        AT_Sprintf(res, "^NBBW: nIndex=%u", ret.nIndex);
                        AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
                        AT_TC(g_sw_SPEC, "AABB----------ret.nIndex = %d----------\n", ret.nIndex);
                    }
                    else
                    {
                        eResult = ERR_AT_CME_EXE_FAIL;
                        AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
                    }
                }
            }

            break;

        case AT_CMD_TEST:  // AT^SBBW=?
            // copy result info & Create success result
            AT_MemZero(res, 100);
            AT_Sprintf(res, "^NBBW: 1-5, 3");
            AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res), pParam->nDLCI);
            break;

        default: // error Type
            // CMD type error, Create error result
            eResult = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            break;
        }
    }

    // send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    AT_TC(g_sw_SPEC, "AABB----------SBBW Cmd END------------\n");
}

VOID reg_BytesToString(UINT8 *bytes, UINT32 bytesSize, UINT8 *pString)
{
    UINT32 i = 0;

    memset(pString, 0x00, bytesSize * 2);

    for(i = 0; i < bytesSize; i++)
    {
        sprintf(pString, "%s%02x", pString, bytes[i]);
    }
}

UINT8 charToUint8(UINT8 c)
{
    if ( c >= '0' && c <= '9')
    {
        return  c - '0';
    }
    else if ( c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else if ( c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else
        return 0xff;
}

INT32 reg_StringToBytes(UINT8 *pString, UINT32 strSize, UINT8 *Hex)
{
    UINT8 tmp[3] = {0x0};
    UINT32 i = 0;
    UINT8 *p = Hex;
    UINT8 low4bit, high4bit;

    if ((strSize % 2) != 0)
    {
        return -1;
    }

    for (i = 0 ; i < strSize; i += 2)
    {
        strncpy(tmp, pString + i, 2);
        high4bit = charToUint8(tmp[0]);
        low4bit = charToUint8(tmp[1]);

        if ((high4bit == 0xff) || (low4bit == 0xff))
        {
            return -1;
        }

        *p++ = ((high4bit << 4) & 0xf0) | (low4bit & 0x0f);
        //AT_TC(g_sw_SPEC, "*p=%x", *(p-1));
    }
    //AT_TC_MEMBLOCK(g_sw_PBK, Hex, p-Hex, 16);
    return (p - Hex);
}

INT32 AT_CFGINIT_GetPara(UINT8 *pRes, INT32 *Count, INT32 *Index, UINT8 *pStr, UINT32 *pStrLen)
{
    INT32 iResult;
    UINT8 *pBeg = NULL;
    UINT8 *pEnd = NULL;
    UINT8 uParamCount = 0;
    UINT8 NumBuf[10] = {0,};
    //    INT32 ParaNum = 0;

    pBeg = pEnd = pRes;
    iResult = AT_Util_GetParaCount(pRes, &uParamCount);
    if ((ERR_SUCCESS != iResult) || (uParamCount != 3))
    {
        AT_TC(g_sw_GPRS, "AT+CFGINIT:NULL == pParam\n");
        return -1;
    }

    //get total count
    while(*pEnd != ',') pEnd++ ;
    AT_StrNCpy(NumBuf, pBeg, (pEnd - pBeg));
    *Count = atoi(NumBuf);

    //get index
    pBeg = ++pEnd;
    while(*pEnd != ',') pEnd++ ;
    AT_StrNCpy(NumBuf, pBeg, (pEnd - pBeg));
    *Index = atoi(NumBuf);

    //get get date from ap
    pEnd += 2;
    pBeg = pEnd;
    while (*pEnd != '"') pEnd++;

    if ((pEnd - pBeg) < *pStrLen)
    {
        *pStrLen = pEnd - pBeg;
    }

    AT_StrNCpy(pStr, pBeg, *pStrLen);
    return ERR_SUCCESS;

}

//add by lijingyu  Wed Jan  9 15:54:00 CST 2013
VOID AT_EMOD_CFGUPDATE_Indictation(COS_EVENT *pParam)
{
    UINT16 count = 0;
    UINT16 tmp = 0;
    UINT16 i = 0;
    INT32 iRet = 0;
    UINT32 read_size = 0;
    UINT16 index = 0;
    UINT8 nDLCI = 1;
    PAT_CMD_RESULT pResult = NULL;
    UINT8 read_data[CFG_SENDDATALEN + 1] = {0,};
    UINT8 send_data[CFG_SENDDATALEN + 1] = {0,};
    UINT8 CallBackData[CFG_SENDDATALEN + 25] = {0,};

    UINT32 uOffset = pParam->nParam1;
    UINT32 upd_size = pParam->nParam2;

    tmp = uOffset % (CFG_SENDDATALEN / 2);
    if (tmp != 0)
    {
        uOffset -= tmp;
        upd_size += tmp;
    }

    count += upd_size / (CFG_SENDDATALEN / 2);
    count += (upd_size % (CFG_SENDDATALEN / 2)) ? 1 : 0;
    index = uOffset / (CFG_SENDDATALEN / 2);

    for (i = 0; i < count; i++)
    {
        iRet = NV_Read(index * (CFG_SENDDATALEN / 2), read_data, CFG_SENDDATALEN / 2, &read_size);
        if (iRet != ERR_SUCCESS || &read_size < 0)
        {
            AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, nDLCI);
            AT_TC(g_sw_SPEC, "NV_Read failed. uOffset = %d", uOffset);
            break;
        }

        reg_BytesToString(read_data, read_size, send_data);
        sprintf(CallBackData, "+CFGIND:%d,%s", index, send_data);

        pResult = AT_CreateRC(CSW_IND_NOTIFICATION, CMD_RC_CR, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0 , CallBackData, AT_StrLen(CallBackData), nDLCI);

        AT_Notify2ATM(pResult, nDLCI);
        if (pResult != NULL)
        {
            AT_FREE(pResult);
            pResult = NULL;
        }

        index++;
    }
}


VOID AT_EMOD_CmdFunc_CFGINIT(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    INT32 Count = 0;
    INT32 Index = 0;
    INT32 Ret = 0;
    UINT32 LenWrited  = 0;
    UINT32 len = CFG_SENDDATALEN;
    UINT8 buf[CFG_SENDDATALEN + 1] = {0,};
    UINT8 WriteBuf[CFG_SENDDATALEN] = {0x00};

    Ret = AT_CFGINIT_GetPara(pParam->pPara, &Count, &Index, buf, &len);
    if (Ret != ERR_SUCCESS)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        goto end;
    }

    INT32 WriteBufSize = reg_StringToBytes(buf, len, WriteBuf);
    if (WriteBufSize < 0 || WriteBufSize != (len / 2))
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        goto end;
    }

    Ret = NV_Write(Index * CFG_SENDDATALEN / 2, WriteBuf, WriteBufSize, &LenWrited);
    if (WriteBufSize != LenWrited || Ret != ERR_SUCCESS)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        goto end;
    }

    AT_EX_SUCC_CreateRC(pResult, 0, 0, 0, pParam->nDLCI);

    if (Count == Index)
    {
#ifdef CHIP_HAS_AP
        cfg_set_ind_to_at(1);
#endif
    }

end:
    //send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    return ;
}


/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/

VOID AT_EMOD_CmdFunc_CFGGET(AT_CMD_PARA *pParam)
{
    INT32 Count = 0;
    PAT_CMD_RESULT pResult = NULL;
    UINT32 read_size = 0;
    UINT8 read_data[CFG_SENDDATALEN + 1] = {0,};
    UINT8 send_data[CFG_SENDDATALEN + 1] = {0,};
    UINT8 CallBackData[CFG_SENDDATALEN + 25] = {0,};
    INT32 index = atoi(pParam->pPara);
    INT32 iRet = 0;

    UINT32 cfw_nv_size = NV_GetNvSize();
    AT_TC(2, "cfw_nv_size = %d", cfw_nv_size);
    Count = (cfw_nv_size * 2) / CFG_SENDDATALEN + (((cfw_nv_size * 2 % CFG_SENDDATALEN) > 0) ? 0 : -1);

    if (Count < index)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        goto end;
    }

    iRet = NV_Read(index * (CFG_SENDDATALEN / 2), read_data, CFG_SENDDATALEN / 2, &read_size);
    if (iRet != ERR_SUCCESS || &read_size < 0)
    {
        AT_EX_FAIL_CreateRC(pResult, ERR_AT_CME_PARAM_INVALID , CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
        goto end;
    }

    reg_BytesToString(read_data, read_size, send_data);
    sprintf(CallBackData, "+CFGGET:%d,%d,%s", (INT)Count, (INT)index, send_data);
    AT_EX_SUCC_CreateRC(pResult, 0, CallBackData, AT_StrLen(CallBackData), pParam->nDLCI);

    if (Count == index)
    {
        AT_TC(g_sw_SPEC, "CFGINIT success,REG_SetIndAT 1");
#ifdef CHIP_HAS_AP
        cfg_set_ind_to_at(1);
#endif
    }

end:
    //send result to atm
    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    return ;
}

static UINT8 g_bt_status = 0;

VOID AT_EMOD_CmdFunc_BTSET(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 bt_state = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,};

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(Rsp, "+BTSET: (0,1)");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);

    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(Rsp, "+BTSET: %d", g_bt_status);
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (uParaCount != 1)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &bt_state, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (bt_state == 0)
        {
#ifdef __BT_RDABT__
            g_bt_status = 0;
            rdabt_antenna_off();
#endif
            OK_RETURN(pResult, "+BTSET: 0", pParam->nDLCI);
            return;
        }
        else if (bt_state == 1)
        {
            g_bt_status = 1;
#ifdef __BT_RDABT__
            rdabt_init();
            rdabt_set_uart_rx_cb(uart_SendDataBTCallBack);
            rdabt_antenna_on();
#endif
            OK_RETURN(pResult, "+BTSET: 1", pParam->nDLCI);
            return;
        }
        else
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }
}

//
//Only for test STK command
//Simulate sim message
//

#include "itf_api.h"
#include "itf_msgc.h"

VOID AT_EMOD_CmdFunc_TSTK(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 nTestStkCmdStr[250] = {0x00,};
    UINT16 nTestStkCmdStrLen = 250;
    UINT8 Rsp[20] = {0,};

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(Rsp, "+TSTK: (0,1)");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
        return;

    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(Rsp, "+TSTK: ");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uParaCount != 1)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, nTestStkCmdStr, &nTestStkCmdStrLen))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_SPEC, "TSTK Command simulate sim message\n ");
        AT_TC(g_sw_SPEC, "TSTK Command nTestStkCmdStrLen %d %s\n ", nTestStkCmdStrLen, nTestStkCmdStr);

        UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

        api_SimFetchInd_t *pFetchInd = NULL;

        UINT16 nStruSize = SIZEOF(api_SimFetchInd_t);

        //Systen will Free pSendSmsReq

        pFetchInd = (api_SimFetchInd_t *)AT_MALLOC(nStruSize);

        if(!pFetchInd)
        {
            AT_TC(g_sw_SPEC, "TSTK Command  malloc error!!! \n ");
            return;
        }


        SUL_ZeroMemory8(pFetchInd, nStruSize);

        AT_TC(g_sw_SPEC, "TSTK Command nStruSize %d\n ", nStruSize);

        pFetchInd->Length = SMS_PDUAsciiToBCD(nTestStkCmdStr, nTestStkCmdStrLen, pFetchInd->Data);

        CSW_TC_MEMBLOCK(CFW_SMS_TS_ID, (UINT8 *) pFetchInd->Data, pFetchInd->Length, 16);

        CFW_BalSendMessage(CFW_MBX, API_SIM_FETCH_IND, pFetchInd, nStruSize, nSim);
        AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        if (NULL != pResult)
        {
            AT_Notify2ATM(pResult, pParam->nDLCI);
            AT_FREE(pResult);
            pResult = NULL;
        }
        AT_FREE(pFetchInd);
    }
    else
    {
        ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        return;
    }
}



VOID AT_EMOD_CmdFunc_ASER(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 nASER = 0x00;
    UINT16 ASERLen = 250;
    UINT8 Rsp[20] = {0,};

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(Rsp, "+ASER: (0,1)");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
        return;

    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(Rsp, "+ASER: ");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uParaCount != 1)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nASER, &ASERLen))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_SPEC, "ASER Command nASER %d\n ",  nASER);


        if(EMOD_ASER_START == nASER)
        {
            //abb_SpiReg_Esd_RecoveryCheck_Start();
        }
        else
        {
            //abb_SpiReg_Esd_RecoveryCheck_Stop();
        }

        AT_EX_SUCC_CreateRC(pResult, 0, NULL, 0, pParam->nDLCI);  // Return ok
        if (NULL != pResult)
        {
            AT_Notify2ATM(pResult, pParam->nDLCI);
            AT_FREE(pResult);
            pResult = NULL;
        }
    }
    else
    {
        ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        return;
    }
}




BOOL _CheckITFValue(UINT8 nITF)
{
    //
    //0~7:     ApMode Sel = CALL
    //10~15：ApMode Sel = Music Play
    //
    if(((0 <= nITF)&&(nITF <= 7))||((10 <= nITF)&&(nITF <= 15)))
        return TRUE;
    else
        return FALSE;
}

VOID AT_EMOD_CmdFunc_CAVCP(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    UINT8 *nParam = NULL;
    nParam = (UINT8*)AT_MALLOC(1024);

    if(!nParam)
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);

    memset(nParam,0x00,1024);

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_CALIB_RETURN(pResult, "+CAVCP ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_CALIB_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (uParaCount != 3)
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1024;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_STRING, nParam, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        AT_TC(g_sw_SPEC, "------%s, param_len=%d", __FUNCTION__, param_len);
        AT_TC(g_sw_SPEC, "------%s, nParam len=%d", __FUNCTION__, strlen(nParam));

        if(!_CheckITFValue(nITF))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!((0 <= nCtrl)&&(nCtrl<=23)))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(nCtrl % 2 == 0)
        {
            if((AUD_ITF_RECEIVER <= nITF) && (nITF <= AUD_ITF_TV) && ((nITF) != (audioItf)))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
            if((nITF == AUD_ITF_BLUETOOTH) && (aud_GetBtNRECFlag()))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
            if((nITF == CALIB_ITF_BLUETOOTH_NREC) && (!aud_GetBtNRECFlag()))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
            if((nITF == AUD_ITF_EAR_PIECE) && (aud_GetForceReceiverMicSelectionFlag()))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
            if((nITF == CALIB_ITF_EAR_PIECE_THREE) && (!aud_GetForceReceiverMicSelectionFlag()))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
        }

        UINT8 *pParambuff;
        CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

        switch(nCtrl)
        {
        case 0:
            calib_SetAudioVocHPFParam(nITF, nParam, param_len);
            vpp_SpeechSetHpfPara(  &(calibPtr->audio_voc->voc[nITF].hpf));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 1:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocHPFParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 2:
            calib_SetAudioVocFIRTXParam(nITF, nParam, param_len);
            vois_SetMsdfReloadFlag();
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 3:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocFIRTXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 4:
            calib_SetAudioVocFIRRXParam(nITF, nParam, param_len);
            vois_SetMsdfReloadFlag();
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 5:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocFIRRXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 6:
            calib_SetAudioVocAECParam(nITF, nParam, param_len);
            vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[nITF].aec));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 7:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocAECParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 8:
            calib_SetAudioVocNSTXParam(nITF, nParam, param_len);
            vpp_SpeechSetNoiseSuppressParaTX(&(calibPtr->audio_voc->voc[nITF].speex.tx));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 9:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocNSTXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 10:
            calib_SetAudioVocNSRXParam(nITF, nParam, param_len);
            vpp_SpeechSetNoiseSuppressParaRX(&(calibPtr->audio_voc->voc[nITF].speex.rx));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 11:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocNSRXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 12:
            calib_SetAudioVocAGCTXParam(nITF, nParam, param_len);
            vpp_SpeechSetNonClipParaTX(&(calibPtr->audio_voc->voc[nITF].agc.tx));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 13:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocAGCTXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 14:
            calib_SetAudioVocAGCRXParam(nITF, nParam, param_len);
            vpp_SpeechSetNonClipParaRX(&(calibPtr->audio_voc->voc[nITF].agc.rx));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 15:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocAGCRXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 16:
            calib_SetAudioVocEQParam(nITF, nParam, param_len);
            vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[nITF].eq));
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 17:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocEQParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 18:
            calib_SetAudioVocDigGain(nITF, nParam, param_len);
            vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[nITF].gain), SND_SPK_VOL_7);
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 19:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioVocDigGain(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 20:
            calib_SetAudioVocWebRtcAGCTXParam(nITF, nParam, param_len);
            vpp_SpeechSetWebRtcAgcParaTx(&(calibPtr->audio_voc->voc[nITF].webrtcagc.tx));
            vpp_SpeechSetWebRtcAgcReloadFlag(1,0);
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 21:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocWebRtcAGCTXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 22:
            calib_SetAudioVocWebRtcAGCRXParam(nITF, nParam, param_len);
            vpp_SpeechSetWebRtcAgcParaRx(&(calibPtr->audio_voc->voc[nITF].webrtcagc.rx));
            vpp_SpeechSetWebRtcAgcReloadFlag(0,1);
            OK_CALIB_RETURN(pResult, "+CAVCP: ", pParam->nDLCI);

        case 23:
            strcpy(nParam,"+CAVCP: " );
            pParambuff = nParam+strlen("+CAVCP: ");
            *pParambuff = '"';
            param_len =  calib_GetAudioVocWebRtcAGCRXParam(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        default:
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

_exit:
    if (nParam)
    {
        AT_FREE(nParam);
        nParam = NULL;
    }

    return;
}


VOID AT_EMOD_CmdFunc_CACCP(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    UINT8 *nParam = NULL;
    nParam = (UINT8*)AT_MALLOC(1024);

    if(!nParam)
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);

    memset(nParam,0x00,1024);

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_CALIB_RETURN(pResult, "+CACCP ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_CALIB_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (uParaCount != 3)
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1024;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_STRING, nParam, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        AT_TC(g_sw_SPEC, "------%s, param_len=%d", __FUNCTION__, param_len);
        AT_TC(g_sw_SPEC, "------%s, nParam len=%d", __FUNCTION__, strlen(nParam));

        if(!_CheckITFValue(nITF) )
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!((0 <= nCtrl)&&(nCtrl<=7)))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(nCtrl % 2 == 0)
        {
            if ((0 <= nITF)&&(nITF <= 7))
            {
                if((AUD_ITF_RECEIVER <= nITF) && (nITF <= AUD_ITF_TV) && ((nITF) != (audioItf)))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }
                if((nITF == AUD_ITF_BLUETOOTH) && (aud_GetBtNRECFlag()))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }
                if((nITF == CALIB_ITF_BLUETOOTH_NREC) && (!aud_GetBtNRECFlag()))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }
                if((nITF == AUD_ITF_EAR_PIECE) && (aud_GetForceReceiverMicSelectionFlag()))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }
                if((nITF == CALIB_ITF_EAR_PIECE_THREE) && (!aud_GetForceReceiverMicSelectionFlag()))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }

            }
            else
            {
                if ((nITF-10) != (musicItf))
                {
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
                }
            }
        }

        UINT8 *pParambuff;
        AUD_LEVEL_T *pAudioLevelCfg = aud_CodecCommonGetLatestLevel();
        VOIS_AUDIO_CFG_T* pVoisAudioCfg = vois_GetAudioCfg();

        switch(nCtrl)
        {
        case 0:
        {
            if(nITF>=10)
            {
                calib_SetAudioCodecIngain(nITF-10, nParam,param_len );
                aud_Setup(nITF-10, pAudioLevelCfg);
            }
            else
            {
                calib_SetAudioCodecIngain(nITF, nParam,param_len );
                if(nITF == CALIB_ITF_EAR_PIECE_THREE)
                {
                    nITF = AUD_ITF_EAR_PIECE;
                }
                if(nITF == CALIB_ITF_BLUETOOTH_NREC)
                {
                    nITF = AUD_ITF_BLUETOOTH;
                }
                vois_Setup(nITF, pVoisAudioCfg);
            }
            OK_CALIB_RETURN(pResult, "+CACCP: ", pParam->nDLCI);
        }
        case 1:
        {
            strcpy(nParam,"+CACCP: " );
            pParambuff = nParam+strlen("+CACCP: ");
            *pParambuff = '"';
            if(nITF>=10)
                param_len = calib_GetAudioCodecIngain(nITF-10, pParambuff+1);
            else
                param_len = calib_GetAudioCodecIngain(nITF,  pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);
        }
        case 2:
        {
            if(nITF>=10)
            {
#ifdef CHIP_HAS_AP
                    calib_SetAudioCodecOutGains(nITF-10, nParam,param_len,TRUE );
#else
                    calib_SetAudioCodecOutGainsFP(nITF-10, nParam,param_len,TRUE );
#endif

                    aud_Setup(nITF-10, pAudioLevelCfg);
                }
                else
                {
#ifdef CHIP_HAS_AP
                    calib_SetAudioCodecOutGains(nITF, nParam,param_len , FALSE);
#else
                    calib_SetAudioCodecOutGainsFP(nITF, nParam,param_len , FALSE);
#endif
                    if(nITF == CALIB_ITF_EAR_PIECE_THREE)
                    {
                        nITF = AUD_ITF_EAR_PIECE;
                    }
                    if(nITF == CALIB_ITF_BLUETOOTH_NREC)
                    {
                        nITF = AUD_ITF_BLUETOOTH;
                    }
                    vois_Setup(nITF, pVoisAudioCfg);
                }
                OK_CALIB_RETURN(pResult, "+CACCP: ", pParam->nDLCI);
            }
            case 3:
            {
                strcpy(nParam,"+CACCP: " );
                pParambuff = nParam+strlen("+CACCP: ");
                *pParambuff ='"';
                if(nITF>=10)
#ifdef CHIP_HAS_AP
                    param_len = calib_GetAudioCodecOutgains(nITF-10, pParambuff+1, TRUE);
#else
                    param_len = calib_GetAudioCodecOutgainsFP(nITF-10, pParambuff+1, TRUE);
#endif
                else
#ifdef CHIP_HAS_AP
                    param_len = calib_GetAudioCodecOutgains(nITF, pParambuff+1, FALSE);
#else
                    param_len = calib_GetAudioCodecOutgainsFP(nITF, pParambuff+1, FALSE);
#endif
            pParambuff[param_len+1] = '"';
            pParambuff[param_len+2] = 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);
        }
        case 4:
        {

            if(nITF>=10)
            {
                calib_SetAudioCodecSidetone(nITF-10, nParam,param_len );
                aud_Setup(nITF-10, pAudioLevelCfg);
            }
            else
            {
                calib_SetAudioCodecSidetone(nITF, nParam ,param_len);
                if(nITF == CALIB_ITF_EAR_PIECE_THREE)
                {
                    nITF = AUD_ITF_EAR_PIECE;
                }
                if(nITF == CALIB_ITF_BLUETOOTH_NREC)
                {
                    nITF = AUD_ITF_BLUETOOTH;
                }
                vois_Setup(nITF, pVoisAudioCfg);
            }

            OK_CALIB_RETURN(pResult, "+CACCP: ", pParam->nDLCI);
        }
        case 5:
        {
            strcpy(nParam,"+CACCP: " );
            pParambuff = nParam+strlen("+CACCP: ");
            *pParambuff ='"';
            if(nITF>=10)
                param_len = calib_GetAudioCodecSidtone(nITF-10, pParambuff+1);
            else
                param_len = calib_GetAudioCodecSidtone(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);
        }

        case 6:
        {
            if(nITF>=10)
            {
                calib_SetAudioMusicInGainsRecord(nITF-10, nParam, param_len);
                aud_Setup(nITF-10, pAudioLevelCfg);
            }
            else
            {
                calib_SetAudioMusicInGainsRecord(nITF, nParam, param_len);
                if(nITF == CALIB_ITF_EAR_PIECE_THREE)
                {
                    nITF = AUD_ITF_EAR_PIECE;
                }
                if(nITF == CALIB_ITF_BLUETOOTH_NREC)
                {
                    nITF = AUD_ITF_BLUETOOTH;
                }
                vois_Setup(nITF, pVoisAudioCfg);
            }

            OK_CALIB_RETURN(pResult, "+CACCP: ", pParam->nDLCI);
        }
        case 7:
        {
            strcpy(nParam,"+CACCP: " );
            pParambuff = nParam+strlen("+CACCP: ");
            *pParambuff ='"';
            if(nITF>=10)
                param_len =calib_GetAudioMusicInGainsRecord(nITF-10, pParambuff+1);
            else
                param_len =calib_GetAudioMusicInGainsRecord(nITF, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);
        }

        default:
        {

            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        }
    }
    else
    {

        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

_exit:
    if (nParam)
    {
        AT_FREE(nParam);
        nParam = NULL;
    }

    return;
}

VOID AT_EMOD_CmdFunc_CAMCP(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    UINT8 *nParam = NULL;
    nParam = (UINT8*)AT_MALLOC(1024);

    if(!nParam)
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);

    memset(nParam,0x00,1024);

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_CALIB_RETURN(pResult, "+CAMCP ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_CALIB_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (uParaCount != 3)
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1024;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_STRING, nParam, &param_len))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        AT_TC(g_sw_SPEC, "------%s, param_len=%d", __FUNCTION__, param_len);
        AT_TC(g_sw_SPEC, "------%s, nParam len=%d", __FUNCTION__, strlen(nParam));

        if(!_CheckITFValue(nITF))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!((0 <= nCtrl)&&(nCtrl <= 5)))
        {
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(nCtrl % 2 == 0)
        {
            if((nITF-10) != (musicItf))
            {
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_ITF_DIFFERENT, pParam->nDLCI);
            }
        }

        UINT8 *pParambuff;

        switch(nCtrl)
        {
        case 0:
            calib_SetAudioMusicEQParam(nITF-10, nParam, param_len);
#ifndef CHIP_HAS_AP
            vpp_AudioMP3DecSetReloadFlag();
#endif
            OK_CALIB_RETURN(pResult, "+CAMCP: ", pParam->nDLCI);

        case 1:
            strcpy(nParam,"+CAMCP: " );
            pParambuff = nParam+strlen("+CAMCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioMusicEQParam(nITF-10, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        case 2:
            calib_SetAudioMusicALCParam(nITF-10, nParam, param_len);
            OK_CALIB_RETURN(pResult, "+CAMCP: ", pParam->nDLCI);

        case 3:
            strcpy(nParam,"+CAMCP: " );
            pParambuff = nParam+strlen("+CAMCP: ");
            *pParambuff = '"';
            param_len = calib_GetAudioMusicALCParam(nITF-10, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);


        default:
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

_exit:
    if (nParam)
    {
        AT_FREE(nParam);
        nParam = NULL;
    }

    return;
}

VOID AT_EMOD_CmdFunc_CAWTF(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_RETURN(pResult, "+CAWTF ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (uParaCount != 3)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }


        if(!(0 == nITF))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!(0 == nCtrl))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        switch(nCtrl)
        {
        case 0:
            if(!calib_BurnFlash())
            {
#ifdef CHIP_HAS_AP
                ERROR_RETURN(pResult, ERR_AT_CME_BURN_FLASH_FAIL, pParam->nDLCI);
#else
                ERROR_CALIB_RESET_RETURN(pResult, ERR_AT_CME_BURN_FLASH_FAIL, pParam->nDLCI);
                hal_TimDelay(5 MS_WAITING);
                hal_SysShutdown();
#endif
            }

#ifdef CHIP_HAS_AP
            OK_RETURN(pResult, "+CAWTF: ", pParam->nDLCI);
#else
            OK_CALIB_RESET_RETURN(pResult, "+CAWTF: ", pParam->nDLCI);
            hal_TimDelay(5 MS_WAITING);
#ifdef __VDS_QUICK_FLUSH__
            if(calib_GetDrvSuspendDisableFlag())
            {
                calib_SetDrvSuspendDisableFlag(FALSE);
                DRV_SuspendEnable();
            }
#endif
            hal_SysShutdown();
#endif

        default:
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }
}

PRIVATE VOID AT_FREE_Buffer(VOID)
{
    UINT8 *pBuffer = calib_GetBufferPointers();
    if (pBuffer)
    {
        sxr_Free(pBuffer);
        pBuffer = NULL;
    }

    return;
}

VOID AT_EMOD_CmdFunc_CAIET(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 nOffset = 0;
    UINT16 nLength = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    UINT8 *nParam = NULL;
    nParam = (UINT8*)AT_MALLOC(1024);

    if(!nParam)
    {
        AT_FREE_Buffer();
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

    memset(nParam,0x00,1024);

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_FREE_Buffer();
        OK_CALIB_RETURN(pResult, "+CAIET ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_FREE_Buffer();
        strcpy(Rsp, "Not Support");
        OK_CALIB_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (uParaCount != 5)
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 2;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT16, &nOffset, &param_len))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 2;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT16, &nLength, &param_len))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1024;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_STRING, nParam, &param_len))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        AT_TC(g_sw_SPEC, "nOffset=%d,nLength=%d", nOffset,nLength);

        if(!(0 == nITF))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!((0 <= nCtrl)&&(nCtrl <= 1)))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if((nOffset < 0))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if((nLength < 0))
        {
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        UINT8 *pParambuff;

        switch(nCtrl)
        {
        case 0:
            if((nLength * 2) != param_len)
            {
                AT_FREE_Buffer();
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            }
            else if((nOffset + nLength) > sizeof(CALIB_BUFFER_T))
            {
                AT_FREE_Buffer();
                ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            }
            else if((nOffset + nLength) == sizeof(CALIB_BUFFER_T))
            {
                if(!calib_SetBufferData(nOffset, nLength, nParam))
                {
                    AT_FREE_Buffer();
                    ERROR_CALIB_RESET_RETURN(pResult, ERR_AT_CME_BURN_FLASH_FAIL, pParam->nDLCI);
                    hal_TimDelay(5 MS_WAITING);
                    hal_SysShutdown();
                }

                OK_CALIB_RESET_RETURN(pResult, "+CAIET: ", pParam->nDLCI);
                hal_TimDelay(5 MS_WAITING);
#ifdef __VDS_QUICK_FLUSH__
                if(calib_GetDrvSuspendDisableFlag())
                {
                    calib_SetDrvSuspendDisableFlag(FALSE);
                    DRV_SuspendEnable();
                }
#endif
                hal_SysShutdown();
            }
            else
            {
                if(!calib_SetBufferData(nOffset, nLength, nParam))
                {
                    AT_FREE_Buffer();
                    ERROR_CALIB_RETURN(pResult, ERR_AT_CME_BURN_FLASH_FAIL, pParam->nDLCI);
                }

                OK_CALIB_RETURN(pResult, "+CAIET: ", pParam->nDLCI);
            }

        case 1:
            strcpy(nParam,"+CAIET: " );
            pParambuff = nParam+strlen("+CAIET: ");
            *pParambuff = '"';
            param_len = calib_GetBufferData(nOffset, nLength, pParambuff+1);
            pParambuff[param_len+1]= '"';
            pParambuff[param_len+2]= 0;
            OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

        default:
            AT_FREE_Buffer();
            ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        AT_FREE_Buffer();
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

_exit:
    if (nParam)
    {
        AT_FREE(nParam);
        nParam = NULL;
    }

    return;
}

VOID AT_EMOD_CmdFunc_CAVCT(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,};
    UINT8 *pParambuff;

    UINT8 *nParam = NULL;
    nParam = (UINT8*)AT_MALLOC(1024);

    if(!nParam)
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);

    memset(nParam,0x00,1024);

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_CALIB_RETURN(pResult, "+CAVCT ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_CALIB_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (uParaCount != 3)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }


        if(!(0 == nITF))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!(0 == nCtrl))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        strcpy(nParam,"+CAVCT: " );
        pParambuff = nParam+strlen("+CAVCT: ");
        *pParambuff = '"';
        param_len = calib_GetAudioVersionParam(pParambuff+1);
        pParambuff[param_len+1]= '"';
        pParambuff[param_len+2]= 0;
        OK_CALIB_RETURN(pResult, nParam, pParam->nDLCI);

    }
    else
    {
        ERROR_CALIB_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }

_exit:
    if (nParam)
    {
        AT_FREE(nParam);
        nParam = NULL;
    }

    return;
}

#ifdef VOIS_DUMP_PCM
VOID AT_EMOD_CmdFunc_CADTF(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 uParaCount = 0;
    UINT8 iIndex = 0;
    UINT8 nITF = 0;
    UINT8 nCtrl = 0;
    UINT16 param_len = 1;
    UINT8 Rsp[20] = {0,} ;

    if (pParam->iType == AT_CMD_TEST)
    {
        OK_RETURN(pResult, "+CADTF ", pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        strcpy(Rsp, "Not Support");
        OK_RETURN(pResult, Rsp, pParam->nDLCI);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (uParaCount != 3)
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nITF, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        param_len = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, iIndex++, AT_UTIL_PARA_TYPE_UINT8, &nCtrl, &param_len))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }


        if(!(0 == nITF))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        if(!((0 <= nCtrl)&&(nCtrl <= 1)))
        {
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }

        switch(nCtrl)
        {
        case 0:
#ifndef CHIP_HAS_AP
            vois_DumpPcmDataToTFlashStop();
            OK_RETURN(pResult, "+CADTF: ", pParam->nDLCI);
#else
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
#endif

        case 1:
#ifndef CHIP_HAS_AP
#ifndef ONLY_AT_SUPPORT
            if(checkTCardExist())
#endif
            {
                vois_DumpPcmDataToTFlashStart();
                OK_RETURN(pResult, "+CADTF: ", pParam->nDLCI);
            }
#ifndef ONLY_AT_SUPPORT
            else
                ERROR_RETURN(pResult, ERR_AT_CME_TFLASH_NOT_EXIST, pParam->nDLCI);
#endif
#else
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
#endif

        default:
            ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
        }
    }
    else
    {
        ERROR_RETURN(pResult, ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
    }
}

#endif

//
//  Return the up time of the system in milliseconds.
//
VOID AT_EMOD_CmdFunc_UPTIME(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        UINT32 ms = COS_Tick2Msec(COS_GetTickCount());

        UINT8 resp[50];
        AT_Sprintf(resp, "^UPTIME: %d", ms);
        at_CmdRespInfoText(pParam->engine, resp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
