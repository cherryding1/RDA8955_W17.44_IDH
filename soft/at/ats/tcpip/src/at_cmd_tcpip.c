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
#include "stdio.h"
#include "at.h"
#include "cfw.h"
#include "at_module.h"
#include "at_dispatch.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "base_prv.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_nw.h"
#include "sockets.h"
#include "at_cfg.h"

#define AT_TCPIP_ADDRLEN 20
#define AT_TCPIP_DEFAULTAPN "CMNET"
#define AT_TCPIP_HOSTLEN 256

typedef enum {
    CIP_INITIAL,
    CIP_START,
    CIP_CONFIG,
    CIP_GPRSACT,
    CIP_STATUS,
    CIP_PROCESSING,
    CIP_CONNECTED,
    CIP_CLOSING,
    CIP_CLOSED,
    CIP_PDPDEACT
} CIP_STATUS_T;

typedef enum {
    CIP_SERVER_OPENING,
    CIP_SERVER_LISTENING,
    CIP_SERVER_CLOSING
} CIP_SERVER_STATE_T;

typedef enum {
    CIPSERVER_CLOSE,
    CIPSERVER_OPEN,
    CIPSERVER_OPENING
} CIPSERVER_MODE;

typedef enum {
    BEARER_GPRS_DEF,
    BEARER_GPRS_2,
    BEARER_WIFI,
    BEARER_MAX_NUM,
} CIP_BEARER_T;

typedef struct _stAT_Tcpip_Paras
{
    UINT8 uType;
    UINT8 uProtocol;
    UINT8 uSocket;
    UINT8 uDomain;
    UINT16 uPort;
    UINT8 uaIPAddress[AT_TCPIP_HOSTLEN];
    UINT8 udpmode; // 0: udp normal mode 1: udp extended mode 2: set udp address
    CIP_STATUS_T uState;
    CIP_BEARER_T uConnectBearer;
} stAT_Tcpip_Paras;

typedef struct
{
    UINT8 nCid;
    CIP_STATUS_T nState;
} stAT_Bearer_Paras;

typedef struct
{
    UINT8 mode;
    UINT8 channelid;
    UINT8 nSocketId;
    CIP_BEARER_T nConnectBearer;
    UINT32 port;
} CIPSERVER;

typedef struct
{
    UINT16 tcp_port;
    UINT16 udp_port;
} CLOCAL_PORT;

typedef struct
{
    stAT_Tcpip_Paras nTcpipParas[MEMP_NUM_NETCONN];
    stAT_Bearer_Paras nBearerParas[BEARER_MAX_NUM]; //0,1:GPRS 2:WIFI
} CIP_CONTEXT_T;

CIP_CONTEXT_T g_uCipContexts;
CIP_BEARER_T gCipBearer; //0,1:GPRS 2:WIFI
CIPSERVER gCipserver;
CLOCAL_PORT gClocalport[MEMP_NUM_NETCONN];

//keepalive Parameters
int g_keepalive = 0;
int g_keepidle = 7200;
int g_keepinterval = 75;
int g_keepcount = 9;

//RD Timer
int g_rdsigtimer = 2000;
int g_rdmuxtimer = 3500;

static at_tcpip_setting_t gCIPSettings = {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 5, 2, 1024, 1, 0, 1460, 50, 0, 0, 3, 1, 2000, 3500, 0, "CMNET", {0}, {0}};

void cipSettings_Init()
{
    if (!gAtCurrentSetting.tcpip.cipSCONT_unSaved)
        gCIPSettings = gAtCurrentSetting.tcpip;
}

#define cipSCONT_unSaved gCIPSettings.cipSCONT_unSaved
#define cipMux_multiIp gCIPSettings.cipMux_multiIp
#define cipQSEND_quickSend gCIPSettings.cipQSEND_quickSend
#define cipRXGET_manualy gCIPSettings.cipRXGET_manualy
#define cipMODE_transParent gCIPSettings.cipMODE_transParent // 0: normal mode 1: transparent mode
#define cipCCFG_esc gCIPSettings.cipCCFG_esc                 // wether handle escape character; 0: no 1: yes
#define cipCCFG_NmRetry gCIPSettings.cipCCFG_NmRetry         // 3 - 8
#define cipCCFG_WaitTm gCIPSettings.cipCCFG_WaitTm           // 1 - 10
#define cipCCFG_SendSz gCIPSettings.cipCCFG_SendSz           // 1 - 1460
#define cipCCFG_Rxmode gCIPSettings.cipCCFG_Rxmode           //0: output data to serial port without interval  1: within <Rxtimer> interval
#define cipCCFG_RxSize gCIPSettings.cipCCFG_RxSize           //50 - 1460
#define cipCCFG_Rxtimer gCIPSettings.cipCCFG_Rxtimer         //20 - 1000
#define cipATS_setTimer gCIPSettings.cipATS_setTimer         // 0: not set timer 1: set timer
#define cipATS_time gCIPSettings.cipATS_time                 // value of data send timer
#define cipSPRT_sendPrompt gCIPSettings.cipSPRT_sendPrompt   // prompt for IPSEND, 0: no ">" but "ok"  1: ">" + "ok"  2: no ">", no "ok"
#define cipHEAD_addIpHead gCIPSettings.cipHEAD_addIpHead     // CIPHEAD, 0: not add; 1: add
#define cipSRIP_showIPPort gCIPSettings.cipSRIP_showIPPort   // CIPSRIP 0:don't disp 1:disp
#define cipCSGP_setGprs gCIPSettings.cipCSGP_setGprs         // 1: GPRS(Default) 0: CSD
#define cipApn gCIPSettings.cipApn
#define cipUserName gCIPSettings.cipUserName
#define cipPassWord gCIPSettings.cipPassWord
#define cipDPDP_detectPDP gCIPSettings.cipDPDP_detectPDP // 0: don't detect pdp 1: detect pdp
#define cipDPDP_interval gCIPSettings.cipDPDP_interval   // 1 < interval <= 180(s)
#define cipDPDP_timer gCIPSettings.cipDPDP_timer         // 1 < timer <= 10
#define cipSHOWTP_dispTP gCIPSettings.cipSHOWTP_dispTP   // 0: disabled 1: enabled

// UINT8 g_uATTcpipValid;
const UINT8 g_strATTcpipStatus[10][32] = {"IP INITIAL", "IP START", "IP CONFIG", "IP GPRSACT",
                                          "IP STATUS", "IP PROCESSING", "CONNECT OK", "IP CLOSING",
                                          "CLOSED", "IP PDPDEACT"};

typedef struct
{
    uint8_t *buff;   // buffer, shared by all mode
    UINT32 buffLen;  // existed buffer data byte count
    UINT32 buffSize; // buffer size
    HANDLE mutex;
} BYPASS_BUFFER_T;

BYPASS_BUFFER_T *g_bypass_buf = NULL;

struct BYPASS_BUFFER_T *at_TCPIPBufCreate()
{
    BYPASS_BUFFER_T *bypass_buff = (BYPASS_BUFFER_T *)AT_MALLOC(sizeof(*bypass_buff));

    bypass_buff->buff = (uint8_t *)AT_MALLOC(AT_CMD_LINE_BUFF_LEN);
    bypass_buff->buffLen = 0;
    bypass_buff->buffSize = AT_CMD_LINE_BUFF_LEN;
    bypass_buff->mutex = COS_CreateMutex();
    return bypass_buff;
}
VOID at_TCPIPBufDestroy(BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
        return;
    COS_DeleteMutex(bypass_buff->mutex);
    AT_FREE(bypass_buff->buff);
    AT_FREE(bypass_buff);
}

VOID at_delayAsync(void *param)
{
    UINT8 nDlc = *(UINT8 *)(param);
    if (g_bypass_buf == NULL)
    {
        AT_TC(g_sw_TCPIP, "at_delayAsync error,param == null");
        at_StartCallbackTimer(cipATS_time * 1000, at_delayAsync, param);
        return;
    }
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDlc);
    AT_TC(g_sw_TCPIP, "at_delayAsync,nDlc=%d,th->len=%d", nDlc, g_bypass_buf->buffLen);
    at_ModuleRunCommandExtra(engine, g_bypass_buf->buff, g_bypass_buf->buffLen);
}

int bypassDataRecv(void *param, uint8_t *data, unsigned length)
{
    AT_TC(g_sw_TCPIP, "bypassDataRecv,length=%d ", length);
    if (g_bypass_buf == NULL)
        g_bypass_buf = at_TCPIPBufCreate();
    UINT8 escapeMode = TRUE;
    if (param != NULL)
        escapeMode = *(UINT8 *)param;
    if (escapeMode)
    {
        //TODO: implement escape mode
    }
    if (gATCurrenteCommandEchoMode)
        AT_WriteUart(data, length);
    const uint8_t *p = data;
    COS_LockMutex(g_bypass_buf->mutex);
    if (g_bypass_buf->buffLen + length > g_bypass_buf->buffSize) // overflow
    {
        AT_TC(g_sw_TCPIP, "bypassDataRecv overflow,buffLen=%d ", g_bypass_buf->buffLen);
        g_bypass_buf->buffLen = 0; // drop existed buffer
        if (length > g_bypass_buf->buffLen)
        {
            p += length - g_bypass_buf->buffLen;
            length = g_bypass_buf->buffLen;
        }
    }
    AT_MemCpy(g_bypass_buf->buff + g_bypass_buf->buffLen, p, length);
    g_bypass_buf->buffLen += length;
    COS_UnlockMutex(g_bypass_buf->mutex);
    return length;
}

VOID bypassDataSend(void *param)
{
    AT_DISPATCH_T *dispatch = (AT_DISPATCH_T *)param;
    if (g_bypass_buf == NULL)
    {
        AT_TC(g_sw_TCPIP, "bypassDataSend error,g_bypass_buf == null");
        goto restart;
    }
    if (g_bypass_buf->buffLen < cipCCFG_SendSz)
    {
        AT_TC(g_sw_TCPIP, "bypassDataSend do nothing buffLen=%d", g_bypass_buf->buffLen);
        goto restart;
    }
    else
    {
        UINT8 *data = AT_MALLOC(cipCCFG_SendSz);
        if (data == NULL)
        {
            AT_TC(g_sw_TCPIP, "bypassDataSend MALLOC FAILS cipCCFG_SendSz=%d", cipCCFG_SendSz);
            return;
        }
        memset(data, 0, cipCCFG_SendSz);
        memcpy(data, g_bypass_buf->buff, cipCCFG_SendSz);
        COS_LockMutex(g_bypass_buf->mutex);
        memmove(g_bypass_buf->buff, g_bypass_buf->buff + cipCCFG_SendSz, g_bypass_buf->buffLen - cipCCFG_SendSz);
        g_bypass_buf->buffLen -= cipCCFG_SendSz;
        COS_UnlockMutex(g_bypass_buf->mutex);
        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
        if (CFW_TcpipSocketSend(tcpipParas->uSocket, data, cipCCFG_SendSz, 0) == SOCKET_ERROR)
        {
            AT_TC(g_sw_TCPIP, "TCPIP send socket data error");
        }
        AT_FREE(data);
    }
restart:
    if (at_DispatchIsDataMode(dispatch))
        at_StartCallbackTimer(cipCCFG_WaitTm * 100, bypassDataSend, dispatch);
}

static UINT8 *i8tostring(UINT8 value)
{
    static UINT8 buff[4];
    UINT8 b, s, g, i = 0;
    b = value / 100;
    s = value % 100 / 10;
    g = value % 100 % 10;
    if (b > 0)
        buff[i++] = '0' + b;
    if (b + s > 0)
        buff[i++] = '0' + s;
    buff[i++] = '0' + g;
    buff[i] = 0;
    return buff;
}

#define DEF_TCPIP 0
#define TCPIP_PARAMS(muxid) (&g_uCipContexts.nTcpipParas[muxid])

static inline void set_udpmode(UINT8 muxid, UINT16 mode)
{
    TCPIP_PARAMS(muxid)
        ->udpmode = mode;
}
static inline void update_udp_remote_socket(UINT8 muxid, UINT8 *ipaddr, UINT8 iplen, UINT16 port)
{
    AT_MemSet(TCPIP_PARAMS(muxid)->uaIPAddress, 0, sizeof(TCPIP_PARAMS(muxid)->uaIPAddress));
    AT_MemCpy(TCPIP_PARAMS(muxid)->uaIPAddress, ipaddr, iplen);
    TCPIP_PARAMS(muxid)
        ->uPort = port;
}

static void start_transparent_mode(UINT8 nDlc)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDlc);
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_START, nDlc);
    at_StartCallbackTimer(cipCCFG_WaitTm * 100, bypassDataSend, dispatch);
    at_DataSetBypassMode(at_DispatchGetDataEngine(dispatch), bypassDataRecv, NULL);
}

static void stop_transparent_mode(UINT8 nDlc)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDlc);
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_END, nDlc);
    at_StopCallbackTimer(bypassDataSend, dispatch);
    at_TCPIPBufDestroy(g_bypass_buf);
    g_bypass_buf = NULL;
}

/*****************************************************************************
* Name:         tcpip_rsp
* Description:  AT TCPIP module asynchronous event dispatch and process;
* Parameter:    COS_EVENT *pEvent
* Return:       VOID
* Remark:       n/a
* Author:       YangYang
* Data:         2008-5-15
******************************************************************************/
INT32 AT_TCPIP_Connect(UINT8 nDLCI, UINT8 nMuxIndex);
INT32 AT_TCPIP_ServerStart(UINT8 nDLCI);
static void start_tcpip_close_timer(UINT8 nDLCI, UINT8 nMuxIndex);
static void stop_tcpip_close_timer(UINT8 nDLCI, UINT8 nMuxIndex);

typedef struct tcpip_dns_param
{
    UINT8 nDLCI;
    UINT8 nMuxIndex;
} TCPIP_DNS_PARAM;

static void tcpip_dnsReq_callback(COS_EVENT *ev)
{
    TCPIP_DNS_PARAM *param = ev->nParam3;
    UINT8 aucBuffer[40] = {0};
    UINT8 nDLCI = param->nDLCI;
    UINT8 nMuxIndex = param->nMuxIndex;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
    AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect(), CFW_GethostbynameEX() tcpip_dnsReq_callback\n");
    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        INT32 iResult = 0;
        ip_addr_t *ipaddr = (ip_addr_t *)ev->nParam1;
        UINT8 *pcIpAddr = ipaddr_ntoa(ipaddr);
        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
        AT_StrNCpy(tcpipParas->uaIPAddress, pcIpAddr, AT_TCPIP_ADDRLEN);
        iResult = AT_TCPIP_Connect(nDLCI, nMuxIndex);
        if (iResult != ERR_SUCCESS)
        {
            at_CmdRespErrorCode(engine, ERR_AT_CME_EXE_FAIL);
            if (!cipMux_multiIp)
                AT_Sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
            else
                AT_Sprintf(aucBuffer, "%d,CONNECT FAIL", nMuxIndex);
            at_CmdRespUrcText(engine, aucBuffer);
        }
    }
    else if (ev->nEventId == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        if (!cipMux_multiIp)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
            AT_Sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
        }
        else
        {
            AT_Sprintf(aucBuffer, "%d,CONNECT FAIL", nMuxIndex);
        }
        at_CmdRespUrcText(engine, aucBuffer);
    }
    AT_FREE(param);
    AT_FREE(ev);
}

