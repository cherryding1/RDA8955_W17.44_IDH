/*********************************************************
 *
 * File Name
 *  ?
 * Author
 *  ?
 * Date
 *  2007/11/25
 * Descriptions:
 *  ...
 *
 *********************************************************/

#include "at.h"
#include "at_module.h"
#include "at_cmd_nw.h"
#include "at_cmd_gc.h"
#include "at_cfg.h"

UINT8 g_uATNWECSQFlag   = 0;
UINT32 g_COPN_Index      = 1;
UINT8 g_COPS_Mode       = 2;

UINT8 g_FirstREG                    = 1;
UINT8 g_COPS_OperID[6]              = { 0 };
UINT8 g_COPS_OperID_Temp[6]         = { 0 };
extern BOOL Speeching;  // added by yangtt at 2008-05-05 for bug 8227
extern UINT8 g_cfg_cfun[2];  // added by yangtt at 2008-05-05 for bug 8411
extern BOOL g_auto_calib;
extern UINT8 g_set_ACLB;
extern UINT8 g_ACLB_FM;
extern UINT8 g_ACLB_SIM1_FM;
extern BOOL gATSATLocalInfoFlag[];
extern BOOL gATSATComQualifier[];

UINT8 g_ucFlag = 0x00;
#ifdef LTE_NBIOT_SUPPORT
UINT8 g_uAtCscon = 0;
UINT8 g_nwEdrxPtw = 0;
UINT8 g_nwEdrxValue = 0;
#endif

extern UINT32 _SetACLBStatus(UINT8 nACLBNWStatus);
VOID AT_NW_GetStoreListString(UINT8 nSim, UINT8 *nDataString);
extern UINT32 CFW_NW_SendPLMNList(CFW_StoredPlmnList *plmnlist, UINT8 nSimID);

// AT_20071112_CAOW_E
#ifdef AT_FTP_SUPPORT
BOOL AT_SetOTATimer(UINT32 nElapse);
#endif
AT_CS_STAUTS gCsStauts;

VOID cfw_PLMNtoBCD(UINT8 *pIn, UINT8 *pOut, UINT8 *nInLength);
#ifdef AT_USER_DBS
VOID AT_NW_POSI_AddData(UINT8 nflag, CFW_TSM_CURR_CELL_INFO *pCurrCellInfo,
                        CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo);
VOID AT_NW_POSI_SendData(UINT8 nSim);

#endif
VOID AT_NW_Result_OK(UINT32 uReturnValue,
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

VOID AT_NW_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim)
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

BOOL NW_OperaterStrToBytes(UINT8 *pDestBytes, UINT8 *pSource)
{
    if ((pDestBytes == NULL) || (pSource == NULL))
    {
        return FALSE;
    }

    while (*pSource != 0)
    {
        //if ((*pSource < '0') || (*pSource > '9')||(*pSource!='f')||(*pSource!='F'))
        {
            // return FALSE;
        }
        if((*pSource >= '0') && (*pSource <= '9'))
        {
            *pDestBytes++ = *pSource++ - '0';
        }
        else if((*pSource == 'f') || (*pSource == 'F'))
        {
            *pDestBytes++ = 0x0f;
            pSource++;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;

}

UINT8 Gprs_APN[8];
UINT8 Gprs_APN_Len = 0x00;

VOID NW_GetPreferredOperators(UINT8 nDLCI, UINT8 nSim)
{
    // UINT32 nResult = CFW_SimGetPrefOperatorListEX(1,0);
    UINT32 nResult = CFW_SimGetPrefOperatorList(nDLCI, nSim);

    if (ERR_SUCCESS == nResult)
    {
        AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_AT_NW, "nw-CFW_SimGetPrefOperatorList error\n");
        UINT32 nErrorCode = AT_SetCmeErrorCode(nResult, FALSE);

        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, nDLCI, nSim);
        return;
    }

    return;
}

// for +cpol=?  get the max num of preferred operators
VOID NW_AsyncEventProcess_GetPreferredOperatorMaxNum(CFW_EVENT cfwEvent)
{
    UINT32 nErrorCode = 0;

    UINT8 nSim = cfwEvent.nFlag;
    AT_TC(g_sw_AT_NW, "NW-EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP come!\n");

    if (cfwEvent.nType == 0)
    {
        UINT8 arrRes[30];

        AT_MemZero(arrRes, sizeof(arrRes));
        //AT_Sprintf(arrRes, "+CPOL: (1-%u),(0,2)", cfwEvent.nParam1);
        AT_Sprintf(arrRes, "+CPOL: (1-%u),2", cfwEvent.nParam1);
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), cfwEvent.nUTI, nSim);
        return;
    }
    else if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP error");
        nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP:  error!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

}

VOID NW_AsyncEventProcess_GetPreferredOperators_Set(CFW_EVENT cfwEvent)
{

    UINT8 *pTemp         = (UINT8 *)(cfwEvent.nParam1);
    UINT8 nOperatorCount = cfwEvent.nParam2;
    UINT8 nSim = cfwEvent.nFlag;

    if ((gATCurrentnPreferredOperatorIndex > nOperatorCount + 9)
            || ((gATCurrentnPreferredOperatorIndex == 0) && (gATCurrentpArrPreferredOperater[0] == 0xff)))
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEventProcess_GetPreferredOperators_Set:  param error!!!\n\r");
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

    if (gATCurrentnPreferredOperatorIndex == 0)
    {
        UINT8 nTemp = 0;

        for (nTemp = 0; nTemp < nOperatorCount; nTemp++)

        {
            if (pTemp[nTemp * 6] == 0xff)
            {
                gATCurrentnPreferredOperatorIndex = nTemp + 1;
                break;
            }
        }

        if (gATCurrentnPreferredOperatorIndex == 0)
        {
            AT_TC(g_sw_AT_NW, "NW_AsyncEventProcess_GetPreferredOperators_Set error:  full !!!\n\r");
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }
    }

    UINT8 nTemp = 0;

    UINT8 nTempBase = 0;

    for (nTemp = 0; nTemp < nOperatorCount; nTemp++)
    {
        nTempBase = 6 * nTemp;
        AT_TC(g_sw_AT_NW, "NW-get -no.%d--0x%x-%x-%x-%x-%x-%x!\n",
              nTemp + 1, pTemp[nTempBase], pTemp[nTempBase + 1], pTemp[nTempBase + 2],
              pTemp[nTempBase + 3], pTemp[nTempBase + 4], pTemp[nTempBase + 5]);
    }

    UINT8 *arrOperatorList = NULL ;
    UINT32 narrOperatorListSize = 0x00;

    if(nOperatorCount < gATCurrentnPreferredOperatorIndex)
    {
        nOperatorCount = gATCurrentnPreferredOperatorIndex;
    }

    if(nOperatorCount)
    {
        narrOperatorListSize = nOperatorCount * 6;
    }
    else
    {
        narrOperatorListSize = 6;
    }

    arrOperatorList = (UINT8 *)AT_MALLOC(narrOperatorListSize);

    AT_MemSet(arrOperatorList, 0xff, narrOperatorListSize);
    AT_MemCpy(arrOperatorList, pTemp, narrOperatorListSize);
    AT_MemCpy(&(arrOperatorList[(gATCurrentnPreferredOperatorIndex - 1) * 6]), gATCurrentpArrPreferredOperater,
              sizeof(gATCurrentpArrPreferredOperater));

    pTemp     = arrOperatorList;
    nTempBase = 0;

    for (nTemp = 0; nTemp < nOperatorCount; nTemp++)
    {
        nTempBase = 6 * nTemp;
        AT_TC(g_sw_AT_NW, "NW-to set -no.%d--0x%x-%x-%x-%x-%x-%x!\n",
              nTemp + 1, pTemp[nTempBase], pTemp[nTempBase + 1], pTemp[nTempBase + 2],
              pTemp[nTempBase + 3], pTemp[nTempBase + 4], pTemp[nTempBase + 5]);
    }
    AT_TC(g_sw_AT_NW, "NW_AsyncEventProcess_GetPreferredOperators_Set  sizeof(arrOperatorList) %d\n\r", narrOperatorListSize);

    AT_TC(g_sw_AT_NW, "gATCurrentnPreferredOperatorIndex %d\n\r", gATCurrentnPreferredOperatorIndex);

    AT_TC_MEMBLOCK(g_sw_AT_NW, arrOperatorList, narrOperatorListSize, 16);

    AT_TC_MEMBLOCK(g_sw_AT_NW, gATCurrentpArrPreferredOperater, sizeof(gATCurrentpArrPreferredOperater), 16);

    UINT32 nResult = CFW_SimSetPrefOperatorList(arrOperatorList, narrOperatorListSize, cfwEvent.nUTI, nSim);
    AT_FREE(arrOperatorList);
    if (ERR_SUCCESS == nResult)
    {
        AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_AT_NW, "nw-CFW_SimSetPrefOperatorList error\n");
        UINT32 nErrorCode = AT_SetCmeErrorCode(nResult, FALSE);

        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

    return;
}


void at_CPOL_Read(CFW_EVENT cfwEvent)
{
    UINT8 nSimID = cfwEvent.nFlag;

    AT_TC(g_sw_AT_NW, "NW-EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP");
    UINT8 *data = (UINT8*)(cfwEvent.nParam1);
    UINT16 length = cfwEvent.nParam2;
    AT_TC(g_sw_AT_NW, "length = %d", length);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, data, length, 'c');

    UINT8 response[400] = {0};
    UINT16 j = 0;

    for(UINT16 i = 0; i < length; i += 3)
    {
        AT_TC(g_sw_AT_NW, "i = %d, j = %d", i, j);
        CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, data + i, 6, "c");
        if((data[i] != 0xFF) && (data[i + 1] != 0xFF) && (data[i + 2] != 0xFF))
        {
            UINT8 format = NW_PREFERRED_OPERATOR_FORMAT_NUMERIC;
            UINT8 index = i / 3 + 1;
            if ((data[i + 1] & 0xF0) == 0xF0)
            {
                AT_Sprintf(response, "+CPOL: %u,%u,\"%u%u%u%u%u\"\r\n", index, format,
                     data[i] & 0x0F, data[i] >> 4, data[i + 1] & 0x0F, 
                     data[i + 2] & 0x0F, data[i + 2] >> 4);
            }
            else
            {
                AT_Sprintf(response, "+CPOL: %u,%u,\"%u%u%u%u%u%u\"\r\n", index, format,
                     data[i] & 0x0F, data[i] >> 4, data[i + 1] & 0x0F,
                     data[i + 2] & 0x0F, data[i + 2] >> 4, data[i + 1] >> 4);
            }
            AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, response, AT_StrLen(response), cfwEvent.nUTI, nSimID);
        }
    }

    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSimID);
    return;
}
VOID NW_AsyncEventProcess_GetPreferredOperators_Read(CFW_EVENT cfwEvent)
{
    UINT8 arrRes[100];
    UINT32 nResult = ERR_SUCCESS;
    UINT8 arrOperatorName[17];
    UINT8 nTemp          = 0;
    UINT8 *pTemp         = (UINT8 *)(cfwEvent.nParam1);
    UINT8 nOperatorCount = cfwEvent.nParam2;
    UINT8 nTempBase      = 0;
    UINT8 *pOperatorName = NULL;

    UINT8 nSim = cfwEvent.nFlag;
    AT_TC(g_sw_AT_NW, "NW-EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP ---%d!\n", nOperatorCount);

    for (nTemp = 0; nTemp < nOperatorCount; nTemp++)
    {
        nTempBase = 6 * nTemp;
        AT_TC(g_sw_AT_NW, "NW-NW_AsyncEventProcess_GetPreferredOperators_Read -no.%d--0x%x-%x-%x-%x-%x-%x!\n",
              nTemp + 1, pTemp[nTempBase], pTemp[nTempBase + 1], pTemp[nTempBase + 2],
              pTemp[nTempBase + 3], pTemp[nTempBase + 4], pTemp[nTempBase + 5]);
    }

    for (nTemp = 0; nTemp < nOperatorCount; nTemp++)
    {
        nTempBase = 6 * nTemp;

        if (pTemp[nTempBase] == 0xFF)
        {
            continue;
        }

        AT_MemZero(arrRes, sizeof(arrRes));

        if(pTemp[nTempBase ] != 0x0F)
        {

            if (pTemp[nTempBase + 5] != 0x0F)
            {
                AT_Sprintf(arrRes, "+CPOL: %u,%u,\"%u%u%u%u%u%u\"", nTemp + 1, NW_PREFERRED_OPERATOR_FORMAT_NUMERIC,
                           pTemp[nTempBase], pTemp[nTempBase + 1], pTemp[nTempBase + 2],
                           pTemp[nTempBase + 3], pTemp[nTempBase + 4], pTemp[nTempBase + 5]);
            }
            else
            {
                AT_Sprintf(arrRes, "+CPOL: %u,%u,\"%u%u%u%u%u\"", nTemp + 1, NW_PREFERRED_OPERATOR_FORMAT_NUMERIC,
                           pTemp[nTempBase], pTemp[nTempBase + 1], pTemp[nTempBase + 2],
                           pTemp[nTempBase + 3], pTemp[nTempBase + 4]);
            }

        }

        if (gATCurrentnPreferredOperFormat == NW_PREFERRED_OPERATOR_FORMAT_ALPHANUMERIC_LONG)
        {
            AT_MemZero(arrOperatorName, sizeof(arrOperatorName));
            pOperatorName = arrOperatorName;
            nResult       = CFW_CfgNwGetOperatorName(pTemp + nTempBase, &pOperatorName);

            if (nResult == ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "CFW_CfgNwGetOperatorName---success!\n");
                AT_MemZero(arrRes, sizeof(arrRes));
                AT_Sprintf(arrRes, "+CPOL: %u,%u,\"%s\"", nTemp + 1, NW_PREFERRED_OPERATOR_FORMAT_ALPHANUMERIC_LONG,
                           pOperatorName);
            }
            else
            {
                AT_TC(g_sw_AT_NW, "CFW_CfgNwGetOperatorName---error!\n");
            }
        }
        AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), cfwEvent.nUTI, nSim);

    }

    AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, cfwEvent.nUTI, nSim);
    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
    return;
}

// for +cpol?  get the preferred operators
VOID NW_AsyncEventProcess_GetPreferredOperators(CFW_EVENT cfwEvent)
{
    UINT32 nErrorCode = 0;
    UINT8 nSim = cfwEvent.nFlag;

    if (cfwEvent.nType == 0)
    {
        if (gATCurrentnGetPreferredOperatorsCaller == NW_GETPREFERREDOPERATORS_CALLER_READ)
        {
            //NW_AsyncEventProcess_GetPreferredOperators_Read(cfwEvent);
            at_CPOL_Read(cfwEvent);
            return;
        }
        else if (gATCurrentnGetPreferredOperatorsCaller == NW_GETPREFERREDOPERATORS_CALLER_SET)
        {
            NW_AsyncEventProcess_GetPreferredOperators_Set(cfwEvent);
            return;
        }

    }
    else if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP error");
        nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP:  error!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

}

// for +cpol,  add or delete the preferred operator
VOID NW_AsyncEventProcess_SetPreferredOperators(CFW_EVENT cfwEvent)
{
    UINT32 nErrorCode = 0;

    UINT8 nSim = cfwEvent.nFlag;
    AT_TC(g_sw_AT_NW, "NW-EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP come!\n");

    if (cfwEvent.nType == 0)
    {
        AT_TC(g_sw_AT_NW, "NW-cpol set success!\n");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
        return;
    }
    else if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP error");
        nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_TC(g_sw_AT_NW, "EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP:  error!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

}
extern BOOL gbPowerCommMsgFlag[];
VOID NW_AsyncEventProcess_SetCom(CFW_EVENT cfwEvent)
{
    UINT8 nSim = cfwEvent.nFlag;
    UINT8 nAclb_step = _GetACLBStep();
    UINT8 nEventFromAT = AT_IsAsynCmdAvailable("+CFUN", 0, cfwEvent.nUTI);
    AT_TC(g_sw_AT_NW, "NW_AsyncEventProcess_SetCom: start = %d,nType = %d,ACLBStep = %d,nEventFromAT = %d\n", nSim,cfwEvent.nType,nAclb_step,nEventFromAT);
    if (cfwEvent.nType != 0)
    {
        AT_TC(g_sw_AT_NW, "AsyncEventProcess_SetCom: CfwEvent.nType = %d", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    if( 0xfe == cfwEvent.nParam2 )
    {
        AT_TC(g_sw_AT_NW, "AsyncEventProcess_SetCom:change Rat succes");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
        return ;
    }
    if(CFW_SIM_0 == nSim)
    {
        switch (nAclb_step)
        {
        case 1:
        {
            _SetACLBStatus(TRUE);
            _SetACLBStep(2);
            CFW_SetComm(CFW_ENABLE_COMM, 0, cfwEvent.nUTI, nSim);
        }
        break;
        case 2:
        {
            CFW_COMM_MODE nMode;
            CFW_GetComm( &nMode , nSim);
            if(nMode == CFW_ENABLE_COMM)
            {
                _SetACLBStep(3);
                CFW_SetComm(CFW_DISABLE_COMM, 0, cfwEvent.nUTI, nSim);
                return ;
            }
        }
        break;

        case 3:
        {
            VOID calib_DaemonUsrDataProcess(VOID);
            calib_DaemonUsrDataProcess();
            if( CFW_ENABLE_COMM == _GetACLBNWStatus(nSim)/*g_ACLB_FM*/)
            {
                _SetACLBStep(4);
                _SetACLBStatus(FALSE);
                CFW_SetComm(CFW_ENABLE_COMM, 0, cfwEvent.nUTI, nSim);
            }
            else
            {
                _SetACLBStatus(FALSE);
                _GetACLBStep(0);
                _SetACLBNWStatus(CFW_CHECK_COMM,nSim);
                if(CFW_ENABLE_COMM == g_ACLB_SIM1_FM )
                {
                    g_ACLB_SIM1_FM = CFW_CHECK_COMM;
                    CFW_SetComm(CFW_ENABLE_COMM, 0, cfwEvent.nUTI, 1);
                }
                // if(nEventFromAT == TRUE)
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
            }
        }
        break;
        case 4:
        {
            _SetACLBStatus(FALSE);
            _SetACLBStep ( 0);
            _SetACLBNWStatus(CFW_CHECK_COMM,nSim);
            //if(nEventFromAT == TRUE)
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);

            if(CFW_ENABLE_COMM == g_ACLB_SIM1_FM )
            {
                g_ACLB_SIM1_FM = CFW_CHECK_COMM;
                CFW_SetComm(CFW_ENABLE_COMM, 0, cfwEvent.nUTI, 1);
            }
        }
        break;
        default:
        {
            if(nEventFromAT == TRUE)
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
        }
        break;
        }
    }
    else if((nAclb_step > 0) && (1 == nSim))
    {
    }
    else if((0 == nAclb_step) && (1 == nSim))
    {
        if(nEventFromAT == TRUE)
        {
            AT_TC(g_sw_AT_NW, "---------------nEventFromAT------------------");
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
        }
    }
    else
    {
        if(TRUE == gbPowerCommMsgFlag[nSim] )
        {
            gbPowerCommMsgFlag[nSim] = FALSE;
        }
        else
        {
            if(nEventFromAT == TRUE)
            {
                AT_TC(g_sw_AT_NW, "---------------nEventFromAT------------------");
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
            }
        }
    }
    return;
}

#ifdef AT_USER_DBS
// for +POSI
AT_POSI_LIST gPosiList;
AT_ARRES gPosiArres;

extern CFW_INIT_INFO cfwInitInfo;

VOID NW_AsyncEventProcess_GetCellInfo(CFW_EVENT cfwEvent)
{
    UINT32 nErrorCode              = 0;
    UINT32 nRet                    = 0;
    static BOOL nICflag            = FALSE;
    static UINT8 nCellInfoTimes[2] = { 0, };
    UINT8 nSim = cfwEvent.nFlag;
    AT_POSI tPosiData;
    CFW_TSM_CURR_CELL_INFO tCurrCellInf;
    CFW_TSM_ALL_NEBCELL_INFO tNeighborCellInfo;

    AT_MemSet(&tCurrCellInf, 0x0, sizeof(CFW_TSM_CURR_CELL_INFO));
    AT_MemSet(&tNeighborCellInfo, 0x0, sizeof(CFW_TSM_ALL_NEBCELL_INFO));
    AT_MemSet(&tPosiData, 0x0, sizeof(AT_POSI));

    AT_TC(g_sw_AT_NW, "NW_AsyncEvent come in DBS!\n");
    if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR");
        nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    if (cfwEvent.nType != 0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent:   ERROR!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

    {

        AT_TC(g_sw_AT_NW, "NW_AsyncEvent nCellInfoTimes:%d!\n", nCellInfoTimes);

        // if(0 == nCellInfoTimes[nSim])
        {
            AT_TC(g_sw_AT_NW, "NW_AsyncEvent 000 nParam2:%d!\n", cfwEvent.nParam2);

            if (cfwEvent.nParam2 == CFW_TSM_CURRENT_CELL)
            {
                nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);
                if (nRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR nRet:0x%x!\n", nRet);
                    CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
                    AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                    return;
                }
                AT_NW_POSI_AddData(0, &tCurrCellInf, &tNeighborCellInfo);

                // no have CellInf
                if (0xffff == tCurrCellInf.nTSM_Arfcn)
                {
                    AT_TC(g_sw_AT_NW, "NW_AsyncEvent no have CellInf ! \n");

                    if (CFW_INIT_STATUS_NO_SIM == cfwInitInfo.noSim[nSim].nType)
                    {
                        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
                        nCellInfoTimes[nSim] = 1;
                        if (((0 == nSim) && (1 == nCellInfoTimes[1])) || ((1 == nSim) && (1 == nCellInfoTimes[0])))
                        {
                            AT_NW_POSI_SendData(nSim);
                            nCellInfoTimes[0] = 0x00;
                            nCellInfoTimes[1] = 0x00;
                        }
                    }
                    else
                    {
                        gPosiList.iNum = 0xff;
                        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
                        nCellInfoTimes[nSim] = 0x00;
                        AT_NW_POSI_SendData(nSim);
                    }
                    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
                    return;
                }

            }

            if (cfwEvent.nParam2 == CFW_TSM_NEIGHBOR_CELL)
            {

                AT_TC(g_sw_AT_NW, "NW_AsyncEvent NEIGHBOR_CELL!:\n");
                nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);
                if (nRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "NW_AsyncEvent NEIGHBOR_CELL ERROR:0x!%x\n", nRet);
                    AT_GetFreeUTI(0, &cfwEvent.nUTI);
                    nCellInfoTimes[nSim] = 1;
                    UINT32 nRet = CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
                    if(ERR_CFW_NOT_EXIST_AO != nRet)
                        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                    return;
                }

                AT_TC(g_sw_AT_NW, "NW_AsyncEvent nTSM_NebCellNUM :%d!\n", tNeighborCellInfo.nTSM_NebCellNUM);

                if (tNeighborCellInfo.nTSM_NebCellNUM != 0)
                {
                    AT_NW_POSI_AddData(1, &tCurrCellInf, &tNeighborCellInfo);
                }

                if (CFW_INIT_STATUS_NO_SIM == cfwInitInfo.noSim[nSim].nType)
                {
                    // CFW_NwStopStack(0);
                    nCellInfoTimes[nSim] = 1;
                    if (((0 == nSim) && (1 == nCellInfoTimes[1])) || ((1 == nSim) && (1 == nCellInfoTimes[0])))
                    {
                        AT_NW_POSI_SendData(nSim);
                        nCellInfoTimes[0] = 0x00;
                        nCellInfoTimes[1] = 0x00;
                    }
                }
                else
                {
                    nCellInfoTimes[nSim] = 0x00;
                    AT_NW_POSI_SendData(nSim);
                }
                AT_GetFreeUTI(0, &cfwEvent.nUTI);
                CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
            }
            return;
        }
    }

    AT_TC(g_sw_AT_NW, "NW-POSI set success!\n");

    return;
}

