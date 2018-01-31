#ifdef __IOT_SUPPORT__

#include "stdio.h"
#include "at.h"
#include "cfw.h"
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "base_prv.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_nw.h"
#include "sockets.h"

#ifndef EDP_USE
#define EDP_USE
#endif

#ifdef EDP_USE
extern SOCKET edp_socketId ;
extern void recv_thread_func(CFW_EVENT* CfwEv);
#endif

extern int test_main(UINT8 cmd);
extern SOCKET edp_socketId ;
extern in_addr edp_serIP;
extern BOOL AT_KillEdpTestTimer(VOID);

BOOL check_edpEvent(COS_EVENT* pEvent)
{
    CFW_EVENT CfwEv;
    UINT8 nSim; // = (UINT8 )(pEvent->nParam3&0x000000ff);

    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;

    if(edp_socketId ==(UINT8)CfwEv->nParam1)
    {
        AT_TC(g_sw_TCPIP, "###EDP AT_TCPIP_AsyncEventProcess,edp_socketId,   receive event id: %d",CfwEv->nEventId);

        switch (CfwEv->nEventId)
        {
        case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
            test_main(2);  //CMD_AUTH
            break;
        case EV_CFW_TCPIP_REV_DATA_IND:
            recv_thread_func(CfwEv);
            break;
        case EV_CFW_TCPIP_CLOSE_IND:
        case EV_CFW_TCPIP_ERR_IND:
            AT_KillEdpTestTimer();
            CFW_TcpipSocketClose(edp_socketId);
            break;
        default:
            break;
        }
        
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "CONNECT OK", AT_StrLen("CONNECT OK"), CfwEv.nUTI);
        return TRUE;
    }

    if(EV_CFW_DNS_RESOLV_SUC_IND==(CfwEv->nEventId))
    {
        if(0 ==strcmp(CfwEv->nParam2, "jjfaedp.hedevice.com"))
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_AsyncEventProcess, receive event id: %d",CfwEv->nEventId);
            ip4_addr_t *ipaddr = ip_2_ip4((ip_addr_t*)CfwEv->nParam1);
            edp_serIP.s_addr = ipaddr->addr;
            test_main(1); //CMD_CONNECT
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, "CONNECT OK", AT_StrLen("CONNECT OK"), CfwEv.nUTI);
            return TRUE;
        }
    }
    return FALSE;
}


VOID AT_IOT_CmdFunc_CDEVCFG(AT_CMD_PARA *pParam)
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
    // input parameter check.

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
        // Call csw api to set the Req QOS.
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
#endif