static void attact_rsp(COS_EVENT *pEvent)
{
    AT_TC(g_sw_TCPIP, "attact_rsp Got %s: %d,0x%x,0x%x,0x%x",
          TS_GetEventName(pEvent->nEventId), pEvent->nEventId, pEvent->nParam1, pEvent->nParam2, pEvent->nParam3);

    CFW_EVENT CfwEv;
    CFW_GPRS_PDPCONT_INFO *pPdpInfo;
    UINT8 uUIT;
    UINT8 nS0Staus;
    TCHAR buffer[100] = {0};
    UINT8 nSim;

    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;
    UINT8 nCid = pEvent->nParam1;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(CfwEv.nUTI);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    AT_TC(g_sw_TCPIP, "attact_rsp nCid =%d,g_uCipContexts.nCid=%d", nCid, bearerParas->nCid);
    if (CfwEv.nEventId == EV_CFW_GPRS_ATT_RSP)
    {
        if (CFW_GPRS_ATTACHED == CfwEv.nType)
        {
            if (CFW_GprsActEX(AT_GPRS_ACT, bearerParas->nCid, CfwEv.nUTI, nSim, 0, attact_rsp) != ERR_SUCCESS)
            {
                AT_TC(g_sw_TCPIP, "attact_rsp Activate PDP error while TCP/IP connection");
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            if (bearerParas->nState == CIP_INITIAL)
            {
                if (gCipserver.mode == CIPSERVER_OPENING)
                {
                    AT_TC(g_sw_TCPIP, "attact_rsp Activate PDP error gCipserver");
                    gCipserver.mode = CIPSERVER_CLOSE;
                    at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                }
                else
                    at_CmdRespUrcText(engine, "STATE:PDP DEACT\r\n\r\nCONNECT FAIL");
            }
            else if (bearerParas->nState == CIP_START)
            {
                AT_TC(g_sw_TCPIP, "attact_rsp Activate PDP error CSTT");
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
    }
    else if (bearerParas->nCid == nCid)
    {
        switch (CfwEv.nEventId)
        {
        case EV_CFW_GPRS_CXT_ACTIVE_IND:
            AT_TC(g_sw_GPRS, "attact_rsp got EV_CFW_GPRS_CXT_ACTIVE_IND");
            break;
        case EV_CFW_GPRS_CXT_DEACTIVE_IND:
            AT_TC(g_sw_GPRS, "attact_rsp got EV_CFW_GPRS_CXT_DEACTIVE_IND");
            bearerParas->nCid = 0;
            bearerParas->nState = CIP_INITIAL;
            break;
        case EV_CFW_GPRS_ACT_RSP:
        {
            AT_TC(g_sw_GPRS, "attact_rsp EV_CFW_GPRS_ACT_RSP nType: %d,nMuxState:%d", CfwEv.nType, bearerParas->nState);
            if (CFW_GPRS_ACTIVED == CfwEv.nType)
            {
                if (bearerParas->nState == CIP_INITIAL)
                {
                    bearerParas->nState = CIP_STATUS;
                    if (CIPSERVER_OPENING == gCipserver.mode)
                    {
                        if (ERR_SUCCESS == AT_TCPIP_ServerStart(CfwEv.nUTI))
                        {
                            gCipserver.mode = CIPSERVER_OPEN;
                            at_CmdRespOK(engine);
                            at_CmdRespUrcText(engine, "SERVER OK");
                        }
                        else
                        {
                            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                        }
                    }
                    else
                    {
                        INT32 iResult = AT_TCPIP_Connect(CfwEv.nUTI, 0);
                        if (iResult != ERR_SUCCESS)
                        {
                            AT_TC(g_sw_GPRS, "attact_rsp TCPIP connect failed: %d", iResult);
                            AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, CfwEv.nUTI);
                            break;
                        }
                    }
                }
                else if (bearerParas->nState == CIP_CONFIG)
                {
                    bearerParas->nState = CIP_GPRSACT;
                    at_CmdRespOK(engine);
                    break;
                }
                break;
            }
            else
            {
                if (cipMODE_transParent)
                {
                    stop_transparent_mode(CfwEv.nUTI);
                }
                else
                {
                    bearerParas->nCid = 0;
                    bearerParas->nState = CIP_INITIAL;
                    for (int i = 0; i < MEMP_NUM_NETCONN; i++)
                    {
                        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                        if (tcpipParas->uConnectBearer == gCipBearer)
                        {
                            memset(tcpipParas, 0, sizeof(stAT_Tcpip_Paras));
                        }
                    }
                    at_CmdRespOKText(engine, "SHUT OK");
                }
            }
        }
        break;
        default:
            AT_TC(g_sw_GPRS, "attact_rsp unprocessed event:%d", pEvent->nEventId);
            break;
        }
    }
    else
    {
        AT_TC(g_sw_GPRS, "attact_rsp g_uCipContexts[gCipBearer].nCid != nCid");
        at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
    }
    AT_FREE(pEvent);
}

static INT8 set_userpwdapn(UINT8 nDlc, UINT8 *nUser, UINT8 *nPwd, UINT8 *nApn)
{
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    UINT8 nCid = bearerParas->nCid;

    AT_TC(g_sw_TCPIP, "set_userpwdapn nCid = %d", nCid);
    if (nApn == NULL)
    {
        if (AT_StrLen(cipApn) < 1)
            nApn = AT_TCPIP_DEFAULTAPN;
        else
            nApn = cipApn;
    }
    if (nUser == NULL)
        nUser = cipUserName;
    if (nPwd == NULL)
        nPwd = cipPassWord;

    AT_TC(g_sw_TCPIP, "set_userpwdapn nCid = %d,nApn=%s,nPwd=%s,nUser=%s", nCid, nApn, nPwd, nUser);
    UINT16 size = sizeof(CFW_GPRS_PDPCONT_INFO) + strlen(nApn) + strlen(nUser) + strlen(nPwd);
    CFW_GPRS_PDPCONT_INFO *pPDPCnt = (CFW_GPRS_PDPCONT_INFO *)AT_MALLOC(size);
    if (pPDPCnt == NULL)
    {
        AT_TC(g_sw_TCPIP, "set_userpwdapn(), no MORE MEMORY");
        return -1;
    }
    AT_MemSet(pPDPCnt, 0, size);

    AT_TC(g_sw_TCPIP, "pPDPCnt = %x(%d)", pPDPCnt, size);
    pPDPCnt->nPdpType = CFW_GPRS_PDP_TYPE_IP;
    pPDPCnt->nApnSize = strlen(nApn);
    pPDPCnt->pApn = (UINT8 *)pPDPCnt + sizeof(CFW_GPRS_PDPCONT_INFO);
    memcpy(pPDPCnt->pApn, nApn, strlen(nApn));

    pPDPCnt->nApnUserSize = strlen(nUser);
    pPDPCnt->pApnUser = pPDPCnt->pApn + pPDPCnt->nApnSize;
    memcpy(pPDPCnt->pApnUser, nUser, strlen(nUser));

    pPDPCnt->nApnPwdSize = strlen(nPwd);
    pPDPCnt->pApnPwd = pPDPCnt->pApnUser + pPDPCnt->nApnUserSize;
    memcpy(pPDPCnt->pApnPwd, nPwd, strlen(nPwd));
    if (CFW_GprsSetPdpCxt(nCid, pPDPCnt, nDlc) != ERR_SUCCESS)
    {
        AT_TC(g_sw_TCPIP, "set_userpwdapn(), CFW_GprsSetPdpCxt error");
        AT_FREE(pPDPCnt);
        return -1;
    }
    return 0;
}

static INT8 cip_getCid(UINT8 nDlc)
{
    UINT8 cid = 0;
    UINT8 nSim = AT_SIM_ID(nDlc);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    // begin execute...
    if (0 == bearerParas->nCid)
    {
        UINT32 ret = CFW_GetFreeCID(&cid, nSim);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "cip_getCid, no cid allowed for TCPIP\n");
            return -1;
        }
        bearerParas->nCid = cid;
    }
    return bearerParas->nCid;
}

static INT8 do_attact(UINT8 nDlc)
{
    INT32 iResult = 0;
    UINT8 nSim = AT_SIM_ID(nDlc);
    UINT8 m_uAttState = CFW_GPRS_DETACHED;
    UINT8 uPDPState = 0;
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    UINT8 nCid = bearerParas->nCid;

    if (0 == nCid)
    {
        AT_TC(g_sw_TCPIP, "do_attact, no cid defined for TCPIP\n");
        return -1;
    }
    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS)
    {
        AT_TC(g_sw_TCPIP, "do_attact, get GPRS attach state error\n");
        return -1;
    }
    if (AT_GPRS_DAT == m_uAttState)
    {
        iResult = CFW_GprsAttEX(AT_GPRS_ATT, nDlc, nSim, attact_rsp);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "do_attact, attach failed: %d\n", iResult);
            return -1;
        }
        return 0;
    }
    else if (CFW_GetGprsActState(nCid, &uPDPState, nSim) != ERR_SUCCESS)
    {
        AT_TC(g_sw_TCPIP, "do_attact, get GPRS act state error\n");
        return -1;
    }
    if (uPDPState == CFW_GPRS_ACTIVED)
    {

        AT_TC(g_sw_TCPIP, "do_attact,######################## maybe wrong state ##############################\n");
        return -1;
    }
    else
    {
        iResult = CFW_GprsActEX(AT_GPRS_ACT, nCid, nDlc, nSim, 0, attact_rsp);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "do_attact, activate failed: %d\n", iResult);
            return -1;
        }
        return 0;
    }
}