#else
// for +CCED
#ifdef GPS_SUPPORT
AT_CCED gpsPosiList;
#endif
BOOL AT_EMOD_CCED_ProcessData(UINT8 nSim, CFW_TSM_CURR_CELL_INFO *pCurrCellInfo,
                              CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo, UINT8 *nStrPosiList)
{
    UINT8 i                   = 0;
    VOLATILE AT_CCED PosiList ;
    UINT8 nStr[100]           = { 0 };

    SUL_ZeroMemory8((VOID *)&PosiList, sizeof(AT_CCED));

    AT_TC(g_sw_AT_NW, "AT_EMOD_CCED_ProcessData!");
    if ((NULL == pCurrCellInfo && NULL == pNeighborCellInfo) || (NULL == nStrPosiList))
    {
        AT_TC(g_sw_AT_NW, "AT_EMOD_CCED_ProcessData InputErr\n");
        return FALSE;
    }
    AT_Sprintf(nStrPosiList, "+CCED:");

    AT_TC(g_sw_AT_NW, "pCurrCellInfo != NULL");
    if (pCurrCellInfo != NULL)
    {
        PosiList.sMcc[0] = pCurrCellInfo->nTSM_LAI[0] & 0x0F;
        PosiList.sMcc[1] = pCurrCellInfo->nTSM_LAI[0] >> 4;
        PosiList.sMcc[2] = pCurrCellInfo->nTSM_LAI[1] & 0x0F;
        PosiList.nArfcn = pCurrCellInfo->nTSM_Arfcn;

        PosiList.sMnc[0] = pCurrCellInfo->nTSM_LAI[2] & 0x0F;
        PosiList.sMnc[1] = pCurrCellInfo->nTSM_LAI[2] >> 4;
        PosiList.sMnc[2] = pCurrCellInfo->nTSM_LAI[1] >> 4;
        if (PosiList.sMnc[2] > 9)
            PosiList.sMnc[2] = 0;

        PosiList.sLac    = (pCurrCellInfo->nTSM_LAI[3] << 8) | (pCurrCellInfo->nTSM_LAI[4]);
        PosiList.sCellID = (pCurrCellInfo->nTSM_CellID[0] << 8) | (pCurrCellInfo->nTSM_CellID[1]);

        PosiList.iBsic     = pCurrCellInfo->nTSM_Bsic;

        //PosiList.iRxLev    = ((110 - pCurrCellInfo->nTSM_AvRxLevel) * 100) / 64;
        //Get Real RSSI for 8810 CCED issue by XP 20130930


        PosiList.iRxLevSub = pCurrCellInfo->nTSM_RxQualSub;
        PosiList.iRxLev = pCurrCellInfo->nTSM_AvRxLevel;

        ////Add by wulc&licheng for 8810 cced issue  20150412
        if (PosiList.iRxLev > 113)
        {
            PosiList.iRxLev = 0;
        }
        else if ((PosiList.iRxLev <= 113) && (PosiList.iRxLev >= 51))
        {
            PosiList.iRxLev = (UINT8)(31 - ((PosiList.iRxLev - 51) / 2));
        }
        else if (PosiList.iRxLev < 51)
        {
            PosiList.iRxLev = 31;
        }
        else
        {
            PosiList.iRxLev = 99;
        }

        AT_TC(g_sw_AT_NW,
              "AT_EMOD_CCED_ProcessData pCurrCellInfo\n sMcc=%d%d%d,sMnc=%d%d%d,sLac=%x,sCellID=%x,iBsic=%d,iRxLev=%d,iRxLevSub=%d PosiList.nArfcn=0x%x",
              PosiList.sMcc[0], PosiList.sMcc[1], PosiList.sMcc[2], PosiList.sMnc[0], PosiList.sMnc[1], PosiList.sMnc[2],
              PosiList.sLac, PosiList.sCellID, PosiList.iBsic, PosiList.iRxLev, PosiList.iRxLevSub, PosiList.nArfcn);
#ifndef CHIP_HAS_AP
        SUL_StrPrint(nStr, "%d%d%d,%d%d%d,%x,%x,%d,%d,%d,%x",
                     PosiList.sMcc[0], PosiList.sMcc[1], PosiList.sMcc[2],
                     PosiList.sMnc[0], PosiList.sMnc[1], PosiList.sMnc[2],
                     PosiList.sLac, PosiList.sCellID, PosiList.iBsic, PosiList.iRxLev, PosiList.iRxLevSub, PosiList.nArfcn);
#else
        SUL_StrPrint(nStr, "%d%d%d,%d%d%d,%x,%x,%d,%d,%d",
                     PosiList.sMcc[0], PosiList.sMcc[1], PosiList.sMcc[2],
                     PosiList.sMnc[0], PosiList.sMnc[1], PosiList.sMnc[2],
                     PosiList.sLac, PosiList.sCellID, PosiList.iBsic, PosiList.iRxLev, PosiList.iRxLevSub);
#endif
        SUL_StrCat(nStrPosiList, nStr);

    }

    if (pNeighborCellInfo != NULL)
    {
        int iCount = 0x00;
        for (i = 0; i < pNeighborCellInfo->nTSM_NebCellNUM; i++)
        {
            if (!((pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[0] == 0)
                    && (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[1] == 0)))
            {

                PosiList.sMcc[0] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] & 0x0F;
                PosiList.sMcc[1] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] >> 4;
                PosiList.sMcc[2] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] & 0x0F;

#  if 1
                PosiList.sMnc[0] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] & 0x0F;
                PosiList.sMnc[1] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] >> 4;
                PosiList.sMnc[2] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] >> 4;
                if (PosiList.sMnc[2] > 9)
                    PosiList.sMnc[2] = 0;
#else
                PosiList.sMnc[0] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] >> 4;
                if (PosiList.sMnc[0] > 9)
                    PosiList.sMnc[0] = 0;
                PosiList.sMnc[1]   = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] & 0x0F;
                PosiList.sMnc[2]   = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] >> 4;
#endif

                PosiList.sLac =
                    (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[3] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[4]);
                PosiList.sCellID =
                    (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[0] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].
                            nTSM_CellID[1]);

                PosiList.iBsic  = pNeighborCellInfo->nTSM_NebCell[i].nTSM_Bsic;
                //Get Real RSSI for 8810 CCED issue by XP 20130930
                //PosiList.iRxLev = ((110 - pNeighborCellInfo->nTSM_NebCell[i].nTSM_AvRxLevel) * 100) / 64;
                PosiList.iRxLev = pNeighborCellInfo->nTSM_NebCell[i].nTSM_AvRxLevel;
                //reopen by wulc &licheng for 8810 cced issue 20150412
                if (PosiList.iRxLev > 113)
                {
                    PosiList.iRxLev = 0;
                }
                else if ((PosiList.iRxLev <= 113) && (PosiList.iRxLev >= 51))
                {
                    PosiList.iRxLev = (UINT8)(31 - ((PosiList.iRxLev - 51) / 2));
                }
                else if (PosiList.iRxLev < 51)
                {
                    PosiList.iRxLev = 31;
                }
                else
                {
                    PosiList.iRxLev = 99;
                }

                if (pCurrCellInfo != NULL || i > 0)
                {
                    AT_TC(g_sw_AT_NW, "pCurrCellInfo 0x%x,i=%d", pCurrCellInfo, i);
                    if(iCount > 0)
                        SUL_StrCat(nStrPosiList, ",");
                }
                iCount ++;

                SUL_StrPrint(nStr, "%d%d%d,%d%d%d,%d,%d,%d,%d",
                             PosiList.sMcc[0], PosiList.sMcc[1], PosiList.sMcc[2],
                             PosiList.sMnc[0], PosiList.sMnc[1], PosiList.sMnc[2],
                             PosiList.sLac, PosiList.sCellID, PosiList.iBsic, PosiList.iRxLev);

                AT_TC(g_sw_AT_NW,
                      "AT_EMOD_CCED_ProcessData pNeighborCellInfo[%d] sMcc=%d%d%d,sMnc=%d%d%d,sLac=%x,sCellID=%x,iBsic=%d,iRxLev=%d,iRxLevSub=%d",
                      i, PosiList.sMcc[0], PosiList.sMcc[1], PosiList.sMcc[2], PosiList.sMnc[0], PosiList.sMnc[1],
                      PosiList.sMnc[2], PosiList.sLac, PosiList.sCellID, PosiList.iBsic, PosiList.iRxLev);
                SUL_StrCat(nStrPosiList, nStr);
            }
        }
    }
#ifdef GPS_SUPPORT
   memcpy(&gpsPosiList,&PosiList,sizeof(gpsPosiList));
#endif	
    return TRUE;
}

VOID NWSATCellInfo(CFW_EVENT cfwEvent)
{
    UINT8 nSim = cfwEvent.nFlag;

    AT_TC(g_sw_AT_NW, "NWSATCellInfo cfwEvent.nType0x%x!\n", cfwEvent.nType);

    if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR");
        UINT32 nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    if (cfwEvent.nType != 0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent:	ERROR!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    AT_TC(g_sw_AT_NW, "NWSATCellInfo cfwEvent.nParam2 0x%x!\n", cfwEvent.nParam2);

    if (cfwEvent.nParam2 == CFW_TSM_CURRENT_CELL)
    {
        CFW_TSM_CURR_CELL_INFO tCurrCellInf;
        CFW_TSM_ALL_NEBCELL_INFO tNeighborCellInfo;

        AT_MemSet(&tCurrCellInf, 0x0, sizeof(CFW_TSM_CURR_CELL_INFO));
        AT_MemSet(&tNeighborCellInfo, 0x0, sizeof(CFW_TSM_ALL_NEBCELL_INFO));

        UINT32 nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);
        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);

        // hal_DbgAssert("NW_AsyncEventProcess_GetCellInfo CFW_TSM_CURRENT_CELL", NULL);
        if (nRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR nRet:0x%x!\n", nRet);
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }
        if(0x00 == gATSATComQualifier[nSim])
        {
            SAT_LOCALINFO_RSP LocaInf;
            UINT8 nMNClen = 3;
            memset(&LocaInf, 0, sizeof(SAT_LOCALINFO_RSP));
            LocaInf.nCellIdentify = (tCurrCellInf.nTSM_CellID[0] << 8) | (tCurrCellInf.nTSM_CellID[1]);


            cfw_PLMNtoBCD(tCurrCellInf.nTSM_LAI, LocaInf.nMCC_MNC_Code, &nMNClen);

            LocaInf.nLocalAreaCode = (tCurrCellInf.nTSM_LAI[3] << 8 ) | (tCurrCellInf.nTSM_LAI[4]);

            UINT8 nUTI = 0x00;
            AT_GetFreeUTI(CFW_NW_SRV_ID, &nUTI);

            CFW_NW_STATUS_INFO nStatusInfo;
            UINT32 nStatus = 0x00;
            if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSim))
            {
                AT_TC(g_sw_AT_NW, "CFW_NwGetStatus Error\n");

                AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                return;
            }
            AT_TC(g_sw_AT_NW, "NWSATCellInfo nStatusInfo.nStatus  %d\n", nStatusInfo.nStatus);

            if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING || nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                    nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
            {
                nStatus = 0x06;
            }

            if(0x00 == nStatus)
                nRet = CFW_SatResponse (0x26, 0, 0x0, &LocaInf, sizeof(SAT_LOCALINFO_RSP), nUTI, nSim);
            else
            {
                memset(&LocaInf,0,sizeof(LocaInf));
                nRet = CFW_SatResponse (0x26, 0, 0x0, &LocaInf, sizeof(SAT_LOCALINFO_RSP), nUTI, nSim);
            }
        }
        else if(0x05 == gATSATComQualifier[nSim])
        {
            SAT_TIMINGADV_RSP nTA;
            nTA.nME_Status = 0x00;
            nTA.nTimingAdv = tCurrCellInf.nTSM_TimeADV;
            UINT32 nStatus = 0x00;
            CFW_NW_STATUS_INFO nStatusInfo;
            if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSim))
            {
                AT_TC(g_sw_AT_NW, "CFW_NwGetStatus Error\n");

                AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                return;
            }
            AT_TC(g_sw_AT_NW, "NWSATCellInfo nStatusInfo.nStatus  %d\n", nStatusInfo.nStatus);

            if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING || nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                    nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
            {
                nStatus = 0x06;
            }
            if(0x00 == nStatus)
                nRet = CFW_SatResponse (0x26, nStatus, 0x0, &nTA, sizeof(SAT_TIMINGADV_RSP), AT_ASYN_GET_DLCI(nSim), nSim);
            else
                nRet = CFW_SatResponse (0x26, nStatus, 0x0, NULL, 0, AT_ASYN_GET_DLCI(nSim), nSim);
        }


        AT_TC(g_sw_AT_NW, "CFW_SatResponse nRet:0x%x!\n", nRet);


    }
}
extern UINT32 gSATCurrentCmdStamp[];
VOID NW_AsyncEventProcess_GetCellInfo(CFW_EVENT cfwEvent)
{
    UINT32 nErrorCode              = 0;
    UINT32 nRet                    = 0;
    UINT8 nSim = cfwEvent.nFlag;
    BOOL bRet = FALSE;
    CFW_TSM_CURR_CELL_INFO tCurrCellInf;
    CFW_TSM_ALL_NEBCELL_INFO tNeighborCellInfo;
    UINT8 pTxtBuf[500] = { 0 };

    AT_MemSet(&tCurrCellInf, 0x0, sizeof(CFW_TSM_CURR_CELL_INFO));
    AT_MemSet(&tNeighborCellInfo, 0x0, sizeof(CFW_TSM_ALL_NEBCELL_INFO));

    AT_TC(g_sw_AT_NW, "NW_AsyncEvent come gATSATLocalInfoFlag[%d] %d!\n", nSim, gATSATLocalInfoFlag[nSim]);


    if(TRUE == gATSATLocalInfoFlag[nSim] )
    {
        //gSATCurrentCmdStamp[nSim] = 0x00;
        NWSATCellInfo(cfwEvent);
        return;
    }

    if (cfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR");
        nErrorCode = AT_SetCmeErrorCode(cfwEvent.nParam1, FALSE);
        AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }
    if (cfwEvent.nType != 0)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent:   ERROR!!!---CfwEvent.nType --0x%x\n\r", cfwEvent.nType);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        return;
    }

    AT_TC(g_sw_AT_NW, "NW_AsyncEvent 000 nParam2:%d!\n", cfwEvent.nParam2);

    if (cfwEvent.nParam2 == CFW_TSM_CURRENT_CELL)
    {
        nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);

        // hal_DbgAssert("NW_AsyncEventProcess_GetCellInfo CFW_TSM_CURRENT_CELL", NULL);
        if (nRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "NW_AsyncEvent ERROR nRet:0x%x!\n", nRet);
            CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }

        bRet = AT_EMOD_CCED_ProcessData(nSim, &tCurrCellInf, NULL, pTxtBuf);
        if (bRet)
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pTxtBuf, AT_StrLen(pTxtBuf), cfwEvent.nUTI, nSim);
        else
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);

        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
    }
    else if (cfwEvent.nParam2 == CFW_TSM_NEIGHBOR_CELL)
    {
        AT_TC(g_sw_AT_NW, "NW_AsyncEvent NEIGHBOR_CELL!:\n");
        nRet = CFW_GetCellInfo(&tCurrCellInf, &tNeighborCellInfo, nSim);

        // hal_DbgAssert("NW_AsyncEventProcess_GetCellInfo CFW_TSM_NEIGHBOR_CELL", NULL);

        if (nRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "NW_AsyncEvent NEIGHBOR_CELL ERROR:0x!%x\n", nRet);
            CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }

        AT_TC(g_sw_AT_NW, "NW_AsyncEvent nTSM_NebCellNUM :%d!\n", tNeighborCellInfo.nTSM_NebCellNUM);

        // tNeighborCellInfo.nTSM_NebCellNUM
        bRet = AT_EMOD_CCED_ProcessData(nSim, NULL, &tNeighborCellInfo, pTxtBuf);
        if (bRet)
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pTxtBuf, AT_StrLen(pTxtBuf), cfwEvent.nUTI, nSim);
        else
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);

        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
    }
    else
    {
        CFW_EmodOutfieldTestEnd(cfwEvent.nUTI, nSim);
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
    }
}
#endif

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_NW_CmdFunc_CCED(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    INT32 iRet      = 0;
    UINT8 nMode    = 0, nRequestedDump = 0;
    UINT16 nTypSize = 0;
    UINT8 nIndex   = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_GPRS, "AT+CCED:AT_NW_CmdFunc_CCED");
    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 2))
        {
            AT_TC(g_sw_GPRS, "AT+CCED ERROR:paraCount: %d", paraCount);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // index is 0,get MODE
        nIndex   = 0;
        nTypSize = 1;
        iRet     = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nMode, &nTypSize);
        AT_TC(g_sw_GPRS, "AT+CCED nMode: %d", nMode);
        if (ERR_SUCCESS != iRet || nMode > 1)
        {
            AT_TC(g_sw_GPRS, "AT+CCED ERROR:nMode!=1 !");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // index is 1,get RequestedDump
        nIndex++;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nRequestedDump, &nTypSize);

        if (iRet == ERR_SUCCESS && nRequestedDump != 1 && nRequestedDump != 2 && nRequestedDump != 8)
        {
            AT_TC(g_sw_GPRS, "AT+CCED ERROR:iRet: %d, nRequestedDump ", nRequestedDump);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;

        }
        else if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CCED ERROR:iRet: %d, GetPara ", iRet);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (8 == nRequestedDump)
        {
            if(0 == nMode)
            {
                AT_KillCCEDTimer();
            }
            else if(1 == nMode)
            {
                AT_SetCCEDTimer(ATE_CCED_ELAPSE);
            }
            // dispaly RSSI same as CSQ
            UINT8 nSignalLevel = 0;
            UINT8 nBitError    = 0;
            UINT8 nBuf[20]     = { 0 };

            iRet = CFW_NwGetSignalQuality(&nSignalLevel, &nBitError, nSim);
            AT_TC(g_sw_AT_NW, "AT+CCED CFW_NwGetSignalQuality:nRet = 0x%x\n\r", iRet);
            AT_TC(g_sw_AT_NW, "AT+CCED=0,8:   nSignalLevel = %d,nBitError = %d\n\r", nSignalLevel, nBitError);

            if (ERR_SUCCESS == iRet)
            {
                UINT8 nSL[3] = { 0 };
                UINT8 nBE[5] = { 0 };

                SUL_ZeroMemory8(nBuf, 20);
                SUL_StrPrint(nSL, (PCSTR)"%d", nSignalLevel);
                SUL_StrPrint(nBE, (PCSTR)"%d", nBitError);
                SUL_StrCopy(nBuf, "+CCED: ");
                SUL_StrCat(nBuf, nSL);
                SUL_StrCat(nBuf, ",");
                SUL_StrCat(nBuf, nBE);

                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nBuf, AT_StrLen(nBuf), pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            // get Cell info
            CFW_TSM_FUNCTION_SELECT tSelecFUN;
            SUL_ZeroMemory8(&tSelecFUN, SIZEOF(CFW_TSM_FUNCTION_SELECT));

            UINT8 nUTI = 0;

            if (1 == nRequestedDump)
            {
                tSelecFUN.nServingCell = 1;
            }
            else
            {
                tSelecFUN.nNeighborCell = 1;
            }

            AT_GetFreeUTI(0, &nUTI);
            iRet = CFW_EmodOutfieldTestStart(&tSelecFUN, pParam->nDLCI, nSim);
            if (iRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "AT+CCED ERROR:iRet: %d, Emod", iRet);
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 60, NULL, 0, pParam->nDLCI, nSim);
        }

        break;

    case AT_CMD_READ:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:
    {
        UINT8 AtTri[64] = { 0 };

        AT_Sprintf(AtTri, "+CCED: (0),(1,2,8)");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtTri, (UINT16)AT_StrLen(AtTri), pParam->nDLCI, nSim);
    }
    break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_ReportRssi(UINT8 nDLCI)
{
    INT32 iRet      = 0;

    UINT8 nSignalLevel = 0;
    UINT8 nBitError    = 0;
    UINT8 nBuf[20]     = { 0 };
    UINT8 nSim = AT_SIM_ID(nDLCI);

    iRet = CFW_NwGetSignalQuality(&nSignalLevel, &nBitError, nSim);
    AT_TC(g_sw_AT_NW, "AT+CCED CFW_NwGetSignalQuality:nRet = 0x%x\n\r", iRet);
    AT_TC(g_sw_AT_NW, "AT+CCED=0,8:   nSignalLevel = %d,nBitError = %d\n\r", nSignalLevel, nBitError);

    if (ERR_SUCCESS == iRet)
    {
        UINT8 nSL[3] = { 0 };
        UINT8 nBE[5] = { 0 };

        SUL_ZeroMemory8(nBuf, 20);
        SUL_StrPrint(nSL, (PCSTR)"%d", nSignalLevel);
        SUL_StrPrint(nBE, (PCSTR)"%d", nBitError);
        SUL_StrCopy(nBuf, "+CCED: ");
        SUL_StrCat(nBuf, nSL);
        SUL_StrCat(nBuf, ",");
        SUL_StrCat(nBuf, nBE);

        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nBuf, AT_StrLen(nBuf), nDLCI, nSim);
        return;
    }

}

BOOL g_simopen_creg_flag = FALSE;

typedef struct
{
   UINT16         Arfcn ;   // [0..1024]
   UINT16         Rssi  ;   // [0..120] dBm after L1_LOW_RLA  it's noise only
} NW_FREQ_INFO ;
VOID AT_NW_AsyncEventProcess(COS_EVENT *pEvent)
{
    UINT8 uSignal[20]    = { 0 };
    UINT8 uRegStatus[40] = { 0 };
    UINT8 ucSignal       = 0;
    CFW_EVENT cfwEvent;
    UINT8 AreaStr[5]  = { 0 };
    UINT8 CellStr[5]  = { 0 };
    AT_CosEvent2CfwEvent(pEvent, &cfwEvent);

    UINT16 uiCellCode = LOUINT16(cfwEvent.nParam2);
    UINT16 uiAreaCode = HIUINT16(cfwEvent.nParam2);
    UINT8 nSim = cfwEvent.nFlag;

    if((cfwEvent.nUTI != 0x4)&&(cfwEvent.nUTI != 0x8))
    {
        cfwEvent.nUTI = AT_ASYN_GET_DLCI(nSim);
    }

    AT_TC(g_sw_AT_NW, "NW_AsyncEvent:nEventId = %d\n\r", pEvent->nEventId);
    switch (pEvent->nEventId)
    {
    // AT_20071112_CAOW_B //add for +CIEV

    case EV_CFW_NW_SIGNAL_QUALITY_IND:
        AT_TC(g_sw_AT_NW, "CFW_NwGetSignalQuality:_ind[nSim] = %d,nType=%d\n\r", gATCurrentuCmer_ind[nSim],cfwEvent.nType);
        if (gATCurrentuCmer_ind[nSim] == 2)
        {
            if (cfwEvent.nType == 0)
            {
                if (cfwEvent.nParam1 == 0 || cfwEvent.nParam1 == 99)
                    ucSignal = 0;
                else if (cfwEvent.nParam1 >= 1 && cfwEvent.nParam1 < 7)
                    ucSignal = 1;
                else if (cfwEvent.nParam1 >= 7 && cfwEvent.nParam1 < 13)
                    ucSignal = 2;
                else if (cfwEvent.nParam1 >= 13 && cfwEvent.nParam1 < 19)
                    ucSignal = 3;
                else if (cfwEvent.nParam1 >= 19 && cfwEvent.nParam1 < 25)
                    ucSignal = 4;
                else if (cfwEvent.nParam1 >= 25 && cfwEvent.nParam1 < 32)
                    ucSignal = 5;

                AT_TC(g_sw_AT_NW, "NW_AsyncEvent:CSQFlag = %d,nParam1=0x%x,nParam2=0x%x\n\r", g_uATNWECSQFlag,cfwEvent.nParam1,cfwEvent.nParam2);
                if (g_uATNWECSQFlag == 1)
                {
                    SUL_ZeroMemory8(uSignal, sizeof(uSignal));
                    SUL_StrPrint(uSignal, "+ECSQ: %d,%d", cfwEvent.nParam1, cfwEvent.nParam2);
                    AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uSignal, AT_StrLen(uSignal), cfwEvent.nUTI, nSim);
                }

            }
        }

        AT_ZERO_PARAM1(pEvent);
        break;

    case EV_CFW_NW_REG_STATUS_IND:
        // ---------------------------------------------------------
        // CREG URC
        // ---------------------------------------------------------
        AT_ZERO_PARAM1(pEvent);
        gCsStauts.nParam1 = cfwEvent.nParam1;
        gCsStauts.nType   = cfwEvent.nType;
        if (2 == cfwEvent.nType)
        {

            if(g_simopen_creg_flag == TRUE)
            {
                AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, NULL, 0, cfwEvent.nUTI, nSim);
                g_simopen_creg_flag = FALSE;
                return;
            }
            // AT_20071112_CAOW_B   //add for +CIEV
            if (gATCurrentuCmer_ind[nSim] == 2)
            {
                SUL_ZeroMemory8(uRegStatus, 40);

                if (cfwEvent.nParam1 == CFW_NW_STATUS_REGISTERED_HOME || cfwEvent.nParam1 == CFW_NW_STATUS_REGISTERED_ROAMING)
                    SUL_StrPrint(uRegStatus, "+CIEV: service,  1\r\n");
                else
                    SUL_StrPrint(uRegStatus, "+CIEV: service,  0\r\n");

                if (cfwEvent.nParam1 == CFW_NW_STATUS_REGISTERED_ROAMING)
                    SUL_StrCat(uRegStatus, "+CIEV: roam, 1");
                else
                    SUL_StrCat(uRegStatus, "+CIEV: roam, 0");
                AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uRegStatus, AT_StrLen(uRegStatus), cfwEvent.nUTI, nSim);
                SUL_ZeroMemory8(uRegStatus, 40);


                if(2 == cfwEvent.nParam2)
                    SUL_StrPrint(uRegStatus, "+GSM	Service\r\n");
                else if(4 == cfwEvent.nParam2)
                    SUL_StrPrint(uRegStatus, "+UMTS Service\r\n");

                AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uRegStatus, AT_StrLen(uRegStatus), cfwEvent.nUTI, nSim);
            }

            // AT_20071112_CAOW_E

            if ((CFW_NW_STATUS_REGISTERED_HOME == cfwEvent.nParam1) || (CFW_NW_STATUS_REGISTERED_ROAMING == cfwEvent.nParam1))
            {
                if (g_FirstREG)
                {
                    UINT8 ucMode = 0;

                    CFW_NwGetCurrentOperator(g_COPS_OperID, &ucMode, nSim);
                    g_COPS_Mode = 0;
                    g_FirstREG  = 0;
                }
            }

            UINT8 n = 0;
            UINT32 nRet = CFW_CfgGetNwStatus(&n, nSim);
            if (ERR_SUCCESS != nRet)
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                return;
            }

            if (0 == n)
            {
                return;
            }

            UINT8 uOutstr[80] = { 0 };  // yy [mod] 2008-4-24 for Bug ID:8176

            if ((0 != cfwEvent.nParam2) && (2 == n))
            {
                // [[ yy [mod]  2008-4-23 for Bug ID:8176
#  if 0
                UINT8 AreaStr[5]  = { 0 };
                UINT8 CellStr[5]  = { 0 };
                UINT16 uiCellCode = LOUINT16(cfwEvent.nParam2);
                UINT16 uiAreaCode = HIUINT16(cfwEvent.nParam2);
#endif
                // ]] yy [mod] 2008-4-23 for Bug ID:8176

                if (uiAreaCode < 16)
                {
                    SUL_StrPrint(AreaStr, "000%x", uiAreaCode);
                }
                else if (uiAreaCode < 256)
                {
                    SUL_StrPrint(AreaStr, "00%x", uiAreaCode);
                }
                else if (uiAreaCode < 4096)
                {
                    SUL_StrPrint(AreaStr, "0%x", uiAreaCode);
                }
                else
                {
                    SUL_StrPrint(AreaStr, "%x", uiAreaCode);
                }

                if (uiCellCode < 16)
                {
                    SUL_StrPrint(CellStr, "000%x", uiCellCode);
                }
                else if (uiCellCode < 256)
                {
                    SUL_StrPrint(CellStr, "00%x", uiCellCode);
                }
                else if (uiCellCode < 4096)
                {
                    SUL_StrPrint(CellStr, "0%x", uiCellCode);
                }
                else
                {
                    SUL_StrPrint(CellStr, "%x", uiCellCode);
                }

                AT_StrUpr(CellStr);

                AT_StrUpr(AreaStr);

                // AT_SIM_20071011_CAOW_B
#  if 0
                SUL_StrPrint(uOutstr, "+CREG: %d,\"%s\",\"%s\"\r\n", cfwEvent.nParam1, AreaStr, CellStr);
#else
                SUL_StrPrint(uOutstr, "+CREG: %d,\"%s\",\"%s\"", cfwEvent.nParam1, AreaStr, CellStr);
#endif
                // AT_SIM_20071011_CAOW_E

            }
            else
            {
                SUL_StrPrint(uOutstr, "+CREG: %d", cfwEvent.nParam1);
                AT_TC(g_sw_GPRS, "+CREG: %d", cfwEvent.nParam1);
#ifdef AT_FTP_SUPPORT
                // set a timer for OTA
                BOOL bOTATimer = FALSE;
                bOTATimer= AT_SetOTATimer(ATE_OTA_CHECK_ELAPSE);
                AT_TC(g_sw_GPRS, "SetOTATimer: %d", bOTATimer);
#endif
            }


            AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutstr, AT_StrLen(uOutstr), cfwEvent.nUTI, nSim);
            SUL_ZeroMemory8(uOutstr, 80);
            if(2 == cfwEvent.nParam2)
                SUL_StrPrint(uOutstr, "+GSM	Service\r\n");
            else if(4 == cfwEvent.nParam2)
                SUL_StrPrint(uOutstr, "+UMTS Service\r\n");

            AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutstr, AT_StrLen(uOutstr), cfwEvent.nUTI, nSim);
            return;
        }

        break;

    case EV_CFW_NW_DEREGISTER_RSP:
        // ---------------------------------------------------------
        // COPS deregister Event
        // ---------------------------------------------------------

        if (0 == pEvent->nParam1)
        {
            UINT8 uOutstr[80] = { 0 };
#ifdef LTE_NBIOT_SUPPORT
		  if(CFW_RAT_NBIOT_ONLY == CFW_NWGetRat(nSim))
		      SUL_StrPrint(uOutstr, "+CEREG: 0\r\n");
		  else
#endif
          {
             SUL_StrPrint(uOutstr, "+CREG: 0\r\n");
          }
            g_COPS_Mode = 2;
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutstr, AT_StrLen(uOutstr), cfwEvent.nUTI, nSim);
            return;
        }
        else
        {
            AT_NW_Result_Err(pEvent->nParam1 - ERR_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }

        break;

    case EV_CFW_NW_SET_REGISTRATION_RSP:
        // ---------------------------------------------------------
        // COPS register Event
        // ---------------------------------------------------------
        if (cfwEvent.nType != 0)
            AT_ZERO_PARAM1(pEvent);

        AT_TC(g_sw_AT_NW, "EV_CFW_NW_SET_REGISTRATION_RSP cfwEvent.nType==%d\r\n", cfwEvent.nType);
        if(gATCurrentCOPS_Mode_Temp == 4)
        {
            UINT8 nOperatorId[6] = { 0 };
            if(0 != cfwEvent.nType)
            {
                CFW_NwSetRegistration(nOperatorId, 0, cfwEvent.nUTI, nSim);
                g_COPS_Mode = gATCurrentCOPS_Mode_Temp = 0;
                return;
            }
        }

        if (at_ModuleGetCopsFlag(nSim) && (0 == cfwEvent.nType))
        {
            at_ModuleSetCopsFlag(nSim, FALSE);
            g_COPS_Mode           = gATCurrentCOPS_Mode_Temp;

            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, cfwEvent.nUTI, nSim);
            return;
        }
        // AT_20071029_CAOW_B
        at_ModuleSetCopsFlag(nSim, FALSE);
        if (0 == cfwEvent.nType)
        {
            g_COPS_Mode = gATCurrentCOPS_Mode_Temp;

            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_CR, 0, NULL, 0, cfwEvent.nUTI, nSim);
            return;
        }
        else
        {
            AT_NW_Result_Err(ERR_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            return;
        }
        // AT_20071029_CAOW_E
        break;
    case EV_CFW_NW_GET_AVAIL_OPERATOR_RSP:
        // ---------------------------------------------------------
        // COPS Event
        // ---------------------------------------------------------
    {
        CFW_NW_OPERATOR_INFO *pOperInfo = NULL;
        UINT32 nRet;
        UINT8 outstr[400] = { 0 ,};
        UINT8 outemp[200]  = { 0 ,};
        UINT8 nOprNam[10] = {0,};
        UINT8 *pOperName  = NULL;
        UINT8 i;
        if (0 == cfwEvent.nType)
        {
            pOperInfo = (CFW_NW_OPERATOR_INFO *)(cfwEvent.nParam1);
            SUL_ZeroMemory8(outstr, 400);
            SUL_StrCat(outstr, "+COPS: ");

            for (i = 0; i < cfwEvent.nParam2; i++)
            {
                if (i > 0)
                {
                    SUL_StrCat(outstr, ",");
                }
                AT_TC(g_sw_AT_NW, "_GET_AVAIL_OPERATOR_RSP cfwEvent.nType==%d\r\n", cfwEvent.nType);

                nRet = CFW_CfgNwGetOperatorName(pOperInfo[i].nOperatorId, &pOperName);  /* required format is long format, so get name */

                if (nRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "_GET_AVAIL_OPERATOR_RSP nRet != ERR_SUCCESS nRet==%d\r\n", nRet);
                    if( (pOperInfo[i].nOperatorId[0] + pOperInfo[i].nOperatorId[1] + pOperInfo[i].nOperatorId[2] ) > 0)
                    {
                        UINT32 nValue = 0;
                        if(0x0f == pOperInfo[i].nOperatorId[5] )
                        {
                            nValue = pOperInfo[i].nOperatorId[0]*10000+ pOperInfo[i].nOperatorId[1]*1000+ pOperInfo[i].nOperatorId[2]*100+ pOperInfo[i].nOperatorId[3]*10+ pOperInfo[i].nOperatorId[4]*1;
                        }
                        else
                        {
                            nValue = pOperInfo[i].nOperatorId[0]*100000+ pOperInfo[i].nOperatorId[1]*10000+ pOperInfo[i].nOperatorId[2]*1000+ pOperInfo[i].nOperatorId[3]*100+ pOperInfo[i].nOperatorId[4]*10+ pOperInfo[i].nOperatorId[5]*1;
                        }
                        AT_TC(g_sw_AT_NW, "Tst 000 nValue: %d\r\n", nValue);
                        SUL_Itoa(nValue, nOprNam, 10);
                        pOperName = nOprNam;
                        AT_TC(g_sw_AT_NW, "Tst 000 nOprNam: %s, pOperName: %s\r\n", nOprNam, pOperName);
                    }
                    else
                    {
                        if (AT_isWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI))
                        {
                            AT_DelWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI);
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
                        }
                        return;
                    }
                }

                SUL_ZeroMemory8(outemp, 200);
                if(0x0f == pOperInfo[i].nOperatorId[5])
                {
                    SUL_StrPrint(outemp, "(%d,\"%s\",\"%s\",\"%d%d%d%d%d\")", pOperInfo[i].nStatus, pOperName, pOperName,
                                 pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                 pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4]);
                }
                else
                {
                    SUL_StrPrint(outemp, "(%d,\"%s\",\"%s\",\"%d%d%d%d%d%d\")", pOperInfo[i].nStatus, pOperName, pOperName,
                                 pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                 pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4],pOperInfo[i].nOperatorId[5]);
                }
                SUL_StrCat(outstr, outemp);
            }

            if (AT_isWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI))
            {
                AT_DelWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI);
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, outstr, AT_StrLen(outstr), cfwEvent.nUTI, nSim);
            }
            return;
        }
        else
        {
            if (AT_isWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI))
            {
                AT_DelWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, cfwEvent.nUTI);
                AT_NW_Result_Err(pEvent->nParam1 - ERR_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            }
            return;
        }
    }
    break;

    case EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP:
        // for +cpol=?  get the max num of preferred operators
        NW_AsyncEventProcess_GetPreferredOperatorMaxNum(cfwEvent);
        AT_ZERO_PARAM1(pEvent);
        break;

    case EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP:
        // for +cpol?  get the preferred operators
        NW_AsyncEventProcess_GetPreferredOperators(cfwEvent);
        break;

    case EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP:
        // for +cpol,  add or delete the preferred operator
        NW_AsyncEventProcess_SetPreferredOperators(cfwEvent);
        break;
    case EV_CFW_TSM_INFO_IND:

        // for +cpol,  add or delete the preferred operator
        //#ifdef AT_USER_DBS
        NW_AsyncEventProcess_GetCellInfo(cfwEvent);
        //#endif
        break;
    case EV_CFW_SET_COMM_RSP:
        AT_CosEvent2CfwEvent(pEvent, &cfwEvent);//Add for gcf auto test when dlci equ 4
        NW_AsyncEventProcess_SetCom(cfwEvent);
        break;
    case EV_CFW_NW_STORELIST_IND:
    {
        UINT8 uOutStr[sizeof(CFW_StoredPlmnList) * 3] = {0x00,};

        AT_TC(g_sw_AT_NW, "Get EV_CFW_NW_STORELIST_IND \r\n");
        memset(uOutStr, 0x00,CFW_STORE_MAX_COUNT * 3);
        strcpy(uOutStr, "+PLMNLIST: ");

        AT_NW_GetStoreListString(nSim, uOutStr + strlen(uOutStr));
        AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, uOutStr, AT_StrLen(uOutStr), cfwEvent.nUTI, nSim);
        break;
    }
    case EV_CFW_NW_NETWORKINFO_IND:
    {
        CFW_NW_NETWORK_INFO *p = (CFW_NW_NETWORK_INFO *)cfwEvent.nParam1;

        if(p)
        {
            AT_TC(g_sw_AT_NW, "Get EV_CFW_NW_NETWORKINFO_IND \r\n");
            AT_TC_MEMBLOCK(g_sw_AT_NW, (UINT8*)p, sizeof(CFW_NW_NETWORK_INFO), 16);

            // "yy/mm/dd,hh:mm:ss(+/-)tz"
            // tz is in number of quarter-hours
            UINT8 W[200] = {0x00,};
            SUL_StrPrint(W, "+CTZV:%x/%x/%x,%x:%x:%x,%x",
                         p->nUniversalTimeZone[0], p->nUniversalTimeZone[1], p->nUniversalTimeZone[2], p->nUniversalTimeZone[3],
                         p->nUniversalTimeZone[4], p->nUniversalTimeZone[5], p->nUniversalTimeZone[6]);
            AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, W, AT_StrLen(W), cfwEvent.nUTI, nSim);
        }

        break;
    }