static INT8 recv_data(UINT8 uSocket, UINT32 uDataSize, UINT8 nDlc, UINT8 nMuxIndex, UINT8 mode)
{
    INT32 iResult = 0;
    UINT16 uIPHlen = 0;
    UINT8 *pData = NULL;
    UINT8 uaRspStr[60] = {
        0,
    };
    UINT8 nSim = AT_SIM_ID(nDlc);
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDlc);
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    pData = AT_MALLOC(uDataSize + 25);
    if (NULL == pData)
    {
        AT_TC(g_sw_TCPIP, "EV_CFW_TCPIP_REV_DATA_IND, memory error");
        at_CmdRespCmeError(engine, ERR_AT_CME_NO_MEMORY);
        return -1;
    }
    AT_MemZero(pData, uDataSize + 25);
    if (mode == 0 && cipSRIP_showIPPort && (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP))
    {
        if (!cipMux_multiIp)
        {
            AT_Sprintf(uaRspStr, "+RECV FROM:%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        else
        {
            AT_Sprintf(uaRspStr, "+RECEIVE,%d,%d,%s:%d", nMuxIndex, uDataSize, tcpipParas->uaIPAddress, tcpipParas->uPort);
        }
        at_CmdRespInfoText(engine, uaRspStr);
    }
    if (mode != 4)
    {
        if (cipHEAD_addIpHead)
        {
            if (!cipMux_multiIp)
            {
                if (cipSHOWTP_dispTP)
                {
                    UINT8 *tp = tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP ? "TCP" : "UDP";
                    AT_Sprintf(pData, "+IPD,%d,%s:", uDataSize, tp);
                }
                else
                {
                    AT_Sprintf(pData, "+IPD,%d:", uDataSize);
                }
                uIPHlen = strlen(pData);
            }
            else
            {
                AT_Sprintf(pData, "+RECEIVE,%d,%d:", nMuxIndex, uDataSize);
                uIPHlen = strlen(pData);
            }
        }
        if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
        {
            iResult = CFW_TcpipSocketRecv(uSocket, pData + uIPHlen, uDataSize, nSim);
            if (SOCKET_ERROR == iResult)
            {
                AT_FREE(pData);
                AT_TC(g_sw_TCPIP, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                AT_FREE(pData);
                return -1;
            }
        }
        else
        {
            CFW_TCPIP_SOCKET_ADDR from_addr = {
                0,
            };
            INT32 fromLen = sizeof(CFW_TCPIP_SOCKET_ADDR);
            iResult = CFW_TcpipSocketRecvfrom(uSocket, pData + uIPHlen, uDataSize, 0, &from_addr, &fromLen);
            if (SOCKET_ERROR == iResult)
            {
                AT_FREE(pData);
                AT_TC(g_sw_TCPIP, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                AT_FREE(pData);
                return -1;
            }
            if (!cipMux_multiIp)
            {
                AT_Sprintf(uaRspStr, "+RECV FROM:%s:%hu", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
            }
            else
            {
                AT_Sprintf(uaRspStr, "+RECEIVE,%d,%d,%s:%hu", nMuxIndex, uDataSize, inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
            }
            at_CmdRespInfoText(engine, uaRspStr);
        }
        if (mode == 2 || mode == 3)
        {
            UINT8 uaIpStr[30] = {
                0,
            };
            if (!cipMux_multiIp)
            {
                AT_Sprintf(uaRspStr, "+CIPRXGET:%d,%d,%d", mode, uDataSize, iResult);
            }
            else
            {
                AT_Sprintf(uaRspStr, "+CIPRXGET:%d,%d,%d,%d", mode, nMuxIndex, uDataSize, iResult);
            }
            if (cipSRIP_showIPPort)
            {
                AT_Sprintf(uaIpStr, ",%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
                AT_StrCat(uaRspStr, uaIpStr);
            }
            at_CmdRespInfoText(engine, uaRspStr);
        }
        if (mode == 2 || mode == 0)
            at_CmdRespInfoNText(engine, pData, uIPHlen + iResult);
        else if (mode == 3)
        {
            UINT8 *pHexData = AT_MALLOC(2 * uDataSize + 25);
            if (pHexData == NULL)
            {
                AT_TC(g_sw_TCPIP, "recv_data malloc fails for hex data");
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                AT_FREE(pData);
                return -1;
            }
            AT_MemCpy(pHexData, pData, uIPHlen);
            for (int i = 0; i < iResult; i++)
            {
                UINT8 tmp[3] = {0};
                AT_Sprintf(tmp, "%02x", pData[uIPHlen + i]);
                AT_StrCat(pHexData, tmp);
            }
            at_CmdRespInfoNText(engine, pHexData, uIPHlen + 2 * iResult);
            AT_FREE(pHexData);
        }
    }
    else
    {
        iResult = CFW_TcpipGetRecvAvailable(uSocket);
        if (SOCKET_ERROR == iResult)
        {
            AT_FREE(pData);
            AT_TC(g_sw_TCPIP, "recv_data, CFW_TcpipGetRecvAvailable error");
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            AT_FREE(pData);
            return -1;
        }
        if (!cipMux_multiIp)
        {
            AT_Sprintf(uaRspStr, "+CIPRXGET:%d,%d", mode, iResult);
        }
        else
        {
            AT_Sprintf(uaRspStr, "+CIPRXGET:%d,%d,%d", mode, nMuxIndex, iResult);
        }
        at_CmdRespInfoText(engine, uaRspStr);
    }
    AT_FREE(pData);
    return 0;
}

static VOID tcpip_connect_done(UINT8 nDLCI, UINT8 nMuxIndex)
{
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    tcpipParas->uState = CIP_CONNECTED;
    if (cipMODE_transParent)
    {
        start_transparent_mode(nDLCI);
    }
    else
    {
        UINT8 uaRspStr[30] = {
            0,
        };
        if (!cipMux_multiIp)
            AT_Sprintf(uaRspStr, "CONNECT OK");
        else
            AT_Sprintf(uaRspStr, "%d,CONNECT OK", nMuxIndex);
        AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
        at_CmdRespUrcText(engine, uaRspStr);
    }
}

static VOID tcpip_rsp(COS_EVENT *pEvent)
{
    UINT8 uaRspStr[60] = {
        0,
    };
    INT32 iResult = 0;
    AT_TC(g_sw_TCPIP, "tcpip_rsp Got %s: 0x%x,0x%x,0x%x,0x%x",
          TS_GetEventName(pEvent->nEventId), pEvent->nEventId, pEvent->nParam1, pEvent->nParam2, pEvent->nParam3);

    UINT8 nDlc = pEvent->nParam3;
    UINT8 nMuxIndex = pEvent->nParam3 >> 16;
    UINT8 nSim = AT_SIM_ID(nDlc);
    UINT8 uSocket = (UINT8)pEvent->nParam1;
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDlc);

    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);

    AT_TC(g_sw_TCPIP, "tcpip_rsp uSocket=%d,tcpipParas->uSocket=%d,state=%d,nMuxIndex=%d", uSocket, tcpipParas->uSocket, tcpipParas->uState, nMuxIndex);

    switch (pEvent->nEventId)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
        if (tcpipParas->uSocket == uSocket)
        {
            tcpip_connect_done(nDlc, nMuxIndex);
        }
        break;
    case EV_CFW_TCPIP_ACCEPT_IND:
    {
        CFW_TCPIP_SOCKET_ADDR addr;
        INT8 newSocket = CFW_TcpipSocketAccept(uSocket, &addr, sizeof(addr));
        for (int i = 0; i < MEMP_NUM_NETCONN; i++)
        {
            if (g_uCipContexts.nTcpipParas[i].uState == CIP_INITIAL)
            {
                g_uCipContexts.nTcpipParas[i].uState = CIP_CONNECTED;
                g_uCipContexts.nTcpipParas[i].uSocket = newSocket;
                gCipserver.channelid = i;
                CFW_TcpipSocketSetParam(newSocket, tcpip_rsp, i << 16 | nDlc);
                break;
            }
        }
        AT_Sprintf(uaRspStr, "%d, REMOTE IP: %s", gCipserver.channelid, ip4addr_ntoa((ip4_addr_t *)&(addr.sin_addr)));
        at_CmdRespUrcText(engine, uaRspStr);
    }
    break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
        AT_TC(g_sw_ATE, "RECEIVED EV_CFW_TCPIP_SOCKET_CLOSE_RSP ...");
        if (cipMODE_transParent)
        {
            stop_transparent_mode(nDlc);
        }
        if (0xff == nMuxIndex)
        {
            at_CmdRespUrcText(engine, "SERVER CLOSED");
            gCipserver.nSocketId = 0;
            break;
        }
        else if (tcpipParas->uState == CIP_CLOSING)
        {
            if (!cipMux_multiIp)
                AT_Sprintf(uaRspStr, "CLOSED");
            else
                AT_Sprintf(uaRspStr, "%d,CLOSED", nMuxIndex);
            stop_tcpip_close_timer(nDlc, nMuxIndex);
            at_CmdRespUrcText(engine, uaRspStr);
        }
        else if (tcpipParas->uState != CIP_CLOSED)
        {
            if (!cipMux_multiIp)
                AT_Sprintf(uaRspStr, "CLOSE OK");
            else
                AT_Sprintf(uaRspStr, "%d,CLOSE OK", nMuxIndex);
            stop_tcpip_close_timer(nDlc, nMuxIndex);
            at_CmdRespOKText(engine, uaRspStr);
        }
        tcpipParas->uState = CIP_CLOSED;
        break;
    case EV_CFW_TCPIP_ERR_IND:
        if (tcpipParas->uSocket == uSocket)
        {
            INT8 err = (INT8)pEvent->nParam2;
            if (tcpipParas->uState == CIP_PROCESSING)
            {
                if (!cipMux_multiIp)
                    AT_Sprintf(uaRspStr, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                else
                    AT_Sprintf(uaRspStr, "%d,CONNECT FAIL", nMuxIndex);
                at_CmdRespUrcText(engine, uaRspStr);
            }
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                UINT8 *string = lwip_strerr(err);
                tcpipParas->uState = CIP_CLOSING;
                start_tcpip_close_timer(nDlc, nMuxIndex);
                //AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, string, AT_StrLen(string), nDlc);
            }
        }
        break;

    case EV_CFW_TCPIP_CLOSE_IND:
        if (tcpipParas->uSocket == uSocket)
        {
            if (CFW_TcpipSocketClose(uSocket) == ERR_SUCCESS)
            {
                tcpipParas->uState = CIP_CLOSING;
                start_tcpip_close_timer(nDlc, nMuxIndex);
            }
        }
        break;
    case EV_CFW_TCPIP_SOCKET_SEND_RSP:
        AT_TC(g_sw_TCPIP, "EV_CFW_TCPIP_SOCKET_SEND_RSP");
        if (!cipMODE_transParent)
        {
            memset(uaRspStr, 0, 30);
            if (!cipQSEND_quickSend && cipSPRT_sendPrompt != 2)
            {
                if (!cipMux_multiIp)
                    AT_Sprintf(uaRspStr, "SEND OK");
                else
                    AT_Sprintf(uaRspStr, "%d,SEND OK", nMuxIndex);
            }
            at_CmdRespOKText(engine, uaRspStr);
        }
        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
        AT_TC(g_sw_TCPIP, "EV_CFW_TCPIP_REV_DATA_IND");
        if (tcpipParas->uSocket == uSocket)
        {
            UINT16 uDataSize = 0;
            uDataSize = (UINT16)pEvent->nParam2; // maybe contain check bits
            if (cipRXGET_manualy == 1)
            {
                AT_Sprintf(uaRspStr, "+CIPRXGET:%d", nMuxIndex);
                at_CmdRespUrcText(engine, uaRspStr);
            }
            else
            {
                recv_data(uSocket, uDataSize, nDlc, nMuxIndex, 0);
            }
        }
        break;
    default:
        AT_TC(g_sw_TCPIP, "TCPIP unexpect asynchrous event/response %d", pEvent->nEventId);
        at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        break;
    }
    AT_FREE(pEvent);
}

VOID AT_TCPIP_CmdFunc_CIPMUX(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPSTART ... ... pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (mode == cipMux_multiIp || CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState && CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_2].nState && CIP_STATUS > g_uCipContexts.nBearerParas[BEARER_WIFI].nState)
        {
            if (mode != cipMux_multiIp)
            {
                for (int i = 0; i < MEMP_NUM_NETCONN; i++)
                {
                    memset(&(g_uCipContexts.nTcpipParas[i]), 0, sizeof(stAT_Tcpip_Paras));
                }
            }
            cipMux_multiIp = mode;
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else
        {
            AT_TC(g_sw_TCPIP, "AT+CIPMUX ERROR,current cipMux_multiIp:%d, status:%d", cipMux_multiIp, g_uCipContexts.nBearerParas[gCipBearer].nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPMUX:%d", cipMux_multiIp);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPMUX:(0,1)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPMUX, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPSGTXT(AT_CMD_PARA *pParam)
{
    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (mode == 2)
        {
            struct netif *netif = getEtherNetIf(0x11);
            if (netif == NULL)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

            if (g_uCipContexts.nBearerParas[mode].nState < CIP_GPRSACT)
                g_uCipContexts.nBearerParas[mode].nState = CIP_GPRSACT;
            g_uCipContexts.nBearerParas[mode].nCid = 0x11;
        }
        else if (!cipMux_multiIp)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        gCipBearer = mode;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[60] = {0};
        UINT8 *status1 = "DEACTIVE";
        UINT8 *status2 = "DEACTIVE";
        UINT8 *status3 = "DEACTIVE";
        if (g_uCipContexts.nBearerParas[0].nState > CIP_CONFIG && g_uCipContexts.nBearerParas[0].nState < CIP_PDPDEACT)
            status1 = "ACTIVE";
        if (g_uCipContexts.nBearerParas[1].nState > CIP_CONFIG && g_uCipContexts.nBearerParas[1].nState < CIP_PDPDEACT)
            status2 = "ACTIVE";
        if (getEtherNetIf(0x11) != NULL)
            status3 = "ACTIVE";

        AT_Sprintf(aucBuffer, "+CIPSGTXT:%d,(0,%s),(1,%s),(2,%s)", gCipBearer, status1, status2, status3);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPSGTXT:(0,1,2)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSGTXT, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPSTART
* Description: Start up TCPIP connection
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-15
******************************************************************************/

VOID AT_TCPIP_CmdFunc_CIPSTART(AT_CMD_PARA *pParam)
{
    UINT8 nSimStatus = 0;

    AT_TC(g_sw_TCPIP, "AT+CIPSTART ... ... pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        if (gCipBearer != BEARER_WIFI)
        {
            nSimStatus = CFW_GetSimStatus(pParam->nDLCI);
            if (1 != nSimStatus)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_SIM_NOT_INSERTED));
            }
        }

        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if ((!cipMux_multiIp && (CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState || CIP_STATUS == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState)) || (cipMux_multiIp && (CIP_STATUS == bearerParas->nState || CIP_GPRSACT == bearerParas->nState)))
        {
            if (g_uAtGprsCidNum > AT_PDPCID_MAX)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

            if ((!cipMux_multiIp && pParam->paramCount != 3) || (cipMux_multiIp && pParam->paramCount != 4))
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

            bool paramok = true;
            UINT8 uMuxIndex = 0;
            UINT8 nType, nProtocol;
            UINT16 uPort = 0;
            if (cipMux_multiIp)
            {
                uMuxIndex = at_ParamUintInRange(pParam->params[0], 0, 7, &paramok);
            }
            const uint8_t *mode = at_ParamStr(pParam->params[0 + cipMux_multiIp], &paramok);
            const uint8_t *ipaddress = at_ParamStr(pParam->params[1 + cipMux_multiIp], &paramok);
            uPort = at_ParamUintInRange(pParam->params[2 + cipMux_multiIp], 0, 65535, &paramok);
            if (!paramok)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

            if (AT_StrNCaselessCmp(mode, "TCP", 3) == 0)
            {
                nType = CFW_TCPIP_SOCK_STREAM;
                nProtocol = CFW_TCPIP_IPPROTO_TCP;
            }
            else if (AT_StrNCaselessCmp(mode, "UDP", 3) == 0)
            {
                nType = CFW_TCPIP_SOCK_DGRAM;
                nProtocol = CFW_TCPIP_IPPROTO_UDP;
                update_udp_remote_socket(uMuxIndex, ipaddress, AT_StrLen(ipaddress), uPort);
            }
            else
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }

            at_CmdRespOK(pParam->engine); //Result Ok when param is right

            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);

            UINT8 aucBuffer[40] = {0};
            if (tcpipParas->uState > CIP_STATUS &&
                tcpipParas->uState < CIP_CLOSED)
            {
                if (cipMux_multiIp)
                    AT_Sprintf(aucBuffer, "%d,ALREAY CONNECT", uMuxIndex);
                else
                    AT_Sprintf(aucBuffer, "ALREAY CONNECT");
                AT_CMD_RETURN(at_CmdRespUrcText(pParam->engine, aucBuffer));
            }

            tcpipParas->uType = nType;
            tcpipParas->uProtocol = nProtocol;
            tcpipParas->uDomain = CFW_TCPIP_AF_INET;
            AT_MemSet(tcpipParas->uaIPAddress, 0, AT_TCPIP_HOSTLEN);
            AT_StrNCpy(tcpipParas->uaIPAddress, ipaddress, AT_TCPIP_ADDRLEN);
            tcpipParas->uPort = uPort;
            tcpipParas->uConnectBearer = gCipBearer;
            tcpipParas->uState = bearerParas->nState;

            INT32 iResult = 0;
            if (CIP_INITIAL == bearerParas->nState)
            {
                if (cip_getCid(pParam->nDLCI) < 0 || set_userpwdapn(pParam->nDLCI, NULL, NULL, NULL) < 0 || do_attact(pParam->nDLCI) < 0)
                    iResult = -1;
            }
            else
            {
                iResult = AT_TCPIP_Connect(pParam->nDLCI, uMuxIndex);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSTART(), connect failed: %d\n", iResult);
                }
            }
            if (iResult != ERR_SUCCESS)
            {
                if (!cipMux_multiIp)
                    AT_Sprintf(aucBuffer, "STATE:%s\r\n\r\nCONNECT FAIL", g_strATTcpipStatus[tcpipParas->uState]);
                else
                    AT_Sprintf(aucBuffer, "%d,CONNECT FAIL", uMuxIndex);
                at_CmdRespUrcText(pParam->engine, aucBuffer);
            }
        }
        else
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSTART(), state error: %d\n", bearerParas->nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 *pRstStr = AT_MALLOC(85);
        if (NULL == pRstStr)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));

        AT_MemSet(pRstStr, 0, 85);
        if (cipMux_multiIp)
            AT_StrCpy(pRstStr, "+CIPSTART: (0-7), (\"TCP\", \"UDP\"), (\"(0-255).(0-255).(0-255).(0-255)\"), (0-65535)");
        else
            AT_StrCpy(pRstStr, "+CIPSTART: (\"TCP\", \"UDP\"), (\"(0-255).(0-255).(0-255).(0-255)\"), (0-65535)");

        at_CmdRespInfoText(pParam->engine, pRstStr);
        at_CmdRespOK(pParam->engine);
        AT_FREE(pRstStr);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSTART(), command type not allowed");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************

* Name:        VOID AT_TCPIP_CmdFunc_CIFSR

* Description: get local IP address. Only at the status of activated scene:
IP GPRSACT, TCP/UDP connecting,
                    CONNECT OK, IP CLOSE can get local IP address by AT+CIFSR
. otherwise response ERROR.

* Parameter:  AT_CMD_PARA *pParam

* Return:       VOID

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIFSR(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_TCPIP, "CIFSR processing\n");

    if (pParam->iType == AT_CMD_EXE)
    {
        if (0 != pParam->paramCount)
        {
            AT_TC(g_sw_TCPIP, "AT+CIFSR: too parameter presented");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if (CIP_GPRSACT > bearerParas->nState ||
            CIP_CLOSED < bearerParas->nState)
        {
            AT_TC(g_sw_TCPIP, "AT+CIFSR: execute the command at wrong state:%d", bearerParas->nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        else
        {
            struct netif *netif;
            if (bearerParas->nCid == 0x11)
                netif = getEtherNetIf(bearerParas->nCid);
            else
                netif = getGprsNetIf(nSim, bearerParas->nCid);

            if (netif == NULL)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

            ip4_addr_t *ip_addr = netif_ip4_addr(netif);
            bearerParas->nState = CIP_STATUS;
            at_CmdRespInfoText(pParam->engine, inet_ntoa(*ip_addr));
            at_CmdRespOK(pParam->engine);
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT+CIFSR: command not supported");
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        VOID AT_TCPIP_CmdFunc_CIPCLOSE

* Description: Only close the connection at the status of TCP/UDP CONNECTING or CONNECT OK. otherwise
                    response error. after close the connection, the status is IP CLOSE.

* Parameter:  AT_CMD_PARA *pParam

* Return:       VOID

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPCLOSE(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "CIPSTATUS processing\n");

    if (pParam->iType == AT_CMD_EXE || pParam->iType == AT_CMD_SET)
    {
        if (AT_CMD_EXE == pParam->iType && cipMux_multiIp)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCLOSE: AT_CMD_EXE Only work at nomal mode");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (AT_CMD_SET == pParam->iType && ((pParam->paramCount > 1) && (!cipMux_multiIp)) && ((pParam->paramCount > 2) && (cipMux_multiIp == 1)))
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCLOSE: pPara is error cipMux_multiIp=%d,uParaCount=%d", cipMux_multiIp, pParam->paramCount);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        UINT8 uMuxIndex = 0;
        bool paramok = true;
        UINT8 isQuickClose = 0;

        if (pParam->paramCount > 0)
        {
            if (cipMux_multiIp)
            {
                uMuxIndex = at_ParamUintInRange(pParam->params[0], 0, 7, &paramok);
                if (pParam->paramCount > 1)
                {
                    isQuickClose = at_ParamUintInRange(pParam->params[1], 0, 1, &paramok);
                }
            }
            else
            {
                isQuickClose = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            }
        }
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
        // only on connecting state , the command can be executed.
        if (CIP_PROCESSING != tcpipParas->uState && CIP_CONNECTED != tcpipParas->uState)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCLOSE: can't run at this state:%d", tcpipParas->uState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        INT32 iRetValue;
        iRetValue = CFW_TcpipSocketClose(tcpipParas->uSocket);
        if (iRetValue == ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "AT+IPCLOSE: close success");
            start_tcpip_close_timer(pParam->nDLCI, uMuxIndex);
            //AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI);
            if (isQuickClose)
            {
                UINT8 uaRspStr[60] = {
                    0,
                };
                if (!cipMux_multiIp)
                    AT_Sprintf(uaRspStr, "CLOSE OK");
                else
                    AT_Sprintf(uaRspStr, "%d,CLOSE OK", uMuxIndex);
                tcpipParas->uState = CIP_CLOSED;
                at_CmdRespOKText(pParam->engine, uaRspStr);
            }
            return;
        }
        else
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCLOSE: CSW execute wrong");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT+CIPCLOSE: command not supported");
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************

* Name:        VOID AT_TCPIP_CmdFunc_CIPSHUT

* Description: Disconnect the wireless connection. Except at the status of IP INITIAL, you can close moving
                   scene by AT+CIPSHUT. After closed, the status is IP INITIAL.

* Parameter:  AT_CMD_PARA *pParam

* Return:       VOID

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPSHUT(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_TCPIP, "CIPSHUT processing\n");

    if (pParam->iType == AT_CMD_EXE)
    {
        if (0 != pParam->paramCount)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSHUT: too many parameter");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
        AT_TC(g_sw_TCPIP, "AT+CIPSHUT: bearerParas.status %d\n", bearerParas->nState);

        // only on connecting state , the command can be executed.
        if (CIP_INITIAL == bearerParas->nState)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSHUT: can't ran in CIP_INITIAL");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        INT32 nOperationRet = 0;

        if (CIP_INITIAL < bearerParas->nState &&
            CIP_PDPDEACT > bearerParas->nState)
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[i]);
                if ((CIP_PROCESSING == tcpipParas->uState || CIP_CONNECTED == tcpipParas->uState) && tcpipParas->uConnectBearer == gCipBearer)
                {
                    tcpipParas->uState = CIP_CLOSING;
                    if (CFW_TcpipSocketClose(tcpipParas->uSocket) != ERR_SUCCESS)
                        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                    start_tcpip_close_timer(pParam->nDLCI, i);
                }
            }
            if (gCipBearer != BEARER_WIFI)
            {
                if (bearerParas->nState < CIP_GPRSACT)
                {
                    CFW_GprsRemovePdpCxt(bearerParas->nCid, nSim);
                    CFW_ReleaseCID(bearerParas->nCid, nSim);
                    bearerParas->nCid = 0;
                    bearerParas->nState = CIP_INITIAL;
                    at_CmdRespOKText(pParam->engine, "SHUT OK");
                }
                else
                {
                    COS_Sleep(500);
                    nOperationRet = CFW_GprsActEX(AT_GPRS_DEA, bearerParas->nCid, pParam->nDLCI, nSim, 0, attact_rsp);
                    if (nOperationRet == ERR_SUCCESS)
                    {
                        AT_SetAsyncTimerMux(pParam->engine, 0);
                        return;
                    }
                    else
                    {
                        AT_TC(g_sw_TCPIP, "exe       in AT_TCPIP_CmdFunc_CIPSHUT(), Socket closing failed: %x\n", nOperationRet);
                        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                    }
                }
            }
            else
            {
                bearerParas->nState = CIP_GPRSACT;
                at_CmdRespOKText(pParam->engine, "SHUT OK");
            }
        }
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT+CIPSHUT: command not support");
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIICR
* Description: Bring up wireless connection with GPRS
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-19
******************************************************************************/

VOID AT_TCPIP_CmdFunc_CIICR(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if (bearerParas->nState != CIP_START)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIICR, TCPIP state error:%d\n", bearerParas->nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (gCipBearer == BEARER_WIFI)
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

        if (0 != pParam->paramCount)
        {
            AT_TC(g_sw_TCPIP, "exe       in AT_TCPIP_CmdFunc_CIICR, too many parameters\n");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (do_attact(pParam->nDLCI) < 0)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            bearerParas->nState = CIP_CONFIG;
            AT_SetAsyncTimerMux(pParam->engine, 0);
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CSTT
* Description:  Set TCPIP moving scence. Start task and set APN, USER, PWD
* Parameter:    AT_CMD_PARA *pParam
* Return:       VOID
* Remark:       n/a
* Author:       YangYang
* Data:         2008-5-19
******************************************************************************/
VOID AT_TCPIP_CmdFunc_CSTT(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    if (AT_CMD_SET == pParam->iType)
    {
        if (bearerParas->nState != CIP_INITIAL)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT, TCPIP state error:%d\n", bearerParas->nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (gCipBearer == BEARER_WIFI)
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

        if (pParam->paramCount > 3)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT, parameter error\n");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        bool paramok = true;
        UINT8 uMuxIndex = 0;
        UINT8 nType, nProtocol;
        UINT16 uPort = 0;
        const uint8_t *uaApn = NULL;
        const uint8_t *uaPwd = NULL;
        const uint8_t *uaUsr = NULL;
        if (pParam->paramCount > 0)
            uaApn = at_ParamStr(pParam->params[0], &paramok);
        if (pParam->paramCount > 1)
            uaPwd = at_ParamStr(pParam->params[1], &paramok);
        if (pParam->paramCount > 2)
            uaUsr = at_ParamStr(pParam->params[2], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (cip_getCid(pParam->nDLCI) < 0 || set_userpwdapn(pParam->nDLCI, uaUsr, uaPwd, uaApn) < 0)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        bearerParas->nState = CIP_START;
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_EXE == pParam->iType)
    {
        if (bearerParas->nState != CIP_INITIAL)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT, TCPIP state error:%d\n", bearerParas->nState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (cip_getCid(pParam->nDLCI) < 0 || set_userpwdapn(pParam->nDLCI, NULL, NULL, NULL) < 0)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        bearerParas->nState = CIP_START;
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        INT32 iResult = 0;
        UINT8 *pRsp = NULL;
        CFW_GPRS_PDPCONT_INFO sPdpCont;
        pRsp = AT_MALLOC(AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
        if (NULL == pRsp)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT(), read mode, memory failed");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));
        }
        if (0 == bearerParas->nCid)
        {
            AT_MemZero(pRsp, AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
            AT_Sprintf(pRsp, "+CSTT: \"CMNET\",  \"\", \"\"");
            at_CmdRespInfoText(pParam->engine, pRsp);
            AT_FREE(pRsp);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }

        UINT8 *buffer = AT_MALLOC(100 + 18 + 200 + 20);
        if (buffer == NULL)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));
        AT_MemSet(buffer, 0, 338);
        sPdpCont.pApn = buffer;                     //AT_MALLOC(100);  // the max length of the APN is 100
        sPdpCont.pPdpAddr = buffer + 100;           //AT_MALLOC(18); // the max length of the PDPADDR is 18
        sPdpCont.pApnUser = buffer + 100 + 18;      //(UINT8 *)AT_MALLOC(200);
        sPdpCont.pApnPwd = buffer + 100 + 18 + 200; //(UINT8 *)AT_MALLOC(20);

        if ((iResult = CFW_GprsGetPdpCxt(bearerParas->nCid, &sPdpCont, nSim)) != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT(), read mode, CFW_GprsGetPdpCxt:%d,nSim=%d,bearerParas->nCid=%d", iResult, nSim, bearerParas->nCid);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            AT_MemZero(pRsp, AT_GPRS_APN_MAX_LEN + AT_GPRS_USR_MAX_LEN + AT_GPRS_USR_MAX_LEN + 15);
            AT_Sprintf(pRsp, "+CSTT: \"%s\",  \"%s\", \"%s\"", sPdpCont.pApn,
                       sPdpCont.pApnUser,
                       sPdpCont.pApnPwd);
            at_CmdRespInfoText(pParam->engine, pRsp);
            at_CmdRespOK(pParam->engine);
        }
        AT_FREE(pRsp);
        AT_FREE(buffer);
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 *pRsp = AT_MALLOC(30);
        if (NULL == pRsp)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT(), read mode, memory failed");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));
        }
        AT_MemZero(pRsp, 30);
        AT_Sprintf(pRsp, "+CSTT: \"APN\", \"USER\", \"PWD\"");

        at_CmdRespInfoText(pParam->engine, pRsp);
        at_CmdRespOK(pParam->engine);
        AT_FREE(pRsp);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CSTT(), command type not allowed");
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

/********************************************************************************************
* Name:                    AT_TCPIP_CmdFunc_CIPSEND
* Description:      Send data through TCP or UDP connection
* Parameter:        AT_CMD_PARA *pParam
* Return:               VOID
* Remark:               After cmds +CGATT,+CGDCONT,+CGACT,+CIPSTART return OK,this cmd can be run
* Author/Modifier:      YangYang,wulc
* Data:                 2008-5-21
********************************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPSEND(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPSEND: pParam->iType = %d, pParam->nDLCI = %d", pParam->iType, pParam->nDLCI);

    if (AT_CMD_EXE == pParam->iType || AT_CMD_SET == pParam->iType)
    {
        if (AT_CMD_EXE == pParam->iType && cipMux_multiIp)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSEND: AT_CMD_EXE Only work at nomal mode");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (AT_CMD_EXE == pParam->iType && pParam->paramCount != 0)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSEND: pPara number is error");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_CMD_SET == pParam->iType && (pParam->paramCount == 1 && cipMux_multiIp || pParam->paramCount == 2 && !cipMux_multiIp || pParam->paramCount > 2))
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSEND: pPara is error cipMux_multiIp=%d,uParaCount=%d", cipMux_multiIp, pParam->paramCount);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        INT32 iResult = 0;
        UINT8 PromptMsg[8] = {0};
        UINT16 uSize = 0;
        UINT16 uPara1 = 0;
        UINT16 uLength = 0;
        UINT8 uIndex = 0;
        UINT8 uMuxIndex = 0;

        if (pParam->paramCount > 0)
        {
            uSize = sizeof(uLength);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &uLength, &uSize);
            uIndex++;
            if (pParam->paramCount > 1)
            {
                uMuxIndex = uLength;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &uLength, &uSize);
            }
            if (iResult != 0)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }

        stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
        if (AT_CMD_LINE_BUFF_LEN < uLength || tcpipParas->uState != CIP_CONNECTED)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSEND: uLength is %d, tcpipParas->uState=%d", uLength, tcpipParas->uState);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        AT_TC(g_sw_TCPIP, "AT+CIPSEND: pParam->iType = %d, uMuxIndex = %d,uLength=%d", pParam->iType, uMuxIndex, uLength);
        if (!pParam->iExDataLen)
        {
            if (cipSPRT_sendPrompt == 1)
                at_CmdRespOutputPrompt(pParam->engine);

            AT_TC(g_sw_TCPIP, "exe in AT_TCPIP_CmdFunc_CIPSEND(), auto sending timer:%d seconds",
                  cipATS_time);

            if (uLength > 0)
                at_CmdSetRawMode(pParam->engine, uLength);
            else if (cipATS_setTimer)
            {
                //AT_TC(g_sw_TCPIP,"th->bufflen=%d",g_bypass_buf->buffLen);
                at_StartCallbackTimer(cipATS_time * 1000, at_delayAsync, &pParam->nDLCI);
                at_CmdSetBypassMode(pParam->engine, bypassDataRecv, NULL);
            }
            else
                at_CmdSetPromptMode(pParam->engine);
            return;
        }
        else
        {
            if (uLength == 0 && !cipATS_setTimer)
            {
                if (pParam->pExData[pParam->iExDataLen - 1] == 27)
                {
                    AT_TC(g_sw_TCPIP, "end with esc, cancel send");
                    AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                    return;
                }
                else if (pParam->pExData[pParam->iExDataLen - 1] != 26 || pParam->iExDataLen <= 1)
                {
                    AT_TC(g_sw_TCPIP, "not end with ctl+z, err happen");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_INVALID_LEN));
                }
            }
            else if (uLength != 0 && uLength != pParam->iExDataLen)
            {
                AT_TC(g_sw_TCPIP, "pParam->iExDataLen=%d,uLength=%d", pParam->iExDataLen, uLength);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_INVALID_LEN));
            }

            if (tcpipParas->uProtocol != CFW_TCPIP_IPPROTO_UDP)
            {
                iResult = CFW_TcpipSocketSend(tcpipParas->uSocket, (UINT8 *)pParam->pExData, (UINT16)pParam->iExDataLen - ((uLength == 0) && !cipATS_setTimer), 0);
                if (iResult == SOCKET_ERROR)
                {
                    AT_TC(g_sw_TCPIP, "TCPIP send socket data error");
                }
            }
            else
            {
                CFW_TCPIP_SOCKET_ADDR nDestAddr;
                nDestAddr.sin_family = CFW_TCPIP_AF_INET;
                nDestAddr.sin_port = htons(tcpipParas->uPort);
                nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
                iResult = CFW_TcpipInetAddr(tcpipParas->uaIPAddress);
                if (SOCKET_ERROR == iResult)
                {
                    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSEND CFW_TcpipInetAddr() failed\n");
                }
                else
                {
                    nDestAddr.sin_addr.s_addr = iResult;
                    AT_TC(g_sw_TCPIP, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
                    iResult = CFW_TcpipSocketSendto(tcpipParas->uSocket, (UINT8 *)pParam->pExData, (UINT16)pParam->iExDataLen - ((uLength == 0) && !cipATS_setTimer), 0, &nDestAddr, nDestAddr.sin_len);
                    if (iResult == SOCKET_ERROR)
                    {

                        AT_TC(g_sw_TCPIP, "UDP send socket data error");
                    }
                }
            }
            if (cipATS_setTimer)
            {
                at_TCPIPBufDestroy(g_bypass_buf);
                g_bypass_buf = NULL;
            }
            UINT8 aucBuffer[40] = {0};
            AT_SetAsyncTimerMux(pParam->engine, 120);
            if (iResult != SOCKET_ERROR)
            {
                if (cipQSEND_quickSend)
                {
                    if (!cipMux_multiIp)
                        AT_Sprintf(aucBuffer, "DATA ACCEPT:%d", (pParam->iExDataLen - ((uLength == 0) && !cipATS_setTimer)));
                    else
                        AT_Sprintf(aucBuffer, "DATA ACCEPT:%d,%d", uMuxIndex, (pParam->iExDataLen - ((uLength == 0) && !cipATS_setTimer)));
                }
                else if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
                {
                    if (cipSPRT_sendPrompt != 2)
                    {
                        if (!cipMux_multiIp)
                            AT_Sprintf(aucBuffer, "SEND OK");
                        else
                            AT_Sprintf(aucBuffer, "%d,SEND OK", uMuxIndex);
                    }
                    else
                    {
                        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, ""));
                    }
                }
                if (strlen(aucBuffer) != 0)
                {
                    AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, aucBuffer));
                }
            }
            else
            {
                if (!cipMux_multiIp)
                    AT_Sprintf(aucBuffer, "SEND FAIL");
                else
                    AT_Sprintf(aucBuffer, "%d,SEND FAIL", uMuxIndex);
                AT_CMD_RETURN(at_CmdRespErrorText(pParam->engine, aucBuffer));
            }
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (!cipMux_multiIp)
            AT_Sprintf(aucBuffer, "+CIPSEND:<length>");
        else
            AT_Sprintf(aucBuffer, "+CIPSEND:<0-7>,<length>");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (!cipMux_multiIp)
            AT_Sprintf(aucBuffer, "+CIPSEND:<size>");
        else
            AT_Sprintf(aucBuffer, "+CIPSEND:<n>,<size>");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSEND(), command type not allowed");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPQSEND(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPQSEND ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        else
            cipQSEND_quickSend = mode;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPQSEND:%d", cipQSEND_quickSend);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPQSEND:(0,1)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPQSEND, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPRXGET(AT_CMD_PARA *pParam)
{
    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 4, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if ((mode == 1 || mode == 0) && pParam->paramCount == 1)
        {
            if (cipRXGET_manualy == 1 && mode == 1 && !cipMux_multiIp)
            {
                UINT8 uaIpStr[30] = {
                    0,
                };
                if (cipSRIP_showIPPort)
                    AT_Sprintf(uaIpStr, "+CIPRXGET:1,%s:%d", g_uCipContexts.nTcpipParas[0].uaIPAddress, g_uCipContexts.nTcpipParas[0].uPort);
                else
                    AT_Sprintf(uaIpStr, "+CIPRXGET:1");
                at_CmdRespInfoText(pParam->engine, uaIpStr);
            }
            cipRXGET_manualy = mode;
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else if (mode > 0 && pParam->paramCount == 2 + cipMux_multiIp ||
                 ((mode == 1 || mode == 4) && pParam->paramCount == 1 + cipMux_multiIp))
        {
            UINT8 uMuxIndex = 0;
            UINT32 requestLen = 0;
            UINT8 range = 0;
            if (cipMux_multiIp)
                uMuxIndex = at_ParamUintInRange(pParam->params[1], 0, 7, &paramok);
            if (mode == 2)
                range = 1460;
            else if (mode == 3)
                range = 730;
            if (range != 0)
                requestLen = at_ParamUintInRange(pParam->params[1 + cipMux_multiIp], 1, range, &paramok);

            if (paramok)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
                if (mode == 1)
                {
                    UINT8 uaIpStr[30] = {
                        0,
                    };
                    if (!cipMux_multiIp)
                    {
                        if (cipSRIP_showIPPort)
                            AT_Sprintf(uaIpStr, "+CIPRXGET:1,%s:%d", tcpipParas->uaIPAddress, tcpipParas->uPort);
                        else
                            AT_Sprintf(uaIpStr, "+CIPRXGET:1");
                    }
                    else
                    {
                        if (cipSRIP_showIPPort)
                            AT_Sprintf(uaIpStr, "+CIPRXGET:1,%d,%s:%d", uMuxIndex, tcpipParas->uaIPAddress, tcpipParas->uPort);
                        else
                            AT_Sprintf(uaIpStr, "+CIPRXGET:1,%d", uMuxIndex);
                    }
                    at_CmdRespInfoText(pParam->engine, uaIpStr);
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                }
                else if (recv_data(tcpipParas->uSocket, requestLen, pParam->nDLCI, uMuxIndex, mode) == ERR_SUCCESS)
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
            }
            else
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        else
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPRXGET, mode:%d,cipRXGET_manualy:%d,cipMux_multiIp:%d", mode, cipRXGET_manualy, cipMux_multiIp);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPRXGET:%d", cipRXGET_manualy);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (cipMux_multiIp)
        {
            AT_Sprintf(aucBuffer, "+CIPRXGET:(0-4), (0-7), (1-1460)");
            at_CmdRespInfoText(pParam->engine, aucBuffer);
        }
        else
        {
            AT_Sprintf(aucBuffer, "+CIPRXGET:(0-4), (1-1460)");
            at_CmdRespInfoText(pParam->engine, aucBuffer);
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPRXGET, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CDNSCFG
* Description:  Config DNS
* Parameter:    AT_CMD_PARA *pParam
* Return:       VOID
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

VOID AT_TCPIP_CmdFunc_CDNSCFG(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CDNSCFG ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
    UINT8 nDlc = pParam->nDLCI;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount > 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (bearerParas->nCid != 0x11)
        {
            bool paramok = true;
            const uint8_t *priDNS = NULL;
            const uint8_t *secDNS = NULL;
            if (pParam->paramCount > 0)
            {
                priDNS = at_ParamStr(pParam->params[0], &paramok);
                if (pParam->paramCount > 1)
                    secDNS = at_ParamStr(pParam->params[1], &paramok);
                else
                    secDNS = "0.0.0.0";
            }
            if (!paramok)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            if (strcmp(priDNS, "") == 0 || strcmp(secDNS, "") == 0 || IPADDR_NONE == CFW_TcpipInetAddr(priDNS) || IPADDR_NONE == CFW_TcpipInetAddr(secDNS))
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSCFG(),param error");
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSCFG(), cid = %d, priDNS = %s, secDNS=%s ", bearerParas->nCid, priDNS, secDNS);
            CFW_SetDnsServerbyPdp(bearerParas->nCid, nSim, priDNS, secDNS);
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        if (bearerParas->nCid != 0x11)
        {
            UINT8 outString[50] = {
                0x00,
            };
            UINT8 tmpString[30] = {
                0x00,
            };
            UINT32 *server = getDNSServer(bearerParas->nCid, nSim);
            ip_addr_t dns_server = IPADDR4_INIT(server[0]);
            ip_addr_t dns_server2 = IPADDR4_INIT(server[1]);
            AT_Sprintf(outString, "PrimaryDns: <%s>\r\n", ipaddr_ntoa(&dns_server));
            //AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSCFG(),PrimaryDns: %s\r\n", ipaddr_ntoa(&dns_server));
            AT_Sprintf(tmpString, "SecondaryDns: <%s>", ipaddr_ntoa(&dns_server2));
            //AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSCFG(),SecondaryDns: %s", ipaddr_ntoa(&dns_server2));
            AT_StrCat(outString, tmpString);
            at_CmdRespInfoText(pParam->engine, outString);
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        return;
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 outString[50] = {
            0x00,
        };
        AT_Sprintf(outString, "+CDNSCFG: (\"PrimaryDNS\"),(\"SecondaryDNS\")");
        at_CmdRespInfoText(pParam->engine, outString);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSCFG, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPSPRT
* Description:  Config DNS
* Parameter:    AT_CMD_PARA *pParam
* Return:       VOID
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

VOID AT_TCPIP_CmdFunc_CIPSPRT(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPSPRT ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    UINT8 nDlc = pParam->nDLCI;

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 prompt = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        cipSPRT_sendPrompt = prompt;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPSPRT:%d", cipSPRT_sendPrompt);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPSPRT:(0,1,2)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSPRT, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_CIPHEAD
* Description:  
* Parameter:    AT_CMD_PARA *pParam
* Return:       VOID
* Remark:       n/a
* Author:       
* Data:         20170907
******************************************************************************/

VOID AT_TCPIP_CmdFunc_CIPHEAD(AT_CMD_PARA *pParam)
{
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 header = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        cipHEAD_addIpHead = header;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPHEAD:%d", cipHEAD_addIpHead);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPHEAD:(0,1)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPHEAD, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPSRIP(AT_CMD_PARA *pParam)
{
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 header = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        cipSRIP_showIPPort = header;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPSRIP:%d", cipSRIP_showIPPort);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPSRIP:(0,1)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSRIP, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_CmdFunc_CIPATS
* Description: Set auto sending timer
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-21
******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPATS(AT_CMD_PARA *pParam)
{
    UINT16 time;
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    AT_TC(g_sw_TCPIP, "AT+CIPATS ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if ((mode == 0 && pParam->paramCount != 1) || (mode == 1 && pParam->paramCount != 2))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (1 == mode)
            time = at_ParamUintInRange(pParam->params[1], 1, 100, &paramok);
        else
            time = 1;
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        cipATS_setTimer = mode;
        cipATS_time = time;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPATS:%d,%d", cipATS_setTimer, cipATS_time);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};

        AT_Sprintf(aucBuffer, "+CIPATS:(0,1),(1-100)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPATS, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************

* Name:        VOID AT_TCPIP_CmdFunc_CIPSTATUS

* Description: Query the current connection status.

* Parameter:  AT_CMD_PARA *pParam

* Return:       VOID

* Remark:      n/a

* Author:       Chang Yuguang

* Data:          2008-5-19

******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPSTATUS(AT_CMD_PARA *pParam)
{
    UINT8 aucBuffer[256] = {0};

    AT_TC(g_sw_TCPIP, "AT+CIPSTATUS ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (pParam->iType == AT_CMD_SET)
    {
        if (!cipMux_multiIp)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSTATUS, command type:%d not allowed", pParam->iType);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
        }
        else
        {
            bool paramok = true;
            UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 5, &paramok);
            if (!paramok)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            AT_Sprintf(aucBuffer, "+CIPSTATUS:%d,%d,%s,%s,%d,%s ", mode,
                       g_uCipContexts.nTcpipParas[mode].uConnectBearer,
                       g_uCipContexts.nTcpipParas[mode].uProtocol == CFW_TCPIP_IPPROTO_UDP ? "UDP" : "TCP",
                       g_uCipContexts.nTcpipParas[mode].uaIPAddress,
                       g_uCipContexts.nTcpipParas[mode].uPort,
                       g_strATTcpipStatus[g_uCipContexts.nTcpipParas[mode].uState]);
            at_CmdRespUrcText(pParam->engine, aucBuffer);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        if (pParam->paramCount != 0)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        CIP_STATUS_T state = CIP_INITIAL;

        for (int i = 0; i < MEMP_NUM_NETCONN; i++)
        {
            CIP_STATUS_T s2 = g_uCipContexts.nTcpipParas[i].uState;
            if (state < s2)
                state = s2;
        }
        if (state != CIP_INITIAL)
            state = CIP_PROCESSING;
        else
        {
            for (int i = 0; i < BEARER_MAX_NUM; i++)
            {
                CIP_STATUS_T s = g_uCipContexts.nBearerParas[i].nState;
                if (state < s)
                    state = s;
            }
        }

        if (cipMux_multiIp)
        {
            at_CmdRespOK(pParam->engine);
            AT_Sprintf(aucBuffer, "STATE:%s ", g_strATTcpipStatus[state]);
            at_CmdRespUrcText(pParam->engine, aucBuffer);

            if (gCipserver.mode == CIPSERVER_OPEN)
            {
                AT_Sprintf(aucBuffer, "S:0,0,\"%d\",\"%s\" ", gCipserver.port, "LISTENING");
                at_CmdRespUrcText(pParam->engine, aucBuffer);
            }

            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                AT_Sprintf(aucBuffer, "C:%d,%d,%s,%s,%d,%s ", i,
                           g_uCipContexts.nTcpipParas[i].uConnectBearer,
                           g_uCipContexts.nTcpipParas[i].uProtocol == CFW_TCPIP_IPPROTO_UDP ? "UDP" : "TCP",
                           g_uCipContexts.nTcpipParas[i].uaIPAddress,
                           g_uCipContexts.nTcpipParas[i].uPort,
                           g_strATTcpipStatus[g_uCipContexts.nTcpipParas[i].uState]);
                at_CmdRespUrcText(pParam->engine, aucBuffer);
            }
        }
        else
        {
            if (state == CIP_PROCESSING)
                state = g_uCipContexts.nTcpipParas[0].uState;

            at_CmdRespOK(pParam->engine);
            AT_Sprintf(aucBuffer, "STATE:%s ", g_strATTcpipStatus[state]);
            at_CmdRespUrcText(pParam->engine, aucBuffer);
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        //AT_Sprintf(aucBuffer, "+CIPSTATUS:");
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSTATUS, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_CmdFunc_CIPSCONT
* Description: Save TCP/IP parameter into flash/Display TCPIP settings.
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-21
******************************************************************************/
VOID AT_TCPIP_CmdFunc_CIPSCONT(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (AT_CMD_EXE == pParam->iType)
    {
        cipSCONT_unSaved = 0;
        gAtCurrentSetting.tcpip = gCIPSettings;
        UINT8 ret = at_CfgSetAtSettings(0);
        if (ret)
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        else
            cipSCONT_unSaved = 1;
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 *pRsp = NULL;

        pRsp = AT_MALLOC(20);

        if (NULL == pRsp)
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_NO_MEMORY, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        AT_MemSet(pRsp, 0, 20);
        AT_Sprintf(pRsp, "+CIPSPRT: %d", cipSPRT_sendPrompt);
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI, nSim);
        AT_MemSet(pRsp, 0, 20);
        AT_Sprintf(pRsp, "+CIPHEAD: %d", cipHEAD_addIpHead);
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI, nSim);
        AT_MemSet(pRsp, 0, 20);
        AT_Sprintf(pRsp, "+CIPSRIP: %d", cipSRIP_showIPPort);
        AT_GPRS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI, nSim);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        AT_FREE(pRsp);
        return;
    }
    else
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT));
        return;
    }
}

VOID AT_TCPIP_CmdFunc_CIPACK(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPACK ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (pParam->iType == AT_CMD_EXE)
    {
        if (!cipMux_multiIp)
        {
            stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[0]);
            INT32 acklen = CFW_TcpipSocketGetAckedSize(tcpipParas->uSocket);
            INT32 sendlen = CFW_TcpipSocketGetSentSize(tcpipParas->uSocket);
            UINT8 aucBuffer[40] = {0};
            if (acklen < 0)
                acklen = 0;
            if (sendlen < 0)
                sendlen = 0;
            AT_Sprintf(aucBuffer, "+CIPACK:%d,%d,%d", sendlen, acklen, sendlen - acklen);
            at_CmdRespInfoText(pParam->engine, aucBuffer);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
    }
    else if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT8 uMuxIndex = 0;
        if (cipMux_multiIp && pParam->paramCount == 1)
        {
            uMuxIndex = at_ParamUintInRange(pParam->params[0], 0, 7, &paramok);
            if (paramok)
            {
                stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
                INT32 acklen = CFW_TcpipSocketGetAckedSize(tcpipParas->uSocket);
                INT32 sendlen = CFW_TcpipSocketGetSentSize(tcpipParas->uSocket);
                UINT8 aucBuffer[40] = {0};
                if (acklen < 0)
                    acklen = 0;
                if (sendlen < 0)
                    sendlen = 0;
                AT_Sprintf(aucBuffer, "+CIPACK:%d,%d,%d", sendlen, acklen, sendlen - acklen);
                at_CmdRespInfoText(pParam->engine, aucBuffer);
                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
            }
            else
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }
        else
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPACK, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CLPORT(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CLPORT ... ... pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        bool paramok = true;
        UINT16 uPort = 0;
        UINT8 uMuxIndex = 0;
        if (cipMux_multiIp)
        {
            uMuxIndex = at_ParamUintInRange(pParam->params[0], 0, 7, &paramok);
        }
        const uint8_t *mode = at_ParamStr(pParam->params[0 + cipMux_multiIp], &paramok);
        uPort = at_ParamUintInRange(pParam->params[1 + cipMux_multiIp], 0, 65535, &paramok);
        if (paramok)
        {
            if (AT_StrNCaselessCmp(mode, "TCP", 3) == 0)
            {
                gClocalport[uMuxIndex].tcp_port = uPort;
            }
            else if (AT_StrNCaselessCmp(mode, "UDP", 3) == 0)
            {
                gClocalport[uMuxIndex].udp_port = uPort;
            }
            else
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (!cipMux_multiIp)
        {
            AT_Sprintf(aucBuffer, "+CLPORT:TCP:%d,UDP:%d", gClocalport[0].tcp_port, gClocalport[0].udp_port);
            at_CmdRespInfoText(pParam->engine, aucBuffer);
        }
        else
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                AT_Sprintf(aucBuffer, "+CLPORT:%d,TCP:%d,UDP:%d", i, gClocalport[i].tcp_port, gClocalport[i].udp_port);
                at_CmdRespInfoText(pParam->engine, aucBuffer);
            }
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (!cipMux_multiIp)
            AT_Sprintf(aucBuffer, "+CLPORT: (\"TCP\",\"UDP\"),(0-65535)");
        else
            AT_Sprintf(aucBuffer, "+CLPORT: (0-7),(\"TCP\",\"UDP\"),(0-65535)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CLPORT, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPSERVER(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPSERVER ...gCipserver.channelid= %d", gCipserver.channelid);

    if (AT_CMD_SET == pParam->iType)
    {
        stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

        if ((!cipMux_multiIp && (CIP_INITIAL == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState || CIP_STATUS == g_uCipContexts.nBearerParas[BEARER_GPRS_DEF].nState)) || CIP_STATUS == bearerParas->nState)
        {
            bool paramok = true;
            UINT8 mode = 0;
            UINT32 port = 0;
            mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (pParam->paramCount == 1)
            {
                if (gCipserver.mode == CIPSERVER_OPEN && mode == CIPSERVER_CLOSE && paramok)
                {
                    if (gCipserver.channelid != 0xff)
                        CFW_TcpipSocketClose(g_uCipContexts.nTcpipParas[gCipserver.channelid].uSocket);
                    if (CFW_TcpipSocketClose(gCipserver.nSocketId) != ERR_SUCCESS)
                    {
                        AT_TC(g_sw_TCPIP, "AT+CIPSERVER: CSW execute wrong");
                        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                    }
                    else
                    {
                        gCipserver.mode = mode;
                        at_CmdRespOK(pParam->engine);
                    }
                }
                else
                {
                    if (gCipserver.mode == CIPSERVER_CLOSE && mode == CIPSERVER_CLOSE)
                    {
                        UINT8 aucBuffer[40] = {0};
                        AT_Sprintf(aucBuffer, "Already closed");
                        at_CmdRespInfoText(pParam->engine, aucBuffer);
                    }
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
            }
            else if (pParam->paramCount == 2)
            {
                port = at_ParamUintInRange(pParam->params[1], 0, 65535, &paramok);
                if (gCipserver.mode == CIPSERVER_CLOSE && mode == CIPSERVER_OPEN && paramok)
                {
                    if (CIP_INITIAL == bearerParas->nState)
                    {
                        gCipserver.port = port;
                        gCipserver.mode = CIPSERVER_OPENING;
                        if (cip_getCid(pParam->nDLCI) < 0 || set_userpwdapn(pParam->nDLCI, NULL, NULL, NULL) < 0 || do_attact(pParam->nDLCI) < 0)
                        {
                            gCipserver.mode = CIPSERVER_CLOSE;
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                        }
                    }
                    else
                    {
                        gCipserver.port = port;
                        if (ERR_SUCCESS == AT_TCPIP_ServerStart(pParam->nDLCI))
                        {
                            gCipserver.mode = mode;
                            at_CmdRespOK(pParam->engine);
                            at_CmdRespUrcText(pParam->engine, "SERVER OK");
                        }
                        else
                        {
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                        }
                    }
                }
                else
                {
                    if (gCipserver.mode == CIPSERVER_OPEN && mode == CIPSERVER_OPEN)
                    {
                        UINT8 aucBuffer[40] = {0};
                        AT_Sprintf(aucBuffer, "Already opened");
                        at_CmdRespInfoText(pParam->engine, aucBuffer);
                    }
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
            }
        }
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        if (gCipserver.mode == CIPSERVER_OPEN)
        {
            AT_Sprintf(aucBuffer, "+CIPSERVER:%d,%d", gCipserver.mode, gCipserver.port);
        }
        else
        {
            AT_Sprintf(aucBuffer, "+CIPSERVER:%d", gCipserver.mode);
        }
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 aucBuffer[40] = {0};
        AT_Sprintf(aucBuffer, "+CIPSERVER:(0-CLOSE SERVER,1-OPEN SERVER),(1,65535)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPSERVER, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}
VOID AT_TCPIP_CmdFunc_CIPCSGP(AT_CMD_PARA *pParam)
{

    AT_TC(g_sw_TCPIP, "AT+CIPCSGP >> pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        UINT8 index = 0;
        UINT8 mode;
        UINT8 *apn = NULL;
        UINT8 *user = "";
        UINT8 *password = "";
        UINT8 dlci = pParam->nDLCI;
        UINT8 param_count = pParam->paramCount;
        bool paramRet = TRUE;
        if (param_count < 2)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCSGP invalid params count param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get mode
        mode = at_ParamUintInRange(pParam->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCSGP fail to get mode param");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // don't support CSD
        if (mode == 0)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCSGP don't support CSD");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT));
        }
        index++;
        // get apn
        apn = at_ParamStr(pParam->params[index], &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCSGP fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (param_count > 2)
        {
            index++;
            // get user name
            user = at_ParamStr(pParam->params[index], &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPCSGP fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }
        if (param_count > 3)
        {
            index++;
            // get password
            password = at_ParamStr(pParam->params[index], &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPCSGP fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }
        cipCSGP_setGprs = mode;
        UINT8 apnlen = AT_StrLen(apn);
        UINT8 usrlen = AT_StrLen(user);
        UINT8 pwlen = AT_StrLen(password);
        if (apnlen > 0 && apnlen < 51 && usrlen > 0 && usrlen < 51 && pwlen > 0 && pwlen < 51)
        {
            AT_MemCpy(cipApn, apn, apnlen);
            AT_MemCpy(cipUserName, user, usrlen);
            AT_MemCpy(cipPassWord, password, pwlen);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else
        {
            AT_TC(g_sw_TCPIP, "CIPCSGP apnlen=%d,srlen=%d,pwlen=%d", apnlen, usrlen, pwlen);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CIPCSGP: 0-CSD,DIALNUMBER,USER NAME,PASSWORD,RATE(0-3) -- NOT SUPPORT");
        at_CmdRespInfoText(pParam->engine, "+CIPCSGP: 1-GPRS,APN,USER NAME,PASSWORD");
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 *rspstr = NULL;
        if (cipCSGP_setGprs == 1)
        {
            UINT8 tmp_buff[180] = {0};
            AT_Sprintf(tmp_buff, "+CIPCSGP:1,\"%s\",\"%s\",\"%s\"",
                       cipApn, cipUserName, cipPassWord);
            rspstr = tmp_buff;
        }
        else
        {
            rspstr = "+CIPCSGP:<mode>,<apn>,<user name>,<password>"; //[<rate>]
        }
        at_CmdRespInfoText(pParam->engine, rspstr);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPDPDP(AT_CMD_PARA *pParam)
{
    UINT8 simid = AT_SIM_ID(pParam->nDLCI);

    AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
}

VOID AT_TCPIP_CmdFunc_CIPMODE(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CIPMODE >> pParam->nDLCI= %d", pParam->nDLCI);

    if (cipMux_multiIp)
    {
        AT_TC(g_sw_TCPIP, "AT+CIPMODE must be used in single connection mode");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount > 2)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPMODE invalid param_count:%d", pParam->paramCount);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        bool paramRet = TRUE;
        UINT8 mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
        if (!paramRet)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        UINT8 currentMode = cipMODE_transParent;
        cipMODE_transParent = mode;
        if (currentMode && mode == 0)
        {
            stop_transparent_mode(pParam->nDLCI);
        }
        else
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CIPMODE: (0-NORMAL MODE,1-TRANSPARENT MODE)");
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 rspstr[32] = {0};
        AT_Sprintf(rspstr, "+CIPMODE:%d", cipMODE_transParent);
        at_CmdRespInfoText(pParam->engine, rspstr);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPCCFG(AT_CMD_PARA *pParam)
{
    UINT8 simid = AT_SIM_ID(pParam->nDLCI);
    UINT8 param_count = pParam->paramCount;
    INT32 ret;
    UINT8 index = 0;
    UINT8 retry_number;
    UINT8 wait_time;
    UINT16 send_size;
    UINT8 escape = 1;
    UINT8 rxmode;
    UINT16 rxsize = 1460;
    UINT16 rxtimer = 50;
    UINT8 paramRet = TRUE;

    AT_TC(g_sw_TCPIP, "AT+CIPCCFG >> pParam->nDLCI= %d", pParam->nDLCI);

    if (cipMux_multiIp)
    {
        AT_TC(g_sw_TCPIP, "AT+CIPCCFG must be used in single connection mode");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }

    if (AT_CMD_SET == pParam->iType)
    {
        if (param_count < 4 || param_count > 7)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCCFG invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get retry number
        retry_number = at_ParamUintInRange(pParam->params[index], 3, 8, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        index++;
        // get wait time
        wait_time = at_ParamUintInRange(pParam->params[index], 1, 10, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        index++;
        // get send size
        send_size = at_ParamUintInRange(pParam->params[index], 1, 1460, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        index++;
        // get escape character flag
        escape = at_ParamUintInRange(pParam->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        cipCCFG_esc = escape;
        cipCCFG_WaitTm = wait_time;
        cipCCFG_NmRetry = retry_number;
        cipCCFG_SendSz = send_size;

        if (param_count > 4)
        {
            index++;
            // get rxmode
            rxmode = at_ParamUintInRange(pParam->params[index], 0, 1, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            cipCCFG_Rxmode = rxmode;

            index++;
            // get rxsize
            rxsize = at_ParamUintInRange(pParam->params[index], 50, 1460, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            cipCCFG_RxSize = rxsize;

            index++;
            // get rxtimer
            rxtimer = at_ParamUintInRange(pParam->params[index], 20, 1000, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPCCFG fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            cipCCFG_Rxtimer = rxtimer;
        }

        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CIPCCFG: (NmRetry:3-8),(WaitTm:1-10),(SendSz:1-1460),(esc:0,1),(Rxmode:0,1),(RxSize:50-1460),(Rxtimer:20-1000)");
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 rspstr[64] = {0};
        AT_Sprintf(rspstr, "+CIPCCFG:%d,%d,%d,%d,%d,%d,%d", cipCCFG_NmRetry, cipCCFG_WaitTm,
                   cipCCFG_SendSz, cipCCFG_esc, cipCCFG_Rxmode, cipCCFG_RxSize, cipCCFG_Rxtimer);
        at_CmdRespInfoText(pParam->engine, rspstr);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPSHOWTP(AT_CMD_PARA *pParam)
{
    UINT8 simid = AT_SIM_ID(pParam->nDLCI);
    UINT8 param_count = pParam->paramCount;
    INT32 ret;
    UINT8 index = 0;
    UINT8 mode;
    bool paramRet = TRUE;

    AT_TC(g_sw_TCPIP, "AT+CIPSHOWTP >> pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        // get mode
        mode = at_ParamUintInRange(pParam->params[index], 0, 1, &paramRet);
        if (!paramRet)
        {
            AT_TC(g_sw_TCPIP, "AT+CIPSHOWTP fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        cipSHOWTP_dispTP = mode;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "+CIPSHOWTP: (0,1)");
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 rspstr[32] = {0};
        ret = AT_Sprintf(rspstr, "+CIPSHOWTP:%d", cipSHOWTP_dispTP);
        at_CmdRespInfoText(pParam->engine, rspstr);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPUDPMODE(AT_CMD_PARA *pParam)
{
    UINT8 simid = AT_SIM_ID(pParam->nDLCI);
    UINT8 param_count = pParam->paramCount;
    UINT8 index = 0;
    UINT8 mode;
    UINT8 *ipaddr = NULL;
    UINT8 iplen;
    UINT16 port;
    bool paramRet = TRUE;

    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE >> pParam->nDLCI= %d", pParam->nDLCI);

    if (AT_CMD_SET == pParam->iType)
    {
        if (cipMux_multiIp)
        {
            if ((param_count != 2 && param_count != 4))
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE invalid param_count:%d", param_count);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            UINT8 uMuxIndex = 0;
            // get mode
            uMuxIndex = at_ParamUintInRange(pParam->params[index], 0, 7, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            // get mode
            index++;
            mode = at_ParamUintInRange(pParam->params[index], 0, 2, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }

            if (mode == 2 && param_count != 4)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE dismatched mode:%d and param_count:%d", mode, param_count);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            if (mode == 2)
            {
                index++;
                // get ip address
                ipaddr = at_ParamStr(pParam->params[index], &paramRet);
                if (!paramRet)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                iplen = AT_StrLen(ipaddr);
                if (AT_GPRS_IPAddrAnalyzer(ipaddr, iplen) < 0)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE invalid ipaddr param:%s", ipaddr);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                index++;
                // get port
                port = at_ParamUintInRange(pParam->params[index], 1, 65535, &paramRet);
                if (!paramRet)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                update_udp_remote_socket(uMuxIndex, ipaddr, iplen, port);
            }
            else
            {
                set_udpmode(uMuxIndex, mode);
            }
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
        else
        {
            if ((param_count != 1 && param_count != 3))
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE invalid param_count:%d", param_count);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            // get mode
            mode = at_ParamUintInRange(pParam->params[index], 0, 2, &paramRet);
            if (!paramRet)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }

            if (mode == 2 && param_count != 3)
            {
                AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE dismatched mode:%d and param_count:%d", mode, param_count);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            if (mode == 2)
            {
                index++;
                // get ip address
                ipaddr = at_ParamStr(pParam->params[index], &paramRet);
                if (!paramRet)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                iplen = AT_StrLen(ipaddr);
                if (AT_GPRS_IPAddrAnalyzer(ipaddr, iplen) < 0)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE invalid ipaddr param:%s", ipaddr);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                index++;
                // get port
                port = at_ParamUintInRange(pParam->params[index], 1, 65535, &paramRet);
                if (!paramRet)
                {
                    AT_TC(g_sw_TCPIP, "AT+CIPUDPMODE fail to get param %d", index);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                update_udp_remote_socket(DEF_TCPIP, ipaddr, iplen, port);
            }
            else
            {
                set_udpmode(DEF_TCPIP, mode);
            }
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        if (cipMux_multiIp)
            at_CmdRespInfoText(pParam->engine, "+CIPUDPMODE: (0-7),(0-2),(\"(0-255).(0-255).(0-255).(0-255)\"),(1-65535)");
        else
            at_CmdRespInfoText(pParam->engine, "+CIPUDPMODE: (0-2),(\"(0-255).(0-255).(0-255).(0-255)\"),(1-65535)");

        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 *rspstr = NULL;
        UINT8 tmp_buff[64] = {0};
        if (cipMux_multiIp)
        {
            for (int i = 0; i < MEMP_NUM_NETCONN; i++)
            {
                AT_Sprintf(tmp_buff, "+CIPUDPMODE:%d,%d,%s,%d", i,
                           g_uCipContexts.nTcpipParas[i].udpmode,
                           g_uCipContexts.nTcpipParas[i].uaIPAddress,
                           g_uCipContexts.nTcpipParas[i].uPort);
                at_CmdRespUrcText(pParam->engine, tmp_buff);
            }
        }
        else
        {
            AT_Sprintf(tmp_buff, "+CIPUDPMODE:%d,%s,%d",
                       g_uCipContexts.nTcpipParas[0].udpmode,
                       g_uCipContexts.nTcpipParas[0].uaIPAddress,
                       g_uCipContexts.nTcpipParas[0].uPort);
            at_CmdRespInfoText(pParam->engine, tmp_buff);
        }
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

/*****************************************************************************
* Name:        AT_TCPIP_Result_OK
* Description: TCP/IP module execute ok process.
* Parameter:  UINT32 uReturnValue,
                    UINT32 uResultCode,
                    UINT8  nDelayTime,
                    UINT8* pBuffer,
                    UINT16 nDataSize
* Return:       VOID
* Remark:      n/a
* Author:       YangYang
* Data:          2008-5-15
******************************************************************************/
VOID AT_TCPIP_Result_OK(UINT32 uReturnValue,
                        UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)
{

    PAT_CMD_RESULT pResult = NULL;

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

/*****************************************************************************
* Name:        AT_TCPIP_Result_Err
* Description: TCP/IP module execute error process.
* Parameter:   UINT32 uErrorCode, UINT8 nErrorType
* Return:      VOID
* Remark:      n/a
* Author:      YangYang
* Data:        2008-5-15
******************************************************************************/
VOID AT_TCPIP_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI)
{

    PAT_CMD_RESULT pResult = NULL;

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

/*****************************************************************************
* Name:        AT_TCPIP_Connect
* Description: TCP/IP module TCP/IP establish connection process.
* Parameter:   none
* Return:      VOID
* Remark:      May be call either during command process or in asynchronous process.
* Author:      YangYang
* Data:        2008-5-16
******************************************************************************/
INT32 AT_TCPIP_Connect(UINT8 nDLCI, UINT8 nMuxIndex)
{
    INT32 iResult;
    INT32 iResultBind;
    UINT8 nSim = AT_SIM_ID(nDLCI);
    CFW_TCPIP_SOCKET_ADDR nDestAddr;
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);

    iResult =
        CFW_TcpipSocketEX(tcpipParas->uDomain,
                          tcpipParas->uType,
                          tcpipParas->uProtocol, tcpip_rsp, nMuxIndex << 16 | nDLCI);
    if (SOCKET_ERROR == iResult)
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect(), CFW_TcpipSocket() failed\n");
        return iResult;
    }
    if (g_keepalive == 1)
    {
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
        CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&g_keepidle, sizeof(g_keepidle));
        CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&g_keepinterval, sizeof(g_keepinterval));
        CFW_TcpipSocketSetsockopt(iResult, IPPROTO_TCP, TCP_KEEPCNT, (void *)&g_keepcount, sizeof(g_keepcount));
    }
    else
    {
        CFW_TcpipSocketSetsockopt(iResult, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
    }
    tcpipParas->uState = CIP_PROCESSING;
    tcpipParas->uSocket = iResult;

    struct netif *netif = NULL;
    struct ifreq iface = {0};

    if (bearerParas->nCid == 0x11)
        netif = getEtherNetIf(bearerParas->nCid);
    else
        netif = getGprsNetIf(nSim, bearerParas->nCid);
    if (netif == NULL)
        return -1;

    iface.ifr_name[0] = netif->name[0];
    iface.ifr_name[1] = netif->name[1];
    strcat(iface.ifr_name, i8tostring(netif->num));
    AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect() netif:%s\n", iface.ifr_name);

    ip4_addr_t *ip_addr = netif_ip4_addr(netif);

    CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
        0,
    };
    stLocalAddr.sin_len = 0;
    stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
    if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_UDP)
        stLocalAddr.sin_port = htons(gClocalport[nMuxIndex].udp_port);
    else
        stLocalAddr.sin_port = htons(gClocalport[nMuxIndex].tcp_port);
    stLocalAddr.sin_addr.s_addr = ip_addr->addr;

    iResultBind = CFW_TcpipSocketBind(iResult, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));
    if (SOCKET_ERROR == iResultBind)
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect(), CFW_TcpipSocketBind() failed\n");
        return iResultBind;
    }

    ip_addr_t nIpAddr;
    TCPIP_DNS_PARAM *dnsparam = AT_MALLOC(sizeof(TCPIP_DNS_PARAM));
    dnsparam->nDLCI = nDLCI;
    dnsparam->nMuxIndex = nMuxIndex;
    iResult = CFW_GethostbynameEX(tcpipParas->uaIPAddress, &nIpAddr, bearerParas->nCid, nSim, tcpip_dnsReq_callback, dnsparam);
    if (iResult == RESOLV_QUERY_INVALID)
    {
        AT_TC(g_sw_GPRS, "tcpip connect gethost by name error");
        AT_FREE(dnsparam);
        return -1;
    }
    else if (iResult == RESOLV_COMPLETE)
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect(), CFW_GethostbynameEX() RESOLV_COMPLETE\n");
        AT_FREE(dnsparam);
        UINT8 *pcIpAddr = ipaddr_ntoa(&nIpAddr);
        AT_StrNCpy(tcpipParas->uaIPAddress, pcIpAddr, AT_TCPIP_ADDRLEN);
        if (tcpipParas->uProtocol == CFW_TCPIP_IPPROTO_TCP || tcpipParas->udpmode == 0)
        {
            nDestAddr.sin_family = CFW_TCPIP_AF_INET;
            nDestAddr.sin_port = htons(tcpipParas->uPort);
            iResult = CFW_TcpipInetAddr(tcpipParas->uaIPAddress);
            if (SOCKET_ERROR == iResult)
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect(), CFW_TcpipInetAddr() failed\n");
                return iResult;
            }
            else
            {
                nDestAddr.sin_addr.s_addr = iResult;
            }
            iResult = CFW_TcpipSocketConnect(tcpipParas->uSocket, &nDestAddr, SIZEOF(CFW_TCPIP_SOCKET_ADDR));
            if (SOCKET_ERROR == iResult)
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_Connect, CFW_TcpipSocketConnect() failed\n");
                return iResult;
            }
        }
        else
        {
            tcpip_connect_done(nDLCI, nMuxIndex);
        }
    }
    else if (iResult == RESOLV_QUERY_QUEUED)
    {
        AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
        AT_SetAsyncTimerMux(engine, 60);
        return ERR_SUCCESS;
    }

    return ERR_SUCCESS;
}

INT32 AT_TCPIP_ServerStart(UINT8 nDLCI)
{
    INT32 iResult;
    stAT_Bearer_Paras *bearerParas = &(g_uCipContexts.nBearerParas[gCipBearer]);
    iResult = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP, tcpip_rsp, 0xff << 16 | nDLCI);
    if (SOCKET_ERROR == iResult)
    {
        AT_TC(g_sw_TCPIP, "CFW_TcpipSocketEX() failed\n");
        return iResult;
    }
    gCipserver.nSocketId = iResult;
    if (g_keepalive == 1)
    {
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&g_keepidle, sizeof(g_keepidle));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&g_keepinterval, sizeof(g_keepinterval));
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, IPPROTO_TCP, TCP_KEEPCNT, (void *)&g_keepcount, sizeof(g_keepcount));
    }
    else
    {
        CFW_TcpipSocketSetsockopt(gCipserver.nSocketId, SOL_SOCKET, SO_KEEPALIVE, (void *)&g_keepalive, sizeof(g_keepalive));
    }
    struct netif *netif = NULL;
    struct ifreq iface = {0};
    UINT8 nSim = AT_SIM_ID(nDLCI);

    if (bearerParas->nCid == 0x11)
        netif = getEtherNetIf(bearerParas->nCid);
    else
        netif = getGprsNetIf(nSim, bearerParas->nCid);
    if (netif == NULL)
    {
        return -1;
    }

    iface.ifr_name[0] = netif->name[0];
    iface.ifr_name[1] = netif->name[1];
    strcat(iface.ifr_name, i8tostring(netif->num));
    AT_TC(g_sw_TCPIP, "AT+CIPSERVER netif: %s", iface.ifr_name);

    ip4_addr_t *ip_addr = netif_ip4_addr(netif);

    CFW_TCPIP_SOCKET_ADDR stLocalAddr = {
        0,
    };
    stLocalAddr.sin_len = 0;
    stLocalAddr.sin_family = CFW_TCPIP_AF_INET;
    stLocalAddr.sin_port = htons(gCipserver.port);
    stLocalAddr.sin_addr.s_addr = ip_addr->addr;

    if (SOCKET_ERROR == CFW_TcpipSocketBind(gCipserver.nSocketId, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR)))
    {
        AT_TC(g_sw_TCPIP, "CFW_TcpipSocketBind() failed\n");
        return -1;
    }

    if (CFW_TcpipSocketListen(gCipserver.nSocketId, 0) != ERR_SUCCESS)
    {
        AT_TC(g_sw_TCPIP, "CFW_TcpipSocketListen() failed\n");
        return -1;
    }
    return ERR_SUCCESS;
}

/*****************************************************************************
* Name          :   AT_TCPIP_CmdFunc_CDNSGIP
* Description       :   T+CDNSCFG command
* Parameter     :   AT_CMD_PARA *pParam
* Return            :   VOID
* Remark        :   n/a
* Author            :   wulc
* Data          :     20013-1-31
******************************************************************************/
static void dnsReq_callback(COS_EVENT *ev)
{
    AT_CMD_ENGINE_T *engine = (AT_CMD_ENGINE_T *)ev->nParam3;
    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        UINT8 PromptBuff[130] = {0};
        ip_addr_t *ipaddr = (ip_addr_t *)ev->nParam1;
        char acHostname[101] = {0};
        INT8 *pcIpAddr;
        AT_MemCpy(acHostname, (CONST VOID *)(ev->nParam2), SIZEOF(acHostname));
        pcIpAddr = ipaddr_ntoa(ipaddr);
        at_CmdRespOK(engine);
        AT_Sprintf(PromptBuff, "+CDNSGIP: 1,\"%s\",\"%s\"", acHostname, pcIpAddr);
        at_CmdRespUrcText(engine, PromptBuff);
    }
    else if (ev->nEventId == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        UINT32 ulRet = 8; //DNS common error
        UINT8 PromptBuff[20] = {0};
        at_CmdRespError(engine);
        AT_Sprintf(PromptBuff, "+CDNSGIP: 0,%d", ulRet);
        at_CmdRespUrcText(engine, PromptBuff);
    }
    AT_FREE(ev);
}

VOID AT_TCPIP_CmdFunc_CDNSGIP(AT_CMD_PARA *pParam)
{
    AT_TC(g_sw_TCPIP, "AT+CDNSGIP ... ... pParam->nDLCI= %d", pParam->nDLCI);
    UINT32 ulRet = 0;
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    UINT8 nDlc = pParam->nDLCI;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        bool paramok = true;
        UINT32 ulIpAddr = 0;
        const uint8_t *acHostname = at_ParamStr(pParam->params[0], &paramok);

        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        ip_addr_t nIpAddr = IPADDR4_INIT(ulIpAddr);

        ulRet = CFW_GethostbynameEX(acHostname, &nIpAddr, g_uCipContexts.nBearerParas[gCipBearer].nCid, nSim, dnsReq_callback, pParam->engine);
        AT_TC(g_sw_TCPIP, "CDNSGIP CFW_Gethostbyname ret %d\n", ulRet);

        if (ulRet == RESOLV_COMPLETE)
        {
            INT8 *pcIpAddr = NULL;
            UINT8 PromptBuff[130] = {0};
            pcIpAddr = ipaddr_ntoa(&nIpAddr);
            at_CmdRespOK(pParam->engine);
            AT_Sprintf(PromptBuff, "+CDNSGIP: 1,\"%s\",\"%s\"", acHostname, pcIpAddr);
            at_CmdRespUrcText(pParam->engine, PromptBuff);
        }
        else if (ulRet == RESOLV_QUERY_QUEUED)
        {
            AT_SetAsyncTimerMux(pParam->engine, 60);
        }
        else
        {
            UINT8 PromptBuff[20] = {0};
            UINT32 retErrno = 8; // DNS common error
            at_CmdRespError(pParam->engine);
            AT_Sprintf(PromptBuff, "+CDNSGIP: 0,%d", retErrno);
            at_CmdRespUrcText(pParam->engine, PromptBuff);
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CDNSGIP, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

#ifdef LTE_NBIOT_SUPPORT
UINT8 nas_rai = 0;

VOID AT_TCPIP_CmdFunc_CTDCP(AT_CMD_PARA *pParam)
{
#if 0
    UINT8 uParamCount          = 0;
    INT32 iResult              = 0;
    UINT32 errCode            = CMD_FUNC_SUCC ;
    UINT8 uCidState            = 0;
    UINT16 uSize                = 0;
    //UINT8 rai                      =0;
   // UINT8 pdnType              = 0;
    CFW_GPRS_DATA *pGprsData = NULL;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 AtRet[100]           = { 0 }; /* max 20 charactors per cid */
    CFW_TCPIP_SOCKET_ADDR nDestAddr;

    nDestAddr.sin_family = CFW_TCPIP_AF_INET;
#ifndef AT_NO_GPRS
    nDestAddr.sin_port   = htons(g_stATTcpipParas[pParam->nDLCI].uPort);
#endif
    nDestAddr.sin_len    = 4;
    // dest_addr.sin_addr.s_addr = CFW_TcpipInetAddr("116.228.4.122");
#ifndef AT_NO_GPRS
    iResult = CFW_TcpipInetAddr(g_stATTcpipParas[pParam->nDLCI].uaIPAddress);
#endif
    AT_TC_MEMBLOCK(g_sw_AT_SMS, g_stATTcpipParas[pParam->nDLCI].uaIPAddress, 20, 16);
    if (AT_TCPIP_ERR == iResult)
    {
        AT_TC(g_sw_TCPIP, "exe       in AT_TCPIP_CmdFunc_CIPSTART(), CFW_TcpipInetAddr() failed\n");
        AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return ;
    }
    else
    {
        nDestAddr.sin_addr.s_addr = iResult;
    }

    AT_TC(g_sw_TCPIP, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
   // AT_Gprs_CidInfo  *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];
    AT_TC(g_sw_GPRS, "+CTDCP: nSim= %d; nDLCI= 0x%x", nSim, pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "exe       in AT_TCPIP_CmdFunc_NIPDATA, parameters is NULL\n");
	 errCode = ERR_AT_CME_PARAM_INVALID;
        goto ctdcp_result;

    }

    if (AT_CMD_SET == pParam->iType)
    {
        // check paracount
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);

        if ((iResult != ERR_SUCCESS) || (uParamCount > 2) || (uParamCount == 0))
        {
            AT_TC(g_sw_GPRS, "exe       in AT_TCPIP_CmdFunc_CTDCP, parameters error or no parameters offered \n");
            errCode = 54;
            goto ctdcp_result;
        }
#ifndef AT_NO_GPRS
        CFW_GetGprsActState(1, &uCidState, nSim);
#endif
        if (uCidState != CFW_GPRS_ACTIVED)
        {
            AT_TC(g_sw_GPRS, "pdp have not be activated!!");
            errCode = 55;
            goto ctdcp_result;
        }

	 uSize = AT_GPRS_CP_DATA_MAX_LEN;
        if(pGprsData == NULL)
        {
            if((pGprsData = AT_MALLOC(AT_GPRS_CP_DATA_MAX_LEN+4+1)) == NULL)
            {
	         errCode = ERR_AT_CME_NO_MEMORY;
                goto ctdcp_result;
             }
         }
         iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, pGprsData->pData, &uSize);
	  if(uSize >= 0)
         {
	      if(uSize > AT_GPRS_CP_DATA_MAX_LEN) /*may be operate invalid address, attention*/
	      {
	          errCode = ERR_AT_CME_TEXT_LONG;
                 goto ctdcp_result;
	       }
		else if((uSize == 0) || (pGprsData->pData == NULL) || strlen(pGprsData->pData) == 0)
		{
		    goto ctdcp_result;
		}
		pGprsData->nDataLength = uSize;
	       
         }
	    
         if (uParamCount == 2)
         {
             uSize   = 1;
             iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nas_rai, &uSize);

              if (iResult != ERR_SUCCESS)
              {
                  errCode = 56;
                  goto ctdcp_result;
               }
         }
	     if(nas_rai > 2)
	     {
	         errCode = 57;
             goto ctdcp_result;
	     }
         if(udpflag == 1)
         {
#ifndef AT_NO_GPRS
                iResult = CFW_TcpipSocketSendto(g_uATTcpipSocket[pParam->nDLCI], (UINT8 *)pGprsData->pData, pGprsData->nDataLength, 0, &nDestAddr, nDestAddr.sin_len);
#endif
                if (iResult == SOCKET_ERROR)
                {
                    AT_TC(g_sw_TCPIP, "UDP send socket data error");
                    //AT_TCPIP_Result_Err(ERR_AT_CMS_INVALID_LEN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    //return;
		            errCode = ERR_AT_CMS_INVALID_LEN;
		            goto ctdcp_result;
                }
                //g_uATTcpipOpType[pParam->nDLCI] = AT_TCPIP_OPTYPE_SEND;
                //AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
         }
	  else
	  {
	      errCode = ERR_AT_CME_GPRS_OPTION_NOT_SUPPORTED;
	  }
    	  goto ctdcp_result;
	  
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        AT_MemSet(AtRet, 0, SIZEOF(AtRet));
        AT_Sprintf(AtRet, "+CTDCP: (\"This is a CP Data\"), [RAI]");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtRet, AT_StrLen(AtRet), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_GPRS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    ctdcp_result:
    if(pGprsData!=NULL)
    {
        AT_FREE(pGprsData);
    }
    if(errCode == CMD_FUNC_SUCC)
    {
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
	 return;
    }
    else
    {
        AT_TC(g_sw_GPRS, "errCode : %d", errCode);
        AT_GPRS_Result_Err(errCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	 return;
    }
#endif
}

UINT8 filterSwitch = 0;
void AT_TCPIP_CmdFunc_IPFLT(AT_CMD_PARA *pParam)
{
    INT32 iResult;
    UINT8 nCount = 0;
    UINT8 fltSwitch = 0;
    UINT16 nSize = 0;
    if (NULL == pParam)
    {
        AT_TC(g_sw_TCPIP, "AT+IPFLT: pParam = NULL");
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0);
        return;
    }
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &nCount);
        if ((iResult != ERR_SUCCESS) || (nCount != 1))
        {
            AT_TC(g_sw_TCPIP, " AT_TCPIP_CmdFunc_IPFLT: Parameter = %s\n", pParam->pPara);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        nSize = SIZEOF(fltSwitch);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &fltSwitch, &nSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_IPFLT: pParam->pPara1 = %s\n", pParam->pPara);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        if ((fltSwitch > 15) || (fltSwitch < 0))
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_IPFLT: pParam->pPara2 = %s\n", pParam->pPara);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        filterSwitch = fltSwitch;
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8 *pTestRsp = "+IPFLT: 0";
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pTestRsp, AT_StrLen(pTestRsp), pParam->nDLCI);
        return;
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        UINT8 pTestRsp[20];
        sprintf(pTestRsp, "+IPFLT: %d", filterSwitch);
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pTestRsp, AT_StrLen(pTestRsp), pParam->nDLCI);
        return;
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_IPFLT: This type does not supported!");
        AT_TCPIP_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}

#endif //LTE_NBIOT_SUPPORT

//set tcp keepalive
VOID AT_TCPIP_CmdFunc_CIPTKA(AT_CMD_PARA *pParam)
{
    UINT8 aucBuffer[256] = {0};

    AT_TC(g_sw_TCPIP, "AT+CIPTKA ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount < 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        bool paramok = true;
        int keepalive = g_keepalive;
        int keepidle = g_keepidle;
        int keepinterval = g_keepinterval;
        int keepcount = g_keepcount;

        keepalive = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (pParam->paramCount > 1)
            keepidle = at_ParamUintInRange(pParam->params[1], 30, 7200, &paramok);
        if (pParam->paramCount > 2)
            keepinterval = at_ParamUintInRange(pParam->params[2], 30, 600, &paramok);
        if (pParam->paramCount > 3)
            keepcount = at_ParamUintInRange(pParam->params[3], 1, 9, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        g_keepalive = keepalive;
        g_keepidle = keepidle;
        g_keepinterval = keepinterval;
        g_keepcount = keepcount;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(aucBuffer, "+CIPTKA: <%d>,<%d>,<%d>,<%d>", g_keepalive, g_keepidle, g_keepinterval, g_keepcount);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(aucBuffer, "+CIPTKA: (0-Disable,1-Enable),(30-7200),(30-600),(1-9)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPTKA, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_TCPIP_CmdFunc_CIPRDTIMER(AT_CMD_PARA *pParam)
{
    UINT8 aucBuffer[128] = {0};

    AT_TC(g_sw_TCPIP, "AT+CIPRDTIMER ... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount < 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        bool paramok = true;
        int rdsigtimer = 0;
        int rdmuxtimer = 0;

        rdsigtimer = at_ParamUintInRange(pParam->params[0], 100, 4000, &paramok);
        rdmuxtimer = at_ParamUintInRange(pParam->params[1], 100, 7000, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        g_rdsigtimer = rdsigtimer;
        g_rdmuxtimer = rdmuxtimer;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(aucBuffer, "+CIPRDTIMER: <%d>,<%d>", g_rdsigtimer, g_rdmuxtimer);
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(aucBuffer, "+CIPRDTIMER: (100-4000),(100-7000)");
        at_CmdRespInfoText(pParam->engine, aucBuffer);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_CIPRDTIMER, command type:%d not allowed", pParam->iType);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

static VOID tcpip_close_timeout(void *param)
{
    UINT32 nParam = param;
    UINT8 nDlc = nParam;
    UINT8 nMuxIndex = nParam >> 16;
    UINT8 uaRspStr[128] = {0};
    AT_TC(g_sw_TCPIP, "tcpip_close_timeout, nDlc %d, nMuxIndex %d", nDlc, nMuxIndex);
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDlc);
    stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[nMuxIndex]);
    if (tcpipParas->uState == CIP_CLOSING)
    {
        if (!cipMux_multiIp)
            AT_Sprintf(uaRspStr, "CLOSED");
        else
            AT_Sprintf(uaRspStr, "%d,CLOSED", nMuxIndex);
        at_CmdRespUrcText(engine, uaRspStr);
    }
    else if (tcpipParas->uState != CIP_CLOSED)
    {
        if (!cipMux_multiIp)
            AT_Sprintf(uaRspStr, "CLOSE OK");
        else
            AT_Sprintf(uaRspStr, "%d,CLOSE OK", nMuxIndex);
        at_CmdRespOKText(engine, uaRspStr);
    }
    tcpipParas->uState = CIP_CLOSED;
}

static void start_tcpip_close_timer(UINT8 nDLCI, UINT8 nMuxIndex)
{
    UINT32 param = nMuxIndex << 16 | nDLCI;
    AT_TC(g_sw_TCPIP, "start_tcpip_close_timer, nDlc %d, nMuxIndex %d", nDLCI, nMuxIndex);
    if (!cipMux_multiIp)
    {
        at_StartCallbackTimer(g_rdsigtimer, tcpip_close_timeout, param);
    }
    else
    {
        at_StartCallbackTimer(g_rdmuxtimer, tcpip_close_timeout, param);
    }
}

static void stop_tcpip_close_timer(UINT8 nDLCI, UINT8 nMuxIndex)
{
    UINT32 param = nMuxIndex << 16 | nDLCI;
    at_StopCallbackTimer(tcpip_close_timeout, param);
}

#endif