#ifdef LTE_NBIOT_SUPPORT
	 case EV_CFW_NW_ABORT_LIST_OPERATORS_RSP:
	  if(cfwEvent.nType != 0) 
	  {
		 AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_RC_BUSY, cfwEvent.nUTI, nSim);
	  }
	  else
	  {
		  AT_NW_Result_Err(ERR_CME_UNKNOWN, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
	  }
	  break;

	 case EV_CFW_ERRC_CONNSTATUS_IND:
	 {
		 UINT8 ret;
		 UINT8 respond[50] = {0};
		 //CFW_NW_STATUS_INFO sStatus;
		 AT_TC(g_sw_AT_NW, "EV_CFW_ERRC_CONNSTATUS_IND, g_uAtCscon/mode:%d/%d",\
					 g_uAtCscon,cfwEvent.nType);
		 ret=AT_Cscon_respond(respond,cfwEvent.nType, FALSE);
			if(ret != 2)
			{
				if(ret == 0)
				{
					AT_MemZero(respond, sizeof(respond));
				}
			    else
			    {
				    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, respond, AT_StrLen(respond), cfwEvent.nUTI, nSim);
			    }
			}
		 else
		 {
			 AT_TC(g_sw_AT_NW, "Get EV_CFW_ERRC_CONNSTATUS_IND error \r\n");
		 }
		 break;
	 }

	 case EV_CFW_GPRS_EDRX_IND:
	 {
		AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_EDRX_IND");
		UINT8 nResp[100] = {0x00,};
		if(nbiot_nvGetEdrxEnable() == 2)
		{
		    UINT8  requested_eDrx[20] = {0};
		    UINT8  nw_provided_eDrx[20] = {0};
		    UINT8  ptw[20] = {0};
            g_nwEdrxValue = cfwEvent.nParam1;
            g_nwEdrxPtw = cfwEvent.nParam2;
		    AT_Util_ByteToBitStr(requested_eDrx, nbiot_nvGetEdrxValue(), 4);
		    AT_Util_ByteToBitStr(nw_provided_eDrx, cfwEvent.nParam1, 4);
		    AT_Util_ByteToBitStr(ptw, cfwEvent.nParam2, 4);
            AT_Sprintf(nResp, "+CEDRXP: 5, %s, %s, %s", requested_eDrx, nw_provided_eDrx, ptw);
            AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nResp, AT_StrLen(nResp), AT_ASYN_GET_DLCI(nSim), nSim);
		}
		break;
	}

	case EV_CFW_GPRS_CCIOTOPT_IND:
	{
		AT_TC(g_sw_GPRS, "We got EV_CFW_GPRS_CCIOTOPT_IND");
		UINT8 nResp[25] = {0x00,};
		if(nbiot_nvGetCiotReport() == 1)
		{
		    AT_Sprintf(nResp, "+CCIOTOPTI:  %d", cfwEvent.nParam1);
            AT_NW_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nResp, AT_StrLen(nResp), AT_ASYN_GET_DLCI(nSim), nSim);
		}
		break;
	}
#endif

    case EV_CFW_XCPU_TEST_RSP:
    {
        if(cfwEvent.nType == 0)
        {
            UINT8 uOutStr[15] = {0};
            SUL_ZeroMemory8(uOutStr, 15);
            AT_XCPUTEST_RSP *pXcputest_rsp  = (AT_XCPUTEST_RSP *)(cfwEvent.nParam1);
            SUL_StrPrint(uOutStr, "+XCPUTEST: %d,%d,%d,%d,%d,%d ",pXcputest_rsp->result,pXcputest_rsp->para[0],pXcputest_rsp->para[1],pXcputest_rsp->para[2],pXcputest_rsp->para[3]);
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), cfwEvent.nUTI, nSim);
            break;
        }
        else
        {
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        }
        break;
    }
    case EV_CFW_WCPU_TEST_RSP:
    {
        if(cfwEvent.nType == 0)
        {
            UINT8 uOutStr[15] = {0};
            SUL_ZeroMemory8(uOutStr, 15);
            AT_WCPUTEST_RSP *pWcputest_rsp  = (AT_WCPUTEST_RSP *)(cfwEvent.nParam1);
            SUL_StrPrint(uOutStr, "+WCPUTEST: %d,%d,%d,%d,%d,%d ",pWcputest_rsp->result,pWcputest_rsp->para[0],pWcputest_rsp->para[1],pWcputest_rsp->para[2],pWcputest_rsp->para[3]);
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), cfwEvent.nUTI, nSim);
            break;
        }
        else
        {
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
        }
        break;
    }
    case EV_CFW_NW_GET_QSCANF_RSP:
        // ---------------------------------------------------------
       // QSCANF Event
        // ---------------------------------------------------------
    {
		NW_FREQ_INFO *pFreqs=NULL;
        UINT8 outstr[40] = { 0 ,};
        UINT8 outemp[40]  = { 0 ,};
        UINT16 i=0;

        if (0 == cfwEvent.nType)
        {
            pFreqs = (NW_FREQ_INFO *)(cfwEvent.nParam1);
            SUL_ZeroMemory8(outstr, 40);
            SUL_StrCat(outstr, "+QSCANF: ");
			AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, outstr, AT_StrLen(outstr), cfwEvent.nUTI, nSim);
			for(i = 0; i < cfwEvent.nParam2; i++)
			{
				SUL_ZeroMemory8(outemp, 40);
				SUL_StrPrint(outemp, "\r\nCH%d,-%d",pFreqs[i].Arfcn , pFreqs[i].Rssi );
				AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, outemp, AT_StrLen(outemp), cfwEvent.nUTI, nSim);
			}
			AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0,NULL , 0, cfwEvent.nUTI, nSim);
            return;
        }
        else
        {
			AT_NW_Result_Err(pEvent->nParam1 - ERR_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, cfwEvent.nUTI, nSim);
            pEvent->nParam1 = 0;
            return;
        }
    }
    break;
    default:
        break;
    }

    return;
}

VOID NW_SetOperFormatTemp(UINT8 nFormat,CFW_SIM_ID nSimID)
{
    gATCurrentOperFormat[nSimID] = nFormat;
    return;
}

VOID NW_GetOperFormatTemp(UINT8 *nFormat,CFW_SIM_ID nSimID)
{
    COS_Sleep(100);
    *nFormat = gATCurrentOperFormat[nSimID];
    AT_TC(g_sw_AT_NW, "NW_GetOperFormatTemp %d, nSimID\r\n", gATCurrentOperFormat[nSimID] ,nSimID);

    return;
}

UINT32 _ClearCommandPatch4Cops(CFW_SIM_ID nSimID)
{
    BOOL copsFlag = at_ModuleGetCopsFlag(nSimID);
    AT_TC(g_sw_AT_NW, "_ClearCommandPatch4Cops COPS_Set_Flag[%d] %d\r\n", nSimID, copsFlag);

    if(copsFlag)
    {
        at_ModuleSetCopsFlag(nSimID, FALSE);
    }

    return ERR_SUCCESS;
}

UINT32 CFW_GetSimSPN(CFW_SIM_ID nSimID, CFW_SPN_INFO *pSPNInfo);
BOOL g_simopen_flag = TRUE;

VOID AT_NW_CmdFunc_COPS(AT_CMD_PARA *pParam)
{
    UINT8 *upPos;
    UINT8 uOutputStr[64] = { 0, };
    UINT8 uOutStr[12]    = { 0,  };
    UINT8 uOperId[6]     = { 0,  };
    UINT8 *pOperId = NULL;
    UINT8 nOperatorId[7] = { 0,  };
    UINT8 nMode          = 0;
    UINT8 uParaCount     = 0;
    UINT16 uStrLen        = 0;
    UINT32 nRet          = CMD_ERROR_CODE_OK;
    UINT32 eParamOk;
    UINT8 nFormat = 2;
    UINT8 curMode = 0;
    CFW_NW_STATUS_INFO nStatusInfo;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (at_ModuleGetCopsFlag(nSim) == TRUE)  // added by yangtt at 2008-05-05 for bug 8144
    {
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (g_cfg_cfun[nSim]  == 6)  // added by yangtt at 2008-05-05 for bug 8411
    {
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if ((1 > uParaCount) || (3 < uParaCount))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* has at least 1 param, first get mode */
        upPos = pParam->pPara;
        uStrLen = 1;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nMode, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get mode failed */
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nMode > 4)
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // added by yangtt at 05-16 for bug 8227  begin
        if (nMode == 4)
        {
            if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSim))
            {
                AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            } // nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING || //deleted by yangtt at 05-23 for bug 8541
            else if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING ||
                     nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
            {
                AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        if ((nMode == 0) && (uParaCount == 3))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nMode == 2)
        {
            if(g_simopen_flag != FALSE)
            {
                CFW_SimClose(pParam->nDLCI, nSim);
            }
            g_simopen_flag = FALSE;
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else if (nMode == 3)  /* set format only */
        {
            if (uParaCount > 1)
            {
                if (uParaCount != 2)
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nFormat, &uStrLen);

                if (eParamOk != ERR_SUCCESS)  /* failed or format out of range */
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                AT_TC(g_sw_AT_NW, "AT+COPS:   nFormat = %d\n\r", nFormat);

                if ((0 != nFormat) && (1 != nFormat) && (2 != nFormat))
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    NW_SetOperFormatTemp(nFormat,nSim);
                    AT_TC(g_sw_AT_NW, "AT+COPS:   should be ok\n\r");

                    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                    return;
                }
            }
            else
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else  /* register to a network */
        {
            if(g_simopen_flag != TRUE)
            {
                CFW_SetCommSIMOpen(nSim);
                g_simopen_creg_flag = TRUE;
                g_simopen_flag = TRUE;
            }

            if (((1 == nMode) || (4 == nMode)) && (uParaCount != 3))
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (uParaCount > 1) /* has at least 2 param, get format param */
            {
                uStrLen  = 1;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nFormat, &uStrLen);
                if (eParamOk != ERR_SUCCESS)  /* failed or format out of range */
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                AT_TC(g_sw_AT_NW, "AT+COPS:   nFormat = %d uParaCount %d\n\r", nFormat, uParaCount);
                if ((0 != nFormat) && (2 != nFormat))
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uParaCount == 3)
                {
                    if (nFormat == 2) /* numberic format,but need to change to hex mode when send operid to stack */
                    {
                        uStrLen  = 9;
                        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, nOperatorId, &uStrLen);
                        if (eParamOk != ERR_SUCCESS)  /* failed to get numberic id */
                        {
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        else  /* change string mode operid to hex mode */
                        {
                            UINT8 i;

                            NW_SetOperFormatTemp(nFormat,nSim);
                            for (i = 0; i < uStrLen; i++)
                            {
                                if(('f' == nOperatorId[i])||('F' == nOperatorId[i]))
                                {
                                    nOperatorId[i] = 0x0f;
                                }
                                else
                                {
                                    nOperatorId[i] -= '0';
                                }
                            }

                            if (uStrLen == 5)
                                nOperatorId[i] = 0x0f;
                            AT_TC(g_sw_AT_NW, "AT+COPS:nFormat == 2, OperId = %x %x %x %x %x %x\n\r",  nOperatorId[0],
                                  nOperatorId[1], nOperatorId[2], nOperatorId[3], nOperatorId[4], nOperatorId[5]);
                        }
                    }
                    else if (nFormat == 0)  /* long format */
                    {
                        uStrLen = 64;

                        SUL_ZeroMemory8(uOutputStr, uStrLen);
                        eParamOk = AT_Util_GetParaWithRule(upPos, 2, AT_UTIL_PARA_TYPE_STRING, (UINT32 *)uOutputStr, &uStrLen);
                        if (eParamOk != ERR_SUCCESS)  /* failed to get numberic id */
                        {
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        else
                        {
                            AT_TC(g_sw_AT_NW, "AT+COPS:   pOutputStr = %s\n\r", uOutputStr);
                            pOperId = uOperId;
                            SUL_ZeroMemory8(uOperId, 6);
                            eParamOk = CFW_CfgNwGetOperatorId(&pOperId, uOutputStr);
                            AT_TC(g_sw_AT_NW, "AT+COPS:   eParamOk = 0x%x, OperId = %x %x %x %x %x %x\n\r", eParamOk, uOperId[0],
                                  uOperId[1], uOperId[2], uOperId[3], uOperId[4], uOperId[5]);

                            if (eParamOk != ERR_SUCCESS)
                            {
                                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                                return;
                            }
                            else
                            {
                                NW_SetOperFormatTemp(nFormat,nSim);
                                SUL_MemCopy8(nOperatorId, pOperId, 6);
                            }
                        }
                    }
                }

                else if ((uParaCount == 2) && (nMode == 0))
                {
                    NW_SetOperFormatTemp(nFormat,nSim);
                    SUL_ZeroMemory8(nOperatorId, 7);
                }
            }
            else
            {
                SUL_ZeroMemory8(nOperatorId, 7);
            }

            AT_TC(g_sw_AT_NW, "AT+COPS:   eParamOk = 0x%x, OperId = %x %x %x %x %x %x\n\r", eParamOk, nOperatorId[0],
                  nOperatorId[1], nOperatorId[2], nOperatorId[3], nOperatorId[4], nOperatorId[5]);

            CFW_NwGetCurrentOperator(nOperatorId, &curMode, nSim);
            if (nMode == curMode)
            {
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 180, NULL, 0, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                nRet = CFW_NwSetRegistration(nOperatorId, nMode, pParam->nDLCI, nSim);
                if (ERR_SUCCESS == nRet)
                {
                    g_COPS_Mode = gATCurrentCOPS_Mode_Temp = nMode;
                    AT_TC(g_sw_AT_NW, "CFW_NwSetRegistration nMode==%d\r\n", nMode);
                    if (Speeching == TRUE)
                    {
                        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    at_ModuleSetCopsFlag(nSim, TRUE);
                    AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 180, NULL, 0, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    if (ERR_CFW_INVALID_PARAMETER == nRet)
                    {
                        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                    else if (ERR_MENU_ALLOC_MEMORY == nRet)
                    {
                        AT_NW_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {
                        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }
        }
        break;

    case AT_CMD_TEST:  /* deal with the test command */
        if (Speeching == TRUE)  // added by yangtt at 2008-05-05 for bug 8227
        {
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSim))
        {
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nRet = CFW_NwGetAvailableOperators(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == nRet)
        {
            AT_AddWaitingEvent(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP, nSim, pParam->nDLCI);
#ifdef LTE_NBIOT_SUPPORT
            AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 600, NULL, 0, pParam->nDLCI, nSim);
#else
			AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 240, NULL, 0, pParam->nDLCI, nSim);
#endif
            return;
        }
        else
        {
            if (ERR_MENU_ALLOC_MEMORY == nRet)
            {
                AT_NW_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        break;

    case AT_CMD_READ:
    {
        if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSim))
        {
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                 nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING ||
                 nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
        {
            SUL_ZeroMemory8(uOutStr, 12);

            SUL_StrPrint(uOutStr, (UINT8 *)"+COPS: %d", g_COPS_Mode);
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
            return;
        }

        nRet = CFW_NwGetCurrentOperator(nOperatorId, &nMode, nSim); // added by yangtt at 2008-05-12 for bug 8282
        if (ERR_SUCCESS != nRet)
        {
            AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* make up string of current operator */
        UINT8 *pOperName = NULL;
        NW_GetOperFormatTemp(&nFormat,nSim);
        if (0 == nFormat)
        {
            CFW_SPN_INFO SPNInfo;

            SUL_ZeroMemory8(&SPNInfo, SIZEOF(CFW_SPN_INFO));
            SUL_ZeroMemory8(&nStatusInfo, SIZEOF(CFW_NW_STATUS_INFO));

            nRet = CFW_NwGetStatus(&nStatusInfo, nSim);
            if (ERR_SUCCESS != nRet)
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return ;
            }
            if (nStatusInfo.nStatus == CFW_NW_STATUS_REGISTERED_HOME)
            {
                CFW_GetSimSPN(nSim, &SPNInfo);
            }

            AT_TC(g_sw_AT_NW, "COPS: nSPNameLen %d nSPNDisplayFlag %d\n\r", SPNInfo.nSPNameLen, SPNInfo.nSPNDisplayFlag);

            //Considering the shipping risk, to close the issue.Later to do a complete verification.
            //By Lixp 20150518
#ifndef CHIP_HAS_AP
            if(SPNInfo.nSPNameLen && SPNInfo.nSPNDisplayFlag)
            {
                pOperName = SPNInfo.nSpnName;
                AT_TC(g_sw_AT_NW, "COPS_frank: SPN name  %s\n\r", pOperName);
            }
            else
#endif
            {
                nRet = CFW_CfgNwGetOperatorName(nOperatorId, &pOperName); /* required format is long format, so get name */
                AT_TC(g_sw_AT_NW, "COPS_frank: Table name  %s\n\r", pOperName);
            }

            AT_TC(g_sw_AT_NW, "AT+COPS:   CFW_CfgNwGetOperatorName   nRet = 0X%x\n\r", nRet);

            if (nRet != ERR_SUCCESS)
            {
                /* not found operator name, use numberic*/
                nFormat = 2;
            }
        }

        SUL_ZeroMemory8(uOutputStr, 40);
        AT_TC(g_sw_AT_SAT, "COPS nFormat %d", nFormat);
        if (nFormat == 2) /* numberic format */
        {
            if (0x0f == nOperatorId[5])
            {
                SUL_StrPrint(uOutputStr, (UINT8 *)"+COPS: %d,%d,\"%d%d%d%d%d\"", nMode, nFormat,
                             nOperatorId[0], nOperatorId[1], nOperatorId[2], nOperatorId[3], nOperatorId[4]);
            }
            else
            {
                SUL_StrPrint(uOutputStr, (UINT8 *)"+COPS: %d,%d,\"%d%d%d%d%d%d\"", nMode, nFormat,
                             nOperatorId[0], nOperatorId[1], nOperatorId[2], nOperatorId[3], nOperatorId[4], nOperatorId[5]);
            }
        }
        else  /* long or short format */
        {
            SUL_StrPrint(uOutputStr, (UINT8 *)"+COPS: %d,%d,\"%s\"", nMode, nFormat, pOperName);
        }

        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
        break;
    }

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_NW_CmdFunc_CSQ(AT_CMD_PARA *pParam)
{
    UINT8 nSignalLevel    = 0;
    UINT8 nBitError       = 0;
    UINT32 nRet           = 0;
    UINT8 nBuf[20]        = { 0 };
    UINT8 uHelpString[40] = { 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Check [in] Param

    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_EXE:

            if (pParam->pPara == NULL)
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                nRet = CFW_NwGetSignalQuality(&nSignalLevel, &nBitError, nSim);
                AT_TC(g_sw_AT_NW, "AT+CSQ:   nSignalLevel = %d,nBitError = %d\n\r", nSignalLevel, nBitError);
                if (ERR_SUCCESS == nRet)
                {
                    UINT8 nSL[3] = { 0 };
                    UINT8 nBE[5] = { 0 };

                    SUL_ZeroMemory8(nBuf, 20);
                    SUL_StrPrint(nSL, (PCSTR)"%d", nSignalLevel);
                    SUL_StrPrint(nBE, (PCSTR)"%d", nBitError);
                    SUL_StrCopy(nBuf, "+CSQ: ");
                    SUL_StrCat(nBuf, nSL);
                    SUL_StrCat(nBuf, ",");
                    SUL_StrCat(nBuf, nBE);

                    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nBuf, AT_StrLen(nBuf), pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            break;

        case AT_CMD_TEST:
            AT_StrCpy(uHelpString, "+CSQ: (0-31,99),(0-7,99)");
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uHelpString, AT_StrLen(uHelpString), pParam->nDLCI, nSim);
            break;

        default:
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}

VOID AT_NW_CmdFunc_ECSQ(AT_CMD_PARA *pParam)
{
    UINT8 iResult     = 0;
    UINT8 uParamCount = 0;
    UINT16 uSize       = 0;
    UINT8 uState      = 0;
    UINT8 AtRet[20]   = { 0X00, };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
        {
            AT_TC(g_sw_AT_NW, "exe       in AT_NW_CmdFunc_ECSQ, parameters error or no parameters offered \n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get <state>
        uSize = 1;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uState, &uSize);

        if ((iResult != ERR_SUCCESS) || ((uState != 1) && (uState != 0)))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_NW, "AT+ECSQ:   uState = 0x%x\n\r", uState);

            if (uState == 1)
            {
                g_uATNWECSQFlag = 1;
            }
            else
            {
                g_uATNWECSQFlag = 0;
            }
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        if (g_uATNWECSQFlag == 1)
        {
            AT_Sprintf(AtRet, "+ECSQ: 1");
        }
        else if (g_uATNWECSQFlag == 0)
        {
            AT_Sprintf(AtRet, "+ECSQ: 0");
        }
        else
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_Sprintf(AtRet, "+ECSQ: (0,1)");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_NW_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;

}



UINT32 _GetBCCHList(AT_CMD_PARA *pParam)
{
    UINT8 uOutData  = 0x00;
    UINT8 nLen      = 4 * 33;
    UINT16 nParaLen     = 0x00;
    UINT8 i         = 0x00;

    UINT8 nSim      = AT_SIM_ID(pParam->nDLCI);

    UINT8 uParamCount = 0x00;

    UINT16 uData[33] = {0x00};
    UINT32 nRet = ERR_SUCCESS;

    CFW_StoredPlmnList nStorePlmnList;
    memset(&nStorePlmnList, 0x00, sizeof(CFW_StoredPlmnList));

    AT_TC(g_sw_AT_CC, "LOCKBCCH:  nLen %d uData %s", nLen, uData);

    SUL_AsciiToGsmBcdExForNw((INT8*)uData, nLen, &uOutData);


    nStorePlmnList.Plmn[0] = 0x64;
    nStorePlmnList.Plmn[1] = 0xF0;
    nStorePlmnList.Plmn[2] = 0x00;

    INT32 iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
    if (iResult != ERR_SUCCESS)
    {
        AT_TC(g_sw_AT_NW, "_GetBCCHList, parameters error or no parameters offered \n");
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return iResult;
    }

    if(uParamCount > CFW_STORE_MAX_COUNT)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }
    AT_TC(g_sw_AT_CC, "uParamCount:%d",uParamCount);

    for (i = 1; i < uParamCount; i++)
    {
        UINT8 nBCCHDataString[4]= {0x00,};
        nParaLen = 4;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_STRING, nBCCHDataString, &nParaLen);
        uData[i-1] = SUL_StrAToI(nBCCHDataString);

        // if para is null , set nParaLen[i] = 0;
        if (iResult == ERR_AT_UTIL_CMD_PARA_NULL)
        {
            nParaLen = 0;
        }


        if ((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL))     // || TEMP[i]<0))
        {
            AT_TC(g_sw_AT_CC, "AT_Util_GetParaWithRule iResult:%x",iResult);
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return iResult;
        }

        SUL_AsciiToGsmBcdExForNw((INT8*)nBCCHDataString, nParaLen, (UINT8*)(&(nStorePlmnList.ArfcnL[i-1])));
        AT_TC(g_sw_AT_CC, "LOCKBCCH: BCCH %d iResult 0x%x i %d ArfcnL 0x%x",
              uData[i-1],iResult,i,nStorePlmnList.ArfcnL[i-1]);
        nStorePlmnList.nCount++;
    }


    if(0x00 == nStorePlmnList.ArfcnL[0])
        nStorePlmnList.nCount = 0;

    AT_TC(g_sw_AT_CC, "nStorePlmnList.nCount:%d",nStorePlmnList.nCount);

    nRet = CFW_CfgSetStoredPlmnList(&nStorePlmnList, nSim);

    return nRet;

}


VOID AT_NW_CmdFunc_LOCKBCCH(AT_CMD_PARA *pParam)
{

    UINT8 iResult   = 0;
    UINT16 uSize        = 0;
    UINT8 uMode     = 0;
    UINT32 nRet     = ERR_SUCCESS;
    UINT8 AtRet[20]     = { 0x00, };
    UINT8 uOutStr[40 + 4 * 33]    = { 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        if (NULL == pParam->pPara)
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uMode, &uSize);
        if ((iResult != ERR_SUCCESS)
                || ((0 != uMode) && (1 != uMode)&& (2 != uMode)))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_NW, "AT+LOCKBCCH:   uMode = 0x%x\n\r", uMode);

            if ((0 == uMode) || (1 == uMode) )
            {
                if(uMode == CFW_NwGetLockBCCH())
                {
                    //Already OK
                }
                else
                {
                    if(1 == uMode)
                    {
                        nRet = _GetBCCHList(pParam);
                        AT_TC(g_sw_AT_NW, "_GetBCCHList nRet 0x%x\n\r", nRet);
                        if (ERR_CFG_PARAM_OUT_RANGR == nRet)
                        {
                            AT_NW_Result_Err(ERR_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        else
                        {
                            //No process...
                        }
                    }

                    nRet = CFW_NwSetLockBCCH(uMode,pParam->nDLCI, nSim);
                    AT_TC(g_sw_AT_NW, "CFW_NwSetLockBCCH nRet 0x%x\n\r", nRet);
                }
            }
            else if (2 == uMode) //Later
            {
                //g_uATNWECSQFlag = 0;
            }
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        CFW_StoredPlmnList nStorePlmnList;
        memset(&nStorePlmnList, 0x00, sizeof(CFW_StoredPlmnList));

        nRet = CFW_CfgGetStoredPlmnList(&nStorePlmnList, nSim);

        AT_TC(g_sw_AT_NW, "CFW_CfgGetStoredPlmnList nRet = 0x%x\n\r", nRet);

        if (ERR_SUCCESS != nRet)
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            SUL_ZeroMemory8(uOutStr, 40 + 4 * 33);

            SUL_StrPrint(uOutStr, "+LockBCCH: %d \r\n\r\nCount: %d \r\n\r\n",
                         CFW_NwGetLockBCCH(),nStorePlmnList.nCount);

            UINT8 i = 0x00;
            for(; i < nStorePlmnList.nCount; i++)
            {
                UINT8 p[6] = {0x00,};
                SUL_StrPrint(p, "0x%x ", nStorePlmnList.ArfcnL[i]);
                SUL_StrCat((TCHAR *)uOutStr, (CONST TCHAR *)p);
            }


            AT_TC(g_sw_AT_NW, "+LockBCCH: a.ArfcnL %d a.nCount %d\n\r",
                  nStorePlmnList.ArfcnL[0], nStorePlmnList.nCount);

            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
        }

        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_Sprintf(AtRet, "+LockBCCH: mode(0,1),bcch1,bcch2....");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_NW_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;

}



VOID AT_NW_CmdFunc_COPN(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_EXE:

        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount)))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            if (uParaCount)
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                UINT8 *pOperatorId;
                UINT8 *pOperatorName;
                UINT8 pOutputStr[COPN_INFO_LEN] = { 0 };

                UINT32 uOk = ERR_SUCCESS;

                uOk = CFW_CfgNwGetOperatorInfo(&pOperatorId, &pOperatorName, g_COPN_Index);
                AT_TC(g_sw_AT_NW, "+COPN CFW_CfgNwGetOperatorInfo    RET = %d", uOk);

                while (uOk == ERR_SUCCESS)
                {
                    /* init memory */
                    SUL_ZeroMemory8(pOutputStr, COPN_INFO_LEN);

                    /* here to make up the string */

                    if (*(pOperatorId + 5) == 0x0f) /* end with f, f won't be printed,print lenth is 5 */
                    {
                        SUL_StrPrint(pOutputStr, (UINT8 *)"+COPN: \"%d%d%d%d%d\",\"%s\"",
                                     *(pOperatorId), *(pOperatorId + 1), *(pOperatorId + 2), *(pOperatorId + 3),
                                     *(pOperatorId + 4), pOperatorName);
                    }
                    else
                    {
                        SUL_StrPrint(pOutputStr, (UINT8 *)"+COPN: \"%d%d%d%d%d%d\",\"%s\"",
                                     *(pOperatorId), *(pOperatorId + 1), *(pOperatorId + 2), *(pOperatorId + 3),
                                     *(pOperatorId + 4), *(pOperatorId + 5), pOperatorName);

                    }

                    g_COPN_Index++;
                    AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pOutputStr, AT_StrLen(pOutputStr), pParam->nDLCI, nSim);
                    uOk = CFW_CfgNwGetOperatorInfo(&pOperatorId, &pOperatorName, g_COPN_Index);
                }

                if (uOk == ERR_CFW_NOT_EXIST_OPERATOR_ID)
                {
                    g_COPN_Index = 1;
                    AT_NW_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
                    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
        }

        break;

    case AT_CMD_TEST:
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

// ===================================================================
// AT_CMD_CREG
//
// ===================================================================
// #define ERR_CFG_PARAM_OUT_RANGR 0x10001
// #define ERR_AT_CMS_OPER_NOT_ALLOWED         0x10002

VOID AT_NW_CmdFunc_CREG(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount;
    UINT8 uOutputStr[20] = { 0 };
    UINT8 uOutStr[40]    = { 0 };
    UINT8 uString[20]    = { 0 };
    CFW_NW_STATUS_INFO nStatusInfo;

    UINT32 nRet;
    UINT8 n;

    // caowei add for test
    /* UINT8 i,j,k;
       i = j = k = 0;

       if(AT_CIND_NetWork(&i, &j, &k))
       {
       AT_TC(g_sw_NW, "caowei test==== i: %d, j:%d, k: %d\n\r",i,j,k);
       }
       */
    // caowei add end

    // check [in] param
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (uParaCount != 1)
                {
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    UINT8 uData;
                    UINT32 eParamOk;
                    UINT16 nLen = 1;

                    eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &nLen);

                    if (eParamOk == ERR_SUCCESS)
                    {
                        //UINT32 nRet;

                        if (uData > 2)
                        {
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        nRet = CFW_CfgSetNwStatus(uData, nSim);
                        if (ERR_SUCCESS == nRet)
                        {
                            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                            return;
                        }
                        else if (ERR_CFG_PARAM_OUT_RANGR == nRet)
                        {
                            AT_NW_Result_Err(ERR_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        else
                        {
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }
                    else  /* failed */
                    {
                        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }

            break;

        case AT_CMD_TEST:
            AT_StrCpy(uOutputStr, "+CREG: (0,1,2)");
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            nRet = CFW_CfgGetNwStatus(&n, nSim);
            if (ERR_SUCCESS != nRet)
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            nRet = CFW_NwGetStatus(&nStatusInfo, nSim);
            AT_TC(g_sw_AT_NW, "AT+CREG:   nRet = 0x%x\n\r", nRet);
            if (ERR_SUCCESS == nRet)
            {
                SUL_ZeroMemory8(uOutStr, 40);
                SUL_StrPrint(uOutStr, "+CREG: %d,%d", n, nStatusInfo.nStatus);
                AT_TC(g_sw_AT_NW, "+CREG: nStatus %d\n\r", nStatusInfo.nStatus);

                if (n == 2)
                {
                    UINT8 AreaStr[5] = { 0 };
                    UINT8 CellStr[5] = { 0 };

                    if (nStatusInfo.nAreaCode[4] < 16)
                    {
                        SUL_StrPrint(AreaStr, "000%x", nStatusInfo.nAreaCode[4]);
                    }
                    else if (nStatusInfo.nAreaCode[3] == 0)
                    {
                        SUL_StrPrint(AreaStr, "00%x", nStatusInfo.nAreaCode[4]);
                    }
                    else if (nStatusInfo.nAreaCode[3] < 16)
                    {
                        SUL_StrPrint(AreaStr, "0%x%x", nStatusInfo.nAreaCode[3], nStatusInfo.nAreaCode[4]);
                    }
                    else
                    {
                        SUL_StrPrint(AreaStr, "%x%x", nStatusInfo.nAreaCode[3], nStatusInfo.nAreaCode[4]);
                    }

                    if (nStatusInfo.nCellId[1] < 16)
                    {
                        SUL_StrPrint(CellStr, "000%x", nStatusInfo.nCellId[1]);
                    }
                    else if (nStatusInfo.nCellId[0] == 0)
                    {
                        SUL_StrPrint(CellStr, "00%x", nStatusInfo.nCellId[1]);
                    }
                    else if (nStatusInfo.nCellId[0] < 16)
                    {
                        SUL_StrPrint(CellStr, "0%x%x", nStatusInfo.nCellId[0], nStatusInfo.nCellId[1]);
                    }
                    else
                    {
                        SUL_StrPrint(CellStr, "%x%x", nStatusInfo.nCellId[0], nStatusInfo.nCellId[1]);
                    }

                    AT_StrUpr(CellStr);
                    AT_StrUpr(AreaStr);
                    SUL_StrPrint((TCHAR *)uString, (CONST TCHAR *)",\"%s\",\"%s\",%d", AreaStr, CellStr,nStatusInfo.nCurrRat);
//                      SUL_StrPrint((TCHAR *)uString, (CONST TCHAR *)",\"%s\",\"%s\",%d", AreaStr, CellStr,nStatusInfo.PsType);

                    SUL_StrCat((TCHAR *)uOutStr, (CONST TCHAR *)uString);
                }
                else
                {
                    ; // SUL_StrCat(pOutputStr,"\r\n");
                }
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
            }
            else
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            break;

        default:
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }
}

BOOL AT_CIND_NetWork(UINT8 *pSignalQ, UINT8 *pRegistS, UINT8 *pRoamS, UINT8 nDLCI, UINT8 nSim)
{
    UINT8 nSignalLevel = 0;
    UINT8 nBitError    = 0;
    CFW_NW_STATUS_INFO nStatusInfo;
    UINT32 nRet;
    nRet = CFW_NwGetStatus(&nStatusInfo, nSim);

    if (ERR_SUCCESS != nRet)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, nDLCI, nSim);
        return FALSE;
    }

    if (nStatusInfo.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {
        *pRoamS   = 1;
        *pRegistS = 1;
    }
    else if (nStatusInfo.nStatus == CFW_NW_STATUS_REGISTERED_HOME)
    {
        *pRoamS   = 0;
        *pRegistS = 1;
    }
    else
    {
        *pRoamS   = 0;
        *pRegistS = 0;
    }
    nRet = CFW_NwGetSignalQuality(&nSignalLevel, &nBitError, nSim);
    if (ERR_SUCCESS != nRet)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, nDLCI, nSim);
        return FALSE;
    }

    if (nSignalLevel == 0)
        *pSignalQ = 0;
    else if (1 <= nSignalLevel && 7 > nSignalLevel)
        *pSignalQ = 1;
    else if (7 <= nSignalLevel && 13 > nSignalLevel)
        *pSignalQ = 2;
    else if (13 <= nSignalLevel && 19 > nSignalLevel)
        *pSignalQ = 3;
    else if (19 <= nSignalLevel && 25 > nSignalLevel)
        *pSignalQ = 4;
    else if (25 <= nSignalLevel && 31 >= nSignalLevel)
        *pSignalQ = 5;

    return TRUE;

}

// set and get preferred operators list
VOID AT_NW_CmdFunc_CPOL(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    UINT32 nResult  = ERR_SUCCESS;
    INT32 iRet      = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_SET:
        AT_TC(g_sw_AT_NW, "CPOL SET IN \n\r");
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        AT_TC(g_sw_AT_NW, "CPOL SET paraCount %d \n\r", paraCount);

        if ((iRet != ERR_SUCCESS) || (paraCount > 3))
        {
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (paraCount == 0)
        {
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        UINT8 index;
        UINT16 length = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &index, &length);
        if(iRet == ERR_AT_UTIL_CMD_PARA_NULL)
        {
            index = 0;
        }
        else if((iRet != ERR_SUCCESS) || (index == 0))
        {
            AT_TC(g_sw_AT_NW, "CPOL SET first parameter error(%d)!", iRet);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nFormat = 0;
        UINT8 PLMN[10] = {0};
        UINT8 MCC[4] = {0};
        UINT8 MNC[4] = {0};
        if(paraCount > 1)
        {
            length = 1;
            iRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nFormat, &length);
            if((iRet != ERR_SUCCESS) || (nFormat != NW_PREFERRED_OPERATOR_FORMAT_NUMERIC))
            {
                AT_TC(g_sw_AT_NW, "CPOL SET second parameter error!(%d)", iRet);
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if(paraCount > 2)
            {
                length = sizeof(PLMN);
                iRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, PLMN, &length);
                if(iRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "CPOL SET third parameter error!(%d)", iRet);
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                length = strlen(PLMN);
                if((length != 5) && (length != 6))
                {
                    AT_TC(g_sw_AT_NW, "CPOL SET length of third parameter error!(%d)", length);
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                AT_TC(g_sw_AT_NW, "CPOL PLMN: %s", PLMN);

                MCC[0] = PLMN[0];
                MCC[1] = PLMN[1];
                MCC[2] = PLMN[2];
                MNC[0] = PLMN[3];
                MNC[1] = PLMN[4];
                if(length == 6)
                    MNC[2] = PLMN[5];
            }
        }

        AT_TC(g_sw_AT_NW, "CPOL SET: index = %d, MCC= %s, MNC= %s", index, MCC,MNC);
        UINT32 RetValue = CFW_SimWritePreferPLMN(index, MCC, MNC, pParam->nDLCI, nSim);
        if( ERR_SUCCESS == RetValue)
        {
            AT_TC(g_sw_AT_NW, "CFW_SimWritePreferPLMN success!");
            AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_TC(g_sw_AT_NW, "CFW_SimWritePreferPLMN failed(%d)!", RetValue);
            AT_NW_Result_Err(RetValue, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    case AT_CMD_READ:
        AT_TC(g_sw_AT_NW, "CPOL READ IN \n\r");
        gATCurrentnGetPreferredOperatorsCaller = NW_GETPREFERREDOPERATORS_CALLER_READ;
        //NW_GetPreferredOperators(pParam->nDLCI, nSim);
        CFW_SimReadPreferPLMN(pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:
        AT_TC(g_sw_AT_NW, "CPOL TEST IN \n\r");

        nResult = CFW_SimGetPrefOperatorListMaxNum(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == nResult)
        {
            AT_TC(g_sw_AT_NW, "CFW_SimGetPrefOperatorListMaxNum success \n\r");
            AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_NW, "nw-CFW_SimGetPrefOperatorListMaxNum error\n");
            UINT32 nErrorCode = AT_SetCmeErrorCode(nResult, FALSE);

            AT_NW_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}
UINT16 SUL_AsciiToGsmBcdExForNw(
    INT8 *pNumber,  // input
    UINT8 nNumberLen,
    UINT8 *pBCD  // output should >= nNumberLen/2+1
)
{
    UINT8 Tmp;
    UINT32 i;
    UINT32 nBcdSize = 0;
    UINT8 *pTmp = pBCD;

    if(pNumber == (CONST INT8 *)NULL || pBCD == (UINT8 *)NULL)
        return FALSE;

    SUL_MemSet8(pBCD, 0, nNumberLen >> 1);

    for(i = 0; i < nNumberLen; i++)
    {
        switch(*pNumber)
        {
        case 0x41:
        case 0x61:
            Tmp = (INT8)0x0A;
            break;
        case 0x42:
        case 0x62:
            Tmp = (INT8)0x0B;
            break;
        case 0x43:
        case 0x63:
            Tmp = (INT8)0x0C;
            break;
        //--------------------------------------------
        //Modify by lixp 20070626 begin
        //--------------------------------------------
        case 0x44:
        case 0x64:
            Tmp = (INT8)0x0D;
            break;
        case 0x45:
        case 0x65:
            Tmp = (INT8)0x0E;
            break;
        case 0x46:
        case 0x66:
            Tmp = (INT8)0x0F;
            break;
        //--------------------------------------------
        //Modify by lixp 20070626 end
        //--------------------------------------------
        default:
            Tmp = (INT8)(*pNumber - '0');
            break;
        }
        if(i % 2)
        {
            *pTmp++ |= (INT8)(Tmp & 0x0F);
        }
        else
        {
            *pTmp = (Tmp << 4) & 0xF0;

        }
        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if(i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}

VOID AT_NW_GetStoreListString(UINT8 nSim, UINT8 *nDataString)
{
    UINT16 i = 0;
    UINT8 tmp[3] = {0x00,};
    CFW_StoredPlmnList plmn_list;
    UINT8 *p = (UINT8 *)&plmn_list;

    memset(&plmn_list, 0x00, sizeof(CFW_StoredPlmnList));
    CFW_CfgGetStoredPlmnList(&plmn_list, nSim);

    for(i = 0; i < sizeof(CFW_StoredPlmnList); i++)
    {
        SUL_StrPrint(tmp, "%02x", *p++);
        strcat(nDataString, tmp);
    }

    AT_TC(g_sw_AT_NW, "AT_NW_GetStoreListString nCount=%d", plmn_list.nCount);
    return;
}

VOID AT_NW_CmdFunc_CBindArfcn(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount;
    UINT8 uOutputStr[20] = { 0 };
    UINT8 uOutStr[CFW_STORE_MAX_COUNT*3]    = { 0 };
    UINT32 nRet;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
            {
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                AT_TC(g_sw_AT_CC, "CBindArfcn: uParaCount %d", uParaCount);
                if (uParaCount != 1)
                {
                    AT_TC(g_sw_AT_CC, "ERROR! CBindArfcn: uParaCount %d", uParaCount);
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                else
                {
                    UINT16 nLen = sizeof(CFW_StoredPlmnList) * 3;
                    UINT8 uData[sizeof(CFW_StoredPlmnList) * 3] = {0x00,};
                    UINT8 RawData[sizeof(CFW_StoredPlmnList) * 3] = {0x00,};

                    if(AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uData, &nLen) == ERR_SUCCESS)
                    {
                        AT_TC(g_sw_AT_CC, "CBindArfcn:  nLen %d ", nLen);
                        SUL_AsciiToGsmBcdExForNw((INT8 *)uData, nLen, RawData);

                        nRet = CFW_CfgSetStoredPlmnList((CFW_StoredPlmnList *)RawData, nSim);
                        if (ERR_SUCCESS == nRet)
                        {
                            CFW_NW_SendPLMNList((CFW_StoredPlmnList *)RawData, nSim);
                            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                            return;
                        }
                        else if (ERR_CFG_PARAM_OUT_RANGR == nRet)
                        {
                            AT_NW_Result_Err(ERR_CME_INVALID_INDEX, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                        else
                        {
                            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }
                    else  /* failed */
                    {
                        AT_TC(g_sw_AT_NW, "ERROR! get uData");
                        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }

            break;

        case AT_CMD_TEST:
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
        {
            memset(uOutStr, 0x00,CFW_STORE_MAX_COUNT * 3);
            strcpy(uOutStr, "+CBindArfcn: ");

            AT_NW_GetStoreListString(nSim, uOutStr + strlen(uOutStr));
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
        }
        break;

        default:
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

}
#define API_GSM_450  ( 1 << 0 )
#define API_GSM_480  ( 1 << 1 )
#define API_GSM_900P ( 1 << 2 )
#define API_GSM_900E ( 1 << 3 )
#define API_GSM_900R ( 1 << 4 )
#define API_GSM_850  ( 1 << 5 )
#define API_DCS_1800 ( 1 << 6 )
#define API_PCS_1900 ( 1 << 7 )
#ifdef __MULTI_RAT__
#define API_WCDMA_BAND1  ( 1 << 0 )
#define API_WCDMA_BAND2  ( 1 << 1 )
#define API_WCDMA_BAND3  ( 1 << 2 )
#define API_WCDMA_BAND4  ( 1 << 3 )
#define API_WCDMA_BAND5  ( 1 << 4 )
#define API_WCDMA_BAND6  ( 1 << 5 )
#define API_WCDMA_BAND7  ( 1 << 6 )
#define API_WCDMA_BAND8  ( 1 << 7 )
#endif

VOID AT_NW_CmdFunc_SETBND(AT_CMD_PARA *pParam)
{
    INT32 iResult                        = 0;
    UINT8 uParaCount                     = 0;
    UINT8 uIndex                         = 0;
    UINT16 uSize                         = 0;
    UINT8 param1  = 0;
    UINT8 param2  = 0;
    UINT8 pRsp[20] = { 0 ,};
    UINT8 CSDBand_Auto = 0;
    UINT32 ret = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_AT_NW, "SETBND, start!\n");

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
        if((iResult != ERR_SUCCESS)||(uParaCount > 3))
        {
            AT_TC(g_sw_AT_NW, "SETBND, GetParaCount error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uIndex  = 0;
        uSize  = SIZEOF(param1);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &param1, &uSize);
        if(iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "SETBND, get param1 error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        CSDBand_Auto = 0;
        if(param1 == 1)
        {
            CFW_CfgNwGetFrequencyBand(&CSDBand_Auto, nSim);
            AT_TC(g_sw_AT_NW, "SETBND, get band value : %d\n" ,CSDBand_Auto);
            memset(&pRsp, 0x0, 20);
#ifdef __MULTI_RAT__
            UINT8 nUTMSBand = 0;
            CFW_CfgNwGetFrequencyUMTSBand(&nUTMSBand,nSim);
            AT_TC(g_sw_AT_NW, "SETBND, get UMTS band value : %d\n" ,nUTMSBand);
            AT_Sprintf(pRsp, "+SETBND:%d,%d",CSDBand_Auto,nUTMSBand);
#else
            AT_Sprintf(pRsp, "+SETBND:%d",CSDBand_Auto);
#endif
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI, nSim);
        }
        else if(param1 == 2)
        {
            uIndex++;
            uSize   = SIZEOF(param2);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &param2, &uSize);
            if(iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "SETBND, get param2 error!\n");
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            CSDBand_Auto = param2;
            AT_TC(g_sw_AT_NW, "SETBND, set band value : %d\n" ,CSDBand_Auto);
            if ( (CSDBand_Auto & API_GSM_450) || (CSDBand_Auto & API_GSM_480) || (CSDBand_Auto & API_GSM_900R))
            {
                AT_TC(g_sw_AT_NW, "SETBND, band value error!\n");
                AT_NW_Result_Err(ERR_AT_CME_OPTION_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            ret = CFW_CfgNwSetFrequencyBand(CSDBand_Auto,nSim);
            AT_TC(g_sw_AT_NW, "SETBND, set band value : %d\n" ,CSDBand_Auto);
            if(ret != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "SETBND, set band value error %d!\n",ret);
                AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            else
            {
#ifdef __MULTI_RAT__
                if(3 == uParaCount)
                {
                    UINT8 nGetUTMSBand = 0;
                    uIndex++;
                    uSize   = SIZEOF(nGetUTMSBand);
                    iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &nGetUTMSBand, &uSize);
                    AT_TC(g_sw_AT_NW, "SETBND, set UMTS band value : %d\n" ,nGetUTMSBand);
                    if ( (nGetUTMSBand & API_WCDMA_BAND2) || (nGetUTMSBand & API_WCDMA_BAND3) || (nGetUTMSBand & API_WCDMA_BAND4) || (nGetUTMSBand & API_WCDMA_BAND5) || (nGetUTMSBand & API_WCDMA_BAND6) || (nGetUTMSBand & API_WCDMA_BAND7))
                    {
                        AT_TC(g_sw_AT_NW, "SETBND,UMTS band value error!\n");
                        AT_NW_Result_Err(ERR_AT_CME_OPTION_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                    ret = CFW_CfgNwSetFrequencyUMTSBand(nGetUTMSBand,nSim);
                    AT_TC(g_sw_AT_NW, "SETBND, set UMTS band value : %d\n" ,nGetUTMSBand);
                }
#endif
                memset(&pRsp, 0x0, 20);
                AT_Sprintf((UINT8 *)&pRsp, "+SETBND:2\n");
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_TC(g_sw_AT_NW, "SETBND, get param1 value error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    return ;
}
UINT8 gChangeRatComm[CFW_SIM_COUNT] = {0,};// 0, is not change Rat,1,change Rat in flight mode,2, change Rat in nomal mode
VOID AT_NW_CmdFunc_CTEC(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount     = 0;
    UINT8 uOutStr[20]    = {0,};
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT16 nCurrentRat   = 0;
    UINT16 nPreferRat    = 0;
    UINT16 uStrLen       = 0;
    UINT32 eParamOk;
    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_NW, "AT+CTEC:pParam == NULL\n\r" );
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {
        case AT_CMD_SET:
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:AT_CMD_SET:\n\r" );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            /* check para count */
            if ( uParaCount != 2)
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:uParaCount:%d\n\r", uParaCount );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            uStrLen = SIZEOF(nCurrentRat);
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, &nCurrentRat, &uStrLen);
            if (eParamOk != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:eParamOk:%d\n\r", eParamOk );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            //Rat,   1:is CFW_RAT_AUTO , 2 is CFW_RAT_GSM_ONLY, 4 is CFW_RAT_UMTS_ONLY.
            uStrLen = SIZEOF(nPreferRat);
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16, &nPreferRat, &uStrLen);
            if (eParamOk != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:eParamOk:222 %d\n\r", eParamOk );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            eParamOk = CFW_NWSetRat(nPreferRat ,nSim);
            if (eParamOk != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:eParamOk:333 %d\n\r", eParamOk );
                AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            if(nPreferRat == nCurrentRat)
            {
                AT_TC(g_sw_AT_NW, "AT+CTEC:The same Rat\n\r");
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return ;
            }
            else
            {
                CFW_COMM_MODE nFM =CFW_DISABLE_COMM;
                AT_TC(g_sw_AT_NW, "AT+CTEC:nMode: %d\n\r", nCurrentRat );
                if (ERR_SUCCESS != CFW_GetComm((CFW_COMM_MODE *)&nFM, nSim))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if(CFW_DISABLE_COMM == nFM )
                {
                    AT_TC(g_sw_AT_NW, "AT+CTEC:ERROR!Now,It's flight mode,can't change rat!!\n\r" );
                    AT_NW_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return ;
                }

                gChangeRatComm[nSim] = 1;
                if (ERR_SUCCESS != CFW_SetComm(CFW_DISABLE_COMM,1, pParam->nDLCI, nSim))
                {
                    AT_TC(g_sw_AT_NW, "AT+CTEC:ERROR!Set flight mode issue!!\n\r" );
                    gChangeRatComm[nSim] = 0;
                    AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return ;
                }
                else
                {
                    AT_TC(g_sw_AT_NW, "AT+CTEC:Set flight mode DISABLE!!\n\r" );
                }

                AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }
            break;
        case AT_CMD_READ:
            nPreferRat = CFW_NWGetRat(nSim);
            SUL_ZeroMemory8(uOutStr, 20);
            SUL_StrPrint(uOutStr, (UINT8 *)"+CTEC: %d,%d", nPreferRat,nPreferRat);
            AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
            break;
        default:
            AT_TC(g_sw_AT_NW, "AT+CTEC:default: %d\n\r", pParam->iType );
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }
    return;
}
#ifdef __MULTI_RAT__

VOID AT_NW_CmdFunc_SNWR(AT_CMD_PARA *pParam)
{
    UINT8 nMode          = 0;
    UINT8 nSimNum      = 0;
    UINT8 nNwRat         = 0;
    UINT8 uParaCount     = 0;
    UINT16 uStrLen        = 0;
    UINT8 uOutStr[20]    = {0,};
    UINT32 eParamOk;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_NW, "AT+SNWR:pParam == NULL\n\r" );

        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
            {
                AT_TC(g_sw_AT_NW, "AT+SNWR:AT_CMD_SET:\n\r" );

                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            /* check para count */
            if ((2 > uParaCount) || (3 < uParaCount))
            {
                AT_TC(g_sw_AT_NW, "AT+SNWR:uParaCount:%d\n\r", uParaCount );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            uStrLen = 1;

            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nMode, &uStrLen);

            if (eParamOk != ERR_SUCCESS)  /* get mode failed */
            {
                AT_TC(g_sw_AT_NW, "AT+SNWR:eParamOk:%d\n\r", eParamOk );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uStrLen = 1;
            //nSimNum,   0:is sim id 0, 1 is sim id 1
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nSimNum, &uStrLen);
            if (eParamOk != ERR_SUCCESS)  /* get nSimNum failed */
            {
                AT_TC(g_sw_AT_NW, "AT+SNWR:eParamOk:111 %d\n\r", eParamOk );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            //nMode: 0,is read, and 1 is write
            if(0 == nMode)
            {
                if(uParaCount != 2)
                {
                    AT_TC(g_sw_AT_NW, "AT+SNWR:uParaCount:111 %d\n\r", uParaCount );
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }

                nNwRat = CFW_NWGetRat(nSimNum);

                SUL_ZeroMemory8(uOutStr, 20);

                SUL_StrPrint(uOutStr, (UINT8 *)"+SNWR: %d", nNwRat);
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutStr, AT_StrLen(uOutStr), pParam->nDLCI, nSim);
            }
            else if(1 == nMode)
            {
                if(uParaCount != 3)
                {
                    AT_TC(g_sw_AT_NW, "AT+SNWR:uParaCount:222 %d\n\r", uParaCount );
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                uStrLen = 1;
                //nNwRat,   1:is CFW_RAT_AUTO , 2 is CFW_RAT_GSM_ONLY, 4 is CFW_RAT_UMTS_ONLY.
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &nNwRat, &uStrLen);
                if (eParamOk != ERR_SUCCESS)  /* get nNwRat failed */
                {
                    AT_TC(g_sw_AT_NW, "AT+SNWR:eParamOk:222 %d\n\r", eParamOk );
                    AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                eParamOk = CFW_NWSetRat(nNwRat ,nSimNum);
                if (eParamOk != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "AT+SNWR:eParamOk:333 %d\n\r", eParamOk );
                    AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_AT_NW, "AT+SNWR:nMode: %d\n\r", nMode );
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            break;

        default:
            AT_TC(g_sw_AT_NW, "AT+SNWR:default:: %d\n\r", pParam->iType );
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}
#endif
// frank add start
#ifdef AT_USER_DBS
VOID AT_NW_POSI_AddData(UINT8 nflag, CFW_TSM_CURR_CELL_INFO *pCurrCellInfo, CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo)
{
    UINT8 i = 0;

    AT_TC(g_sw_AT_NW, "POSI_AddData nflag: %d!\n", nflag);

    if (0 == nflag)
    {
        if (!((pCurrCellInfo->nTSM_CellID[0] == 0) && (pCurrCellInfo->nTSM_CellID[1] == 0)))  // shaochuan
        {
            gPosiList.PosiList[gPosiList.iNum].sMcc[0] = pCurrCellInfo->nTSM_LAI[0] & 0x0F;
            gPosiList.PosiList[gPosiList.iNum].sMcc[1] = pCurrCellInfo->nTSM_LAI[0] >> 4;
            gPosiList.PosiList[gPosiList.iNum].sMcc[2] = pCurrCellInfo->nTSM_LAI[1] & 0x0F;

            gPosiList.PosiList[gPosiList.iNum].sMnc[2] = pCurrCellInfo->nTSM_LAI[1] >> 4;
            gPosiList.PosiList[gPosiList.iNum].sMnc[1] = pCurrCellInfo->nTSM_LAI[2] & 0x0F;
            gPosiList.PosiList[gPosiList.iNum].sMnc[0] = pCurrCellInfo->nTSM_LAI[2] >> 4;

            gPosiList.PosiList[gPosiList.iNum].sLac    = (pCurrCellInfo->nTSM_LAI[3] << 8) | (pCurrCellInfo->nTSM_LAI[4]);
            gPosiList.PosiList[gPosiList.iNum].sCellID =
                (pCurrCellInfo->nTSM_CellID[0] << 8) | (pCurrCellInfo->nTSM_CellID[1]);

            gPosiList.PosiList[gPosiList.iNum].iBsic  = pCurrCellInfo->nTSM_Bsic;
            gPosiList.PosiList[gPosiList.iNum].iRxLev = ((110 - pCurrCellInfo->nTSM_AvRxLevel) * 100) / 64;

            gPosiList.iNum++;
        }
    }
    else
    {

        for (i = 0; i < pNeighborCellInfo->nTSM_NebCellNUM; i++)
        {
            if (!((pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[0] == 0)
                    && (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[1] == 0)))
            {

                gPosiList.PosiList[gPosiList.iNum].sMcc[0] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] & 0x0F;
                gPosiList.PosiList[gPosiList.iNum].sMcc[1] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[0] >> 4;
                gPosiList.PosiList[gPosiList.iNum].sMcc[2] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] & 0x0F;

                gPosiList.PosiList[gPosiList.iNum].sMnc[2] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[1] >> 4;
                gPosiList.PosiList[gPosiList.iNum].sMnc[1] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] & 0x0F;
                gPosiList.PosiList[gPosiList.iNum].sMnc[0] = pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[2] >> 4;

                gPosiList.PosiList[gPosiList.iNum].sLac =
                    (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[3] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI[4]);
                gPosiList.PosiList[gPosiList.iNum].sCellID =
                    (pNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID[0] << 8) | (pNeighborCellInfo->nTSM_NebCell[i].
                            nTSM_CellID[1]);

                gPosiList.PosiList[gPosiList.iNum].iBsic  = pNeighborCellInfo->nTSM_NebCell[i].nTSM_Bsic;
                gPosiList.PosiList[gPosiList.iNum].iRxLev =
                    ((110 - pNeighborCellInfo->nTSM_NebCell[i].nTSM_AvRxLevel) * 100) / 64;

                gPosiList.iNum++;
            }
        }

    }

    return;
}

#define RAND_RAM_LEN 40
typedef struct _AT_RAND_RAM
{
    UINT8 nRandRamBase[RAND_RAM_LEN];
    UINT8 nMod;
    struct AT_RAND_RAM *pBef;
    struct AT_RAND_RAM *pNext;
} AT_RAND_RAM;

AT_RAND_RAM *gRandRamBase = NULL;

BOOL AT_RandRamPush(UINT8 *pRanRam, UINT32 nRamRamLen, UINT8 nMod)
{
    AT_RAND_RAM *p = gRandRamBase;

    if (!p)
    {
        p = (AT_RAND_RAM *)AT_MALLOC(SIZEOF(AT_RAND_RAM));
        if (!p)
        {
            // FAILED
            return FALSE;
        }
        AT_MemZero(p, SIZEOF(AT_RAND_RAM));

        p->pNext = NULL;
        p->pBef  = NULL;
        p->nMod  = nMod;
        AT_MemCpy(p->nRandRamBase, pRanRam, nRamRamLen < RAND_RAM_LEN ? nRamRamLen : RAND_RAM_LEN - 1);

        gRandRamBase = p;
    }
    else
    {
        while (p->pNext)
            p = p->pNext;

        AT_RAND_RAM *q;
        q = (AT_RAND_RAM *)AT_MALLOC(SIZEOF(AT_RAND_RAM));
        if (!q)
        {
            // FAILED
            return FALSE;
        }
        AT_MemZero(q, SIZEOF(AT_RAND_RAM));

        q->pNext = NULL;
        q->pBef  = p;
        p->pNext = q;
        q->nMod  = nMod;
        AT_MemCpy(q->nRandRamBase, pRanRam, nRamRamLen < RAND_RAM_LEN ? nRamRamLen : RAND_RAM_LEN - 1);
    }
    return TRUE;
}

BOOL AT_RandRamPop(UINT8 *pRandRamData, UINT32 *pLen, UINT8 *pMod)
{

    AT_RAND_RAM *p = gRandRamBase;
    if (!p)
    {
        *pLen = 0x00;
        return NULL;
    }
    else
    {
        *pLen = AT_StrLen(p->nRandRamBase);
        *pMod = p->nMod;
        AT_MemCpy(pRandRamData, p->nRandRamBase, *pLen);

        // gRandRamBase = p->pNext;

        AT_RAND_RAM *q;
        q = p->pNext;
        if (q)
            q->pBef    = NULL;
        gRandRamBase = q;
        AT_FREE(p);

    }
}
VOID AT_DBSBubbleSort()
{
    UINT8 i        = 0;
    UINT8 j        = 0;
    UINT8 nRxLev   = 0;
    UINT8 nArr[32] = { 0x00, };

    if (gPosiList.iNum <= 1)
    {
        return;
    }

    for (i = 0; i < gPosiList.iNum - 1; i++)
    {
        for (j = gPosiList.iNum - 2; j > i; j--)
        {
            if (gPosiList.PosiList[j].iRxLev > gPosiList.PosiList[j - 1].iRxLev)
            {
                AT_MemCpy(nArr, gPosiArres.arres[j].ndsposi, 32);
                AT_MemCpy(gPosiArres.arres[j].ndsposi, gPosiArres.arres[j - 1].ndsposi, 32);
                AT_MemCpy(gPosiArres.arres[j - 1].ndsposi, nArr, 32);

                nRxLev                       = gPosiList.PosiList[j].iRxLev;
                gPosiList.PosiList[j].iRxLev = gPosiList.PosiList[j - 1].iRxLev;
                gPosiList.PosiList[j - 1].iRxLev = nRxLev;
            }
        }
    }

    return;
}

VOID AT_ProcessPosiData(UINT8 *pSigndata)
{

    INT nSeedRet        = 0;
    UINT8 nGetSeedLen   = 0;
    UINT8 nGetSeed[63]  = { 0x00, };
    UINT8 nSeedData[44] = { 0x00, };
    UINT8 nSeedBCD[40]  = { 0x00, };

    UINT8 nSignDataLen      = 50;
    UINT8 nSignData[50]     = { 0x00, };
    UINT8 nSrcSignDataLen   = 0;
    UINT8 nSrcSignData[400] = { 0x00, };
    UINT8 randNumASCII_len  = 0;
    UINT8 nRandNumBCD_len   = 0;
    UINT8 nRandNumBCD[20]   = { 0x00, };
    UINT8 nCidver[10]       = { 0x00, };

    if (gPosiList.iNum == 1)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,\"%s\"", gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 2)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 3)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 4)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 5)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 6)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiArres.arres[5].ndsposi,
                   gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 7)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiArres.arres[5].ndsposi,
                   gPosiArres.arres[6].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 8)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiArres.arres[5].ndsposi,
                   gPosiArres.arres[6].ndsposi, gPosiArres.arres[7].ndsposi, gPosiList.sRandNum);
    }
    else if (gPosiList.iNum == 9)
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiArres.arres[5].ndsposi,
                   gPosiArres.arres[6].ndsposi, gPosiArres.arres[7].ndsposi, gPosiArres.arres[8].ndsposi,
                   gPosiList.sRandNum);
    }
    else
    {
        AT_Sprintf(nSrcSignData, "+POSI:%x,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\"%s\"",
                   gPosiList.iMode, gPosiArres.arres[0].ndsposi, gPosiArres.arres[1].ndsposi, gPosiArres.arres[2].ndsposi,
                   gPosiArres.arres[3].ndsposi, gPosiArres.arres[4].ndsposi, gPosiArres.arres[5].ndsposi,
                   gPosiArres.arres[6].ndsposi, gPosiArres.arres[7].ndsposi, gPosiArres.arres[8].ndsposi,
                   gPosiArres.arres[gPosiList.iNum - 1].ndsposi, gPosiList.sRandNum);
    }

    nSrcSignDataLen  = AT_StrLen(nSrcSignData);
    randNumASCII_len = AT_StrLen(gPosiList.sRandNum);
    nRandNumBCD_len  = AT_DBSAsciiToGsmBcd(gPosiList.sRandNum, randNumASCII_len, nRandNumBCD);

    CFW_GetDBSSeed(nGetSeed);
    nGetSeedLen = AT_StrLen(nGetSeed);
    AT_DBSAsciiToGsmBcd(nGetSeed, nGetSeedLen, nSeedBCD);
    {
        AT_MemCpy(nCidver, nSeedBCD, 4);
        AT_MemCpy(nSeedData, (nSeedBCD + 4), 16);
        const UINT8 *idv  = nCidver;
        const UINT8 *gidv = g_Gidver;

        nSeedRet = sarft_sign_data(nSrcSignData, nSrcSignDataLen, nRandNumBCD,
                                   nRandNumBCD_len, idv, gidv, nSeedData, nSignData, &nSignDataLen);
    }
    if (nSeedRet != 0)
    {
        // UINT8* seedstr = sarft_error_str(sarft_sign_errno);
        // AT_TC(g_sw_GPRS, "ProcessPosiData sign error: %s\n", seedstr);
    }
    else
    {
        AT_TC(g_sw_GPRS, "ProcessPosiData sign success!\n");
    }

    AT_Sprintf(pSigndata,
               "%s,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"",
               nSrcSignData, nSignData[0], nSignData[1], nSignData[2], nSignData[3], nSignData[4], nSignData[5],
               nSignData[6], nSignData[7], nSignData[8], nSignData[9], nSignData[10], nSignData[11], nSignData[12],
               nSignData[13], nSignData[14], nSignData[15], nSignData[16], nSignData[17], nSignData[18], nSignData[19],
               nSignData[20], nSignData[21], nSignData[22], nSignData[23]);

    return;
}

VOID AT_NW_POSI_SendData(UINT8 nSim)
{
    UINT8 i              = 0;
    UINT8 nSignData[450] = { 0x00, };

    AT_TC(g_sw_AT_NW, "SendData iNum: %d!\n", gPosiList.iNum);

    if ((gPosiList.iNum > 0) && (gPosiList.iNum < 15))
    {
        UINT32 nRandRamLen = 0x00;
        UINT8 nRandRamMode = 0x00;

        AT_MemZero(gPosiList.sRandNum, 40);
        gPosiList.iMode = 0x00;

        AT_RandRamPop(gPosiList.sRandNum, &nRandRamLen, &gPosiList.iMode);

        for (i = 0; i < gPosiList.iNum; i++)
        {

            if (1 == gPosiList.iNum)  // only one item
            {
                gPosiList.PosiList[gPosiList.iNum - 1].iEndED = 1;

                // send  start and end

                AT_Sprintf(&gPosiArres.arres[0].ndsposi, "%x%x%x,%x%x,%x,%x,%x,%x,%x",
                           gPosiList.PosiList[i].sMcc[0], gPosiList.PosiList[i].sMcc[1], gPosiList.PosiList[i].sMcc[2],
                           gPosiList.PosiList[i].sMnc[1], gPosiList.PosiList[i].sMnc[0], gPosiList.PosiList[i].sLac,
                           gPosiList.PosiList[i].sCellID, gPosiList.PosiList[i].iBsic, gPosiList.PosiList[i].iRxLev,
                           gPosiList.PosiList[i].iEndED);

            }
            else if (i + 1 == gPosiList.iNum) // last  item
            {
                gPosiList.PosiList[gPosiList.iNum - 1].iEndED = 1;

                // send  end
                AT_Sprintf(&gPosiArres.arres[i].ndsposi, "%x%x%x,%x%x,%x,%x,%x,%x,%x",
                           gPosiList.PosiList[i].sMcc[0], gPosiList.PosiList[i].sMcc[1], gPosiList.PosiList[i].sMcc[2],
                           gPosiList.PosiList[i].sMnc[1], gPosiList.PosiList[i].sMnc[0], gPosiList.PosiList[i].sLac,
                           gPosiList.PosiList[i].sCellID, gPosiList.PosiList[i].iBsic, gPosiList.PosiList[i].iRxLev,
                           gPosiList.PosiList[i].iEndED);
            }
            else if (0 == i)  // frist item
            {

                gPosiList.PosiList[gPosiList.iNum - 1].iEndED = 0;

                AT_Sprintf(&gPosiArres.arres[i].ndsposi, "%x%x%x,%x%x,%x,%x,%x,%x,%x",
                           gPosiList.PosiList[i].sMcc[0], gPosiList.PosiList[i].sMcc[1], gPosiList.PosiList[i].sMcc[2],
                           gPosiList.PosiList[i].sMnc[1], gPosiList.PosiList[i].sMnc[0], gPosiList.PosiList[i].sLac,
                           gPosiList.PosiList[i].sCellID, gPosiList.PosiList[i].iBsic, gPosiList.PosiList[i].iRxLev,
                           gPosiList.PosiList[i].iEndED);

            }
            else
            {
                gPosiList.PosiList[gPosiList.iNum - 1].iEndED = 0;

                AT_Sprintf(&gPosiArres.arres[i].ndsposi, "%x%x%x,%x%x,%x,%x,%x,%x,%x",
                           gPosiList.PosiList[i].sMcc[0], gPosiList.PosiList[i].sMcc[1], gPosiList.PosiList[i].sMcc[2],
                           gPosiList.PosiList[i].sMnc[1], gPosiList.PosiList[i].sMnc[0], gPosiList.PosiList[i].sLac,
                           gPosiList.PosiList[i].sCellID, gPosiList.PosiList[i].iBsic, gPosiList.PosiList[i].iRxLev,
                           gPosiList.PosiList[i].iEndED);

            }

        }

        // bubble sort
        AT_DBSBubbleSort();
        AT_ProcessPosiData(nSignData);
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nSignData, AT_StrLen(nSignData), 0, nSim);
        AT_MemZero(&gPosiList, sizeof(gPosiList));
    }
    else if (0 == gPosiList.iNum)
    {
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, 0, nSim);
    }
    else if (0xff == gPosiList.iNum)
    {
        AT_TC(g_sw_AT_NW, "SendData NO cell info!\n");
    }
    else
    {
        AT_NW_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, 0, nSim);
    }
    return;
}
VOID AT_NW_CmdFunc_POSI(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    UINT32 nResult  = ERR_SUCCESS;
    INT32 iRet      = 0;
    UINT8 nMode        = 0;
    UINT8 nTypSize     = 0;
    UINT8 nIndex       = 0;
    UINT8 nRandNum[40] = { 0x00, };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_GPRS, "AT+POSI:AT_NW_CmdFunc_POSI");
    if (pParam == NULL)
    {
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 2))
        {
            AT_TC(g_sw_GPRS, "AT+POSI ERROR:paraCount: %d", paraCount);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // index is 0,get MODE
        nIndex   = 0;
        nTypSize = 1;
        iRet     = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nMode, &nTypSize);
        AT_TC(g_sw_GPRS, "AT+POSI nMode: %d", nMode);
        if (ERR_SUCCESS != iRet)
        {
            AT_TC(g_sw_GPRS, "AT+POSI ERROR:nMode!=1 !");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // index is 1,get RANDNUM
        nIndex++;
        nTypSize = sizeof(nRandNum);
        AT_MemZero(nRandNum, nTypSize);
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_STRING, nRandNum, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+POSI ERROR:iRet: %d, GetPara ", iRet);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // AT_MemZero(&gPosiList, sizeof(gPosiList));
        // memcpy(&gPosiList.sRandNum, nRandNum, nTypSize);
        AT_RandRamPush(nRandNum, nTypSize, nMode);

        // gPosiList.iMode = nMode;

        // get Cell info
        {
            CFW_TSM_FUNCTION_SELECT tSelecFUN;
            UINT16 nUTI = 0;

            tSelecFUN.nNeighborCell = 1;
            tSelecFUN.nServingCell  = 1;
            AT_GetFreeUTI(0, &nUTI);
            iRet = CFW_EmodOutfieldTestStart(&tSelecFUN, pParam->nDLCI, nSim);
            if (iRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "AT+POSI ERROR:iRet: %d, Emod", iRet);
                AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (CFW_INIT_STATUS_NO_SIM == cfwInitInfo.noSim[0].nType)
            {
                // get sim1 cell info
                AT_GetFreeUTI(0, &nUTI);
                iRet = CFW_EmodOutfieldTestStart(&tSelecFUN, pParam->nDLCI, nSim);
                if (iRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "AT+POSI ERROR:iRet: %d, Emod", iRet);
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
            AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        }

        break;

    case AT_CMD_READ:
    // break;

    case AT_CMD_TEST:
    // break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}
UINT16 AT_DBSAsciiToGsmBcd(INT8 *pNumber, UINT8 nNumberLen, UINT8 *pBCD)
{
    UINT8 Tmp;
    UINT32 i;
    UINT32 nBcdSize = 0;
    UINT8 *pTmp     = pBCD;

    if (pNumber == (CONST INT8 *)NULL || pBCD == (UINT8 *)NULL)
        return FALSE;

    SUL_MemSet8(pBCD, 0, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        switch (*pNumber)
        {
        case 'A':
        case 'a':
            Tmp = (INT8)0x0A;
            break;
        case 'B':
        case 'b':
            Tmp = (INT8)0x0B;
            break;
        case 'C':
        case 'c':
            Tmp = (INT8)0x0C;
            break;

        // --------------------------------------------
        // Modify by lixp 20070626 begin
        // --------------------------------------------
        case 'D':
        case 'd':
            Tmp = (INT8)0x0D;
            break;
        case 'E':
        case 'e':
            Tmp = (INT8)0x0E;
            break;
        case 'F':
        case 'f':
            Tmp = (INT8)0x0F;
            break;

        // --------------------------------------------
        // Modify by lixp 20070626 end
        // --------------------------------------------
        default:
            Tmp = (INT8)(*pNumber - '0');
            break;
        }
#  if 1
        if (i % 2)
        {
            *pTmp++ |= Tmp & 0x0F;
        }
        else
        {
            *pTmp = (Tmp & 0x0F) << 4;
        }
#endif

#  if 0
        if (i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (INT8)(Tmp & 0x0F);
        }
#endif
        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;
    if (i % 2)
    {
        *pTmp |= 0x00;
        nBcdSize += 1;
    }
    return nBcdSize;
}
VOID AT_ProcessCgsnData(UINT8 *pSigndata, UINT8 *pRandNum, UINT8 nTypSize)
{
    UINT8 nImei[16]  = { 0x00, };
    UINT8 nImeiLen   = 0;
    UINT8 nImeiD[16] = { 0x00, };

    INT nSeedRet        = 0;
    UINT8 nGetSeedLen   = 0;
    UINT8 nGetSeed[63]  = { 0x00, };
    UINT8 nSeedData[44] = { 0x00, };
    UINT8 nSeedBCD[40]  = { 0x00, };

    UINT8 nSignDataLen      = 50;
    UINT8 nSignData[50]     = { 0x00, };
    UINT8 nSrcSignDataLen   = 0;
    UINT8 nSrcSignData[400] = { 0x00, };
    UINT8 nRandNumASCIILen  = 0;
    UINT8 nRandNumBCDLen    = 0;
    UINT8 nRandNumBCD[20]   = { 0x00, };
    UINT8 nCidver[10]       = { 0x00, };

    UINT8 i                = 0;
    UINT8 nGidverASCII[64] = { 0x00 };
    INT64 iGidverD         = 0;

    nRandNumBCDLen = AT_DBSAsciiToGsmBcd(pRandNum, nTypSize, nRandNumBCD);

    // get imei num
    CFW_EmodGetIMEI(nImei, &nImeiLen, 0);

    AT_Sprintf(nSrcSignData, "+CGSN:%s,\"%s\"", nImei, pRandNum);
    nSrcSignDataLen = AT_StrLen(nSrcSignData);

    for (i = 0; i < 15; i++)
    {
        nImeiD[i] = nImei[i] - '0';
    }

    iGidverD = nImeiD[14] + nImeiD[13] * (10) + nImeiD[12] * (1.0e2) + nImeiD[11] * (1.0e3) + nImeiD[10] * (1.0e4) +
               nImeiD[9] * (1.0e5) + nImeiD[8] * (1.0e6) + nImeiD[7] * (1.0e7) + nImeiD[6] * (1.0e8) + nImeiD[5] * (1.0e9) +
               nImeiD[4] * (1.0e10) + nImeiD[3] * (1.0e11) + nImeiD[2] * (1.0e12) + nImeiD[1] * (1.0e13) + nImeiD[0] * (1.0e14);
    ui64toa(iGidverD, nGidverASCII, 16);
    nRandNumASCIILen = AT_StrLen(nGidverASCII);
    AT_DBSAsciiToGsmBcd(nGidverASCII, nRandNumASCIILen, g_Gidver);

    CFW_GetDBSSeed(nGetSeed);

    nGetSeedLen = AT_StrLen(nGetSeed);
    AT_DBSAsciiToGsmBcd(nGetSeed, nGetSeedLen, nSeedBCD);
    {
        AT_MemCpy(nCidver, nSeedBCD, 4);
        AT_MemCpy(nSeedData, (nSeedBCD + 4), 16);
        const UINT8 *idv  = nCidver;
        const UINT8 *gidv = g_Gidver;

        nSeedRet = sarft_sign_data(nSrcSignData, nSrcSignDataLen, nRandNumBCD,
                                   nRandNumBCDLen, idv, gidv, nSeedData, nSignData, &nSignDataLen);
    }
    if (nSeedRet != 0)
    {
        AT_TC(g_sw_GPRS, "ProcessPosiData: sign error: %d", nSeedRet);

        // sarft_error_str(sarft_sign_errno);
    }
    else
    {
        AT_TC(g_sw_GPRS, "sign success: %d", nSeedRet);
    }

    AT_Sprintf(pSigndata,
               "%s,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"",
               nSrcSignData, nSignData[0], nSignData[1], nSignData[2], nSignData[3], nSignData[4], nSignData[5],
               nSignData[6], nSignData[7], nSignData[8], nSignData[9], nSignData[10], nSignData[11], nSignData[12],
               nSignData[13], nSignData[14], nSignData[15], nSignData[16], nSignData[17], nSignData[18], nSignData[19],
               nSignData[20], nSignData[21], nSignData[22], nSignData[23]);

    return;
}
VOID AT_NW_CmdFunc_CGSN(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    UINT32 nResult  = ERR_SUCCESS;
    INT32 iRet      = 0;

    UINT8 nTypSize     = 0;
    UINT8 nIndex       = 0;
    UINT8 nRandNum[40] = { 0x00, };

    // UINT8 nImei[16]={0};
    UINT8 nImei[16] = { 0x00, };
    UINT8 nImeiLen  = 0;

    UINT8 nArrRes[120];
    UINT8 nSignData[50] = { 0x00, };  // NDS
    int nSignDataLen    = 50;  // NDS
    int nSeedRet;
    UINT8 nWaitSignedData[70] = { 0x00, };
    int nWaitSignedDataLen;
    UINT8 nCidVer[10] = { 0x00, };

    // UINT8 gidver[10] = {0x00,};
    UINT8 nGetSeed[63]    = { 0x00, };
    UINT8 nSeedData[44]   = { 0x00, };
    int nRandNumBCD_len   = 0;
    UINT8 nRandNumBCD[20] = { 0x00, };
    UINT8 nGetSeedLen     = 0;
    UINT8 nSeedBCD[40]    = { 0x00, };
    UINT8 nSeedBCDLen     = 0;
    UINT8 nImeiD[16] = { 0x00, };
    UINT8 nImeiD_len = 0;
    UINT8 nJbcd            = 0;
    UINT8 nGidVerAscii[64] = { 0x00 };
    UINT8 nGidVerAsciiLen  = 0;
    INT64 nGidVerD         = 0;
    UINT8 nGidVerLen       = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_TC(g_sw_GPRS, "AT+CGSN:CmdFunc_CGSN: ");

    if (pParam == NULL)
    {
        AT_TC(g_sw_GPRS, "AT+CGSN:pParam == NULL !");
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            AT_TC(g_sw_GPRS, "AT+CGSN:paraCount: %d", paraCount);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nTypSize = sizeof(nRandNum);
        AT_MemZero(nRandNum, nTypSize);
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_STRING, nRandNum, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CGSN:iRet : %d !", iRet);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nRandNumBCD_len = AT_DBSAsciiToGsmBcd(nRandNum, nTypSize, nRandNumBCD);

        // get imei num
        CFW_EmodGetIMEI(nImei, &nImeiLen, 0);

        AT_Sprintf(nWaitSignedData, "+CGSN:%s,\"%s\"", nImei, nRandNum);
        nWaitSignedDataLen = AT_StrLen(nWaitSignedData);

        // get imei num
        // CFW_EmodGetIMEI(nImei, &nImeiLen,0);
        for (nJbcd = 0; nJbcd < 15; nJbcd++)
        {
            nImeiD[nJbcd] = nImei[nJbcd] - '0';
        }

        nGidVerD =
            nImeiD[14] + nImeiD[13] * (10) + nImeiD[12] * (1.0e2) + nImeiD[11] * (1.0e3) + nImeiD[10] * (1.0e4) +
            nImeiD[9] * (1.0e5) + nImeiD[8] * (1.0e6) + nImeiD[7] * (1.0e7) + nImeiD[6] * (1.0e8) + nImeiD[5] * (1.0e9) +
            nImeiD[4] * (1.0e10) + nImeiD[3] * (1.0e11) + nImeiD[2] * (1.0e12) + nImeiD[1] * (1.0e13) + nImeiD[0] * (1.0e14);
        ui64toa(nGidVerD, nGidVerAscii, 16);
        nGidVerAsciiLen = AT_StrLen(nGidVerAscii);
        nGidVerLen      = AT_DBSAsciiToGsmBcd(nGidVerAscii, nGidVerAsciiLen, g_Gidver);

        CFW_GetDBSSeed(nGetSeed);

        // AT_TC(g_sw_GPRS, "nGetSeed : %s !",nGetSeed);
        nGetSeedLen = AT_StrLen(nGetSeed);
        nSeedBCDLen = AT_DBSAsciiToGsmBcd(nGetSeed, nGetSeedLen, nSeedBCD);

        AT_MemCpy(nCidVer, nSeedBCD, 4);
        AT_MemCpy(nSeedData, (nSeedBCD + 4), 16);
        const UINT8 *idv  = nCidVer;
        const UINT8 *gidv = g_Gidver;

        nSeedRet = sarft_sign_data(nWaitSignedData, nWaitSignedDataLen, nRandNumBCD,
                                   nRandNumBCD_len, idv, gidv, nSeedData, nSignData, &nSignDataLen);

        if (nSeedRet != 0)
        {
            AT_TC(g_sw_GPRS, "sign error: %d", nSeedRet);

            // UINT8* seedstr = sarft_error_str(sarft_sign_errno);

            // AT_TC(g_sw_GPRS, "sign error: %s\n", seedstr);

        }
        else
        {
            AT_TC(g_sw_GPRS, "sign success: %d", nSeedRet);
        }

        AT_Sprintf(nArrRes,
                   "%s,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"",
                   nWaitSignedData, nSignData[0], nSignData[1], nSignData[2], nSignData[3], nSignData[4], nSignData[5],
                   nSignData[6], nSignData[7], nSignData[8], nSignData[9], nSignData[10], nSignData[11], nSignData[12],
                   nSignData[13], nSignData[14], nSignData[15], nSignData[16], nSignData[17], nSignData[18], nSignData[19],
                   nSignData[20], nSignData[21], nSignData[22], nSignData[23]);
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nArrRes, AT_StrLen(nArrRes), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:
    // break;

    case AT_CMD_TEST:
    // break;

    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_NW_CmdFunc_GSMSTATUS(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    INT32 iRet      = 0;

    UINT8 nStauts = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_GPRS, "AT+CGSN:CmdFunc_GSMSTATUS: ");

    if (pParam == NULL)
    {
        AT_TC(g_sw_GPRS, "AT+GSMSTATUS:pParam == NULL !");
        AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 0))
        {
            AT_TC(g_sw_GPRS, "AT+GSMSTATUS:paraCount: %d", paraCount);
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        nStauts = at_ModuleGetInitResult();

        // netwrok error
        if ((2 == gCsStauts.nType) && ((CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING == gCsStauts.nParam1) ||
                                       (CFW_NW_STATUS_NOTREGISTERED_SEARCHING == gCsStauts.nParam1) ||
                                       (CFW_NW_STATUS_UNKNOW == gCsStauts.nParam1)))
        {
            at_CmdRespErrorText(pParam->engine, "NETERR");
            return;
        }

        switch (nStauts)
        {
        // sim error
        case AT_MODULE_INIT_NO_SIM:
        case AT_MODULE_INIT_SIM_AUTH_READY:
        case AT_MODULE_INIT_SIM_AUTH_WAIT_PUK1:
        case AT_MODULE_INIT_SIM_AUTH_WAIT_PIN1:
        case AT_MODULE_INIT_SAT_ERROR:
        case AT_MODULE_INIT_NORMAL_SIM_CARD:
        case AT_MODULE_INIT_CHANGED_SIM_CARD:
            at_CmdRespErrorText(pParam->engine, "SIMERR");
            break;

        // other error
        case AT_MODULE_INIT_SIM_SMS_ERROR:
        case AT_MODULE_INIT_SIM_PBK_ERROR:
        case AT_MODULE_INIT_ME_SMS_ERROR:
        case AT_MODULE_INIT_ME_PBK_ERROR:
            at_CmdRespErrorText(pParam->engine, "OTHERERR");
            break;

        // initing
        case AT_MODULE_INIT_SIM_PBK_OK:
        case AT_MODULE_INIT_ME_SMS_OK:
        case AT_MODULE_INIT_ME_PBK_OK:
        case AT_MODULE_NOT_INIT:
            at_CmdRespErrorText(pParam->engine, "INITERR");
            break;

        // ok
        case AT_MODULE_INIT_SIM_SMS_OK:
            if (2 == gCsStauts.nType)
            {
                if ((CFW_NW_STATUS_REGISTERED_ROAMING == gCsStauts.nParam1) ||
                        (CFW_NW_STATUS_REGISTERED_HOME == gCsStauts.nParam1))
                {
                    at_CmdRespOK(pParam->engine);
                }
                else
                {
                    at_CmdRespErrorText(pParam->engine, "NETERR");
                }
            }
            else
            {
                at_CmdRespErrorText(pParam->engine, "NETERR");
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            break;
        }
        break;

    case AT_CMD_READ:
    // break;

    case AT_CMD_TEST:
    // break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
    return;
}

#endif
// This function to process the AT+CGEQREQ command. include set,read and test command.
// Set command
#ifdef __MULTI_RAT__
VOID AT_NW_CmdFunc_XCPUTEST(AT_CMD_PARA *pParam)
{
    UINT8 Resp[25]= {0,};
    UINT8 uParamCount = 0;
    UINT8 uIndex      = 0;
    UINT16 uSize;
    UINT32 type   = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT32 iResult;
    UINT32 arrCharacterSet[7] = {0,};
    CFW_SIM_ID nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+XCPUTEST:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((ERR_SUCCESS != iResult)||(uParamCount < 1 )|| (uParamCount > 8))
        {
            AT_TC(g_sw_GPRS, "AT+XCPUTEST:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 4;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &type, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+XCPUTEST:The param uCid error,uCid = %d.", type);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        if((type == 1)||(type == 2)||(type == 3))
        {
            for(uIndex=1; uIndex<uParamCount; uIndex++)
            {
                uSize   = 4;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &arrCharacterSet[uIndex-1], &uSize);
                if(ERR_AT_UTIL_CMD_PARA_NULL == iResult)
                {
                    arrCharacterSet[0] = 1;
                }
                if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
                {
                    AT_TC(g_sw_GPRS, "AT+XCPUTEST:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex,iResult);
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto _func_fail;
                }
            }
            AT_Sprintf(Resp, "+XCPUTEST:%d,%d,%d,%d,%d,%d,%d",
                       type, arrCharacterSet[0],arrCharacterSet[1],arrCharacterSet[2],arrCharacterSet[3],arrCharacterSet[4]
                       ,arrCharacterSet[5]);
            CFW_XcpuTest(arrCharacterSet,type,nSim,pParam->nDLCI);
            goto _func_success;
        }
        else
        {
            goto _func_fail;
        }

    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }
_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;
_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}
VOID AT_NW_CmdFunc_WCPUTEST(AT_CMD_PARA *pParam)
{
    UINT8 Resp[25]= {0,};
    UINT8 uParamCount = 0;
    UINT8 uIndex      = 0;
    UINT16 uSize;
    UINT32 type   = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT32 iResult;
    UINT32 arrCharacterSet[7] = {0,};
    CFW_SIM_ID nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+WCPUTEST:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((ERR_SUCCESS != iResult)||(uParamCount < 1 )|| (uParamCount > 8))
        {
            AT_TC(g_sw_GPRS, "AT+WCPUTEST:NULL == pParam");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }
        if (uParamCount >= 1)
        {
            uSize   = 4;
            uIndex  = 0;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &type, &uSize);
            if (ERR_SUCCESS != iResult)
            {
                AT_TC(g_sw_GPRS, "AT+WCPUTEST:The param uCid error,uCid = %d.", type);
                uErrCode = ERR_AT_CME_PARAM_INVALID;
                goto _func_fail;
            }
        }
        if((type == 1)||(type == 2)||(type == 3))
        {
            for(uIndex=1; uIndex<uParamCount; uIndex++)
            {
                uSize   = 4;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT32, &arrCharacterSet[uIndex-1], &uSize);
                if(ERR_AT_UTIL_CMD_PARA_NULL == iResult)
                {
                    arrCharacterSet[0] = 1;
                }
                if ((ERR_SUCCESS != iResult)&&(ERR_AT_UTIL_CMD_PARA_NULL != iResult))
                {
                    AT_TC(g_sw_GPRS, "AT+WCPUTEST:Get uIndex failed,uIndex = 0x%x,iResult = 0x%x.", uIndex,iResult);
                    uErrCode = ERR_AT_CME_PARAM_INVALID;
                    goto _func_fail;
                }
            }
            AT_Sprintf(Resp, "+WCPUTEST:%d,%d,%d,%d,%d,%d,%d",
                       type, arrCharacterSet[0],arrCharacterSet[1],arrCharacterSet[2],arrCharacterSet[3],arrCharacterSet[4]
                       ,arrCharacterSet[5]);
            CFW_WcpuTest(arrCharacterSet,type,nSim,pParam->nDLCI);
            goto _func_success;
        }
        else
        {
            goto _func_fail;
        }

    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
        return;
    }
_func_success:
    AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return;
_func_fail:
    AT_GPRS_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


#endif

#ifdef LTE_NBIOT_SUPPORT

UINT8 AT_Cscon_respond(UINT8 *respond, UINT8 mode, BOOL reportN)
{
   UINT8 ret = 1;
   UINT8 len = 0;
   UINT8 sigConn = g_uAtCscon;
   UINT8 state = (mode==1)?7:0xff;
   UINT8 access = 4; //only fdd
    if(reportN == TRUE)
    {
        len += AT_Sprintf(respond, "+CSCON: %d, ", g_uAtCscon);
        sigConn = 2;
    }
    else
    {
        len += AT_Sprintf(respond, "+CSCON: ");
    }
    switch(sigConn)
    {
        case 0:
	     ret = 0;
	     break;
        case 1:
            AT_Sprintf(respond + len, "%d", mode);
            break;
        case 2:
            len += AT_Sprintf(respond+ len, "%d", mode);
            if(state == 7)
            {
                AT_Sprintf(respond+ len, ", %d", state);
            }
            break;
        case 3:
            len += AT_Sprintf(respond+ len, "%d", mode);
            if(state == 7)
            {
                AT_Sprintf(respond+ len, ", %d, %d", state, access);
            }
            break;
        default:
	     ret = 2;
	     break;
    }
    return ret;
}

VOID AT_GPRS_CmdFunc_CSCON(AT_CMD_PARA *pParam)
{
    UINT8 respond[100] = {0};
    INT32 iResult;
    UINT8 n = 0;
    UINT16 uSize;
    UINT8 uParamCount = 0;
    UINT32 uErrCode   = ERR_SUCCESS;
    UINT8 uIndex      = 0;
    CFW_NW_STATUS_INFO sStatus;
    // "CDMA-IS95B  5","LTE 14"
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT+CSCON:NULL == pParam");
        uErrCode = ERR_AT_CME_PARAM_INVALID;
        goto _func_fail;
    }

    // Process the command. include icommand parameter checking,csw api calling,and responsing...
    if (AT_CMD_SET == pParam->iType)  // Set command.
    {
        // this comman have parameter.
        if (NULL == pParam->pPara)
        {
            AT_TC(g_sw_AT_NW, "AT+CSCON:NULL == pParam->pPara");
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        // Get the count of parameters. This command have 1 parameters.
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if (iResult != ERR_SUCCESS || uParamCount != 1)
        {
            AT_TC(g_sw_AT_NW, "AT+CSCON:Get Param count failed,iResult = 0x%x", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        uSize = SIZEOF(n);

        uIndex = 0;

        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &n, &uSize);

        if (ERR_SUCCESS != iResult)
        {
            AT_TC(g_sw_AT_NW, "AT+CSCON:Get n failed,iResult = 0x%x.", iResult);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        if (n > 3)
        {
            AT_TC(g_sw_AT_NW, "AT+CSCON:The Prama n is error,n = 0x%x.", n);
            uErrCode = ERR_AT_CME_PARAM_INVALID;
            goto _func_fail;
        }

        g_uAtCscon = n;

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

        AT_Cscon_respond(respond, sStatus.ConnMode,  TRUE);
        goto _func_success;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_StrCpy(respond, "+CSCON: (0-3)");
        goto _func_success;
    }
    else
    {
        uErrCode = ERR_AT_CME_OPTION_NOT_SURPORT;
        goto _func_fail;
    }

    // ]] yy[mod] at 2008-6-4 for bug ID 8676
_func_success:

    AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, respond, AT_StrLen(respond), pParam->nDLCI, nSim);
    return;

_func_fail:

    AT_NW_Result_Err(uErrCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;

}

VOID AT_GPRS_CmdFunc_CEDRXRDP(AT_CMD_PARA *pParam)
{
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 aucBitStr1[8];
    UINT8 aucBitStr2[8];
    UINT8 aucBitStr3[8];
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */

    AT_TC(g_sw_AT_NW, "AT+CEDRXRDP: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT+CEDRXRDP: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cedrxrdp_fail;
    }

    if ((AT_CMD_EXE == pParam->iType)
        ||(AT_CMD_READ == pParam->iType))
    {
        AT_Util_ByteToBitStr(&aucBitStr1[0], nbiot_nvGetEdrxValue(), 4);
        AT_Util_ByteToBitStr(&aucBitStr2[0], g_nwEdrxValue, 4);
        AT_Util_ByteToBitStr(&aucBitStr3[0], g_nwEdrxPtw, 4);

        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CEDRXRDP: 5,%s,%s,%s", &aucBitStr1[0], &aucBitStr2[0], &aucBitStr3[0]);
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_NW_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cedrxrdp_fail:
    AT_TC(g_sw_AT_NW, "AT+CEDRXRDP: errCode : %d", errCode);
    AT_NW_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}

VOID AT_GPRS_CmdFunc_CCIOTOPT(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount          = 0;
    UINT8 uIndex = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT16 uSize                = 0;
    UINT8 ucByte = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[200]           = { 0 }; /* max 20 charactors per cid */
    UINT8 supportedCiot = 0;
    UINT8 preferredCiot = 0;
    
    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: nSim= %d; nDLCI= 0x%02x", nSim, pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: parameters is NULL");
	    errCode = ERR_AT_CME_PARAM_INVALID;
        goto cciotopt_fail;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get parameter count error");
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cciotopt_fail;
        }
        if (0 == uParamCount || 3 < uParamCount)
        {
            AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: Parameter count error. count = %d.", uParamCount);
            errCode = ERR_AT_CME_PARAM_INVALID;
            goto cciotopt_fail;
        }

        //n
        if (uParamCount >= 1)
        {
            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
            if (iResult != ERR_SUCCESS || uSize == 0)
            {
                AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para n fail. iResult = %d, uSize = %d", iResult, uSize);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cciotopt_fail;
            }

            if ((ucByte == 2) || (ucByte > 3))
            {
                AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para n error. n = %d", ucByte);
                errCode = ERR_AT_CME_PARAM_INVALID;
                goto cciotopt_fail;
            }

            AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para n=%d successfully", ucByte);
            if (ucByte == 3) // 3	Disable reporting and reset the parameters for CIoT EPS optimization to the default values.
            {
                nbiot_nvSetCiotCpciot(1);
                nbiot_nvSetCiotUpciot(2);
            }
            nbiot_nvSetCiotReport(ucByte);
        }

        if (ucByte != 3)
        {
            //supported_UE_opt
            if (uParamCount >= 2)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para supported_UE_opt fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cciotopt_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte > 3)
                    {
                        AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para supported_UE_opt error. supported_UE_opt = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cciotopt_fail;
                    }

                    if (ucByte == 1)
                    {
                        if (nbiot_nvGetCiotUpciot() >= 2)
                            nbiot_nvSetCiotUpciot(1);
                    }
                    else if (ucByte == 3)
                    {
                        if (nbiot_nvGetCiotUpciot() <= 1)
                            nbiot_nvSetCiotUpciot(2);
                    }
                    //ignore 0 and 2 for NB support cpciot mandatorily
                    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para supported_UE_opt=%d successfully, upciot=%d", 
                        ucByte, nbiot_nvGetCiotUpciot());
                }
            }

            //preferred_UE_opt
            if (uParamCount >= 3)
            {
                uIndex++;
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &ucByte, &uSize);
                if (iResult != ERR_SUCCESS && iResult != ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para preferred_UE_opt fail. iResult = %d", iResult);
                    errCode = ERR_AT_CME_PARAM_INVALID;
                    goto cciotopt_fail;
                }

                if (ERR_AT_UTIL_CMD_PARA_NULL != iResult)               
                {
                    if (ucByte > 2)
                    {
                        AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para preferred_UE_opt error. preferred_UE_opt = %d", ucByte);
                        errCode = ERR_AT_CME_PARAM_INVALID;
                        goto cciotopt_fail;
                    }
                    
                    if (ucByte == 1)
                    {
                        if (nbiot_nvGetCiotUpciot() == 3)
                            nbiot_nvSetCiotUpciot(2);
                    }
                    else if (ucByte == 2)
                    {
                        if (nbiot_nvGetCiotUpciot() == 2)//only prefer UP when supported
                            nbiot_nvSetCiotUpciot(3);
                    }

                    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: get para preferred_UE_opt=%d successfully, upciot=%d", 
                        ucByte, nbiot_nvGetCiotUpciot());
                }
            }
        }
        //if (nvChanged)
        //    CFW_NvParasChangedNotification(NV_CHANGED_EDRX);
        nvmWriteStatic();
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        if (nbiot_nvGetCiotUpciot() <= 1)
            supportedCiot = 1;
        else
            supportedCiot = 3;
        if (nbiot_nvGetCiotUpciot() == 3)
            preferredCiot = 2;
        else
            preferredCiot = 1;
    
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CCIOTOPT: %d,%d,%d", nbiot_nvGetCiotReport(), 
            supportedCiot, preferredCiot);
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CCIOTOPT: n=(0,1,3), supported_UE_opt=[0-3], preferred_UE_opt=[0-2]");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_NW_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

cciotopt_fail:
    AT_TC(g_sw_AT_NW, "AT+CCIOTOPT: errCode : %d", errCode);
    AT_NW_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    return;
}


#endif

// frank add end
//frank add start
VOID AT_NW_CmdFunc_QSCANF(AT_CMD_PARA *pParam)
{
    INT32 iResult                        = 0;
    UINT8 uParaCount                     = 0;
    UINT8 uIndex                         = 0;
    UINT16 uSize                         = 0;
    UINT8 uBand= 0;
    UINT8 uFreqs[120] = { 0 ,};
    UINT16 val_uFreqs[60] = { 0 ,};
    UINT16  freqnb=0;
    UINT8  i = 0,j = 0,k = 0,m= 0;
    UINT16 nFreqLen= 0;
    UINT32 ret = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    CFW_COMM_MODE bFM;
	
    AT_TC(g_sw_AT_NW, "QSCANF, start!\n");
	

    switch (pParam->iType)
    {
    case AT_CMD_SET:
		CFW_GetComm((CFW_COMM_MODE*)&bFM, nSim);
		if (CFW_ENABLE_COMM == bFM)
		{
            AT_TC(g_sw_AT_NW, "QSCANF,Is no flight mode  error!\n");
            AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
		}
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
        if((iResult != ERR_SUCCESS)||(uParaCount != 2))
        {
            AT_TC(g_sw_AT_NW, "QSCANF, GetParaCount error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uIndex  = 0;
        uSize  = SIZEOF(uBand);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uBand, &uSize);
        if(iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "QSCANF, get param1 error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
		nFreqLen =SIZEOF(uFreqs);
		iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING,uFreqs , &nFreqLen);
		if(iResult != ERR_SUCCESS)
		{
			AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "Get Param2 Error", pParam->nDLCI, nSim);
			return;
		}
		AT_TC(g_sw_GPRS, "AT+QSCANF  uFreqs len:%d", SUL_Strlen(uFreqs));
		AT_TC(g_sw_AT_NW, "QSCANF, uFreqs:%s!\n",uFreqs);
		for(i=0; i<SUL_Strlen(uFreqs)+1; i++)
		{
			if(uFreqs[i] == ',')
			{
				freqnb++;
			}
			else
			{
				if(uFreqs[i] >= '0')
				{
					k = i;
					m=0;
					while( 1 )
					{
						k++;
						if(uFreqs[k] == ',')
						  break;
						if(uFreqs[k] == 0)
						  break;

						m++;
					}
					val_uFreqs[j]=(UINT16 )SUL_StrAToI(&uFreqs[i]);
					AT_TC(g_sw_AT_NW, "AT+QSCANF  val_uFreqs:%d", val_uFreqs[j]);
					if((val_uFreqs[j] > 1023)&&(val_uFreqs[j] != 9999))
					{
						AT_TC(g_sw_AT_NW, "AT+QSCANF ERROR!input parameters out of range ");
						AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return ;						
					}
					i = i + m;
					j++;
				}
				if( j >= 120)
				  break;
			}
		}
		freqnb++;
		AT_TC(g_sw_AT_NW, "AT+QSCANF  freqnb:%d", freqnb);
		ret= CFW_GetFreqInfo( uBand, freqnb,val_uFreqs, pParam->nDLCI,  nSim);
		if (ret != ERR_SUCCESS)
		{
			AT_TC(g_sw_AT_NW, "AT+QSCANF ERROR:ret: %d", ret);
			AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
			return;
		}
		AT_NW_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_TEST:
        AT_TC(g_sw_AT_NW, "QSCANF,This is the TEST!\n");
        AT_Sprintf(uFreqs, "+QSCANF:(0-3),(0-1023,9999)");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uFreqs, AT_StrLen(uFreqs), pParam->nDLCI, nSim);
		break;
    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    return ;
}
//frank add end 
//frank add start
//Set Pseudo base station identification
VOID AT_NW_CmdFunc_SDMBS(AT_CMD_PARA *pParam)
{
    INT32 iResult                        = 0;
    UINT8 uParaCount                     = 0;
    UINT8 uIndex                         = 0;
    UINT16 uSize                         = 0;
    UINT8  nSetValue[40]={0,}; 
    UINT8 uDMBS                         = 0;
    UINT32 uRet                        = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
	
    AT_TC(g_sw_AT_NW, "SDMBS, start!\n");

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
        if((iResult != ERR_SUCCESS)||(uParaCount != 1))
        {
            AT_TC(g_sw_AT_NW, "SDMBS, GetParaCount error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        uIndex  = 0;
        uSize  = SIZEOF(uDMBS);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uDMBS, &uSize);
        if(iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "SDMBS, get param1 error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
	    uRet =CFW_CfgNwSetDetectMBS(uDMBS);
		if(uRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_NW, "SDMBS, SetDetectMBS error!\n");
            AT_NW_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
		AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_TEST:
        AT_TC(g_sw_AT_NW, "SDMBS,This is the TEST!\n");
        AT_Sprintf(nSetValue, "+SDMBS:(0,1)");
        AT_NW_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nSetValue, AT_StrLen(nSetValue), pParam->nDLCI, nSim);
		break;
    default:
        AT_NW_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
    return ;
}
//frank add end 
